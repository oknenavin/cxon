// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_HXX_
#define CXON_CIO_HXX_

#include "io.hxx"

namespace cxon { namespace cio { // named parameters

    CXON_PARAMETER(fp_precision, int);      // write: constexpr: floating-points
    CXON_PARAMETER(num_len_max, unsigned);  // read: constexpr: numbers
    CXON_PARAMETER(ids_len_max, unsigned);  // read: constexpr: object key

}}

namespace cxon { namespace cio { // format traits

    struct format_traits {
        struct map {
            static constexpr char               beg     = '{';
            static constexpr char               end     = '}';
            static constexpr char               div     = ':';
            static constexpr char               sep     = ',';
        };
        struct list {
            static constexpr char               beg     = '[';
            static constexpr char               end     = ']';
            static constexpr char               sep     = ',';
        };
        struct string {
            static constexpr char               del     = '"';
            template <typename II>
                static bool del_read(II& i, II e) {
                    return i != e && *i == del && (++i, true);
                }
            template <typename O>
                static bool del_write(O& o) {
                    return poke(o, del);
                }
        };
        struct number {
            static constexpr bool               strict  = false;
        };
        struct id {
#           if __cplusplus >= 201703L
                static constexpr char           nil[]   = "null";
                static constexpr char           pos[]   = "true";
                static constexpr char           neg[]   = "false";
#           else
                static constexpr char const*    nil     = "null";
                static constexpr char const*    pos     = "true";
                static constexpr char const*    neg     = "false";
#           endif
        };
    };

    namespace key {
        template <typename T> struct traits;
    }
    template <typename X>
        using is_key = has_traits<key::traits<X>, X>;

}}

#endif // CXON_CIO_HXX_
