// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_UNORDERED_SET_HXX_
#define CXON_CBOR_LIB_STD_UNORDERED_SET_HXX_

#include "cxon/lang/cbor/common/container.hxx"
#include "bits/set.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct element_reader<CBOR<X>, std::unordered_set<T, R...>>         : cbor::bits::set_element_reader<CBOR<X>, std::unordered_set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct element_reader<CBOR<X>, std::unordered_multiset<T, R...>>    : cbor::bits::set_element_reader<CBOR<X>, std::unordered_multiset<T, R...>> {};

}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::unordered_set<T, R...>>                   : cbor::bits::set_reader<CBOR<X>, std::unordered_set<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::unordered_set<T, R...>>                  : cbor::bits::set_writer<CBOR<X>, std::unordered_set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::unordered_multiset<T, R...>>              : cbor::bits::set_reader<CBOR<X>, std::unordered_multiset<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::unordered_multiset<T, R...>>             : cbor::bits::set_writer<CBOR<X>, std::unordered_multiset<T, R...>> {};

}

#endif // CXON_CBOR_LIB_STD_UNORDERED_SET_HXX_
