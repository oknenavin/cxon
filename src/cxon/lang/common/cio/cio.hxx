// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_HXX_
#define CXON_CIO_HXX_

#include "io.hxx"

namespace cxon { namespace cio { // named parameters

    CXON_PARAMETER(fp_precision, int);                  // write: constexpr: floating-points
    CXON_PARAMETER(allocator, std::allocator<char>);    // read: T*
    CXON_PARAMETER(num_len_max, unsigned);              // read: constexpr: numbers
    CXON_PARAMETER(ids_len_max, unsigned);              // read: constexpr: object key

}}

namespace cxon { namespace cio { // format traits

    struct format_traits {
        struct map {
            static constexpr char           beg             = '{';
            static constexpr char           end             = '}';
            static constexpr char           div             = ':';
            static constexpr char           sep             = ',';
            static constexpr bool           unquoted_keys   = false;
        };
        struct list {
            static constexpr char           beg             = '[';
            static constexpr char           end             = ']';
            static constexpr char           sep             = ',';
        };
        struct string {
            static constexpr char           beg             = '"';
            static constexpr char           end             = '"';
        };
        struct number {
            static constexpr bool           strict          = false;
        };
        struct id {
            static constexpr char const*    nil             = "null";
            static constexpr char const*    pos             = "true";
            static constexpr char const*    neg             = "false";
        };
    };

}}

#endif // CXON_CIO_HXX_
