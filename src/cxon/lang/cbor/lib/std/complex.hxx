// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_COMPLEX_HXX_
#define CXON_CBOR_LIB_STD_COMPLEX_HXX_

#include "cxon/lang/cbor/compound.hxx"

namespace cxon {

    template <typename X, typename T>
        struct read<CBOR<X>, std::complex<T>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::complex<T>& t, II& i, II e, Cx& cx) {
                    T rl, mg;
                    return  cbor::tag::read<Y>(i, e, cx) &&
                            cbor::cnt::read_size_eq<Y>(2, i, e, cx) &&
                                read_value<Y>(rl, i, e, cx) &&
                                read_value<Y>(mg, i, e, cx) &&
                            (t.real(rl), t.imag(mg), true)
                    ;
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, std::complex<T>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::complex<T>& t, Cx& cx) {
                    return  cbor::cnt::write_size<Y>(o, Y::arr, 2, cx) &&
                                write_value<Y>(o, t.real(), cx) &&
                                write_value<Y>(o, t.imag(), cx)
                    ;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_COMPLEX_HXX_
