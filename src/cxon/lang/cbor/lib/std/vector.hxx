// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_VECTOR_HXX_
#define CXON_CBOR_LIB_STD_VECTOR_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon { namespace cbor { namespace cnt {

    template <typename T, typename ...R>
        struct container_mutator<std::vector<T, R...>> {
            template <typename II>
                static bool append(std::vector<T, R...>& c, II f, II l) {
                    return c.insert(c.end(), f, l), true;
                }
        };

}}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::vector<T, R...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::vector<T, R...>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<Y>(t, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::vector<T, R...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::vector<T, R...>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y>(o, t.begin(), t.end(), cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_VECTOR_HXX_