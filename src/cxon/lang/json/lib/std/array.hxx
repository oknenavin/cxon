// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_ARRAY_HXX_
#define CXON_JSON_LIB_STD_ARRAY_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon {

    template <typename T, size_t N>
        struct continuous<std::array<T, N>> {
            static auto range(const std::array<T, N>& i) -> decltype(std::make_pair(&i[0], &i[0] + i.size())) {
                return std::make_pair(&i[0], &i[0] + i.size());
            }
        };

    template <typename X, typename T, size_t N>
        struct read<JSON<X>, std::array<T, N>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::array<T, N>& t, II& i, II e, Cx& cx) {
                    II const o = i;
                        size_t p = 0;
                    return cio::con::read_list<J>(i, e, cx, [&] {
                        return (p != N || (cio::rewind(i, o), cx|cio::read_error::overflow)) &&
                                read_value<J>(t[p++], i, e, cx)
                        ;
                    });
                }
        };
    template <typename X, typename T>
        struct read<JSON<X>, std::array<T, 0>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::array<T, 0>&, II& i, II e, Cx& cx) {
                    return cio::consume<J>(J::list::beg, i, e, cx) && cio::consume<J>(J::list::end, i, e, cx);
                }
        };

    template <typename X, typename T, size_t S>
        struct write<JSON<X>, std::array<T, S>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::array<T, S>& t, Cx& cx) {
                    return cio::con::write_list<J>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_ARRAY_HXX_
