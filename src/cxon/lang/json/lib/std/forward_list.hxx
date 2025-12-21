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

#ifndef CXON_JSON_LIB_STD_FORWARD_LIST_HXX_
#define CXON_JSON_LIB_STD_FORWARD_LIST_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, typename ...R> struct is_list<std::forward_list<T, R...>> : std::true_type {};
}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::forward_list<T, R...>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::forward_list<T, R...>& t, II& i, II e, Cx& cx) {
                    return cio::cnt::read_list<Y>(t, i, e, cx) && (t.reverse(), true);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::forward_list<T, R...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::forward_list<T, R...>& t, Cx& cx) {
                    return cio::cnt::write_list<Y>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_FORWARD_LIST_HXX_
