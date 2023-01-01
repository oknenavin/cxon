// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_VARIANT_HXX_
#   define CXON_LIB_STD_VARIANT_HXX_

#   if __cplusplus >= 201703L
#       if defined(__has_include) && __has_include(<variant>)
#           include <variant>
#           define CXON_HAS_LIB_STD_VARIANT
#       endif
#   endif

#   ifdef CXON_HAS_LIB_STD_VARIANT
#       ifdef CXON_JSON_DEFINED
#           include "cxon/lang/json/lib/std/variant.hxx"
#       endif
#       ifdef CXON_CBOR_DEFINED
#           include "cxon/lang/cbor/lib/std/variant.hxx"
#       endif
#   endif // CXON_HAS_LIB_STD_VARIANT

#endif // CXON_LIB_STD_VARIANT_HXX_
