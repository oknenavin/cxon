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

#ifndef CXON_JSON_LIB_STD_SET_HXX_
#define CXON_JSON_LIB_STD_SET_HXX_

#include "imp/set.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, typename ...R> struct is_list<std::set<T, R...>>      : std::true_type {};
    template <typename T, typename ...R> struct is_list<std::multiset<T, R...>> : std::true_type {};
}}

namespace cxon { namespace cnt {

    template <typename X, typename T, typename ...R>
        struct element_reader<JSON<X>, std::set<T, R...>>       : json::imp::set_element_reader_<JSON<X>, std::set<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct element_reader<JSON<X>, std::multiset<T, R...>>  : json::imp::set_element_reader_<JSON<X>, std::multiset<T, R...>> {};

}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::set<T, R...>>                 : json::imp::set_reader_<JSON<X>, std::set<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::set<T, R...>>                : json::imp::set_writer_<JSON<X>, std::set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::multiset<T, R...>>            : json::imp::set_reader_<JSON<X>, std::multiset<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::multiset<T, R...>>           : json::imp::set_writer_<JSON<X>, std::multiset<T, R...>> {};

}

#endif // CXON_JSON_LIB_STD_SET_HXX_
