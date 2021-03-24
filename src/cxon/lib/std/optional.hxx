// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_OPTIONAL_HXX_
#   define CXON_LIB_STD_OPTIONAL_HXX_

#   if __cplusplus >= 201703L
#       if defined(__has_include) && __has_include(<optional>)
#           include <optional>
#           define CXON_HAS_LIB_STD_OPTIONAL
#       endif
#   endif

#   ifdef CXON_HAS_LIB_STD_OPTIONAL
#       ifdef CXON_JSON_DEFINED
#           include "cxon/lang/json/lib/std/optional.hxx"
#       endif
#       ifdef CXON_CBOR_DEFINED
#           include "cxon/lang/cbor/lib/std/optional.hxx"
#       endif
#   endif // CXON_HAS_LIB_STD_OPTIONAL

#endif // CXON_LIB_STD_OPTIONAL_HXX_
