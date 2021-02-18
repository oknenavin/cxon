// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_VECTOR_HXX_
#   define CXON_LIB_STD_VECTOR_HXX_

#   include <vector>

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/std/vector.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/std/vector.hxx"
#   endif

namespace cxon {

    template <typename T, typename ...R>
        struct continuous<std::vector<T, R...>> {
            static auto range(const std::vector<T, R...>& i) -> decltype(std::make_pair(&i[0], &i[0] + i.size())) {
                return std::make_pair(&i[0], &i[0] + i.size());
            }
        };

}

#endif // CXON_LIB_STD_VECTOR_HXX_
