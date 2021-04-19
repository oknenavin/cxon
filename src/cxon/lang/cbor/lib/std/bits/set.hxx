// Copyright (c) 2017-2021 oknenavin.
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
                    auto v = typename S::value_type {}; // TODO: allocator
                    return  read_value<X>(v, i, e, cx) &&
                            (s.emplace(std::move(v)), true)
                    ;
                }
        };

    template <typename X, typename S>
        struct set_reader {
            template <typename II, typename Cx>
                static bool value(S& s, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<X>(s, i, e, cx);
                }
        };

    template <typename X, typename S>
        struct set_writer {
            template <typename O, typename Cx>
                static bool value(O& o, const S& s, Cx& cx) {
                    return cbor::cnt::write_array<X>(o, std::begin(s), std::end(s), cx);
                }
        };

}}}

#endif // CXON_CBOR_LIB_STD_BITS_SET_HXX_
