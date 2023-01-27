// Copyright (c) 2017-2023 oknenavin.
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
            static constexpr char               div     = ':';  // ':' or '='
            static constexpr char               sep     = ',';  // ',' or ';' or ' ' (meaning whitespace)
        };
        struct list {
            static constexpr char               beg     = '[';
            static constexpr char               end     = ']';
            static constexpr char               sep     = ',';  // ',' or ';' or ' ' (meaning whitespace)
        };
        struct string {
            static constexpr char               del     = '"';  // '"' or '''
        };
        struct number {
            static constexpr bool               strict  = false;
        };
        struct id {
#           if __cplusplus >= 201703L
                static constexpr char           nil[]   = "null";
                static constexpr char           pos[]   = "true";
                static constexpr char           neg[]   = "false";
                struct len {
                    static constexpr std::size_t nil    = sizeof(id::nil) - 1;
                    static constexpr std::size_t pos    = sizeof(id::pos) - 1;
                    static constexpr std::size_t neg    = sizeof(id::neg) - 1;
                };
#           else
                static constexpr char const*    nil     = "null";
                static constexpr char const*    pos     = "true";
                static constexpr char const*    neg     = "false";
                struct len {
                    static constexpr std::size_t nil    = 4;
                    static constexpr std::size_t pos    = 4;
                    static constexpr std::size_t neg    = 5;
                };
#           endif
        };

        static constexpr bool validate_string_encoding  = true;     // read: validate input strings (utf-8)
        static constexpr bool validate_string_escapes   = true;     // read: validate input strings (unescaped control characters)
        static constexpr bool produce_strict_javascript = false;    // write: escape U+2028 LINE SEPARATOR and U+2029 PARAGRAPH SEPARATOR (ECMA-262, 12.3 Line Terminators)
        static constexpr bool quote_unquoted_keys       = false;    // object keys for types serialized without quotes will be quoted (e.g. strings will still be quoted, but numbers will not)
        static constexpr bool unquote_quoted_keys       = false;    // object keys for types serialized with quotes will be unquoted (e.g. strings)
        static constexpr bool allow_comments            = false;    // allow c-style comments
        static constexpr bool allow_trailing_separators = false;    // allow trailing separators (map & lists)
        static constexpr bool assume_unique_object_keys = true;     // assume that object keys are unique (for the class serializers)
        static constexpr bool allow_javascript_nans     = false;    // allow NaN and Infinity
    };

}}

namespace cxon { namespace cio { // key quoting helpers

    namespace key {
        template <typename T, typename E = void>    struct is_quoted;
        template <typename T>                       struct unquoted_traits  : T {};
        template <typename T>                       struct   quoted_traits  : T {};
    }
    template <typename X>
        using is_unquoted_key_context = has_traits<key::unquoted_traits<X>, X>;
    template <typename X>
        using unquoted_key_context = typename std::conditional<is_unquoted_key_context<X>::value, X, bind_traits_t<X, key::unquoted_traits>>::type;
    template <typename X>
        using is_quoted_key_context = has_traits<key::quoted_traits<X>, X>;
    template <typename X>
        using quoted_key_context = typename std::conditional<is_quoted_key_context<X>::value, X, bind_traits_t<X, key::quoted_traits>>::type;

}}

namespace cxon { namespace cio { namespace str {

    template <typename X>
        inline auto delim_en_check(char c) -> enable_if_t<!is_unquoted_key_context<X>::value, bool> {
            CXON_IF_CONSTEXPR (!is_quoted_key_context<X>::value) {
                return c == X::string::del;
            }
            return chr::is<X>::space(c) || c == X::map::div;
        }
    template <typename X>
        inline auto delim_en_check(char c) -> enable_if_t< is_unquoted_key_context<X>::value, bool> {
            return c == X::string::del;
        }

    template <typename X, typename II>
        inline auto delim_be_read(II& i, II e) -> enable_if_t<!is_unquoted_key_context<X>::value, bool> {
            CXON_IF_CONSTEXPR (!is_quoted_key_context<X>::value) {
                return peek(i, e) == X::string::del && (++i, true);
            }
            return true;
        }
    template <typename X, typename II>
        inline auto delim_be_read(II& i, II e) -> enable_if_t< is_unquoted_key_context<X>::value, bool> {
            return peek(i, e) == '\\' && next(i, e) == X::string::del && (++i, true);
        }

    template <typename X, typename II>
        inline auto delim_en_read(II& i, II e) -> enable_if_t<!is_unquoted_key_context<X>::value, bool> {
            CXON_IF_CONSTEXPR (!is_quoted_key_context<X>::value) {
                return peek(i, e) == X::string::del && (++i, true);
            }
            return true;
        }
    template <typename X, typename II>
        inline auto delim_en_read(II& i, II e) -> enable_if_t< is_unquoted_key_context<X>::value, bool> {
            return peek(i, e) == '\\' && next(i, e) == X::string::del && (++i, true);
        }

    template <typename X, typename O>
        inline auto delim_be_write(O& o) -> enable_if_t<!is_unquoted_key_context<X>::value, bool> {
            CXON_IF_CONSTEXPR (!is_quoted_key_context<X>::value) {
                return poke(o, X::string::del);
            }
            return true;
        }
    template <typename X, typename O>
        inline auto delim_be_write(O& o) -> enable_if_t< is_unquoted_key_context<X>::value, bool> {
            return poke(o, '\\') && poke(o, X::string::del);
        }

    template <typename X, typename O>
        inline auto delim_en_write(O& o) -> enable_if_t<!is_unquoted_key_context<X>::value, bool> {
            CXON_IF_CONSTEXPR (!is_quoted_key_context<X>::value) {
                return poke(o, X::string::del);
            }
            return true;
        }
    template <typename X, typename O>
        inline auto delim_en_write(O& o) -> enable_if_t< is_unquoted_key_context<X>::value, bool> {
            return poke(o, '\\') && poke(o, X::string::del);
        }

}}}

namespace cxon { namespace cio { namespace cnt {

    template <typename X, typename Cr, typename II>
        inline auto sep_read(II& i, II e) -> enable_if_t<Cr::sep != ' ', bool> {
            return peek(i, e) == Cr::sep && (++i, true);
        }
    template <typename X, typename Cr, typename II>
        inline auto sep_read(II& i, II e) -> enable_if_t<Cr::sep == ' ', bool> {
            return peek(i, e) != Cr::end;
        }

    template <typename X, typename Cr, typename II, typename Cx>
        inline auto consume_sep(II& i, II e, Cx& cx) -> enable_if_t<Cr::sep != ' ', bool> {
            return consume<X>(i, e, cx) && consume<X>(Cr::sep, i, e);
        }
    template <typename X, typename Cr, typename II, typename Cx>
        inline auto consume_sep(II& i, II e, Cx& cx) -> enable_if_t<Cr::sep == ' ', bool> {
            return consume<X>(i, e, cx) && peek(i, e) != Cr::end;
        }

}}}

#endif // CXON_CIO_HXX_
