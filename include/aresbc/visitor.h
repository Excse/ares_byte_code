#pragma once

#include "attribute_info.h"
#include "method_info.h"
#include "class_file.h"

namespace aresbc {

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit_class(ClassFile& class_file) = 0;

    virtual void visit_classpool_info(ClassFile& class_file, ConstantPoolInfo& constantPoolInfo) = 0;

    virtual void visit_class_interface(ClassFile& class_file, uint16_t interface) = 0;

    virtual void visit_class_field(ClassFile& class_file, FieldInfo& field_info) = 0;

    virtual void visit_class_method(ClassFile& class_file, MethodInfo& method_info) = 0;

    virtual void visit_class_attribute(ClassFile& class_file, AttributeInfo& attribute_info) = 0;

    virtual void visit_field_attribute(ClassFile& class_file, FieldInfo& field_info, AttributeInfo& attribute_info) = 0;

    virtual void visit_method_attribute(ClassFile& class_file, MethodInfo& method_info, AttributeInfo& attribute_info) = 0;
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
