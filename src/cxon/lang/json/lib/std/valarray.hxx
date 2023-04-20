// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_VALARRAY_HXX_
#define CXON_JSON_LIB_STD_VALARRAY_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T> struct is_list<std::valarray<T>> : std::true_type {};
}}

namespace cxon {

    template <typename X, typename T>
        struct read<JSON<X>, std::valarray<T>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::valarray<T>& t, II& i, II e, Cx& cx) { // no, it sucks
                    std::valarray<T> v(4);
                    std::size_t p = 0;
                    bool const r = cio::cnt::read_list<Y>(i, e, cx, [&] {
                        if (p >= v.size()) {
                            std::valarray<T> n(std::move(v));
                            v.resize(p + p);
                                for (std::size_t i = 0; i != p; ++i) v[i] = n[i];
                        }
                        return read_value<Y>(v[p], i, e, cx) && (++p, true);
                    });
                        if (!r) return false;
                    t.resize(p);
                        for (std::size_t j = 0; j != p; ++j) t[j] = v[j];
                    return true;
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, std::valarray<T>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::valarray<T>& t, Cx& cx) {
#                   ifdef _MSC_VER // std::begin/std::end broken for empty std::valarray
                        return (t.size() && cio::cnt::write_list<Y, std::valarray<T>>(o, std::begin(t), std::end(t), cx)) ||
                               (cio::poke<Y>(o, Y::list::beg, cx) && cio::poke<Y>(o, Y::list::end, cx))
                        ;
#                   else
                        return cio::cnt::write_list<Y, std::valarray<T>>(o, std::begin(t), std::end(t), cx);
#                   endif
                }
        };

}

#endif // CXON_JSON_LIB_STD_VALARRAY_HXX_
