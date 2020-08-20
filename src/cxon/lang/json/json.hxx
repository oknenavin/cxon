// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_JSON_HXX_
#define CXON_JSON_JSON_HXX_

namespace cxon { // format traits

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

    struct json_format_traits : format_traits {};

    // access

    template <typename X>
        using mapacc = typename X::map;
    template <typename X>
        using lstacc = typename X::list;

}

namespace cxon { // format selectors
    template <typename T = struct json_format_traits>
        struct JSON;
}

#ifndef CXON_DEFAULT_FORMAT
#   define CXON_DEFAULT_FORMAT JSON<>
#endif
#include "cxon/cxon.hxx"

namespace cxon { // format selectors
    template <typename T>
        struct JSON : format_selector<T> {};
}

#include "fundamental.hxx"
#include "enum.hxx"
#include "compound.hxx"
#include "struct.hxx"

namespace cxon { namespace json {
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
