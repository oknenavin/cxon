// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_MAP_HXX_
#define CXON_MAP_HXX_

#include <map>
#include "bits/map.hxx"

namespace cxon {

#   define CXON_MAP(M)\
        template <typename X, typename K, typename V, typename ...R>\
            struct read<X, M<K, V, R...>> {\
                template <typename II, typename Cx>\
                    static bool value(M<K, V, R...>& t, II& i, II e, Cx& cx) {\
                        return bits::read_map<X>(t, i, e, cx);\
                    }\
            };
        CXON_MAP(std::map)
        CXON_MAP(std::multimap)
#   undef CXON_MAP

#   define CXON_MAP(M)\
        template <typename X, typename K, typename V, typename ...R>\
            struct write<X, M<K, V, R...>> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const M<K, V, R...>& t, Cx& cx) {\
                        return bits::write_map<X>(o, t, cx);\
                    }\
            };
        CXON_MAP(std::map)
        CXON_MAP(std::multimap)
#   undef CXON_MAP

}   // cxon

#endif // CXON_MAP_HXX_
