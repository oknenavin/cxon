// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_FORWARD_LIST_HXX_
#define CXON_CBOR_LIB_STD_FORWARD_LIST_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename T, typename ...R>
        struct element_reader<CBOR<X>, std::forward_list<T, R...>> {
            static auto reference(std::forward_list<T, R...>& c) -> typename std::forward_list<T, R...>::reference {
#               if __cplusplus < 201703L
                    return c.emplace_front(), c.front();
#               else
                    return c.emplace_front();
#               endif
            }
        };

}}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::forward_list<T, R...>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::forward_list<T, R...>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<J>(t, i, e, cx) && (t.reverse(), true);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::forward_list<T, R...>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::forward_list<T, R...>& t, Cx& cx) {
                    return cbor::cnt::write_array<J>(o, t.begin(), t.end(), cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_FORWARD_LIST_HXX_
