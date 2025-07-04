#pragma once

#include <memory>
#include <vector>

namespace aresbc {

class ConstantPoolInfo;

struct AttributeInfo;

class MethodInfo;

class FieldInfo;

class ClassFile {
public:
    enum AccessFlag : uint16_t {
        PUBLIC = 0x0001,
        FINAL = 0x0010,
        SUPER = 0x0020,
        INTERFACE = 0x0200,
        ABSTRACT = 0x0400,
        SYNTHETIC = 0x1000,
        ANNOTATION = 0x2000,
        ENUM = 0x4000,
        MODULE = 0x8000,
    };

    enum ClassVersion : uint16_t {
        UNDEFINED = 0,
        VERSION_1_1 = 45,
        VERSION_1_2 = 46,
        VERSION_1_3 = 47,
        VERSION_1_4 = 48,
        VERSION_5 = 49,
        VERSION_6 = 50,
        VERSION_7 = 51,
        VERSION_8 = 52,
        VERSION_9 = 53,
        VERSION_10 = 54,
        VERSION_11 = 55,
        VERSION_12 = 56,
        VERSION_13 = 57,
        VERSION_14 = 58,
        VERSION_15 = 59,
    };

public:
    [[nodiscard]] auto is_valid_index(unsigned int index) const -> bool;

    [[nodiscard]] auto has_access_flag(AccessFlag access_flag) const -> bool;

    [[nodiscard]] auto size() const -> unsigned int;

public:
    std::vector<uint8_t> byte_code{};

    uint32_t magic_number{};
    uint16_t minor_version{};
    uint16_t major_version{};
    ClassVersion class_version{};
    uint16_t constant_pool_count{};
    std::vector <ConstantPoolInfo> constant_pool{};
    uint16_t access_flags{};
    uint16_t this_class{};
    uint16_t super_class{};
    uint16_t interfaces_count{};
    std::vector <uint16_t> interfaces{};
    uint16_t fields_count{};
    std::vector <FieldInfo> fields{};
    uint16_t method_count{};
    std::vector <MethodInfo> methods{};
    uint16_t attributes_count{};
    std::vector <AttributeInfo> attributes{};
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