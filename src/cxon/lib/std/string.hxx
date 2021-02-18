// Copyright (c) 2017-2020 oknenavin
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

namespace cxon {

    template <typename T, typename ...R>
        struct continuous<std::basic_string<T, R...>> {
            static auto range(const std::basic_string<T, R...>& i) -> decltype(std::make_pair(&i[0], &i[0] + i.size())) {
                return std::make_pair(&i[0], &i[0] + i.size());
            }
        };

}

#endif // CXON_LIB_STD_STRING_HXX_
