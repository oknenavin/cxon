// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_BOOST_CONTAINER_STABLE_VECTOR_HXX_
#   define CXON_LIB_BOOST_CONTAINER_STABLE_VECTOR_HXX_

#   include "boost/container/stable_vector.hpp"

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/boost/container/stable_vector.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/boost/container/stable_vector.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, typename ...R>
        struct traits<boost::container::stable_vector<T, R...>> {
            template <typename II>
                static bool append(boost::container::stable_vector<T, R...>& c, II f, II l) {
                    return c.insert(c.end(), f, l), true;
                }
        };

}}

#endif // CXON_LIB_BOOST_CONTAINER_STABLE_VECTOR_HXX_
