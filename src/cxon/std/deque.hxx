// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_DEQUE_HXX_
#define CXON_DEQUE_HXX_

#include <deque>

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<X, std::deque<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::deque<T, R...>& t, II& i, II e, Cx& cx) {
                    return container::read<X, list<X>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace_back();
                            return read_value<X>(t.back(), i, e, cx);
#                       else
                            return read_value<X>(t.emplace_back(), i, e, cx);
#                       endif
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::deque<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::deque<T, R...>& t, Cx& cx) {
                    return container::write<X, list<X>>(o, t, cx);
                }
        };

}   // cxon

#endif // CXON_DEQUE_HXX_