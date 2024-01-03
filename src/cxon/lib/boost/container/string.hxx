// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_BOOST_CONTAINER_STRING_HXX_
#   define CXON_LIB_BOOST_CONTAINER_STRING_HXX_

#   include "boost/container/string.hpp"

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/boost/container/string.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/boost/container/string.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, typename ...R>
        struct traits<boost::container::basic_string<T, R...>> {
            template <typename II>
                static bool append(boost::container::basic_string<T, R...>& c, II f, II l) {
                    return c.append(f, l), true;
                }
        };

}}

#endif // CXON_LIB_BOOST_CONTAINER_STRING_HXX_
