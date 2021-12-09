// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_BOOST_CONTAINER_LIST_
#   define CXON_LIB_BOOST_CONTAINER_LIST_

#   include "boost/container/list.hpp"

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/boost/container/list.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/boost/container/list.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, typename ...R>
        struct traits<boost::container::list<T, R...>> {
            template <typename II>
                static bool append(boost::container::list<T, R...>& c, II f, II l) {
                    return c.insert(c.end(), f, l), true;
                }
        };

}}

#endif // CXON_LIB_BOOST_CONTAINER_LIST_
