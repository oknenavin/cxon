// Copyright (c) 2017-2023 oknenavin.
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

        static constexpr bool validate_string_encoding  = true;     // read: validate input strings (utf-8)
        static constexpr bool validate_string_escapes   = true;     // read: validate input strings (unescaped control characters)

        static constexpr bool produce_strict_javascript = false;    // write: escape U+2028 LINE SEPARATOR and U+2029 PARAGRAPH SEPARATOR (ECMA-262, 12.3 Line Terminators)

        static constexpr bool quote_unquoted_keys       = true;     // object keys for types serialized without quotes will be quoted (e.g. strings will still be quoted, but numbers will not)
        static constexpr bool unquote_quoted_keys       = false;    // object keys for types serialized with quotes will be unquoted (e.g. strings)
        static constexpr bool allow_comments            = false;    // allow c-style comments
        static constexpr bool allow_trailing_separators = false;    // allow trailing separators for objects and arrays
        static constexpr bool assume_unique_object_keys = true;     // assume that object keys are unique (for the class serializers)
        static constexpr bool allow_javascript_nans     = false;    // allow NaN and Infinity
    };

}}

namespace cxon { namespace cxcf { // format traits

    struct format_traits : json::format_traits {
        struct map : json::format_traits::map {
            static constexpr char div = '=';
            static constexpr char sep = ' ';
        };
        struct list : json::format_traits::list {
            static constexpr char sep = ' ';
        };
        static constexpr bool quote_unquoted_keys       = false;
        static constexpr bool unquote_quoted_keys       = true;
        static constexpr bool allow_comments            = true;
        static constexpr bool allow_javascript_nans     = true;
    };

}}

namespace cxon { // format selector

    template <typename T>
        struct JSON : format_selector<T> {};

    template <typename T = cxcf::format_traits>
        using CXCF = JSON<T>;

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
