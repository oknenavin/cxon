// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_MAP_HXX_
#define CXON_CBOR_LIB_STD_MAP_HXX_

#include "cxon/lang/cbor/common/container.hxx"
#include "bits/map.hxx"

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename K, typename V, typename ...R>
        struct element_reader<CBOR<X>, std::map<K, V, R...>>        : cbor::bits::map_element_reader<CBOR<X>, std::map<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct element_writer<CBOR<X>, std::map<K, V, R...>>        : cbor::bits::map_element_writer<CBOR<X>, std::map<K, V, R...>> {};

    template <typename X, typename K, typename V, typename ...R>
        struct element_reader<CBOR<X>, std::multimap<K, V, R...>>   : cbor::bits::map_element_reader<CBOR<X>, std::multimap<K, V, R...>> {};
    template <typename X, typename K, typename V, typename ...R>
        struct element_writer<CBOR<X>, std::multimap<K, V, R...>>   : cbor::bits::map_element_writer<CBOR<X>, std::multimap<K, V, R...>> {};

}}}

namespace cxon {

    template <typename X, typename K, typename V, typename ...R>
        struct read<CBOR<X>, std::map<K, V, R...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::map<K, V, R...>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<Y>(t, i, e, cx);
                }
        };

    template <typename X, typename K, typename V, typename ...R>
        struct write<CBOR<X>, std::map<K, V, R...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::map<K, V, R...>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y, Y::map>(o, t, cx);
                }
        };

    template <typename X, typename K, typename V, typename ...R>
        struct read<CBOR<X>, std::multimap<K, V, R...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::multimap<K, V, R...>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<Y>(t, i, e, cx);
                }
        };

    template <typename X, typename K, typename V, typename ...R>
        struct write<CBOR<X>, std::multimap<K, V, R...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::multimap<K, V, R...>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y, Y::map>(o, t, cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_MAP_HXX_
