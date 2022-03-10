// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_UTILITY_HXX_
#define CXON_JSON_LIB_STD_UTILITY_HXX_

namespace cxon {

    template <typename X, typename F, typename S>
        struct read<JSON<X>, std::pair<F, S>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::pair<F, S>& t, II& i, II e, Cx& cx) {
                    return  cio::consume<J>(J::list::beg, i, e, cx) &&
                                read_value<J>(t.first, i, e, cx) && cio::consume<J>(J::list::sep, i, e, cx) && read_value<J>(t.second, i, e, cx) &&
                            cio::consume<J>(J::list::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename F, typename S>
        struct write<JSON<X>, std::pair<F, S>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::pair<F, S>& t, Cx& cx) {
                    return  cio::poke<J>(o, J::list::beg, cx) &&
                                write_value<J>(o, t.first, cx) && cio::poke<J>(o, J::list::sep, cx) && write_value<J>(o, t.second, cx) &&
                            cio::poke<J>(o, J::list::end, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_UTILITY_HXX_
