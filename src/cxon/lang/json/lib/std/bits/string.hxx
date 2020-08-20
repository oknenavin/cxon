// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_BITS_STRING_HXX_
#define CXON_JSON_LIB_STD_BITS_STRING_HXX_

#include "cxon/lang/common/chio/char.hxx"
#include "cxon/lang/common/chio/string.hxx"

namespace cxon { namespace json { namespace bits { // std::basic_string read

    template <typename X, typename ...R, typename II, typename Cx> // TODO: common with arrays?
        inline bool basic_string_char_read(std::basic_string<char, R...>& t, II& i, II e, Cx& cx) {
            II const o = i;
                char32_t const c32 = chio::strs::consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return chio::rewind(i, o), false;
                char b[4]; t.append(b, chio::chars::utf32_to_utf8(b, c32));
            return true;
        }
    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_char_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char16_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char16_t)), bool>
        {
            II const o = i;
                char32_t c32 = chio::strs::consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return chio::rewind(i, o), false;
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
            -> enable_if_t<std::is_same<T, char32_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char32_t)), bool>
        {
            II const o = i;
                char32_t const c32 = chio::strs::consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return chio::rewind(i, o), false;
            return t.push_back(T(c32)), true;
        }

    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline bool basic_string_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
            if (!chio::strs::consume_str<X>::beg(i, e, cx)) return false;
                for (char c = chio::peek(i, e); chio::is<X>::real(c); c = chio::peek(i, e)) {
                    if (chio::strs::is_str<X>::end(c))              return chio::strs::consume_str<X>::end(i, e, cx);
                    if (!basic_string_char_read<X>(t, i, e, cx))    return false;
                }
            return cx|chio::read_error::unexpected;
        }

        using chio::strs::pointer_write;
        using chio::strs::uqkey_pointer_write;

}}}

#endif // CXON_JSON_LIB_STD_BITS_STRING_HXX_
