// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CXON_JSON_LIB_STD_ARRAY_HXX_
#define CXON_JSON_LIB_STD_ARRAY_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, std::size_t N> struct is_list<std::array<T, N>> : std::true_type {};
}}

namespace cxon {

    template <typename X, typename T, std::size_t N>
        struct read<JSON<X>, std::array<T, N>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::array<T, N>& t, II& i, II e, Cx& cx) {
                    II const o = i;
                        std::size_t p = 0;
                    return cio::cnt::read_list<Y>(i, e, cx, [&] {
                            if (p == N) return cio::rewind(i, o), cx/json::read_error::overflow;
                        return read_value<Y>(t[p++], i, e, cx);
                    });
                }
        };
    template <typename X, typename T>
        struct read<JSON<X>, std::array<T, 0>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::array<T, 0>&, II& i, II e, Cx& cx) {
                    return cio::consume<Y>(Y::list::beg, i, e, cx) && cio::consume<Y>(Y::list::end, i, e, cx);
                }
        };

    template <typename X, typename T, std::size_t N>
        struct write<JSON<X>, std::array<T, N>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::array<T, N>& t, Cx& cx) {
                    return cio::cnt::write_list<Y>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_ARRAY_HXX_
