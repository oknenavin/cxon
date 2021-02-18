// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_BITS_SET_HXX_
#define CXON_CBOR_LIB_STD_BITS_SET_HXX_

namespace cxon { namespace cbor { namespace bits {

    template <typename X, typename S>
        struct set_element_reader {
            template <typename II, typename Cx>
                static bool read(S& s, II& i, II e, Cx& cx) {
                    auto v = typename S::value_type {};
                    return  read_value<X>(v, i, e, cx) &&
                            (s.emplace(std::move(v)), true)
                    ;
                }
        };

}}}

#endif // CXON_CBOR_LIB_STD_BITS_SET_HXX_
