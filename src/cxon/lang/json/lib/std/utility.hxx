// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_UTILITY_HXX_
#define CXON_JSON_LIB_STD_UTILITY_HXX_

namespace cxon {

    template <typename X, typename F, typename S>
        struct read<X, std::pair<F, S>> {
            template <typename II, typename Cx>
                static bool value(std::pair<F, S>& t, II& i, II e, Cx& cx) {
                    return  io::consume<X>(X::list::beg, i, e, cx) &&
                                read_value<X>(t.first, i, e, cx) && io::consume<X>(X::list::sep, i, e, cx) && read_value<X>(t.second, i, e, cx) &&
                            io::consume<X>(X::list::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename F, typename S>
        struct write<X, std::pair<F, S>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::pair<F, S>& t, Cx& cx) {
                    return  io::poke<X>(o, X::list::beg, cx) &&
                                write_value<X>(o, t.first, cx) && io::poke<X>(o, X::list::sep, cx) && write_value<X>(o, t.second, cx) &&
                            io::poke<X>(o, X::list::end, cx)
                    ;
                }
        };

}   // cxon

#endif // CXON_JSON_LIB_STD_UTILITY_HXX_
