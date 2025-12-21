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

#ifndef CXON_LIB_BOOST_VARIANT2_HXX_
#define CXON_LIB_BOOST_VARIANT2_HXX_

#if defined(__has_include) && __has_include("boost/variant2/variant.hpp")
#   include "boost/variant2/variant.hpp"
#   define CXON_HAS_BOOST_VARIANT2
#endif

#ifdef CXON_HAS_BOOST_VARIANT2
#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/boost/variant2.hxx"
#   endif
#endif

#endif // CXON_LIB_BOOST_VARIANT2_HXX_
