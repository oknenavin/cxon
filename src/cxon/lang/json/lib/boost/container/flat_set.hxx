// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_BOOST_CONTAINER_FLAT_SET_HXX_
#define CXON_JSON_LIB_BOOST_CONTAINER_FLAT_SET_HXX_

#include "../../std/imp/set.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, typename ...R> struct is_list<boost::container::flat_set<T, R...>>      : std::true_type {};
    template <typename T, typename ...R> struct is_list<boost::container::flat_multiset<T, R...>> : std::true_type {};
}}

namespace cxon { namespace cnt {

    template <typename X, typename T, typename ...R>
        struct element_reader<JSON<X>, boost::container::flat_set<T, R...>>         : json::imp::set_element_reader_<JSON<X>, boost::container::flat_set<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct element_reader<JSON<X>, boost::container::flat_multiset<T, R...>>    : json::imp::set_element_reader_<JSON<X>, boost::container::flat_multiset<T, R...>> {};

}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, boost::container::flat_set<T, R...>>                   : json::imp::set_reader_<JSON<X>, boost::container::flat_set<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, boost::container::flat_set<T, R...>>                  : json::imp::set_writer_<JSON<X>, boost::container::flat_set<T, R...>> {};

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, boost::container::flat_multiset<T, R...>>              : json::imp::set_reader_<JSON<X>, boost::container::flat_multiset<T, R...>> {};
    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, boost::container::flat_multiset<T, R...>>             : json::imp::set_writer_<JSON<X>, boost::container::flat_multiset<T, R...>> {};

}

#endif // CXON_JSON_LIB_BOOST_CONTAINER_FLAT_SET_HXX_
