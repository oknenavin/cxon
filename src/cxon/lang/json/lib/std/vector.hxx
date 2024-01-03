// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_VECTOR_HXX_
#define CXON_JSON_LIB_STD_VECTOR_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, typename ...R> struct is_list<std::vector<T, R...>> : std::true_type {};
}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::vector<T, R...>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::vector<T, R...>& t, II& i, II e, Cx& cx) {
                    return cio::cnt::read_list<Y>(t, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::vector<T, R...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::vector<T, R...>& t, Cx& cx) {
                    return cio::cnt::write_list<Y>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_VECTOR_HXX_
