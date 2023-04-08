// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STRING_HXX_
#define CXON_JSON_LIB_STD_STRING_HXX_

#include "cxon/lang/common/cio/string.hxx"

namespace cxon { namespace cio { namespace key {

#   define CXON_QUOTED(T)\
        template <typename ...R> struct is_quoted<std::basic_string<T, R...>> : std::true_type  {};
        CXON_QUOTED(char)
        CXON_QUOTED(wchar_t)
#       if defined(__cpp_char8_t)
            CXON_QUOTED(char8_t)
#       endif
        CXON_QUOTED(char16_t)
        CXON_QUOTED(char32_t)
#   undef CXON_QUOTED

}}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::basic_string<T, R...>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
                    return cio::str::string_read<Y>(t, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::basic_string<T, R...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx) {
                    return cio::str::pointer_write<Y>(o, t.data(), t.size(), cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_STRING_HXX_
