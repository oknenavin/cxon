// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_UNORDERED_SET_HXX_
#define CXON_CBOR_LIB_STD_UNORDERED_SET_HXX_

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::unordered_set<T, R...>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::unordered_set<T, R...>& t, II& i, II e, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::unordered_set<T, R...>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::unordered_set<T, R...>& t, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::unordered_multiset<T, R...>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::unordered_multiset<T, R...>& t, II& i, II e, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::unordered_multiset<T, R...>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::unordered_multiset<T, R...>& t, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_UNORDERED_SET_HXX_
