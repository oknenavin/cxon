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
            static constexpr bool                   unquoted_keys   = true;
        };
        struct list {
            static constexpr char                   beg             = '{';
            static constexpr char                   end             = '}';
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
    };
    static_assert(format_traits::string::beg == format_traits::string::end && (format_traits::string::beg == '"' || format_traits::string::beg == '\''), "not supported");

    struct cxon_format_traits : format_traits {};

    struct json_format_traits : format_traits {
        struct map : format_traits::map {
            static constexpr bool                   unquoted_keys   = false;
        };
        struct list : format_traits::list {
            static constexpr char                   beg             = '[';
            static constexpr char                   end             = ']';
        };
        static constexpr bool                       strict_js       = false;
    };

    // access

    template <typename X>
        using mapacc = typename X::map;
    template <typename X>
        using lstacc = typename X::list;

}   // cxon format traits

namespace cxon { // format selectors

    template <typename T = struct cxon_format_traits>
        struct CXON;

    template <typename T = struct json_format_traits>
        struct JSON;

}

#   ifndef CXON_DEFAULT_FORMAT
#       define CXON_DEFAULT_FORMAT JSON<>
#   endif
#   include "cxon/cxon.hxx"

namespace cxon { // format selectors

    template <typename T>
        struct CXON : format_selector<T> {};

    template <typename T>
        struct JSON : format_selector<T> {};

}

#   include "fundamental.hxx"
#   include "compound.hxx"
#   include "cxon/lang/common/chio/enums.hxx"
#   include "cxon/lang/common/chio/structs.hxx"

namespace cxon { namespace json {
    using read_error = chio::read_error;
    using read_error_category = chio::read_error_category;
    using write_error = chio::write_error;
    using write_error_category = chio::write_error_category;
}}

#endif // CXON_JSON_JSON_HXX_
