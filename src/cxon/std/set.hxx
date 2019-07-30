// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_SET_HXX_
#define CXON_SET_HXX_

#include <set>
#include "bits/set.hxx"

namespace cxon {

#   define CXON_SET(S)\
        template <typename X, typename T, typename ...R>\
            struct read<X, S<T, R...>> {\
                template <typename II, typename Cx>\
                    static bool value(S<T, R...>& t, II& i, II e, Cx& cx) {\
                        return bits::read_set<X>(t, i, e, cx);\
                    }\
            };
        CXON_SET(std::set)
        CXON_SET(std::multiset)
#   undef CXON_SET

#   define CXON_SET(S)\
        template <typename X, typename T, typename ...R>\
            struct write<X, S<T, R...>> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const S<T, R...>& t, Cx& cx) {\
                        return container::write<X, list<X>>(o, t, cx);\
                    }\
            };
        CXON_SET(std::set)
        CXON_SET(std::multiset)
#   undef CXON_SET

}   // cxon

#endif // CXON_SET_HXX_
