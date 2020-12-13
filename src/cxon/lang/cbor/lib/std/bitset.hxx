// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_BITSET_HXX_
#define CXON_CBOR_LIB_STD_BITSET_HXX_

namespace cxon {

    template <typename X, size_t N>
        struct read<CBOR<X>, std::bitset<N>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::bitset<N>& t, II& i, II e, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, size_t N>
        struct write<CBOR<X>, std::bitset<N>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::bitset<N>& t, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_BITSET_HXX_
