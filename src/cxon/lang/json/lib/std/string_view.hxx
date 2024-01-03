// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STRING_VIEW_HXX_
#define CXON_JSON_LIB_STD_STRING_VIEW_HXX_

namespace cxon { namespace cio { // type traits
    template <typename T, typename ...R> struct is_string<std::basic_string_view<T, R...>> : is_char<T> {};
}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::basic_string_view<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::basic_string_view<T, R...>&, II&, II, Cx&) {
                    static_assert(unexpected<std::basic_string_view<T, R...>>(), "std::string_view cannot be modified");
                    return false;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::basic_string_view<T, R...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::basic_string_view<T, R...>& t, Cx& cx) {
                    return cio::str::pointer_write<Y>(o, t.data(), t.size(), cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_STRING_VIEW_HXX_
