// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_ARRAY_HXX_
#define CXON_CBOR_LIB_STD_ARRAY_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

    template <typename X, typename T, std::size_t N>
        struct read<CBOR<X>, std::array<T, N>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::array<T, N>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<Y>(t.begin(), t.end(), i, e, cx);
                }
        };

    template <typename X, typename T, std::size_t N>
        struct write<CBOR<X>, std::array<T, N>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::array<T, N>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y>(o, t, cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_ARRAY_HXX_
