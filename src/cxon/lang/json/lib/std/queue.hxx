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

#ifndef CXON_JSON_LIB_STD_QUEUE_HXX_
#define CXON_JSON_LIB_STD_QUEUE_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, typename ...R> struct is_list<std::queue<T, R...>>            : std::true_type {};
    template <typename T, typename ...R> struct is_list<std::priority_queue<T, R...>>   : std::true_type {};
}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::queue<T, R...>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::queue<T, R...>& t, II& i, II e, Cx& cx) {
                    auto& c = cnt::container(t);
                    return cio::cnt::read_list<Y>(c, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::queue<T, R...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::queue<T, R...>& t, Cx& cx) {
                    auto& c = cnt::container(t);
                    return cio::cnt::write_list<Y>(o, c, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::priority_queue<T, R...>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::priority_queue<T, R...>& t, II& i, II e, Cx& cx) {
                    return cio::cnt::read_list<Y>(i, e, cx, [&] {
                        T o{};
                        return read_value<Y>(o, i, e, cx) && (t.emplace(std::move(o)), true);
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::priority_queue<T, R...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::priority_queue<T, R...>& t, Cx& cx) {
                    auto& c = cnt::container(t);
                    return cio::cnt::write_list<Y>(o, c, cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_QUEUE_HXX_
