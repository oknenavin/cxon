// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_QUEUE_HXX_
#define CXON_QUEUE_HXX_

#include <queue>

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<X, std::queue<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::queue<T, R...>& t, II& i, II e, Cx& cx) {
                    return container::read<X, list<X>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace();
                            return read_value<X>(t.back(), i, e, cx);
#                       else
                            return read_value<X>(t.emplace(), i, e, cx);
#                       endif
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<X, std::priority_queue<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::priority_queue<T, R...>& t, II& i, II e, Cx& cx) {
                    return container::read<X, list<X>>(i, e, cx, [&] {
                        T o{};
                        return read_value<X>(o, i, e, cx) && (t.emplace(std::move(o)), true);
                    });
                }
        };

    namespace bits {

        template <typename A>
            struct adaptor : A {
                static const typename A::container_type& container(const A& a) noexcept { return ((adaptor&)a).c; }
            };

    }

    template <typename X, typename T, typename ...R>
        struct write<X, std::queue<T, R...>> {
            using A = std::queue<T, R...>;
            template <typename O, typename Cx>
                static bool value(O& o, const A& t, Cx& cx) {
                    auto const& c = bits::adaptor<A>::container(t);
                    return container::write<X, list<X>>(o, c.begin(), c.end(), cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::priority_queue<T, R...>> {
            using A = std::priority_queue<T, R...>;
            template <typename O, typename Cx>
                static bool value(O& o, const A& t, Cx& cx) {
                    auto const& c = bits::adaptor<A>::container(t);
                    return container::write<X, list<X>>(o, c.begin(), c.end(), cx);
                }
        };

}   // cxon

#endif // CXON_QUEUE_HXX_
