// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_DEFINED
#define CXON_JSON_DEFINED

#ifndef CXON_DEFAULT_FORMAT
#   define CXON_DEFAULT_FORMAT JSON<>
#endif

#ifndef CXON_JSON_VALIDATE_STRING_ENCODING  // read: validate input strings (utf-8)
#   define CXON_JSON_VALIDATE_STRING_ENCODING true
#endif
#ifndef CXON_JSON_VALIDATE_STRING_ESCAPES   // read: validate input strings (unescaped control characters)
#   define CXON_JSON_VALIDATE_STRING_ESCAPES true
#endif
#ifndef CXON_JSON_PRODUCE_STRICT_JAVASCRIPT // write: escape U+2028 LINE SEPARATOR and U+2029 PARAGRAPH SEPARATOR (ECMA-262, 12.3 Line Terminators)
#   define CXON_JSON_PRODUCE_STRICT_JAVASCRIPT false
#endif
#ifndef CXON_JSON_ASSUME_NO_ESCAPES         // read/write: assume that strings to be read/written do not contain escape characters
#   define CXON_JSON_ASSUME_NO_ESCAPES false
#endif

#include "lang/json/json-fwd.hxx"
#   include "cxon/cxon.hxx"
#include "lang/json/json.hxx"

#endif // CXON_JSON_DEFINED
