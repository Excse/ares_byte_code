#include "aresbc/class_file.h"

#include "aresbc/attribute_info.h"
#include "aresbc/constant_info.h"
#include "aresbc/method_info.h"
#include "aresbc/field_info.h"

using namespace aresbc;

auto ClassFile::is_valid_index(unsigned int index) const -> bool {
    return index > 0 && index < constant_pool_count;
}

auto ClassFile::has_access_flag(AccessFlag access_flag) const -> bool {
    return access_flags & access_flag;
}

auto ClassFile::size() const -> unsigned int {
    size_t size = 24 + 2 * interfaces_count;
    for (const auto& constant_info : constant_pool) {
        size += constant_info.size();
    }
    for (const auto& field_info : fields) {
        size += field_info.size();
    }
    for (const auto& method_info : methods) {
        size += method_info.size();
    }
    for (const auto& attribute_info : attributes) {
        size += attribute_info.size();
    }
    return size;
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
