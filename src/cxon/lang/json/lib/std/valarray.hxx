// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_VALARRAY_HXX_
#define CXON_JSON_LIB_STD_VALARRAY_HXX_

#include "cxon/lang/common/chio/container.hxx"

namespace cxon {

    template <typename X, typename T>
        struct read<X, std::valarray<T>> {
            template <typename II, typename Cx>
                static bool value(std::valarray<T>& t, II& i, II e, Cx& cx) { // no, it sucks
                    std::valarray<T> v(4);
                    size_t p = 0;
                    bool const r = chio::container::read<X, list<X>>(i, e, cx, [&] {
                        if (p >= v.size()) {
                            std::valarray<T> n(std::move(v));
                            v.resize(p + p);
                                for (size_t i = 0; i != p; ++i) v[i] = n[i];
                        }
                        return read_value<X>(v[p], i, e, cx) && (++p, true);
                    });
                        if (!r) return false;
                    t.resize(p);
                        for (size_t j = 0; j != p; ++j) t[j] = v[j];
                    return true;
                }
        };

    template <typename X, typename T>
        struct write<X, std::valarray<T>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::valarray<T>& t, Cx& cx) {
                    return chio::container::write<X, list<X>>(o, std::begin(t), std::end(t), cx);
                }
        };

}   // cxon

#endif // CXON_JSON_LIB_STD_VALARRAY_HXX_
