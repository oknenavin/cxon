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

#ifndef CXON_JSON_LIB_STD_UTILITY_HXX_
#define CXON_JSON_LIB_STD_UTILITY_HXX_

namespace cxon { namespace cio { // type traits
    template <typename F, typename S> struct is_list<std::pair<F, S>> : std::true_type {};
}}

namespace cxon {

    template <typename X, typename F, typename S>
        struct read<JSON<X>, std::pair<F, S>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::pair<F, S>& t, II& i, II e, Cx& cx) {
                    return cio::cnt::read_tuple<Y>(i, e, cx, t.first, t.second);
                }
        };

    template <typename X, typename F, typename S>
        struct write<JSON<X>, std::pair<F, S>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::pair<F, S>& t, Cx& cx) {
                    return cio::cnt::write_tuple<Y>(o, cx, t.first, t.second);
                }
        };

}

#endif // CXON_JSON_LIB_STD_UTILITY_HXX_
