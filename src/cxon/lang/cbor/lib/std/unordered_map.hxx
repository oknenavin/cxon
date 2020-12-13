// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_UNORDERED_MAP_HXX_
#define CXON_CBOR_LIB_STD_UNORDERED_MAP_HXX_

namespace cxon {

    template <typename X, typename K, typename V, typename ...R>
        struct read<CBOR<X>, std::unordered_map<K, V, R...>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::unordered_map<K, V, R...>& t, II& i, II e, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename K, typename V, typename ...R>
        struct write<CBOR<X>, std::unordered_map<K, V, R...>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::unordered_map<K, V, R...>& t, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename K, typename V, typename ...R>
        struct read<CBOR<X>, std::unordered_multimap<K, V, R...>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::unordered_multimap<K, V, R...>& t, II& i, II e, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename K, typename V, typename ...R>
        struct write<CBOR<X>, std::unordered_multimap<K, V, R...>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::unordered_multimap<K, V, R...>& t, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_UNORDERED_MAP_HXX_
