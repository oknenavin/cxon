// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_FLAT_SET_HXX_
#   define CXON_LIB_STD_FLAT_SET_HXX_

#   if __cplusplus >= 202302L
#       if defined(__has_include) && __has_include(<flat_set>)
#           include <flat_set>
#           define CXON_HAS_LIB_STD_FLAT_SET
#       endif
#   endif

#   ifdef CXON_HAS_LIB_STD_FLAT_SET
#       ifdef CXON_JSON_DEFINED
#           include "cxon/lang/json/lib/std/flat_set.hxx"
#       endif
#       ifdef CXON_CBOR_DEFINED
#           include "cxon/lang/cbor/lib/std/flat_set.hxx"
#       endif
#   endif

#endif // CXON_LIB_STD_FLAT_SET_HXX_
