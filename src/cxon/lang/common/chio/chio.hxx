// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHIO_HXX_
#define CXON_CHIO_HXX_

#include "io.hxx"

namespace cxon { namespace chio { // context parameters

    CXON_PARAMETER(allocator, std::allocator<char>);    // read: T*
    CXON_PARAMETER(num_len_max, unsigned);              // read: constexpr: numbers
    CXON_PARAMETER(ids_len_max, unsigned);              // read: constexpr: object key
    CXON_PARAMETER(fp_precision, int);                  // write: constexpr: floating-points

}}

namespace cxon { namespace chio { // format traits

    struct format_traits {
        struct map {
            static constexpr char                   beg             = '{';
            static constexpr char                   end             = '}';
            static constexpr char                   div             = ':';
            static constexpr char                   sep             = ',';
            static constexpr bool                   unquoted_keys   = false;
        };
        struct list {
            static constexpr char                   beg             = '[';
            static constexpr char                   end             = ']';
            static constexpr char                   sep             = ',';
        };
        struct string {
            static constexpr char                   beg             = '"';
            static constexpr char                   end             = '"';
        };
        struct number {
            static constexpr bool                   strict          = false;
        };
        struct id {
            static constexpr char const*            nil             = "null";
            static constexpr char const*            pos             = "true";
            static constexpr char const*            neg             = "false";
        };
        static constexpr bool                       strict_js       = false;
    };
    static_assert(format_traits::string::beg == format_traits::string::end && (format_traits::string::beg == '"' || format_traits::string::beg == '\''), "not supported");

    // access

    template <typename X>
        using map = typename X::map;
    template <typename X>
        using list = typename X::list;

}}

#endif // CXON_CHIO_HXX_
