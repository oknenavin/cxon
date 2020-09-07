// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_COMPLEX_HXX_
#define CXON_JSON_LIB_STD_COMPLEX_HXX_

namespace cxon {

    template <typename X, typename T>
        struct read<JSON<X>, std::complex<T>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::complex<T>& t, II& i, II e, Cx& cx) {
                    T rl, mg;
                    return  cio::consume<J>(J::list::beg, i, e, cx) &&
                                read_value<J>(rl, i, e, cx) && cio::consume<J>(J::list::sep, i, e, cx) && read_value<J>(mg, i, e, cx) &&
                            cio::consume<J>(J::list::end, i, e, cx) &&
                            (t.real(rl), t.imag(mg), true)
                    ;
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, std::complex<T>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::complex<T>& t, Cx& cx) {
                    return  cio::poke<J>(o, J::list::beg, cx) &&
                                write_value<J>(o, t.real(), cx) && cio::poke<J>(o, J::list::sep, cx) && write_value<J>(o, t.imag(), cx) &&
                            cio::poke<J>(o, J::list::end, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_COMPLEX_HXX_
