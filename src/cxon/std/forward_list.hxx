// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_FORWARD_LIST_HXX_
#define CXON_FORWARD_LIST_HXX_

#include <forward_list>

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<X, std::forward_list<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::forward_list<T, R...>& t, II& i, II e, Cx& cx) {
                    return container::read<X, list<X>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace_front();
                            return read_value<X>(t.front(), i, e, cx);
#                       else
                            return read_value<X>(t.emplace_front(), i, e, cx);
#                       endif
                    }) &&
                    (t.reverse(), true);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::forward_list<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::forward_list<T, R...>& t, Cx& cx) {
                    return container::write<X, list<X>>(o, t, cx);
                }
        };

}   // cxon

#endif // CXON_FORWARD_LIST_HXX_