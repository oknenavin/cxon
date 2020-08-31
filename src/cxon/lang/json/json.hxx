// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_JSON_HXX_
#define CXON_JSON_JSON_HXX_

#ifndef CXON_DEFAULT_FORMAT
#   define CXON_DEFAULT_FORMAT JSON<>
#endif

#include "cxon/cxon.hxx"

#include "fundamental.hxx"
#include "enum.hxx"
#include "compound.hxx"
#include "class.hxx"

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace json { // format traits

    struct format_traits : chio::format_traits {
        static constexpr bool strict_js = false; // escape U+2028 LINE SEPARATOR and U+2029 PARAGRAPH SEPARATOR are invalid JavaScript
    };

}}

namespace cxon { // format selector

    template <typename T>
        struct JSON : format_selector<T> {};

}

namespace cxon { namespace json { // named parameters

    using fp_precision = chio::fp_precision;    // write: constexpr: floating-points
    using allocator = chio::allocator;          // read: T*
    using num_len_max = chio::num_len_max;      // read: constexpr: numbers
    using ids_len_max = chio::ids_len_max;      // read: constexpr: object key

}}

namespace cxon { namespace chio { namespace chr { // character classes

    template <typename X>
        struct is<JSON<X>> : is<X> {
            static bool space(char c) noexcept { return JSSP & is_x_[(unsigned char)c]; }
        };

}}}

#endif // CXON_JSON_JSON_HXX_
