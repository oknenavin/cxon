// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_QUEUE_HXX_
#define CXON_CBOR_LIB_STD_QUEUE_HXX_

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::queue<T, R...>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::queue<T, R...>& t, II& i, II e, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::queue<T, R...>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::queue<T, R...>& t, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::priority_queue<T, R...>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::priority_queue<T, R...>& t, II& i, II e, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::priority_queue<T, R...>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::priority_queue<T, R...>& t, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_QUEUE_HXX_
