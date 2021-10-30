// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_FUNDAMENTAL_HXX_
#define CXON_JSON_FUNDAMENTAL_HXX_

#include "cxon/lang/common/cio/char.hxx"
#include "cxon/lang/common/cio/numeric.hxx"

namespace cxon { // nullptr_t

    template <typename X>
        struct read<JSON<X>, std::nullptr_t> {
            template <typename II, typename Cx>
                static bool value(std::nullptr_t& t, II& i, II e, Cx& cx) {
                    II const o = i;
                    return (cio::consume<X>(X::id::nil, i, e) || (cio::rewind(i, o), cx/json::read_error::unexpected)) &&
                            (t = nullptr, true)
                    ;
                }
        };

    template <typename X>
        struct write<JSON<X>, std::nullptr_t> {
            template <typename O, typename Cx>
                static bool value(O& o, std::nullptr_t, Cx& cx) {
                    return cio::poke<X>(o, X::id::nil, cx);
                }
        };

}

namespace cxon { // bool

    template <typename X>
        struct read<JSON<X>, bool> {
            template <typename II, typename Cx>
                static bool value(bool& t, II& i, II e, Cx& cx) {
                    static_assert(*X::id::pos != *X::id::neg, "boolean literals ambiguous"); // for input-iterator, id must be consumed
                    II const o = i;
                        char const c = (cio::consume<X>(i, e), cio::peek(i, e));
                             if (c == *X::id::pos && cio::consume<X>(X::id::pos, i, e))  return t = true,  true;
                        else if (c == *X::id::neg && cio::consume<X>(X::id::neg, i, e))  return t = false, true;
                    return cio::rewind(i, o), cx/json::read_error::boolean_invalid;
                }
        };

    template <typename X>
        struct write<JSON<X>, bool> {
            template <typename O, typename Cx>
                static bool value(O& o, bool t, Cx& cx) {
                    return cio::poke<X>(o, t ? X::id::pos : X::id::neg, cx);
                }
        };

}

namespace cxon { // character

    template <typename X, typename T>
        struct read<JSON<X>, T, enable_if_t<cio::chr::is_char_8<T>::value>> {
            template <typename II, typename Cx>
                static bool value(T& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<X>(X::string::beg, i, e, cx)) return false;
                        II const o = i;
                            char32_t const c32 = cio::chr::utf8_to_utf32<X>(i, e, cx);
                                if (c32 == 0xFFFFFFFF)  return cio::rewind(i, o), false;
                                if (c32 >  0XFF)        return cio::rewind(i, o), cx/json::read_error::character_invalid;
                    return cio::consume<X>(X::string::end, i, e, cx) && (t = char(c32), true);
                }
            };
    template <typename X, typename T>
        struct read<JSON<X>, T, enable_if_t<cio::chr::is_char_16<T>::value>> {
            template <typename II, typename Cx>
                static bool value(T& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<X>(X::string::beg, i, e, cx)) return false;
                        II const o = i;
                            char32_t const c32 = cio::chr::utf8_to_utf32<X>(i, e, cx);
                                if (c32 == 0xFFFFFFFF)  return cio::rewind(i, o), false;
                                if (c32 >  0XFFFF)      return cio::rewind(i, o), cx/json::read_error::character_invalid;
                    return cio::consume<X>(X::string::end, i, e, cx) && (t = T(c32), true);
                }
        };
    template <typename X, typename T>
        struct read<JSON<X>, T, enable_if_t<cio::chr::is_char_32<T>::value>> {
            template <typename II, typename Cx>
                static bool value(T& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<X>(X::string::beg, i, e, cx)) return false;
                        II const o = i;
                            char32_t const c32 = cio::chr::utf8_to_utf32<X>(i, e, cx);
                                if (c32 == 0xFFFFFFFF) return cio::rewind(i, o), false;
                    return cio::consume<X>(X::string::end, i, e, cx) && (t = T(c32), true);
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, T, enable_if_t<is_char<T>::value>> {
            template <typename O, typename Cx>
                static bool value(O& o, T t, Cx& cx) {
                    return cio::chr::write<X>(o, t, cx);
                }
        };

}

namespace cxon { // numeric

    template <typename X, typename T>
        struct read<JSON<X>, T, enable_if_t<is_numeric<T>::value>> {
            template <typename II, typename Cx>
                static bool value(T& t, II& i, II e, Cx& cx) {
                    return cio::num::number_read<X>(t, i, e, cx);
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, T, enable_if_t<is_numeric<T>::value>> {
            template <typename O, typename Cx>
                static bool value(O& o, T t, Cx& cx) {
                    return cio::num::number_write<X>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_FUNDAMENTAL_HXX_
