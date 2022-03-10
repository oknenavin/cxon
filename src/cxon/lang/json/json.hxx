// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_JSON_HXX_
#define CXON_JSON_JSON_HXX_

#include "error.hxx"
#include "fundamental.hxx"
#include "compound.hxx"
#include "enum.hxx"
#include "class.hxx"

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace json { // format traits

    struct format_traits : cio::format_traits {
        using read_error = json::read_error;
        using write_error = json::write_error;

        static constexpr bool read_validate_string_utf8 = true; // validate input strings (utf-8)
        static constexpr bool read_validate_string_ctrl = true; // validate input strings (unescaped control characters)

        static constexpr bool write_strict_js = false;          // escape, U+2028 LINE SEPARATOR and U+2029 PARAGRAPH SEPARATOR are invalid JavaScript
    };

}}

namespace cxon { // format selector

    template <typename T>
        struct JSON : format_selector<T> {};

}

namespace cxon { namespace json { // named parameters

    using fp_precision = cio::fp_precision;     // write: constexpr: floating-points
    using allocator = allocator;                // read: T*
    using num_len_max = cio::num_len_max;       // read: constexpr: numbers
    using ids_len_max = cio::ids_len_max;       // read: constexpr: object key

}}

namespace cxon { namespace cio { namespace chr { // character classes

    template <typename X>
        struct is<JSON<X>> : is<X> {
            static bool space(char c) noexcept { return JSSP & is_x_[(unsigned char)c]; }
        };

}}}

#endif // CXON_JSON_JSON_HXX_
