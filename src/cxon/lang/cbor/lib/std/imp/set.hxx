// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_IMP_SET_HXX_
#define CXON_CBOR_LIB_STD_IMP_SET_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon { namespace cbor { namespace imp {

    template <typename X, typename S>
        struct set_element_reader_ {
            template <typename II, typename Cx>
                static bool read(S& t, II& i, II e, Cx& cx) {
                    auto v = alc::create_using_allocator_of<typename S::value_type>(t);
                    return  read_value<X>(v, i, e, cx) &&
                            (t.emplace(std::move(v)), true)
                    ;
                }
        };

    template <typename X, typename S>
        struct set_reader_ {
            template <typename II, typename Cx>
                static bool value(S& s, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<X>(s, i, e, cx);
                }
        };

    template <typename X, typename S>
        struct set_writer_ {
            template <typename O, typename Cx>
                static bool value(O& o, const S& s, Cx& cx) {
                    return cbor::cnt::write_array<X>(o, s, cx);
                }
        };

}}}

#endif // CXON_CBOR_LIB_STD_IMP_SET_HXX_
