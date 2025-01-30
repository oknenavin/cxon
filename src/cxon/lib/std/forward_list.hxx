// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_FORWARD_LIST_HXX_
#   define CXON_LIB_STD_FORWARD_LIST_HXX_

#   include <forward_list>

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/std/forward_list.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/std/forward_list.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, typename ...R>
        struct traits<std::forward_list<T, R...>> {
            template <typename ...A>
                static auto emplace(std::forward_list<T, R...>& c, A&&... as) -> typename std::forward_list<T, R...>::reference {
#                   if __cplusplus < 201703L
                        return c.emplace_front(std::forward<A>(as)...), c.front();
#                   else
                        return c.emplace_front(std::forward<A>(as)...);
#                   endif
                }
        };

}}

#endif // CXON_LIB_STD_FORWARD_LIST_HXX_
