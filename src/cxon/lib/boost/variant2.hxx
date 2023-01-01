// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

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
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/boost/variant2.hxx"
#   endif
#endif

#endif // CXON_LIB_BOOST_VARIANT2_HXX_
