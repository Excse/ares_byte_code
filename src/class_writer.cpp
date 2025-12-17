#include "aresbc/class_writer.h"

#include <iostream>

#include "aresbc/attribute_info.h"
#include "aresbc/constant_info.h"
#include "aresbc/field_info.h"
#include "aresbc/utils.h"

using namespace aresbc;

ClassWriter::ClassWriter(const unsigned int offset) :
    _offset(offset) {
}

void ClassWriter::visit_class(ClassFile& class_file) {
    _size = class_file.size();
    _byte_code = std::vector<uint8_t>(_size);

    write_u32(class_file.magic_number, class_file);
    write_u16(class_file.minor_version, class_file);
    write_u16(class_file.major_version, class_file);

    write_u16(class_file.constant_pool_count, class_file);
    for (auto& constantPoolInfo : class_file.constant_pool) {
        ClassWriter::visit_classpool_info(class_file, constantPoolInfo);
    }

    write_u16(class_file.access_flags, class_file);
    write_u16(class_file.this_class, class_file);
    write_u16(class_file.super_class, class_file);

    write_u16(class_file.interfaces_count, class_file);
    for (auto index = 0; index < class_file.interfaces_count; index++) {
        write_u16(class_file.interfaces[index], class_file);
    }

    write_u16(class_file.fields_count, class_file);
    for (auto& field_info : class_file.fields) {
        ClassWriter::visit_class_field(class_file, field_info);
    }

    write_u16(class_file.method_count, class_file);
    for (auto& method_info : class_file.methods) {
        ClassWriter::visit_class_method(class_file, method_info);
    }

    write_u16(class_file.attributes_count, class_file);
    for (auto& attribute_info : class_file.attributes) {
        ClassWriter::visit_class_attribute(class_file, attribute_info);
    }
}

void ClassWriter::visit_classpool_info(ClassFile& class_file, ConstantPoolInfo& info) {
    write_u8(reinterpret_cast<uint8_t&>(info.tag), class_file);

    switch (info.tag) {
        case ConstantPoolInfo::CLASS: {
            write_u16(info.info.class_info.name_index, class_file);
            break;
        }
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF: {
            write_u16(info.info.field_method_info.class_index, class_file);
            write_u16(info.info.field_method_info.name_and_type_index, class_file);
            break;
        }
        case ConstantPoolInfo::STRING: {
            write_u16(info.info.string_info.string_index, class_file);
            break;
        }
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::INTEGER: {
            write_u32(info.info.integer_float_info.bytes, class_file);
            break;
        }
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE: {
            write_u32(info.info.long_double_info.high_bytes, class_file);
            write_u32(info.info.long_double_info.low_bytes, class_file);
            break;
        }
        case ConstantPoolInfo::NAME_AND_TYPE: {
            write_u16(info.info.name_and_type_info.name_index, class_file);
            write_u16(info.info.name_and_type_info.descriptor_index, class_file);
            break;
        }
        case ConstantPoolInfo::UTF_8: {
            write_u16(info.info.utf8_info.length, class_file);
            write_u8_array(info.info.utf8_info.bytes, info.info.utf8_info.length, class_file);
            break;
        }
        case ConstantPoolInfo::METHOD_HANDLE: {
            write_u8(info.info.method_handle_info.reference_kind, class_file);
            write_u16(info.info.method_handle_info.reference_index, class_file);
            break;
        }
        case ConstantPoolInfo::METHOD_TYPE: {
            write_u16(info.info.method_type_info.descriptor_index, class_file);
            break;
        }
        case ConstantPoolInfo::INVOKE_DYNAMIC:
        case ConstantPoolInfo::DYNAMIC: {
            write_u16(info.info.dynamic_info.boostrap_method_attr_index, class_file);
            write_u16(info.info.dynamic_info.name_and_type_index, class_file);
            break;
        }
        case ConstantPoolInfo::PACKAGE:
        case ConstantPoolInfo::MODULE: {
            write_u16(info.info.module_package_info.name_index, class_file);
            break;
        }
        case ConstantPoolInfo::UNDEFINED: {
            break;
        }
    }
}

void ClassWriter::visit_class_interface(ClassFile&, uint16_t) {
}

void ClassWriter::visit_class_field(ClassFile& class_file, FieldInfo& field_info) {
    write_u16(field_info.access_flags, class_file);
    write_u16(field_info.name_index, class_file);
    write_u16(field_info.descriptor_index, class_file);

    write_u16(field_info.attributes_count, class_file);
    for (auto& attribute_info : field_info.attributes) {
        ClassWriter::visit_field_attribute(class_file, field_info, attribute_info);
    }
}

void ClassWriter::visit_class_method(ClassFile& class_file, MethodInfo& method_info) {
    write_u16(method_info.access_flags, class_file);
    write_u16(method_info.name_index, class_file);
    write_u16(method_info.descriptor_index, class_file);

    write_u16(method_info.attributes_count, class_file);
    for (auto& attribute_info : method_info.attributes) {
        ClassWriter::visit_method_attribute(class_file, method_info, attribute_info);
    }
}

void ClassWriter::visit_class_attribute(ClassFile& class_file, AttributeInfo& attribute_info) {
    write_u16(attribute_info.attribute_name_index, class_file);
    write_u32(attribute_info.attribute_length, class_file);

    for (size_t index = 0; index < attribute_info.attribute_length; index++) {
        write_u8(attribute_info.info[index], class_file);
    }
}

void ClassWriter::visit_field_attribute(ClassFile& class_file, FieldInfo&, AttributeInfo& attribute_info) {
    ClassWriter::visit_class_attribute(class_file, attribute_info);
}

void ClassWriter::visit_method_attribute(ClassFile& class_file, MethodInfo&, AttributeInfo& attribute_info) {
    ClassWriter::visit_class_attribute(class_file, attribute_info);
}

auto ClassWriter::byte_code() const -> const std::vector<uint8_t>& {
    return _byte_code;
}

auto ClassWriter::write_u8(const uint8_t& data, const ClassFile& class_file) -> bool {
    if (_offset + 1 > class_file.byte_code.size()) {
        std::cerr << "Couldn't write u8 because it is out of bounds." << std::endl;
        return false;
    }

    _byte_code[_offset++] = data & 0xFF;

    return true;
}

auto ClassWriter::write_u16(const uint16_t& data, const ClassFile& class_file) -> bool {
    if (_offset + 2 > class_file.byte_code.size()) {
        std::cerr << "Couldn't write u16 because it is out of bounds." << std::endl;
        return false;
    }

    _byte_code[_offset++] = (data >> 8) & 0xFF;
    _byte_code[_offset++] = data & 0xFF;

    return true;
}

auto ClassWriter::write_u32(const uint32_t& data, const ClassFile& class_file) -> bool {
    if (_offset + 4 > class_file.byte_code.size()) {
        std::cerr << "Couldn't read u32 because it is out of bounds." << std::endl;
        return false;
    }

    _byte_code[_offset++] = (data >> 24) & 0xFF;
    _byte_code[_offset++] = (data >> 16) & 0xFF;
    _byte_code[_offset++] = (data >> 8) & 0xFF;
    _byte_code[_offset++] = data & 0xFF;

    return true;
}

auto ClassWriter::write_u8_array(const uint8_t* data, const unsigned int data_size, const ClassFile& class_file) -> bool {
    if ((_offset + data_size) > class_file.byte_code.size()) {
        std::cerr << "Couldn't write the u8 array because it is out of bounds." << std::endl;
        return false;
    }

    for (size_t index = 0; index < data_size; index++) {
        write_u8(data[index], class_file);
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
