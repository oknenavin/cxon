// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_UTILITY_HXX_
#define CXON_JSON_LIB_STD_UTILITY_HXX_

namespace cxon { namespace cio { // type traits
    template <typename F, typename S> struct is_list<std::pair<F, S>> : std::true_type {};
}}

namespace cxon {

    template <typename X, typename F, typename S>
        struct read<JSON<X>, std::pair<F, S>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::pair<F, S>& t, II& i, II e, Cx& cx) {
                    return  cio::consume<Y>(Y::list::beg, i, e, cx) &&
                                read_value<Y>(t.first, i, e, cx) &&
                                    cio::consume<Y>(cio::cnt::sep_read<Y, typename Y::list, II>, i, e, cx) &&
                                read_value<Y>(t.second, i, e, cx) &&
                            cio::consume<Y>(Y::list::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename F, typename S>
        struct write<JSON<X>, std::pair<F, S>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::pair<F, S>& t, Cx& cx) {
                    return  cio::poke<Y>(o, Y::list::beg, cx) &&
                                write_value<Y>(o, t.first, cx) &&
                                    cio::poke<Y>(o, Y::list::sep, cx) &&
                                write_value<Y>(o, t.second, cx) &&
                            cio::poke<Y>(o, Y::list::end, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_UTILITY_HXX_
