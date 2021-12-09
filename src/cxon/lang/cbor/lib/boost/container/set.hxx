// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_BOOST_CONTAINER_SET_HXX_
#define CXON_CBOR_LIB_BOOST_CONTAINER_SET_HXX_

#include "cxon/lang/cbor/common/container.hxx"
#include "../../std/imp/set.hxx"

namespace cxon { namespace cnt {

    template <typename X, typename T, typename ...R>
        struct element_reader<CBOR<X>, boost::container::set<T, R...>>      : cbor::imp::set_element_reader_<CBOR<X>, boost::container::set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct element_reader<CBOR<X>, boost::container::multiset<T, R...>> : cbor::imp::set_element_reader_<CBOR<X>, boost::container::multiset<T, R...>> {};

}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, boost::container::set<T, R...>>                : cbor::imp::set_reader_<CBOR<X>, boost::container::set<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, boost::container::set<T, R...>>               : cbor::imp::set_writer_<CBOR<X>, boost::container::set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, boost::container::multiset<T, R...>>           : cbor::imp::set_reader_<CBOR<X>, boost::container::multiset<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, boost::container::multiset<T, R...>>          : cbor::imp::set_writer_<CBOR<X>, boost::container::multiset<T, R...>> {};

}

#endif // CXON_CBOR_LIB_BOOST_CONTAINER_SET_HXX_
