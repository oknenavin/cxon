// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

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
