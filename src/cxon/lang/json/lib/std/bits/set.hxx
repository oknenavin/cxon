// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_BITS_SET_HXX_
#define CXON_JSON_LIB_STD_BITS_SET_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace json { namespace bits {

    template <typename X, typename S>
        struct set_reader {
            template <typename II, typename Cx>
                static bool value(S& t, II& i, II e, Cx& cx) {
                    return cio::con::read_list<X>(i, e, cx, [&] {
                        typename S::value_type o{};
                        return  read_value<X>(o, i, e, cx) &&
                                (t.emplace(std::move(o)), true)
                        ;
                    });
                }
        };

    template <typename X, typename S>
        struct set_writer {
            template <typename O, typename Cx>
                static bool value(O& o, const S& t, Cx& cx) {
                    return cio::con::write_list<X>(o, t, cx);
                }
        };

}}}

#endif // CXON_JSON_LIB_STD_BITS_SET_HXX_
