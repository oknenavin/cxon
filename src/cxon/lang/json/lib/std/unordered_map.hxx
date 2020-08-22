// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_UNORDERED_MAP_HXX_
#define CXON_JSON_LIB_STD_UNORDERED_MAP_HXX_

#include "bits/map.hxx"

namespace cxon {

    template <typename X, typename ...R>
        struct read<JSON<X>, std::unordered_map<R...>>  : json::bits::map_reader<JSON<X>, std::unordered_map<R...>> {};

    template <typename X, typename ...R>
        struct write<JSON<X>, std::unordered_map<R...>> : json::bits::map_writer<JSON<X>, std::unordered_map<R...>> {};

    template <typename X, typename ...R>
        struct read<JSON<X>, std::unordered_multimap<R...>>     : json::bits::map_reader<JSON<X>, std::unordered_multimap<R...>> {};

    template <typename X, typename ...R>
        struct write<JSON<X>, std::unordered_multimap<R...>>    : json::bits::map_writer<JSON<X>, std::unordered_multimap<R...>> {};

}

#endif // CXON_JSON_LIB_STD_UNORDERED_MAP_HXX_
