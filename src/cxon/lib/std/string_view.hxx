// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_STRING_VIEW_HXX_
#   define CXON_LIB_STD_STRING_VIEW_HXX_

#   if __cplusplus >= 201703L
#       if defined(__has_include) && __has_include(<string_view>)
#           include <string_view>
#           define CXON_HAS_LIB_STD_STRING_VIEW
#       endif
#   endif

#   ifdef CXON_HAS_LIB_STD_STRING_VIEW
#       ifdef CXON_JSON_DEFINED
#           include "cxon/lang/json/lib/std/string_view.hxx"
#       endif
#       ifdef CXON_CBOR_DEFINED
#           include "cxon/lang/cbor/lib/std/string_view.hxx"
#       endif

#   endif // CXON_HAS_LIB_STD_STRING_VIEW

#endif // CXON_LIB_STD_STRING_VIEW_HXX_
