#pragma once

#include <memory>

#include "attribute_info.h"
#include "constant_info.h"
#include "class_file.h"
#include "visitor.h"

namespace aresbc {

class ClassReader final : Visitor {
public:
    explicit ClassReader(unsigned int offset = 0u);

public:
    void visit_class(ClassFile &class_info) override;

    [[nodiscard]] auto offset() const -> unsigned int;

private:
    void read_class_attributes(ClassFile &class_info);

    void visit_class_attribute(ClassFile &class_info, AttributeInfo &attribute_info) override;

    void read_magic_number(ClassFile &class_info);

    void read_class_version(ClassFile &class_info);

    void read_constant_pool(ClassFile &class_info);

    void visit_classpool_info(ClassFile &class_info, ConstantPoolInfo &info) override;

    void read_class_info(ClassFile &class_info, ConstantInfo::ClassInfo &info);

    void read_utf8_info(ClassFile &class_info, ConstantInfo::UTF8Info &info);

    void read_field_method_info(ClassFile &class_info, ConstantInfo::FieldMethodInfo &info);

    void read_name_and_type(ClassFile &class_info, ConstantInfo::NameAndTypeInfo &info);

    void read_string_info(ClassFile &class_info, ConstantInfo::StringInfo &info);

    void read_double_long(ClassFile &class_info, ConstantInfo::DoubleLongInfo &info);

    void read_float_integer(ClassFile &class_info, ConstantInfo::FloatIntegerInfo &info);

    void read_method_type(ClassFile &class_info, ConstantInfo::MethodTypeInfo &info);

    void read_method_handle(ClassFile &class_info, ConstantInfo::MethodHandleInfo &info);

    void read_dynamic(ClassFile &class_info, ConstantInfo::DynamicInfo &info);

    void read_module_package(ClassFile &class_info, ConstantInfo::ModulePackageInfo &info);

    void read_access_flags(ClassFile &class_info);

    void read_this_class(ClassFile &class_info);

    void read_super_class(ClassFile &class_info);

    void read_interfaces(ClassFile &class_info);

    void visit_class_interface(ClassFile &class_info, uint16_t interface) override;

    void read_fields(ClassFile &class_info);

    void visit_class_field(ClassFile &class_info, FieldInfo &field_info) override;

    void read_field_attributes(ClassFile &class_info, FieldInfo &field_info);

    void visit_field_attribute(ClassFile &class_info, FieldInfo &field_info, AttributeInfo &attribute_info) override;

    void read_methods(ClassFile &class_info);

    void visit_class_method(ClassFile &class_info, MethodInfo &method_info) override;

    void read_method_attributes(ClassFile &class_info, MethodInfo &method_info);

    void visit_method_attribute(ClassFile &class_info, MethodInfo &method_info, AttributeInfo &attribute_info) override;

private:
    auto read_u8(uint8_t &data, ClassFile &class_info) -> bool;

    auto read_u16(uint16_t &data, ClassFile &class_info) -> bool;

    auto read_u32(uint32_t &data, ClassFile &class_info) -> bool;

    auto read_u8_array(uint8_t *data, unsigned int length, ClassFile &class_info) -> bool;

private:
    unsigned int _offset{};
};

} // namespace ares

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