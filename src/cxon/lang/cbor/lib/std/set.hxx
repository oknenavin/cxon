// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_SET_HXX_
#define CXON_CBOR_LIB_STD_SET_HXX_

#include "cxon/lang/cbor/common/container.hxx"
#include "bits/set.hxx"

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename T, typename ...R>
        struct element_reader<CBOR<X>, std::set<T, R...>>       : cbor::bits::set_element_reader<CBOR<X>, std::set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct element_reader<CBOR<X>, std::multiset<T, R...>>  : cbor::bits::set_element_reader<CBOR<X>, std::multiset<T, R...>> {};

}}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::set<T, R...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::set<T, R...>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<Y>(t, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::set<T, R...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::set<T, R...>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y>(o, std::begin(t), std::end(t), cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::multiset<T, R...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::multiset<T, R...>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<Y>(t, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::multiset<T, R...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::multiset<T, R...>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y>(o, std::begin(t), std::end(t), cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_SET_HXX_
