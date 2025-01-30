// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_FLAT_SET_HXX_
#define CXON_CBOR_LIB_STD_FLAT_SET_HXX_

#include "cxon/lang/cbor/common/container.hxx"
#include "imp/set.hxx"

namespace cxon { namespace cnt {

    template <typename X, typename T, typename ...R>
        struct element_reader<CBOR<X>, std::flat_set<T, R...>>      : cbor::imp::set_element_reader_<CBOR<X>, std::flat_set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct element_reader<CBOR<X>, std::flat_multiset<T, R...>> : cbor::imp::set_element_reader_<CBOR<X>, std::flat_multiset<T, R...>> {};

}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::flat_set<T, R...>>                : cbor::imp::set_reader_<CBOR<X>, std::flat_set<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::flat_set<T, R...>>               : cbor::imp::set_writer_<CBOR<X>, std::flat_set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::flat_multiset<T, R...>>           : cbor::imp::set_reader_<CBOR<X>, std::flat_multiset<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::flat_multiset<T, R...>>          : cbor::imp::set_writer_<CBOR<X>, std::flat_multiset<T, R...>> {};

}

#endif // CXON_CBOR_LIB_STD_FLAT_SET_HXX_
