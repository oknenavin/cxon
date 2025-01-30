// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_SPAN_HXX_
#define CXON_JSON_LIB_STD_SPAN_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, std::size_t N>    struct is_list<std::span<T, N>>                     : std::true_type {};
    template <typename T>                   struct is_list<std::span<T, std::dynamic_extent>>   : std::true_type {};
}}

namespace cxon {

    template <typename X, typename T, std::size_t N>
        struct read<JSON<X>, std::span<T, N>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::span<T, N>& t, II& i, II e, Cx& cx) {
                    II const o = i;
                        std::size_t p = 0;
                    return cio::cnt::read_list<Y>(i, e, cx, [&] {
                            if (p == N) return cio::rewind(i, o), cx/json::read_error::overflow;
                        return read_value<Y>(t[p++], i, e, cx);
                    });
                }
        };
    template <typename X, typename T>
        struct read<JSON<X>, std::span<T, 0>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::span<T, 0>&, II& i, II e, Cx& cx) {
                    return cio::consume<Y>(Y::list::beg, i, e, cx) && cio::consume<Y>(Y::list::end, i, e, cx);
                }
        };
    template <typename X, typename T>
        struct read<JSON<X>, std::span<T, std::dynamic_extent>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::span<T, std::dynamic_extent>& t, II& i, II e, Cx& cx) {
                    auto const N = t.size();
                    if (N > 0) {
                        II const o = i;
                            std::size_t p = 0;
                        return cio::cnt::read_list<Y>(i, e, cx, [&] {
                                if (p == N) return cio::rewind(i, o), cx/json::read_error::overflow;
                            return read_value<Y>(t[p++], i, e, cx);
                        });
                    }
                    return cio::consume<Y>(Y::list::beg, i, e, cx) && cio::consume<Y>(Y::list::end, i, e, cx);
                }
        };

    template <typename X, typename T, std::size_t N>
        struct write<JSON<X>, std::span<T, N>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::span<T, N>& t, Cx& cx) {
                    return cio::cnt::write_list<Y>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_SPAN_HXX_
