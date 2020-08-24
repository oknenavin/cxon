// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_VALARRAY_HXX_
#define CXON_JSON_LIB_STD_VALARRAY_HXX_

#include "cxon/lang/common/chio/container.hxx"

namespace cxon {

    template <typename X, typename T>
        struct read<JSON<X>, std::valarray<T>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::valarray<T>& t, II& i, II e, Cx& cx) { // no, it sucks
                    std::valarray<T> v(4);
                    size_t p = 0;
                    bool const r = chio::con::read<J, chio::list<J>>(i, e, cx, [&] {
                        if (p >= v.size()) {
                            std::valarray<T> n(std::move(v));
                            v.resize(p + p);
                                for (size_t i = 0; i != p; ++i) v[i] = n[i];
                        }
                        return read_value<J>(v[p], i, e, cx) && (++p, true);
                    });
                        if (!r) return false;
                    t.resize(p);
                        for (size_t j = 0; j != p; ++j) t[j] = v[j];
                    return true;
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, std::valarray<T>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::valarray<T>& t, Cx& cx) {
                    return chio::con::write<J, chio::list<J>>(o, std::begin(t), std::end(t), cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_VALARRAY_HXX_
