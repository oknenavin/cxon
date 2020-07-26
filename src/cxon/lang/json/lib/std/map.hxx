// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_MAP_HXX_
#define CXON_JSON_LIB_STD_MAP_HXX_

#include "bits/map.hxx"

namespace cxon {

    template <typename X, typename ...R>
        struct read<X, std::map<R...>>  : bits::map_reader<X, std::map<R...>> {};

    template <typename X, typename ...R>
        struct write<X, std::map<R...>> : bits::map_writer<X, std::map<R...>> {};

    template <typename X, typename ...R>
        struct read<X, std::multimap<R...>>     : bits::map_reader<X, std::multimap<R...>> {};

    template <typename X, typename ...R>
        struct write<X, std::multimap<R...>>    : bits::map_writer<X, std::multimap<R...>> {};

}   // cxon

#endif // CXON_JSON_LIB_STD_MAP_HXX_
