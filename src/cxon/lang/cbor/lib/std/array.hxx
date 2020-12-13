// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_ARRAY_HXX_
#define CXON_CBOR_LIB_STD_ARRAY_HXX_

#include "cxon/lang/cbor/compound.hxx"

namespace cxon {

    template <typename X, typename T, size_t N>
        struct read<CBOR<X>, std::array<T, N>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::array<T, N>& t, II& i, II e, Cx& cx) {
                    return cbor::bits::read_array_<J>(t.begin(), t.end(), i, e, cx);
                }
        };
    template <typename X, typename T>
        struct read<CBOR<X>, std::array<T, 0>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::array<T, 0>& t, II& i, II e, Cx& cx) {
                    return cbor::bits::read_array_<J>(t.begin(), t.end(), i, e, cx);
                }
        };

    template <typename X, typename T, size_t N>
        struct write<CBOR<X>, std::array<T, N>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::array<T, N>& t, Cx& cx) {
                    return cbor::bits::write_array_<J>(o, t.begin(), t.end(), cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_ARRAY_HXX_
