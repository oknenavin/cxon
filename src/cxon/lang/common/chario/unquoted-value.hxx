// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHIO_UNQUOTED_VALUE_HXX_
#define CXON_CHIO_UNQUOTED_VALUE_HXX_

#include "chio.hxx"
#include "bits/unquoted-value.hxx"

namespace cxon { namespace unquoted { // unquoted value

    template <typename X, typename T, size_t N, typename II, typename Cx>
        inline bool read_value(T (&t)[N], II& i, II e, Cx& cx) {
            II const o = i;
                if (!bits::value<X>::read(bits::array_adder<T, Cx>(t, cx), i, e)) {
                    return cx.ec == read_error::overflow ? (chio::rewind(i, o), false) : cx|read_error::unexpected;
                }
            return true;
        }

    template <typename X, typename II, typename Cx>
        inline bool read_value(II& i, II e, Cx& cx) {
            return bits::value<X>::read(bits::black_adder<decltype(*i)>(), i, e) || (cx|read_error::unexpected);
        }

}}  // cxon::unquoted value

#endif // CXON_CHIO_UNQUOTED_VALUE_HXX_
