// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_BITS_STRING_HXX_
#define CXON_JSON_LIB_STD_BITS_STRING_HXX_

#include "cxon/lang/common/cio/char.hxx"
#include "cxon/lang/common/cio/string.hxx"

namespace cxon { namespace json { namespace bits { // std::basic_string read

    template <typename X, typename T, typename ...R, typename II, typename Cx> // TODO: common with arrays?
        inline auto basic_string_char_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<cio::chr::is_char_8<T>::value, bool>
        {
            II const o = i;
                char32_t const c32 = cio::chr::str_to_utf32<X>(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return cio::rewind(i, o), false;
                T b[4]; t.append(b, cio::chr::utf32_to_utf8(b, c32));
            return true;
        }
    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_char_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<cio::chr::is_char_16<T>::value, bool>
        {
            II const o = i;
                char32_t c32 = cio::chr::str_to_utf32<X>(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return cio::rewind(i, o), false;
                if (c32 > 0xFFFF) {
                    c32 -= 0x10000;
                    t.push_back(char16_t(0xD800 | (c32 >> 10)));
                    t.push_back(char16_t(0xDC00 | (c32 & 0x3FF)));
                }
                else {
                    t.push_back(char16_t(c32));
                }
            return true;
        }
    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_char_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<cio::chr::is_char_32<T>::value, bool>
        {
            II const o = i;
                char32_t const c32 = cio::chr::str_to_utf32<X>(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return cio::rewind(i, o), false;
            return t.push_back(T(c32)), true;
        }

    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline bool basic_string_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
            if (!cio::consume<X>(X::string::beg, i, e, cx))         return false;
                for (char c = cio::peek(i, e); cio::chr::is<X>::real(c); c = cio::peek(i, e)) {
                    if (c == X::string::end)                        return cio::consume<X>(X::string::end, i, e, cx);
                    if (!basic_string_char_read<X>(t, i, e, cx))    return false;
                }
            return cx/json::read_error::unexpected;
        }

        using cio::str::pointer_write;

}}}

#endif // CXON_JSON_LIB_STD_BITS_STRING_HXX_
