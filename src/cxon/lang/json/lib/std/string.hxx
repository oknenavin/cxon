// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STRING_HXX_
#define CXON_JSON_LIB_STD_STRING_HXX_

#include "cxon/lang/common/cio/string.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, typename ...R> struct is_string<std::basic_string<T, R...>> : is_char<T> {};
}}

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
