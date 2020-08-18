// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_BITS_MAP_HXX_
#define CXON_JSON_LIB_STD_BITS_MAP_HXX_

#include "cxon/lang/common/chario/container.hxx"

namespace cxon { namespace bits {

    template <typename X, typename M>
        struct map_reader {
            template <typename II, typename Cx>
                static bool value(M& t, II& i, II e, Cx& cx) {
                    return chio::container::read<X, map<X>>(i, e, cx, [&] {
                        typename M::key_type k{}; typename M::mapped_type v{};
                        return  chio::read_key<X>(k, i, e, cx) &&
                                read_value<X>(v, i, e, cx) &&
                                (t.emplace(std::move(k), std::move(v)), true);
                    });
                }
        };

    template <typename X, typename M>
        struct map_writer {
            template <typename O, typename Cx>
                static bool value(O& o, const M& t, Cx& cx) {
                    return chio::container::write<X, map<X>>(o, t, cx, [&](const typename M::value_type& e) {
                        return  chio::write_key<X>(o, e.first, cx) &&
                                write_value<X>(o, e.second, cx)
                        ;
                    });
                }
        };

}}   // cxon::bits

#endif // CXON_JSON_LIB_STD_BITS_MAP_HXX_
