// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_COMPLEX_HXX_
#define CXON_JSON_LIB_STD_COMPLEX_HXX_

#include <complex>

namespace cxon {

    template <typename X, typename T>
        struct read<X, std::complex<T>> {
            template <typename II, typename Cx>
                static bool value(std::complex<T>& t, II& i, II e, Cx& cx) {
                    T rl, mg;
                    return  io::consume<X>(X::list::beg, i, e, cx) &&
                                read_value<X>(rl, i, e, cx) && io::consume<X>(X::list::sep, i, e, cx) && read_value<X>(mg, i, e, cx) &&
                            io::consume<X>(X::list::end, i, e, cx) &&
                            (t.real(rl), t.imag(mg), true)
                    ;
                }
        };

    template <typename X, typename T>
        struct write<X, std::complex<T>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::complex<T>& t, Cx& cx) {
                    return  io::poke<X>(o, X::list::beg, cx) &&
                                write_value<X>(o, t.real(), cx) && io::poke<X>(o, X::list::sep, cx) && write_value<X>(o, t.imag(), cx) &&
                            io::poke<X>(o, X::list::end, cx)
                    ;
                }
        };

}   // cxon

#endif // CXON_JSON_LIB_STD_COMPLEX_HXX_
