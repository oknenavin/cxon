// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_LIST_HXX_
#define CXON_JSON_LIB_STD_LIST_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::list<T, R...>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::list<T, R...>& t, II& i, II e, Cx& cx) {
                    return cio::cnt::read_list<J>(t, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::list<T, R...>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::list<T, R...>& t, Cx& cx) {
                    return cio::cnt::write_list<J>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_LIST_HXX_
