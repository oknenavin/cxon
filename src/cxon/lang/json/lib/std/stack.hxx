// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STACK_HXX_
#define CXON_JSON_LIB_STD_STACK_HXX_

#include "cxon/lang/common/container.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<X, std::stack<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::stack<T, R...>& t, II& i, II e, Cx& cx) {
                    return chario::container::read<X, list<X>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace();
                            return read_value<X>(t.top(), i, e, cx);
#                       else
                            return read_value<X>(t.emplace(), i, e, cx);
#                       endif
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::stack<T, R...>> {
            using A = std::stack<T, R...>;
            template <typename O, typename Cx>
                static bool value(O& o, const A& t, Cx& cx) {
                    auto const& c =  bits::adaptor<A>::container(t);
                    return chario::container::write<X, list<X>>(o, c.rbegin(), c.rend(), cx);
                }
        };

}   // cxon

#endif // CXON_JSON_LIB_STD_STACK_HXX_
