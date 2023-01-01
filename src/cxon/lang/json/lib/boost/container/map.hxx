// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_BOOST_CONTAINER_MAP_HXX_
#define CXON_JSON_LIB_BOOST_CONTAINER_MAP_HXX_

#include "../../std/imp/map.hxx"

namespace cxon { namespace cnt {

    template <typename X, typename K, typename V, typename ...R>
        struct element_reader<JSON<X>, boost::container::map<K, V, R...>>       : json::imp::map_element_reader_<JSON<X>, boost::container::map<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct element_writer<JSON<X>, boost::container::map<K, V, R...>>       : json::imp::map_element_writer_<JSON<X>, boost::container::map<K, V, R...>> {};

    template <typename X, typename K, typename V, typename ...R>
        struct element_reader<JSON<X>, boost::container::multimap<K, V, R...>>  : json::imp::map_element_reader_<JSON<X>, boost::container::multimap<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct element_writer<JSON<X>, boost::container::multimap<K, V, R...>>  : json::imp::map_element_writer_<JSON<X>, boost::container::multimap<K, V, R...>> {};

}}

namespace cxon {

    template <typename X, typename K, typename V, typename ...R>
        struct read<JSON<X>, boost::container::map<K, V, R...>>                 : json::imp::map_reader_<JSON<X>, boost::container::map<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct write<JSON<X>, boost::container::map<K, V, R...>>                : json::imp::map_writer_<JSON<X>, boost::container::map<K, V, R...>> {};

    template <typename X, typename K, typename V, typename ...R>
        struct read<JSON<X>, boost::container::multimap<K, V, R...>>            : json::imp::map_reader_<JSON<X>, boost::container::multimap<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct write<JSON<X>, boost::container::multimap<K, V, R...>>           : json::imp::map_writer_<JSON<X>, boost::container::multimap<K, V, R...>> {};

}

#endif // CXON_JSON_LIB_BOOST_CONTAINER_MAP_HXX_
