// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_ARRAY_
#   define CXON_LIB_STD_ARRAY_

#   include <array>

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/std/array.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/std/array.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, size_t N>
        struct continuous<std::array<T, N>> {
            static auto range(const std::array<T, N>& i) -> decltype(std::make_pair(i.data(), i.data() + i.size())) {
                return std::make_pair(i.data(), i.data() + i.size());
            }
        };

}}

#endif // CXON_LIB_STD_ARRAY_
