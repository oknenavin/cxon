// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_ARRAY_HXX_
#define CXON_ARRAY_HXX_

#include <array>

namespace cxon {

    template <typename X, typename T, size_t N>
        struct read<X, std::array<T, N>> {
            template <typename II, typename Cx>
                static bool value(std::array<T, N>& t, II& i, II e, Cx& cx) {
                    II const o = i;
                        size_t p = 0;
                    return container::read<X, list<X>>(i, e, cx, [&] {
                        return (p != N || (bits::rewind(i, o), cx|read_error::overflow)) &&
                                read_value<X>(t[p++], i, e, cx)
                        ;
                    });
                }
        };
    template <typename X, typename T>
        struct read<X, std::array<T, 0>> {
            template <typename II, typename Cx>
                static bool value(std::array<T, 0>&, II& i, II e, Cx& cx) {
                    return io::consume<X>(X::list::beg, i, e, cx) && io::consume<X>(X::list::end, i, e, cx);
                }
        };

    template <typename X, typename T, size_t S>
        struct write<X, std::array<T, S>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::array<T, S>& t, Cx& cx) {
                    return container::write<X, list<X>>(o, t, cx);
                }
        };

}   // cxon

#endif // CXON_ARRAY_HXX_
