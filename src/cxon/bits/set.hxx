// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_BITS_SET_HXX_
#define CXON_BITS_SET_HXX_

namespace cxon { namespace bits {

    template <typename X, typename S, typename II, typename Cx>
        inline bool read_set(S& t, II& i, II e, Cx& cx) {
            return container::read<X, list<X>>(i, e, cx, [&] {
                typename S::value_type o{};
                return read_value<X>(o, i, e, cx) && (t.emplace(std::move(o)), true);
            });
        }

}}   // cxon::bits

#endif // CXON_BITS_SET_HXX_
