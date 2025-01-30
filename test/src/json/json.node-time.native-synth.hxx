// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_NATIVE_SYNTH_HXX_
#define CXON_JSON_TEST_NATIVE_SYNTH_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/utility.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/deque.hxx"


namespace test { namespace synth {

    using object_u8 = std::vector<unsigned char>;
    using object_s8 = std::vector<signed char>;
    using object_u16 = std::vector<std::uint16_t>;
    using object_s16 = std::vector<std::int16_t>;
    using object_u32 = std::vector<std::uint32_t>;
    using object_s32 = std::vector<std::int32_t>;
    using object_u64 = std::vector<std::uint64_t>;
    using object_s64 = std::vector<std::int64_t>;
    using object_fp32 = std::vector<float>;
    using object_fp64 = std::vector<double>;
    using object_null = std::vector<std::nullptr_t>;
    using object_bool = std::deque<bool>; // std::vector<bool> is a bit slow
    using object_string = std::vector<std::string>;
    using object_array = std::vector<std::pair<std::nullptr_t, std::nullptr_t>>;

    struct point_2d {
        std::nullptr_t x, y;

        CXON_JSON_CLS_MEMBER(point_2d, CXON_JSON_CLS_FIELD_ASIS(x), CXON_JSON_CLS_FIELD_ASIS(y))
    };
    using object_object = std::vector<point_2d>;

}}

#endif // CXON_JSON_TEST_NATIVE_SYNTH_HXX_
