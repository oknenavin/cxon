// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_BOOST_CONTAINER_STABLE_VECTOR_HXX_
#define CXON_CBOR_LIB_BOOST_CONTAINER_STABLE_VECTOR_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, boost::container::stable_vector<T, R...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(boost::container::stable_vector<T, R...>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<Y>(t, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, boost::container::stable_vector<T, R...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const boost::container::stable_vector<T, R...>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y>(o, t, cx);
                }
        };

}

#endif // CXON_CBOR_LIB_BOOST_CONTAINER_STABLE_VECTOR_HXX_
