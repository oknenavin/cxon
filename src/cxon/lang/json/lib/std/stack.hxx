// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STACK_HXX_
#define CXON_JSON_LIB_STD_STACK_HXX_

#include "cxon/lang/common/chio/container.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::stack<T, R...>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::stack<T, R...>& t, II& i, II e, Cx& cx) {
                    return chio::con::read<J, chio::list<J>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace();
                            return read_value<J>(t.top(), i, e, cx);
#                       else
                            return read_value<J>(t.emplace(), i, e, cx);
#                       endif
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::stack<T, R...>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::stack<T, R...>& t, Cx& cx) {
                    auto const& c = chio::con::bits::adaptor<std::stack<T, R...>>::container(t);
                    return chio::con::write<J, chio::list<J>>(o, c.rbegin(), c.rend(), cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_STACK_HXX_
