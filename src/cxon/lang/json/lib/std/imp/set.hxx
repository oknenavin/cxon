// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_IMP_SET_HXX_
#define CXON_JSON_LIB_STD_IMP_SET_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace json { namespace imp {

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
                    return cio::cnt::read_list<X>(s, i, e, cx);
                }
        };

    template <typename X, typename S>
        struct set_writer_ {
            template <typename O, typename Cx>
                static bool value(O& o, const S& s, Cx& cx) {
                    return cio::cnt::write_list<X>(o, s, cx);
                }
        };

}}}

#endif // CXON_JSON_LIB_STD_IMP_SET_HXX_
