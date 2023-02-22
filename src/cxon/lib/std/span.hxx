// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_SPAN_HXX_
#   define CXON_LIB_STD_SPAN_HXX_

#   if __cplusplus >= 202002L
#       if defined(__has_include) && __has_include(<span>)
#           include <span>
#           define CXON_HAS_LIB_STD_SPAN
#       endif
#   endif

#   ifdef CXON_HAS_LIB_STD_SPAN
#       ifdef CXON_JSON_DEFINED
#           include "cxon/lang/json/lib/std/span.hxx"
#       endif
#       ifdef CXON_CBOR_DEFINED
#           include "cxon/lang/cbor/lib/std/span.hxx"
#       endif
#   endif // CXON_HAS_LIB_STD_SPAN

#endif // CXON_LIB_STD_SPAN_HXX_
