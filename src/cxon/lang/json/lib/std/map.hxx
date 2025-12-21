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

#ifndef CXON_JSON_LIB_STD_MAP_HXX_
#define CXON_JSON_LIB_STD_MAP_HXX_

#include "imp/map.hxx"

namespace cxon { namespace cio { // type traits
    template <typename K, typename V, typename ...R> struct is_map<std::map<K, V, R...>>        : std::true_type {};
    template <typename K, typename V, typename ...R> struct is_map<std::multimap<K, V, R...>>   : std::true_type {};
}}

namespace cxon { namespace cnt {

    template <typename X, typename K, typename V, typename ...R>
        struct element_reader<JSON<X>, std::map<K, V, R...>>        : json::imp::map_element_reader_<JSON<X>, std::map<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct element_writer<JSON<X>, std::map<K, V, R...>>        : json::imp::map_element_writer_<JSON<X>, std::map<K, V, R...>> {};

    template <typename X, typename K, typename V, typename ...R>
        struct element_reader<JSON<X>, std::multimap<K, V, R...>>   : json::imp::map_element_reader_<JSON<X>, std::multimap<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct element_writer<JSON<X>, std::multimap<K, V, R...>>   : json::imp::map_element_writer_<JSON<X>, std::multimap<K, V, R...>> {};

}}

namespace cxon {

    template <typename X, typename K, typename V, typename ...R>
        struct read<JSON<X>, std::map<K, V, R...>>                  : json::imp::map_reader_<JSON<X>, std::map<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct write<JSON<X>, std::map<K, V, R...>>                 : json::imp::map_writer_<JSON<X>, std::map<K, V, R...>> {};

    template <typename X, typename K, typename V, typename ...R>
        struct read<JSON<X>, std::multimap<K, V, R...>>             : json::imp::map_reader_<JSON<X>, std::multimap<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct write<JSON<X>, std::multimap<K, V, R...>>            : json::imp::map_writer_<JSON<X>, std::multimap<K, V, R...>> {};

}

#endif // CXON_JSON_LIB_STD_MAP_HXX_
