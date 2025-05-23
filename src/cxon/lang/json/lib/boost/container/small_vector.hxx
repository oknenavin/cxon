// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_BOOST_CONTAINER_SMALL_VECTOR_HXX_
#define CXON_JSON_LIB_BOOST_CONTAINER_SMALL_VECTOR_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, std::size_t N, typename ...R> struct is_list<boost::container::small_vector<T, N, R...>> : std::true_type {};
}}

namespace cxon {

    template <typename X, typename T, std::size_t N, typename ...R>
        struct read<JSON<X>, boost::container::small_vector<T, N, R...>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(boost::container::small_vector<T, N, R...>& t, II& i, II e, Cx& cx) {
                    return cio::cnt::read_list<Y>(t, i, e, cx);
                }
        };

    template <typename X, typename T, std::size_t N, typename ...R>
        struct write<JSON<X>, boost::container::small_vector<T, N, R...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const boost::container::small_vector<T, N, R...>& t, Cx& cx) {
                    return cio::cnt::write_list<Y>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_LIB_BOOST_CONTAINER_SMALL_VECTOR_HXX_
