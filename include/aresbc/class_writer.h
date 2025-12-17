#pragma once

#include "visitor.h"

namespace aresbc {

class ClassWriter final : Visitor {
public:
    explicit ClassWriter(unsigned int offset = 0);

public:
    void visit_class(ClassFile& class_file) override;

    [[nodiscard]] auto byte_code() const -> const std::vector<uint8_t>&;

private:
    void visit_classpool_info(ClassFile& class_file, ConstantPoolInfo& info) override;

    void visit_class_interface(ClassFile& class_info, uint16_t interface) override;

    void visit_class_field(ClassFile& class_file, FieldInfo& field_info) override;

    void visit_class_method(ClassFile& class_file, MethodInfo& method_info) override;

    void visit_class_attribute(ClassFile& class_file, AttributeInfo& attribute_info) override;

    void visit_field_attribute(ClassFile& class_file, FieldInfo& field_info, AttributeInfo& attribute_info) override;

    void visit_method_attribute(ClassFile& class_file, MethodInfo& method_info, AttributeInfo& attribute_info) override;

private:
    auto write_u8(const uint8_t& data, const ClassFile& class_file) -> bool;

    auto write_u32(const uint32_t& data, const ClassFile& class_file) -> bool;

    auto write_u16(const uint16_t& data, const ClassFile& class_file) -> bool;

    auto write_u8_array(const uint8_t* data, unsigned int data_size, const ClassFile& class_file) -> bool;

private:
    unsigned int _offset{}, _size{};
    std::vector<uint8_t> _byte_code{};
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
