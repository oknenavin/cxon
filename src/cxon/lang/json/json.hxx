// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_JSON_HXX_
#define CXON_JSON_JSON_HXX_

namespace cxon { // format selector fwd
    namespace json {
        struct format_traits;
    }
    template <typename T = json::format_traits>
        struct JSON;
}
#ifndef CXON_DEFAULT_FORMAT
#   define CXON_DEFAULT_FORMAT JSON<>
#endif

#include "cxon/cxon.hxx"

#include "fundamental.hxx"
#include "enum.hxx"
#include "compound.hxx"
#include "struct.hxx"

namespace cxon { // format selector
    template <typename T>
        struct JSON : format_selector<T> {};
}

namespace cxon { namespace json { // format traits
    struct format_traits : chio::format_traits {};
}}

namespace cxon { namespace json { // errors
    using read_error = chio::read_error;
    using read_error_category = chio::read_error_category;
    using write_error = chio::write_error;
    using write_error_category = chio::write_error_category;
}}

namespace cxon { namespace json { // context parameters
    using allocator = chio::allocator;          // read: T*
    using num_len_max = chio::num_len_max;      // read: constexpr: numbers
    using ids_len_max = chio::ids_len_max;      // read: constexpr: object key
    using fp_precision = chio::fp_precision;    // write: constexpr: floating-points
}}

#endif // CXON_JSON_JSON_HXX_
