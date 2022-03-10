// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STACK_HXX_
#define CXON_JSON_LIB_STD_STACK_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::stack<T, R...>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::stack<T, R...>& t, II& i, II e, Cx& cx) {
                    auto& c = cnt::container(t);
                    return cio::cnt::read_list<J>(c, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::stack<T, R...>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::stack<T, R...>& t, Cx& cx) {
                    auto& c = cnt::container(t);
                    return cio::cnt::write_list<J, std::stack<T, R...>>(o, c.rbegin(), c.rend(), cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_STACK_HXX_
