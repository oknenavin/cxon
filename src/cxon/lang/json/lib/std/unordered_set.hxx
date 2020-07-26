// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_UNORDERED_SET_HXX_
#define CXON_JSON_LIB_STD_UNORDERED_SET_HXX_

#include "bits/set.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<X, std::unordered_set<T, R...>>     : bits::set_reader<X, std::unordered_set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct write<X, std::unordered_set<T, R...>>    : bits::set_writer<X, std::unordered_set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct read<X, std::unordered_multiset<T, R...>>    : bits::set_reader<X, std::unordered_multiset<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct write<X, std::unordered_multiset<T, R...>>   : bits::set_writer<X, std::unordered_multiset<T, R...>> {};

}   // cxon

#endif // CXON_JSON_LIB_STD_UNORDERED_SET_HXX_
