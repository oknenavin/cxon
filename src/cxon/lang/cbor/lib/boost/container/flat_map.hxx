// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_BOOST_CONTAINER_FLAT_MAP_HXX_
#define CXON_CBOR_LIB_BOOST_CONTAINER_FLAT_MAP_HXX_

#include "cxon/lang/cbor/common/container.hxx"
#include "../../std/imp/map.hxx"

namespace cxon { namespace cnt {

    template <typename X, typename K, typename V, typename ...R>
        struct element_reader<CBOR<X>, boost::container::flat_map<K, V, R...>>      : cbor::imp::map_element_reader_<CBOR<X>, boost::container::flat_map<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct element_writer<CBOR<X>, boost::container::flat_map<K, V, R...>>      : cbor::imp::map_element_writer_<CBOR<X>, boost::container::flat_map<K, V, R...>> {};

    template <typename X, typename K, typename V, typename ...R>
        struct element_reader<CBOR<X>, boost::container::flat_multimap<K, V, R...>> : cbor::imp::map_element_reader_<CBOR<X>, boost::container::flat_multimap<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct element_writer<CBOR<X>, boost::container::flat_multimap<K, V, R...>> : cbor::imp::map_element_writer_<CBOR<X>, boost::container::flat_multimap<K, V, R...>> {};

}}

namespace cxon {

    template <typename X, typename K, typename V, typename ...R>
        struct read<CBOR<X>, boost::container::flat_map<K, V, R...>>                : cbor::imp::map_reader_<CBOR<X>, boost::container::flat_map<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct write<CBOR<X>, boost::container::flat_map<K, V, R...>>               : cbor::imp::map_writer_<CBOR<X>, boost::container::flat_map<K, V, R...>> {};

    template <typename X, typename K, typename V, typename ...R>
        struct read<CBOR<X>, boost::container::flat_multimap<K, V, R...>>           : cbor::imp::map_reader_<CBOR<X>, boost::container::flat_multimap<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct write<CBOR<X>, boost::container::flat_multimap<K, V, R...>>          : cbor::imp::map_writer_<CBOR<X>, boost::container::flat_multimap<K, V, R...>> {};

}

#endif // CXON_CBOR_LIB_BOOST_CONTAINER_FLAT_MAP_HXX_
