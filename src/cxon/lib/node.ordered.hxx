// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_NODE_ORDERED_HXX_
#define CXON_LIB_NODE_ORDERED_HXX_

#include "node.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace ordered {

    template <typename K, typename V>
        struct keval {
            K key;
            V value;

            bool operator ==(const keval& t) const noexcept { return key == t.key && value == t.value; }
            bool operator !=(const keval& t) const noexcept { return key != t.key || value != t.value; }
            bool operator  <(const keval& t) const noexcept { return key  < t.key || (key == t.key && value < t.value); }
        };

    template <typename K, typename V, typename ...R>
        using object = std::vector<keval<K, V>, R...>;

}}

#ifdef CXON_JSON_DEFINED

    namespace cxon { namespace json {

        template <typename Al = std::allocator<void>>
            struct ordered_node_traits : cxon::json::node_traits<Al> {
                template <typename K, typename V> using object_type = ordered::object<K, V, alc::rebind_t<Al, ordered::keval<K, V>>>;
            };
        using ordered_node = cxon::json::basic_node<ordered_node_traits<>>;

    }}

#endif

#ifdef CXON_CBOR_DEFINED

    namespace cxon { namespace cbor {

        template <typename Al = std::allocator<void>>
            struct ordered_node_traits : cxon::cbor::node_traits<Al> {
                template <typename K, typename V> using map_type = ordered::object<K, V, alc::rebind_t<Al, ordered::keval<K, V>>>;
            };
        using ordered_node = cxon::cbor::basic_node<ordered_node_traits<>>;

    }}

#endif

// implementation //////////////////////////////////////////////////////////////

namespace cxon { // hash

    template <typename K, typename V>
        struct hash<ordered::keval<K, V>> {
            std::size_t operator ()(const ordered::keval<K, V>& t) const noexcept {
                return make_hash(t.key, t.value);
            }
        };

}

#ifdef CXON_JSON_DEFINED

    namespace cxon {

        template <typename X, typename K, typename V>
            struct read<JSON<X>, ordered::keval<K, V>> {
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(ordered::keval<K, V>& t, II& i, II e, Cx& cx) {
                        return  cio::read_key<Y>(t.key, i, e, cx) &&
                                read_value<Y>(t.value, i, e, cx)
                        ;
                    }
            };
        template <typename X, typename K, typename V>
            struct write<JSON<X>, ordered::keval<K, V>> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const ordered::keval<K, V>& t, Cx& cx) {
                        return  cio::write_key<Y>(o, t.key, cx) &&
                                write_value<Y>(o, t.value, cx)
                        ;
                    }
            };

        template <typename X, typename K, typename V, typename ...R>
            struct read<JSON<X>, ordered::object<K, V, R...>> {
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(ordered::object<K, V, R...>& t, II& i, II e, Cx& cx) {
                        return cio::cnt::read_map<Y>(t, i, e, cx);
                    }
            };
        template <typename X, typename K, typename V, typename ...R>
            struct write<JSON<X>, ordered::object<K, V, R...>> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const ordered::object<K, V, R...>& t, Cx& cx) {
                        return cio::cnt::write_map<Y>(o, t, cx);
                    }
            };

    }

#endif

#ifdef CXON_CBOR_DEFINED

    namespace cxon {

        template <typename X, typename K, typename V>
            struct read<CBOR<X>, ordered::keval<K, V>> {
                template <typename II, typename Cx, typename Y = CBOR<X>>
                    static bool value(ordered::keval<K, V>& t, II& i, II e, Cx& cx) {
                        return  read_value<Y>(t.key, i, e, cx) &&
                                read_value<Y>(t.value, i, e, cx)
                        ;
                    }
            };
        template <typename X, typename K, typename V>
            struct write<CBOR<X>, ordered::keval<K, V>> {
                template <typename O, typename Cx, typename Y = CBOR<X>>
                    static bool value(O& o, const ordered::keval<K, V>& t, Cx& cx) {
                        return  write_value<Y>(o, t.key, cx) &&
                                write_value<Y>(o, t.value, cx)
                        ;
                    }
            };

        template <typename X, typename K, typename V, typename ...R>
            struct read<CBOR<X>, ordered::object<K, V, R...>> {
                template <typename II, typename Cx, typename Y = CBOR<X>>
                    static bool value(ordered::object<K, V, R...>& t, II& i, II e, Cx& cx) {
                        return cbor::cnt::read_array<Y>(t, i, e, cx);
                    }
            };
        template <typename X, typename K, typename V, typename ...R>
            struct write<CBOR<X>, ordered::object<K, V, R...>> {
                template <typename O, typename Cx, typename Y = CBOR<X>>
                    static bool value(O& o, const ordered::object<K, V, R...>& t, Cx& cx) {
                        return cbor::cnt::write_map<Y>(o, t, cx);
                    }
            };

    }

#endif

#endif // CXON_LIB_NODE_ORDERED_HXX_
