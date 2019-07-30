// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_BITS_MAP_HXX_
#define CXON_BITS_MAP_HXX_

namespace cxon { namespace bits {

    template <typename X, typename M, typename II, typename Cx>
        inline bool read_map(M& t, II& i, II e, Cx& cx) {
            return container::read<X, map<X>>(i, e, cx, [&] {
                typename M::key_type k{}; typename M::mapped_type v{};
                return  read_key<X>(k, i, e, cx) &&
                        read_value<X>(v, i, e, cx) &&
                        (t.emplace(std::move(k), std::move(v)), true);
            });
        }

    template <typename X, typename M, typename O, typename Cx>
        inline bool write_map(O& o, const M& t, Cx& cx) {
            return container::write<X, map<X>>(o, t, cx, [&](const typename M::value_type& e) {
                return  write_key<X>(o, e.first, cx) &&
                        write_value<X>(o, e.second, cx)
                ;
            });
        }

}}   // cxon::bits

#endif // CXON_BITS_MAP_HXX_
