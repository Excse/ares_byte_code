#include "aresbc/class_reader.h"

#include <iostream>

#include "aresbc/field_info.h"
#include "aresbc/utils.h"

using namespace aresbc;

#define CHECKED_READ(size, target, error_message)           \
    if(!ClassReader::read_##size(target, class_file)) {     \
        std::cerr << error_message << std::endl;            \
        abort();                                            \
    }

#define CHECKED_ARRAY_READ(size, target, length, error_message)             \
    if(!ClassReader::read_##size##_array(target, length, class_file)) {     \
        std::cerr << error_message << std::endl;                            \
        abort();                                                            \
    }

ClassReader::ClassReader(unsigned int offset) :
    _offset(offset) {
}

void ClassReader::visit_class(ClassFile& class_file) {
    read_magic_number(class_file);
    read_class_version(class_file);
    read_constant_pool(class_file);
    read_access_flags(class_file);
    read_this_class(class_file);
    read_super_class(class_file);
    read_interfaces(class_file);
    read_fields(class_file);
    read_methods(class_file);
    read_class_attributes(class_file);
}

void ClassReader::read_class_attributes(ClassFile& class_file) {
    CHECKED_READ(u16, class_file.attributes_count, "Couldn't read the attribute count.")

    class_file.attributes = std::vector<AttributeInfo>(class_file.attributes_count);

    for (auto& attribute_info : class_file.attributes) {
        ClassReader::visit_class_attribute(class_file, attribute_info);
    }
}

void ClassReader::visit_class_attribute(ClassFile& class_file, AttributeInfo& attribute_info) {
    CHECKED_READ(u16, attribute_info.attribute_name_index, "Couldn't read the name index.")
    CHECKED_READ(u32, attribute_info.attribute_length, "Couldn't read the length.")

    attribute_info.info = new uint8_t[attribute_info.attribute_length];
    CHECKED_ARRAY_READ(u8, attribute_info.info, attribute_info.attribute_length, "Couldn't read the info.")
}

void ClassReader::read_magic_number(ClassFile& class_file) {
    CHECKED_READ(u32, class_file.magic_number, "Couldn't read the magic number.")
}

void ClassReader::read_class_version(ClassFile& class_file) {
    CHECKED_READ(u16, class_file.minor_version, "Couldn't read the minor version.")
    CHECKED_READ(u16, class_file.major_version, "Couldn't read the major version.")

    class_file.class_version = ClassFile::UNDEFINED;
    if (class_file.major_version >= ClassFile::VERSION_1_1 && class_file.major_version <= ClassFile::VERSION_15) {
        class_file.class_version = ClassFile::ClassVersion(class_file.major_version);
    }
}

void ClassReader::read_constant_pool(ClassFile& class_file) {
    CHECKED_READ(u16, class_file.constant_pool_count, "Couldn't read the constant pool count.")

    class_file.constant_pool = std::vector<ConstantPoolInfo>(class_file.constant_pool_count - 1);

    for (auto index = 0; index < class_file.constant_pool_count - 1; index++) {
        auto& info = class_file.constant_pool[index];
        ClassReader::visit_classpool_info(class_file, info);

        if (info.tag == ConstantPoolInfo::DOUBLE || info.tag == ConstantPoolInfo::LONG) {
            index++;
        }
    }
}

void ClassReader::visit_classpool_info(ClassFile& class_file, ConstantPoolInfo& info) {
    uint8_t infoTag{};
    CHECKED_READ(u8, infoTag, "Couldn't read the tag.")

    if (infoTag >= ConstantPoolInfo::UTF_8 && infoTag <= ConstantPoolInfo::PACKAGE && infoTag != 13 && infoTag != 14) {
        info.tag = ConstantPoolInfo::ConstantTag(infoTag);
    } else {
        info.tag = ConstantPoolInfo::UNDEFINED;
    }

    switch (info.tag) {
        case ConstantPoolInfo::CLASS: {
            read_class_info(class_file, info.info.class_info);
            break;
        }
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF: {
            read_field_method_info(class_file, info.info.field_method_info);
            break;
        }
        case ConstantPoolInfo::STRING: {
            read_string_info(class_file, info.info.string_info);
            break;
        }
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::INTEGER: {
            read_float_integer(class_file, info.info.integer_float_info);
            break;
        }
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE: {
            read_double_long(class_file, info.info.long_double_info);
            break;
        }
        case ConstantPoolInfo::NAME_AND_TYPE: {
            read_name_and_type(class_file, info.info.name_and_type_info);
            break;
        }
        case ConstantPoolInfo::UTF_8: {
            read_utf8_info(class_file, info.info.utf8_info);
            break;
        }
        case ConstantPoolInfo::METHOD_HANDLE: {
            read_method_handle(class_file, info.info.method_handle_info);
            break;
        }
        case ConstantPoolInfo::METHOD_TYPE: {
            read_method_type(class_file, info.info.method_type_info);
            break;
        }
        case ConstantPoolInfo::INVOKE_DYNAMIC:
        case ConstantPoolInfo::DYNAMIC: {
            read_dynamic(class_file, info.info.dynamic_info);
            break;
        }
        case ConstantPoolInfo::PACKAGE:
        case ConstantPoolInfo::MODULE: {
            read_module_package(class_file, info.info.module_package_info);
            break;
        }

        default: throw std::runtime_error("Unknown constant pool tag encountered");
    }
}

void ClassReader::read_class_info(ClassFile& class_file, ConstantInfo::ClassInfo& info) {
    CHECKED_READ(u16, info.name_index, "Couldn't read the name index.")
}

void ClassReader::read_utf8_info(ClassFile& class_file, ConstantInfo::UTF8Info& info) {
    CHECKED_READ(u16, info.length, "Couldn't read the length.")

    info.bytes = new uint8_t[info.length];
    CHECKED_ARRAY_READ(u8, info.bytes, info.length, "Couldn't read the bytes.")
}

void ClassReader::read_field_method_info(ClassFile& class_file, ConstantInfo::FieldMethodInfo& info) {
    CHECKED_READ(u16, info.class_index, "Couldn't read the class index.")
    CHECKED_READ(u16, info.name_and_type_index, "Couldn't read the name and type index.")
}

void ClassReader::read_name_and_type(ClassFile& class_file, ConstantInfo::NameAndTypeInfo& info) {
    CHECKED_READ(u16, info.name_index, "Couldn't read the name index.")
    CHECKED_READ(u16, info.descriptor_index, "Couldn't read the descriptor index.")
}

void ClassReader::read_string_info(ClassFile& class_file, ConstantInfo::StringInfo& info) {
    CHECKED_READ(u16, info.string_index, "Couldn't read the string index.")
}

void ClassReader::read_double_long(ClassFile& class_file, ConstantInfo::DoubleLongInfo& info) {
    CHECKED_READ(u32, info.high_bytes, "Couldn't read the high bytes.")
    CHECKED_READ(u32, info.low_bytes, "Couldn't read the low bytes.")
}

void ClassReader::read_float_integer(ClassFile& class_file, ConstantInfo::FloatIntegerInfo& info) {
    CHECKED_READ(u32, info.bytes, "Couldn't read the bytes.")
}

void ClassReader::read_method_type(ClassFile& class_file, ConstantInfo::MethodTypeInfo& info) {
    CHECKED_READ(u16, info.descriptor_index, "Couldn't read the descriptor index.")
}

void ClassReader::read_method_handle(ClassFile& class_file, ConstantInfo::MethodHandleInfo& info) {
    CHECKED_READ(u8, info.reference_kind, "Couldn't read the reference kind.")
    CHECKED_READ(u16, info.reference_index, "Couldn't read the reference index.")
}

void ClassReader::read_dynamic(ClassFile& class_file, ConstantInfo::DynamicInfo& info) {
    CHECKED_READ(u16, info.boostrap_method_attr_index, "Couldn't read the bootstrap method attribute index.")
    CHECKED_READ(u16, info.name_and_type_index, "Couldn't read the name and type index.")
}

void ClassReader::read_module_package(ClassFile& class_file, ConstantInfo::ModulePackageInfo& info) {
    CHECKED_READ(u16, info.name_index, "Couldn't read the name index.")
}

void ClassReader::read_access_flags(ClassFile& class_file) {
    CHECKED_READ(u16, class_file.access_flags, "Couldn't read the access flags.")
}

void ClassReader::read_this_class(ClassFile& class_file) {
    CHECKED_READ(u16, class_file.this_class, "Couldn't read the \"this class\".")
}

void ClassReader::read_super_class(ClassFile& class_file) {
    CHECKED_READ(u16, class_file.super_class, "Couldn't read the \"super class\".")
}

void ClassReader::read_interfaces(ClassFile& class_file) {
    CHECKED_READ(u16, class_file.interfaces_count, "Couldn't read the interface count.")

    class_file.interfaces = std::vector<uint16_t>(class_file.interfaces_count);

    for (auto& interface : class_file.interfaces) {
        CHECKED_READ(u16, interface, "Couldn't read the interface.")
    }
}

void ClassReader::visit_class_interface(ClassFile&, uint16_t) {
}

void ClassReader::read_fields(ClassFile& class_file) {
    CHECKED_READ(u16, class_file.fields_count, "Couldn't read the field count.")

    class_file.fields = std::vector<FieldInfo>(class_file.fields_count);

    for (auto& field_info : class_file.fields) {
        ClassReader::visit_class_field(class_file, field_info);
    }
}

void ClassReader::visit_class_field(ClassFile& class_file, FieldInfo& field_info) {
    CHECKED_READ(u16, field_info.access_flags, "Couldn't read the access flags.")
    CHECKED_READ(u16, field_info.name_index, "Couldn't read the name index.")
    CHECKED_READ(u16, field_info.descriptor_index, "Couldn't read the descriptor index.")

    read_field_attributes(class_file, field_info);
}

void ClassReader::read_field_attributes(ClassFile& class_file, FieldInfo& field_info) {
    CHECKED_READ(u16, field_info.attributes_count, "Couldn't read the attribute count.")

    field_info.attributes = std::vector<AttributeInfo>(field_info.attributes_count);

    for (auto& attribute_info : field_info.attributes) {
        ClassReader::visit_field_attribute(class_file, field_info, attribute_info);
    }
}

void ClassReader::visit_field_attribute(ClassFile& class_file, FieldInfo&, AttributeInfo& attribute_info) {
    ClassReader::visit_class_attribute(class_file, attribute_info);
}

void ClassReader::read_methods(ClassFile& class_file) {
    CHECKED_READ(u16, class_file.method_count, "Couldn't read the method count.")

    class_file.methods = std::vector<MethodInfo>(class_file.method_count);

    for (auto& method_info : class_file.methods) {
        ClassReader::visit_class_method(class_file, method_info);
    }
}

void ClassReader::visit_class_method(ClassFile& class_file, MethodInfo& method_info) {
    CHECKED_READ(u16, method_info.access_flags, "Couldn't read the access flags.")
    CHECKED_READ(u16, method_info.name_index, "Couldn't read the name index.")
    CHECKED_READ(u16, method_info.descriptor_index, "Couldn't read the descriptor index.")

    read_method_attributes(class_file, method_info);
}

void ClassReader::read_method_attributes(ClassFile& class_file, MethodInfo& method_info) {
    CHECKED_READ(u16, method_info.attributes_count, "Couldn't read the attribute count.")

    method_info.attributes = std::vector<AttributeInfo>(method_info.attributes_count);

    for (auto& attribute_info : method_info.attributes) {
        ClassReader::visit_method_attribute(class_file, method_info, attribute_info);
    }
}

void ClassReader::visit_method_attribute(ClassFile& class_file, MethodInfo&, AttributeInfo& attribute_info) {
    ClassReader::visit_class_attribute(class_file, attribute_info);
}

auto ClassReader::offset() const -> unsigned int {
    return _offset;
}

auto ClassReader::read_u8(uint8_t& data, ClassFile& class_file) -> bool {
    if (_offset + 1 > class_file.byte_code.size()) {
        std::cerr << "Couldn't read u8 because it is out of bounds." << std::endl;
        return false;
    }

    data = static_cast<uint32_t>(class_file.byte_code[_offset + 0]);
    _offset += 1;

    return true;
}

auto ClassReader::read_u16(uint16_t& data, ClassFile& class_file) -> bool {
    if (_offset + 2 > class_file.byte_code.size()) {
        std::cerr << "Couldn't read u16 because it is out of bounds." << std::endl;
        return false;
    }

    data = (static_cast<uint32_t>(class_file.byte_code[_offset + 0]) << 8) |
           (static_cast<uint32_t>(class_file.byte_code[_offset + 1]));
    _offset += 2;

    return true;
}

auto ClassReader::read_u32(uint32_t& data, ClassFile& class_file) -> bool {
    if (_offset + 4 > class_file.byte_code.size()) {
        std::cerr << "Couldn't read u32 because it is out of bounds." << std::endl;
        return false;
    }

    data = (static_cast<uint32_t>(class_file.byte_code[_offset + 0]) << 24) |
           (static_cast<uint32_t>(class_file.byte_code[_offset + 1]) << 16) |
           (static_cast<uint32_t>(class_file.byte_code[_offset + 2]) << 8) |
           (static_cast<uint32_t>(class_file.byte_code[_offset + 3]));
    _offset += 4;

    return true;
}

auto ClassReader::read_u8_array(uint8_t* data, unsigned int length, ClassFile& class_file) -> bool {
    if ((_offset + length) > class_file.byte_code.size()) {
        std::cerr << "Couldn't read the u8 array because it is out of bounds." << std::endl;
        return false;
    }

    for (size_t index = 0; index < length; index++) {
        read_u8(data[index], class_file);
    }

    return true;
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
