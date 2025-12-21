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

#ifndef CXON_LIB_NODE_ORDERED_HXX_
#define CXON_LIB_NODE_ORDERED_HXX_

#include "node.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace ordered {

    template <typename K, typename V, typename Al = std::allocator<char>>
        struct keval {
            K first;
            V second;

            keval()
            :   first(),
                second()
            {}
            keval(const Al& al)
            :   first(alc::create_using_allocator<K>(al)),
                second(alc::create_using_allocator<V>(al))
            {}

            template <typename KF, typename VF>
                keval(KF&& k, VF&& v)
                :   first(std::forward<KF>(k)),
                    second(std::forward<VF>(v))
                {}

            bool operator ==(const keval& t) const noexcept { return first == t.first && second == t.second; }
            bool operator !=(const keval& t) const noexcept { return first != t.first || second != t.second; }
            bool operator  <(const keval& t) const noexcept { return first  < t.first || (first == t.first && second < t.second); }
        };

    template <typename K, typename V, typename Al = std::allocator<char>>
        using object = std::vector<keval<K, V, Al>, alc::rebind_t<Al, ordered::keval<K, V, Al>>>;

}}

#ifdef CXON_JSON_DEFINED

    namespace cxon { namespace json {

        template <typename Al = std::allocator<char>>
            struct ordered_node_traits : cxon::json::node_traits<Al> {
                template <typename K, typename V> using object_type = ordered::object<K, V, alc::rebind_t<Al, ordered::keval<K, V, Al>>>;
            };
        using ordered_node = cxon::json::basic_node<ordered_node_traits<>>;

    }}

#endif

#ifdef CXON_CBOR_DEFINED

    namespace cxon { namespace cbor {

        template <typename Al = std::allocator<char>>
            struct ordered_node_traits : cxon::cbor::node_traits<Al> {
                template <typename K, typename V> using map_type = ordered::object<K, V, alc::rebind_t<Al, ordered::keval<K, V, Al>>>;
            };
        using ordered_node = cxon::cbor::basic_node<ordered_node_traits<>>;

    }}

#endif

// implementation //////////////////////////////////////////////////////////////

namespace cxon { // hash

    template <typename K, typename V, typename Al>
        struct hash<ordered::keval<K, V, Al>> {
            std::size_t operator ()(const ordered::keval<K, V, Al>& t) const noexcept {
                return make_hash(t.first, t.second);
            }
        };

}

#ifdef CXON_JSON_DEFINED

    namespace cxon {

        template <typename X, typename K, typename V, typename Al>
            struct read<JSON<X>, ordered::keval<K, V, Al>> {
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(ordered::keval<K, V, Al>& t, II& i, II e, Cx& cx) {
                        return  cio::read_map_key<Y>(t.first, i, e, cx) &&
                                cio::read_map_val<Y>(t.second, i, e, cx)
                        ;
                    }
            };
        template <typename X, typename K, typename V, typename Al>
            struct write<JSON<X>, ordered::keval<K, V, Al>> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const ordered::keval<K, V, Al>& t, Cx& cx) {
                        return  cio::write_map_key<Y>(o, t.first, cx) &&
                                cio::write_map_val<Y>(o, t.second, cx)
                        ;
                    }
            };

        template <typename X, typename K, typename V, typename Al>
            struct read<JSON<X>, ordered::object<K, V, Al>> {
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(ordered::object<K, V, Al>& t, II& i, II e, Cx& cx) {
                        return cio::cnt::read_map<Y>(t, i, e, cx);
                    }
            };
        template <typename X, typename K, typename V, typename Al>
            struct write<JSON<X>, ordered::object<K, V, Al>> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const ordered::object<K, V, Al>& t, Cx& cx) {
                        return cio::cnt::write_map<Y>(o, t, cx);
                    }
            };

    }

#endif

#ifdef CXON_CBOR_DEFINED

    namespace cxon {

        template <typename X, typename K, typename V, typename Al>
            struct read<CBOR<X>, ordered::keval<K, V, Al>> {
                template <typename II, typename Cx, typename Y = CBOR<X>>
                    static bool value(ordered::keval<K, V, Al>& t, II& i, II e, Cx& cx) {
                        return  read_value<Y>(t.first, i, e, cx) &&
                                read_value<Y>(t.second, i, e, cx)
                        ;
                    }
            };
        template <typename X, typename K, typename V, typename Al>
            struct write<CBOR<X>, ordered::keval<K, V, Al>> {
                template <typename O, typename Cx, typename Y = CBOR<X>>
                    static bool value(O& o, const ordered::keval<K, V, Al>& t, Cx& cx) {
                        return  write_value<Y>(o, t.first, cx) &&
                                write_value<Y>(o, t.second, cx)
                        ;
                    }
            };

        template <typename X, typename K, typename V, typename Al>
            struct read<CBOR<X>, ordered::object<K, V, Al>> {
                template <typename II, typename Cx, typename Y = CBOR<X>>
                    static bool value(ordered::object<K, V, Al>& t, II& i, II e, Cx& cx) {
                        return cbor::cnt::read_array<Y>(t, i, e, cx);
                    }
            };
        template <typename X, typename K, typename V, typename Al>
            struct write<CBOR<X>, ordered::object<K, V, Al>> {
                template <typename O, typename Cx, typename Y = CBOR<X>>
                    static bool value(O& o, const ordered::object<K, V, Al>& t, Cx& cx) {
                        return cbor::cnt::write_map<Y>(o, t, cx);
                    }
            };

    }

#endif

#endif // CXON_LIB_NODE_ORDERED_HXX_
