// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_VALARRAY_HXX_
#define CXON_CBOR_LIB_STD_VALARRAY_HXX_

namespace cxon {

    template <typename X, typename T>
        struct read<CBOR<X>, std::valarray<T>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::valarray<T>& t, II& i, II e, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, std::valarray<T>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::valarray<T>& t, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_VALARRAY_HXX_
