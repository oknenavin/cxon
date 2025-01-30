// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_LIST_
#   define CXON_LIB_STD_LIST_

#   include <list>

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/std/list.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/std/list.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, typename ...R>
        struct traits<std::list<T, R...>> {
            template <typename II>
                static bool append(std::list<T, R...>& c, II f, II l) {
                    return c.insert(c.end(), f, l), true;
                }
        };

}}

#endif // CXON_LIB_STD_LIST_
