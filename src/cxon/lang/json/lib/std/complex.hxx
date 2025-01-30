// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_COMPLEX_HXX_
#define CXON_JSON_LIB_STD_COMPLEX_HXX_

namespace cxon { namespace cio { // type traits
    template <typename T> struct is_list<std::complex<T>> : std::true_type {};
}}

namespace cxon {

    template <typename X, typename T>
        struct read<JSON<X>, std::complex<T>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::complex<T>& t, II& i, II e, Cx& cx) {
                    T rl, mg;
                    return cio::cnt::read_tuple<Y>(i, e, cx, rl, mg) && (t.real(rl), t.imag(mg), true);
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, std::complex<T>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::complex<T>& t, Cx& cx) {
                    return cio::cnt::write_tuple<Y>(o, cx, t.real(), t.imag());
                }
        };

}

#endif // CXON_JSON_LIB_STD_COMPLEX_HXX_
