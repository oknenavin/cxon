// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_BOOST_CONTAINER_SMALL_VECTOR_HXX_
#   define CXON_LIB_BOOST_CONTAINER_SMALL_VECTOR_HXX_

#   include "boost/container/small_vector.hpp"

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/boost/container/small_vector.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/boost/container/small_vector.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, std::size_t N, typename ...R>
        struct traits<boost::container::small_vector<T, N, R...>> {
            template <typename II>
                static bool append(boost::container::small_vector<T, N, R...>& c, II f, II l) {
                    return c.insert(c.end(), f, l), true;
                }
        };

}}

#endif // CXON_LIB_BOOST_CONTAINER_SMALL_VECTOR_HXX_
