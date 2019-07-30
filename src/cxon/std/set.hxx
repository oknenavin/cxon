// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_SET_HXX_
#define CXON_SET_HXX_

#include <set>
#include "bits/set.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<X, std::set<T, R...>>   : bits::set_reader<X, std::set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct write<X, std::set<T, R...>>  : bits::set_writer<X, std::set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct read<X, std::multiset<T, R...>>  : bits::set_reader<X, std::multiset<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct write<X, std::multiset<T, R...>> : bits::set_writer<X, std::multiset<T, R...>> {};

}   // cxon

#endif // CXON_SET_HXX_
