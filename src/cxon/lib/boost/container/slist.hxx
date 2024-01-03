// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_BOOST_CONTAINER_SLIST_HXX_
#   define CXON_LIB_BOOST_CONTAINER_SLIST_HXX_

#   include "boost/container/slist.hpp"

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/boost/container/slist.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/boost/container/slist.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, typename ...R>
        struct traits<boost::container::slist<T, R...>> {
            template <typename ...A>
                static auto emplace(boost::container::slist<T, R...>& c, A&&... as) -> typename boost::container::slist<T, R...>::reference {
                    return c.emplace_front(std::forward<A>(as)...);
                }
        };

}}

#endif // CXON_LIB_BOOST_CONTAINER_SLIST_HXX_
