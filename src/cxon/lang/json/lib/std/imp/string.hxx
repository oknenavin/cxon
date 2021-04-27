// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_IMP_STRING_HXX_
#define CXON_JSON_LIB_STD_IMP_STRING_HXX_

#include "cxon/lang/common/cio/char.hxx"
#include "cxon/lang/common/cio/string.hxx"

namespace cxon { namespace json { namespace imp { // std::basic_string read

    template <typename II>
        inline int validate_(II i, II e) {
            auto const c0 = *i;
            if ((c0 & 0x80) == 0)
                return 0;
            if ((c0 & 0xE0) == 0xC0) {
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                return 1;
            }
            if ((c0 & 0xF0) == 0xE0) {
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                return 2;
            }
            if ((c0 & 0xF8) == 0xF0) {
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                return 3;
            }
            return 4;
        }

    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_read_(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<!cio::chr::is_char_8<T>::value || !is_forward_iterator<II>::value, bool>
        {
            return cio::str::imp::string_read_<X>(t, i, e, cx);
        }
    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_read_(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t< cio::chr::is_char_8<T>::value &&  is_forward_iterator<II>::value, bool>
        {
            if (!cio::consume<X>(X::string::beg, i, e, cx))         return false;
            II b = i;
            for ( ; i != e && *i != X::string::end; ++i) {
                if (*i == '\\') {
                    if (b != i)
                        if (!container_append(t, b, i)) return cx/X::read_error::unexpected;
                    {
                        II const o = i;
                        char32_t const c32 = cio::chr::imp::esc_to_utf32_<X>(++i, e);
                            if (c32 == 0xFFFFFFFF) return cio::rewind(i, o), cx/X::read_error::escape_invalid;
                        T bf[4]; t.append(bf, cio::chr::utf32_to_utf8(bf, c32));
                    }
                    b = i, --i;
                }
                else CXON_IF_CONSTEXPR(X::validate) {
                    II const o = i;
                    auto const bs = validate_(i, e);
                        if (bs > 3) return cio::rewind(i, o), cx/X::read_error::character_invalid;
                    i += bs;
                }
            }
            return  (b == i || container_append(t ,b, i) || cx/X::read_error::unexpected) &&
                    cio::consume<X>(X::string::end, i, e, cx)
            ;
        }

}}}

#endif // CXON_JSON_LIB_STD_IMP_STRING_HXX_
