// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_QUEUE_HXX_
#define CXON_JSON_LIB_STD_QUEUE_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::queue<T, R...>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::queue<T, R...>& t, II& i, II e, Cx& cx) {
                    auto& c = cnt::container(t);
                    return cio::cnt::read_list<J>(c, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::queue<T, R...>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::queue<T, R...>& t, Cx& cx) {
                    auto& c = cnt::container(t);
                    return cio::cnt::write_list<J>(o, c, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::priority_queue<T, R...>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::priority_queue<T, R...>& t, II& i, II e, Cx& cx) {
                    return cio::cnt::read_list<J>(i, e, cx, [&] {
                        T o{};
                        return read_value<J>(o, i, e, cx) && (t.emplace(std::move(o)), true);
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::priority_queue<T, R...>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::priority_queue<T, R...>& t, Cx& cx) {
                    auto& c = cnt::container(t);
                    return cio::cnt::write_list<J>(o, c, cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_QUEUE_HXX_
