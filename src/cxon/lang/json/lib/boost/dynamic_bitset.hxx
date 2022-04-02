// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_BOOST_DYNAMIC_BITSET_HXX_
#define CXON_JSON_LIB_BOOST_DYNAMIC_BITSET_HXX_

namespace cxon { namespace cio { namespace key {
    template <typename B, typename A> struct is_quoted<boost::dynamic_bitset<B, A>> : std::true_type {};
}}}

namespace cxon { namespace json { namespace imp {

    template <typename X, typename BS, typename II, typename Cx>
        inline bool read_bits_(BS& t, II& i, II e, Cx& cx) {
            using st = typename BS::size_type;
            constexpr st u = sizeof(typename BS::block_type) * 8;
            st p = u;
            t.resize(t.size() + u);
            for ( ; i != e && *i != '"'; ++i) {
                if (p == 0)
                    t.resize(t.size() * 2), t <<= u, p = u;
                char const c = cio::peek(i, e);
                --p;
                if (c == '1')       t.set(p);
                else if (c != '0')  return cx/json::read_error::unexpected;
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
                    return  cio::consume<Y>(Y::string::template del_read<II>, i, e, cx) &&
                                json::imp::read_bits_<Y>(t, i, e, cx) &&
                            cio::consume<Y>(Y::string::template del_read<II>, i, e, cx)
                    ;
                }
        };

    template <typename X, typename B, typename A>
        struct write<JSON<X>, boost::dynamic_bitset<B, A>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const boost::dynamic_bitset<B, A>& t, Cx& cx) {
                    return  cio::poke<Y>(o, Y::string::template del_write<O>, cx) &&
                                json::imp::write_bits_<Y>(o, t, cx) &&
                            cio::poke<Y>(o, Y::string::template del_write<O>, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_BOOST_DYNAMIC_BITSET_HXX_
