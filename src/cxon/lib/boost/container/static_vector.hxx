// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_BOOST_CONTAINER_STATIC_VECTOR_HXX_
#   define CXON_LIB_BOOST_CONTAINER_STATIC_VECTOR_HXX_

#   include "boost/container/static_vector.hpp"

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/boost/container/static_vector.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/boost/container/static_vector.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, std::size_t N>
        struct traits<boost::container::static_vector<T, N>> {
            template <typename II>
                static bool append(boost::container::static_vector<T, N>& c, II f, II l) {
                    CXON_ASSERT(static_cast<std::size_t>(std::distance(f, l)) <= c.capacity(), "unexpected");
                    return c.insert(c.end(), f, l), true;
                }
        };

}}

#endif // CXON_LIB_BOOST_CONTAINER_STATIC_VECTOR_HXX_
