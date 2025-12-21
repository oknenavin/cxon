// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CXON_JSON_LIB_BOOST_DYNAMIC_BITSET_HXX_
#define CXON_JSON_LIB_BOOST_DYNAMIC_BITSET_HXX_

namespace cxon { namespace cio { // type traits
    template <typename B, typename A> struct is_string<boost::dynamic_bitset<B, A>> : std::true_type {};
}}

namespace cxon { namespace json { namespace imp {

    template <typename X, typename BS, typename II, typename Cx>
        inline bool read_bits_(BS& t, II& i, II e, Cx&) {
            using st = typename BS::size_type;
            constexpr st u = sizeof(typename BS::block_type) * 8;
            st p = u;
            t.resize(t.size() + u);
            for ( ; i != e && (*i == '1' || *i == '0'); ++i) {
                if (p == 0)
                    t.resize(t.size() * 2), t <<= u, p = u;
                --p;
                if (*i == '1') t.set(p);
            }
            return t >>= p, t.resize(t.size() - p), true;
        }

    template <typename X, typename BS, typename O, typename Cx>
        inline bool write_bits_(O& o, const BS& t, Cx& cx) {
            for (std::size_t i = t.size(); i != 0; ) {
                if (!cio::poke<X>(o, t[--i] ? '1' : '0', cx)) return false;
            }
            return true;
        }

}}}

namespace cxon {

    template <typename X, typename B, typename A>
        struct read<JSON<X>, boost::dynamic_bitset<B, A>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(boost::dynamic_bitset<B, A>& t, II& i, II e, Cx& cx) {
                    return  cio::consume<Y>(cio::str::delim_be_read<Y, II>, i, e, cx) &&
                                json::imp::read_bits_<Y>(t, i, e, cx) &&
                            cio::consume<Y>(cio::str::delim_en_read<Y, II>, i, e, cx)
                    ;
                }
        };

    template <typename X, typename B, typename A>
        struct write<JSON<X>, boost::dynamic_bitset<B, A>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const boost::dynamic_bitset<B, A>& t, Cx& cx) {
                    return  cio::poke<Y>(o, cio::str::delim_be_write<Y, O>, cx) &&
                                json::imp::write_bits_<Y>(o, t, cx) &&
                            cio::poke<Y>(o, cio::str::delim_en_write<Y, O>, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_BOOST_DYNAMIC_BITSET_HXX_
