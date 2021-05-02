// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_STRING_HXX_
#   define CXON_LIB_STD_STRING_HXX_

#   include <string>

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/std/string.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/std/string.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, typename ...R>
        struct continuous<std::basic_string<T, R...>> {
            static auto range(const std::basic_string<T, R...>& i) -> decltype(std::make_pair(i.data(), i.data() + i.size())) {
                return std::make_pair(i.data(), i.data() + i.size());
            }
        };

    template <typename T, typename ...R>
        struct traits<std::basic_string<T, R...>> {
            template <typename II>
                static bool append(std::basic_string<T, R...>& c, II f, II l) {
                    return c.append(f, l), true;
                }
        };

}}

#endif // CXON_LIB_STD_STRING_HXX_
