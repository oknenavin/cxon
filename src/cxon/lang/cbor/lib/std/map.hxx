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

#ifndef CXON_CBOR_LIB_STD_MAP_HXX_
#define CXON_CBOR_LIB_STD_MAP_HXX_

#include "cxon/lang/cbor/common/container.hxx"
#include "imp/map.hxx"

namespace cxon { namespace cnt {

    template <typename X, typename K, typename V, typename ...R>
        struct element_reader<CBOR<X>, std::map<K, V, R...>>        : cbor::imp::map_element_reader_<CBOR<X>, std::map<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct element_writer<CBOR<X>, std::map<K, V, R...>>        : cbor::imp::map_element_writer_<CBOR<X>, std::map<K, V, R...>> {};

    template <typename X, typename K, typename V, typename ...R>
        struct element_reader<CBOR<X>, std::multimap<K, V, R...>>   : cbor::imp::map_element_reader_<CBOR<X>, std::multimap<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct element_writer<CBOR<X>, std::multimap<K, V, R...>>   : cbor::imp::map_element_writer_<CBOR<X>, std::multimap<K, V, R...>> {};

}}

namespace cxon {

    template <typename X, typename K, typename V, typename ...R>
        struct read<CBOR<X>, std::map<K, V, R...>>                  : cbor::imp::map_reader_<CBOR<X>, std::map<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct write<CBOR<X>, std::map<K, V, R...>>                 : cbor::imp::map_writer_<CBOR<X>, std::map<K, V, R...>> {};

    template <typename X, typename K, typename V, typename ...R>
        struct read<CBOR<X>, std::multimap<K, V, R...>>             : cbor::imp::map_reader_<CBOR<X>, std::multimap<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct write<CBOR<X>, std::multimap<K, V, R...>>            : cbor::imp::map_writer_<CBOR<X>, std::multimap<K, V, R...>> {};

}

#endif // CXON_CBOR_LIB_STD_MAP_HXX_
