#include "aresbc/vm_check.h"

#include <iostream>

#include "aresbc/field_info.h"

using namespace aresbc;

void VMCheck::visit_class(ClassFile& class_file) {
    if (class_file.magic_number != 0xCAFEBABE) {
        std::cerr << "The magic number doesn't match \"0xCAFEBABE\"." << std::endl;
        abort();
    }

    if (class_file.class_version == ClassFile::UNDEFINED) {
        std::cerr << "Couldn't set the class file version because it is an undefined value." << std::endl;
        abort();
    }

    if (class_file.class_version > ClassFile::VERSION_12 && (class_file.minor_version != 0 && class_file.minor_version != 65535)) {
        std::cerr << "All Java 12 class files need a minor version of 0 or 65535." << std::endl;
        abort();
    }

    for (auto& constant_pool_info : class_file.constant_pool) {
        VMCheck::visit_classpool_info(class_file, constant_pool_info);
    }

    if (class_file.has_access_flag(ClassFile::INTERFACE)) {
        if (!class_file.has_access_flag(ClassFile::ABSTRACT)
            || class_file.has_access_flag(ClassFile::FINAL)
            || class_file.has_access_flag(ClassFile::SUPER)
            || class_file.has_access_flag(ClassFile::ENUM)
            || class_file.has_access_flag(ClassFile::MODULE)) {
            std::cerr << "The class file has invalid interface access flags." << std::endl;
            abort();
        }
    } else if (class_file.has_access_flag(ClassFile::ANNOTATION)) {
        if (!class_file.has_access_flag(ClassFile::INTERFACE)
            || class_file.has_access_flag(ClassFile::ABSTRACT)
            || class_file.has_access_flag(ClassFile::FINAL)) {
            std::cerr << "The class file has invalid annotation access flags." << std::endl;
            abort();
        }
    } else if (class_file.has_access_flag(ClassFile::MODULE)) {
        if (class_file.has_access_flag(ClassFile::ABSTRACT)
            || class_file.has_access_flag(ClassFile::FINAL)) {
            std::cerr << "The class file has invalid module access flags." << std::endl;
            abort();
        }
    }

    if (!class_file.is_valid_index(class_file.this_class)) {
        std::cerr << "The \"this class\" index is not a valid constant pool index." << std::endl;
        abort();
    }

    const auto this_class = class_file.constant_pool[class_file.this_class - 1];
    if (this_class.tag != ConstantPoolInfo::CLASS) {
        std::cerr << "The \"this class\" is not a class constant pool info." << std::endl;
        abort();
    }

    if (class_file.super_class != 0) {
        if (!class_file.is_valid_index(class_file.super_class)) {
            std::cerr << "The \"super class\" index is not a valid constant pool index." << std::endl;
            abort();
        }

        const auto super_class = class_file.constant_pool[class_file.super_class - 1];
        if (super_class.tag != ConstantPoolInfo::CLASS) {
            std::cerr << "The \"super class\" is not a class constant pool info." << std::endl;
            abort();
        }
    }

    for (const auto& interface : class_file.interfaces) {
        VMCheck::visit_class_interface(class_file, interface);
    }

    for (auto& field_info : class_file.fields) {
        VMCheck::visit_class_field(class_file, field_info);
    }

    for (auto& method_info : class_file.methods) {
        VMCheck::visit_class_method(class_file, method_info);
    }

    for (auto& attribute_info : class_file.attributes) {
        VMCheck::visit_class_attribute(class_file, attribute_info);
    }
}

void VMCheck::visit_classpool_info(ClassFile& class_file, ConstantPoolInfo& constantPoolInfo) {
    switch (constantPoolInfo.tag) {
        case ConstantPoolInfo::CLASS: {
            visit_class_info(class_file, constantPoolInfo.info.class_info);
            break;
        }
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF: {
            visit_field_method_info(class_file, constantPoolInfo.info.field_method_info);
            break;
        }
        case ConstantPoolInfo::NAME_AND_TYPE: {
            visit_name_and_type_info(class_file, constantPoolInfo.info.name_and_type_info);
            break;
        }
        case ConstantPoolInfo::STRING: {
            visit_string_info(class_file, constantPoolInfo.info.string_info);
            break;
        }
        case ConstantPoolInfo::METHOD_TYPE: {
            visit_method_type_info(class_file, constantPoolInfo.info.method_type_info);
            break;
        }
        case ConstantPoolInfo::METHOD_HANDLE: {
            visit_method_handle_info(class_file, constantPoolInfo.info.method_handle_info);
            break;
        }
        case ConstantPoolInfo::DYNAMIC:
        case ConstantPoolInfo::INVOKE_DYNAMIC: {
            visit_dynamic_info(class_file, constantPoolInfo.info.dynamic_info);
            break;
        }
        case ConstantPoolInfo::MODULE:
        case ConstantPoolInfo::PACKAGE: {
            visit_module_package_info(class_file, constantPoolInfo.info.module_package_info);
            break;
        }
        case ConstantPoolInfo::UTF_8:
        case ConstantPoolInfo::INTEGER:
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE:
        case ConstantPoolInfo::UNDEFINED: {
            break;
        }
    }
}

void VMCheck::visit_class_interface(ClassFile& class_file, const uint16_t interface) {
    if (!class_file.is_valid_index(interface)) {
        std::cerr << "The interface index is not a valid constant pool index." << std::endl;
        abort();
    }

    const auto constant_pool_info = class_file.constant_pool[interface - 1];
    if (constant_pool_info.tag != ConstantPoolInfo::CLASS) {
        std::cerr << "The interface is not a class constant pool info." << std::endl;
        abort();
    }
}

void VMCheck::visit_class_field(ClassFile& class_file, FieldInfo& field_info) {
    if (field_info.has_access_flag(FieldInfo::PUBLIC)) {
        if (field_info.has_access_flag(FieldInfo::PRIVATE) || field_info.has_access_flag(FieldInfo::PROTECTED)) {
            std::cerr << "The field has invalid public access flags." << std::endl;
            abort();
        }
    } else if (field_info.has_access_flag(FieldInfo::PRIVATE)) {
        if (field_info.has_access_flag(FieldInfo::PUBLIC) || field_info.has_access_flag(FieldInfo::PROTECTED)) {
            std::cerr << "The field has invalid private access flags." << std::endl;
            abort();
        }
    } else if (field_info.has_access_flag(FieldInfo::PROTECTED)) {
        if (field_info.has_access_flag(FieldInfo::PUBLIC) || field_info.has_access_flag(FieldInfo::PRIVATE)) {
            std::cerr << "The field has invalid protected access flags." << std::endl;
            abort();
        }
    }

    if (class_file.has_access_flag(ClassFile::INTERFACE)) {
        if (!field_info.has_access_flag(FieldInfo::PUBLIC) || !field_info.has_access_flag(FieldInfo::STATIC)
            || !field_info.has_access_flag(FieldInfo::FINAL)) {
            std::cerr << "Fields of interfaces need to have public, static and final access modifier set." << std::endl;
            abort();
        }
    }

    if (!class_file.is_valid_index(field_info.name_index)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    const auto field_name = class_file.constant_pool[field_info.name_index - 1];
    if (field_name.tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name is not a utf8 class pool info." << std::endl;
        abort();
    }

    if (!class_file.is_valid_index(field_info.descriptor_index)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }

    const auto field_descriptor = class_file.constant_pool[field_info.descriptor_index - 1];
    if (field_descriptor.tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The descriptor is not a utf8 class pool info." << std::endl;
        abort();
    }

    for (auto& attribute : field_info.attributes) {
        VMCheck::visit_field_attribute(class_file, field_info, attribute);
    }
}

// TODO: Do checks for the descriptor_index in
//  https://docs.oracle.com/javase/specs/jvms/se15/html/jvms-4.html#jvms-4.6
void VMCheck::visit_class_method(ClassFile& class_file, MethodInfo& method_info) {
    if (method_info.has_access_flag(MethodInfo::PUBLIC)) {
        if (method_info.has_access_flag(MethodInfo::PRIVATE)
            || method_info.has_access_flag(MethodInfo::PROTECTED)) {
            std::cerr << "The method has invalid public access flags." << std::endl;
            abort();
        }
    } else if (method_info.has_access_flag(MethodInfo::PRIVATE)) {
        if (method_info.has_access_flag(MethodInfo::PUBLIC)
            || method_info.has_access_flag(MethodInfo::PROTECTED)) {
            std::cerr << "The method has invalid private access flags." << std::endl;
            abort();
        }
    } else if (method_info.has_access_flag(MethodInfo::PROTECTED)) {
        if (method_info.has_access_flag(MethodInfo::PUBLIC)
            || method_info.has_access_flag(MethodInfo::PRIVATE)) {
            std::cerr << "The method has invalid protected access flags." << std::endl;
            abort();
        }
    }

    if (class_file.has_access_flag(ClassFile::INTERFACE)) {
        if (method_info.has_access_flag(MethodInfo::PROTECTED)
            || method_info.has_access_flag(MethodInfo::FINAL)
            || method_info.has_access_flag(MethodInfo::SYNCHRONIZED)
            || method_info.has_access_flag(MethodInfo::NATIVE)) {
            std::cerr << "The access flags for an interface methods are invalid." << std::endl;
            abort();
        }

        if (class_file.class_version < ClassFile::VERSION_8) {
            if (!method_info.has_access_flag(MethodInfo::PUBLIC)
                || !method_info.has_access_flag(MethodInfo::ABSTRACT)) {
                std::cerr << "The access flags for an interface methods are invalid." << std::endl;
                abort();
            }
        } else if (class_file.class_version >= ClassFile::VERSION_8) {
            if (method_info.has_access_flag(MethodInfo::PUBLIC)
                && method_info.has_access_flag(MethodInfo::PRIVATE)) {
                std::cerr << "The access flags for an interface methods are invalid." << std::endl;
                abort();
            }
        }
    }

    if (method_info.has_access_flag(MethodInfo::ABSTRACT)) {
        if (method_info.has_access_flag(MethodInfo::PRIVATE)
            || method_info.has_access_flag(MethodInfo::STATIC)
            || method_info.has_access_flag(MethodInfo::FINAL)
            || method_info.has_access_flag(MethodInfo::SYNCHRONIZED)
            || method_info.has_access_flag(MethodInfo::NATIVE)
            || method_info.has_access_flag(MethodInfo::STRICT)) {
            std::cerr << "The access flags for an interface methods are invalid." << std::endl;
            abort();
        }
    }

    if (!class_file.is_valid_index(method_info.name_index)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    const auto method_name = class_file.constant_pool[method_info.name_index - 1];
    if (method_name.tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 constant pool info." << std::endl;
        abort();
    }

    std::string name;
    name.assign(reinterpret_cast<char*>(method_name.info.utf8_info.bytes), method_name.info.utf8_info.length);
    if (name == "<init>") {
        if (method_info.has_access_flag(MethodInfo::ABSTRACT)
            || method_info.has_access_flag(MethodInfo::NATIVE)
            || method_info.has_access_flag(MethodInfo::BRIDGE)
            || method_info.has_access_flag(MethodInfo::SYNCHRONIZED)
            || method_info.has_access_flag(MethodInfo::FINAL)
            || method_info.has_access_flag(MethodInfo::STATIC)) {
            std::cerr << "The access flags for an interface methods are invalid."
                    << std::endl;
            abort();
        }
    }

    if (!class_file.is_valid_index(method_info.descriptor_index)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }

    const auto method_descriptor = class_file.constant_pool[method_info.descriptor_index - 1];
    if (method_descriptor.tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 constant pool info." << std::endl;
        abort();
    }

    for (auto& attribute_info : method_info.attributes) {
        VMCheck::visit_method_attribute(class_file, method_info, attribute_info);
    }
}

void VMCheck::visit_class_attribute(ClassFile& class_file, AttributeInfo& attribute_info) {
    if (!class_file.is_valid_index(attribute_info.attribute_name_index)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    const auto attribute_name = class_file.constant_pool[attribute_info.attribute_name_index - 1];
    if (attribute_name.tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 class pool info." << std::endl;
        abort();
    }
}

void VMCheck::visit_field_attribute(ClassFile& class_file, FieldInfo&, AttributeInfo& attribute_info) {
    VMCheck::visit_class_attribute(class_file, attribute_info);
}

void VMCheck::visit_method_attribute(ClassFile& class_file, MethodInfo&, AttributeInfo& attribute_info) {
    VMCheck::visit_class_attribute(class_file, attribute_info);
}

void VMCheck::visit_class_info(const ClassFile& class_file, const ConstantInfo::ClassInfo& info) {
    if (!class_file.is_valid_index(info.name_index)) {
        std::cout << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void VMCheck::visit_field_method_info(const ClassFile& class_file, const ConstantInfo::FieldMethodInfo& info) {
    if (!class_file.is_valid_index(info.class_index)) {
        std::cerr << "The class index is not a valid constant pool index." << std::endl;
        abort();
    }

    if (!class_file.is_valid_index(info.name_and_type_index)) {
        std::cerr << "The name and type index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void VMCheck::visit_name_and_type_info(const ClassFile& class_file, const ConstantInfo::NameAndTypeInfo& info) {
    if (!class_file.is_valid_index(info.name_index)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    if (!class_file.is_valid_index(info.descriptor_index)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void VMCheck::visit_string_info(const ClassFile& class_file, const ConstantInfo::StringInfo& info) {
    if (!class_file.is_valid_index(info.string_index)) {
        std::cerr << "The string index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void VMCheck::visit_method_type_info(const ClassFile& class_file, const ConstantInfo::MethodTypeInfo& info) {
    if (!class_file.is_valid_index(info.descriptor_index)) {
        std::cerr << "The descriptor index is not a valid constant pool index" << std::endl;
        abort();
    }
}

void VMCheck::visit_method_handle_info(const ClassFile& class_file, const ConstantInfo::MethodHandleInfo& info) {
    if (info.reference_kind < 1 || info.reference_kind > 9) {
        std::cerr << "The reference kind is not in range of 0 to 9." << std::endl;
        abort();
    }

    if (!class_file.is_valid_index(info.reference_index)) {
        std::cerr << "The reference index is not a valid constant pool index." << std::endl;
        abort();
    }

    const auto constant_pool_info = class_file.constant_pool[info.reference_index - 1];
    const auto reference_kind = info.reference_kind;

    if (reference_kind == ConstantInfo::MethodHandleKind::GetField
        || reference_kind == ConstantInfo::MethodHandleKind::GetStatic
        || reference_kind == ConstantInfo::MethodHandleKind::PutField
        || reference_kind == ConstantInfo::MethodHandleKind::PutStatic) {
        if (constant_pool_info.tag != ConstantPoolInfo::FIELD_REF) {
            std::cerr << "The reference index of the method handle needs to be a field ref." << std::endl;
            abort();
        }
    } else if (reference_kind == ConstantInfo::MethodHandleKind::InvokeVirtual
               || reference_kind == ConstantInfo::MethodHandleKind::NewInvokeSpecial) {
        if (constant_pool_info.tag != ConstantPoolInfo::FIELD_REF) {
            std::cerr << "The reference index of the method handle needs to be a method ref." << std::endl;
            abort();
        }
    } else if (reference_kind == ConstantInfo::MethodHandleKind::InvokeStatic
               || reference_kind == ConstantInfo::MethodHandleKind::InvokeSpecial) {
        if (class_file.class_version < ClassFile::VERSION_8
            && constant_pool_info.tag != ConstantPoolInfo::METHOD_REF) {
            std::cerr << "The reference index of the method handle needs to be a method ref." << std::endl;
            abort();
        }

        if (constant_pool_info.tag != ConstantPoolInfo::METHOD_REF
                   && constant_pool_info.tag != ConstantPoolInfo::INTERFACE_METHOD_REF) {
            std::cerr << "The reference index of the method handle needs to be a method ref or interface method ref." << std::endl;
            abort();
        }
    } else if (reference_kind == ConstantInfo::MethodHandleKind::InvokeInterface) {
        if (constant_pool_info.tag != ConstantPoolInfo::INTERFACE_METHOD_REF) {
            std::cerr << "The reference index of the method handle needs to be a interface method ref." << std::endl;
            abort();
        }
    }

    if (reference_kind == ConstantInfo::MethodHandleKind::InvokeVirtual
        || reference_kind == ConstantInfo::MethodHandleKind::InvokeStatic
        || reference_kind == ConstantInfo::MethodHandleKind::InvokeSpecial
        || reference_kind == ConstantInfo::MethodHandleKind::InvokeInterface
        || reference_kind == ConstantInfo::MethodHandleKind::NewInvokeSpecial) {
        const auto name_and_type = class_file.constant_pool[constant_pool_info.info.field_method_info.name_and_type_index - 1];
        const auto name_utf8 = class_file.constant_pool[name_and_type.info.name_and_type_info.name_index - 1];

        std::string name;
        name.assign(reinterpret_cast<char*>(name_utf8.info.utf8_info.bytes), name_utf8.info.utf8_info.length);

        if (reference_kind == ConstantInfo::MethodHandleKind::NewInvokeSpecial) {
            if (name != "<init>") {
                std::cerr << "The name of the method ref must be \"<init>\"" << std::endl;
                abort();
            }
        } else {
            if (name == "<init>" || name == "<clinit>") {
                std::cerr << "The name of the method ref can't be \"<init>\" or \"<clinit>\"." << std::endl;
                abort();
            }
        }
    }
}

// TODO: Check if the bootstrap method index if correct.
void VMCheck::visit_dynamic_info(const ClassFile& class_file, const ConstantInfo::DynamicInfo& info) {
    if (!class_file.is_valid_index(info.name_and_type_index)) {
        std::cerr << "The name and type index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void VMCheck::visit_module_package_info(const ClassFile& class_file, const ConstantInfo::ModulePackageInfo& info) {
    if (!class_file.is_valid_index(info.name_index)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//==============================================================================
