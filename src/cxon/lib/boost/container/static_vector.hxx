// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

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
