// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_NODE_ORDERED_HXX_
#define CXON_LIB_NODE_ORDERED_HXX_

#include "node.hxx"

// interface ///////////////////////////////////////////////////////////////////

#if defined(CXON_JSON_DEFINED) || defined(CXON_CBOR_DEFINED)

    namespace cxon { namespace ordered {

        template <typename K, typename V, typename ...R>
            struct object : std::vector<std::pair<K, V>, R...> {
                object() : std::vector<std::pair<K, V>, R...>() {}
                object(std::initializer_list<std::pair<K, V>> l) : std::vector<std::pair<K, V>, R...>(l) {}
            };

    }}

#endif

#ifdef CXON_JSON_DEFINED

    namespace cxon { namespace json {

        struct ordered_node_traits : cxon::json::node_traits {
            template <typename K, typename V> using object_type = ordered::object<K, V>;
        };
        using ordered_node = cxon::json::basic_node<ordered_node_traits>;

    }}

#endif

#ifdef CXON_CBOR_DEFINED

    namespace cxon { namespace cbor {

        struct ordered_node_traits : cxon::cbor::node_traits {
            template <typename K, typename V> using map_type = ordered::object<K, V>;
        };
        using ordered_node = cxon::cbor::basic_node<ordered_node_traits>;

    }}

#endif

// implementation //////////////////////////////////////////////////////////////

#ifdef CXON_JSON_DEFINED

    namespace cxon { namespace cnt {

        template <typename X, typename K, typename V, typename ...R>
            struct element_reader<JSON<X>, ordered::object<K, V, R...>> {
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool read(ordered::object<K, V, R...>& t, II& i, II e, Cx& cx) {
                        auto& o = emplace(t);
                        return  cio::read_key<Y>(o.first, i, e, cx) &&
                                read_value<Y>(o.second, i, e, cx)
                        ;
                    }
            };

        template <typename X, typename K, typename V, typename ...R>
            struct element_writer<JSON<X>, ordered::object<K, V, R...>> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool write(O& o, const typename ordered::object<K, V, R...>::value_type& e, Cx& cx) {
                        return  cio::write_key<Y>(o, e.first, cx) &&
                                write_value<Y>(o, e.second, cx)
                        ;
                    }
            };

    }}

    namespace cxon {

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

    namespace cxon { namespace cnt {

        template <typename X, typename K, typename V, typename ...R>
            struct element_reader<CBOR<X>, ordered::object<K, V, R...>> {
                template <typename II, typename Cx, typename Y = CBOR<X>>
                    static bool read(ordered::object<K, V, R...>& t, II& i, II e, Cx& cx) {
                        auto& o = emplace(t);
                        return  read_value<Y>(o.first, i, e, cx) &&
                                read_value<Y>(o.second, i, e, cx)
                        ;
                    }
            };
        template <typename X, typename K, typename V, typename ...R>
            struct element_writer<CBOR<X>, ordered::object<K, V, R...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool write(O& o, const typename ordered::object<K, V, R...>::value_type& t, Cx& cx) {
                    return  write_value<Y>(o, t.first, cx) &&
                            write_value<Y>(o, t.second, cx)
                    ;
                }
            };

    }}

    namespace cxon {

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
