// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TEST_NODE_ORDERED_HXX_
#define CXON_TEST_NODE_ORDERED_HXX_

namespace cxon { namespace json { namespace ordered {

    template <typename K, typename V, typename ...R>
        struct object : std::vector<std::pair<K, V>, R...> {
            object() : std::vector<std::pair<K, V>, R...>() {}
            object(std::initializer_list<std::pair<K, V>> l) : std::vector<std::pair<K, V>, R...>(l) {}
        };

}}}

namespace cxon {

    template <typename X, typename K, typename V, typename ...R, typename II, typename Cx>
        inline bool read_value(json::ordered::object<K, V, R...>& t, II& i, II e, Cx& cx) {
            return chio::con::read_map<X>(i, e, cx, [&] {
#               if __cplusplus < 201703L
                    auto& o = (t.emplace_back(), t.back());
#               else
                    auto& o = t.emplace_back();
#               endif
                return chio::read_key<X>(o.first, i, e, cx) && read_value<X>(o.second, i, e, cx);
            });
        }

    template <typename X, typename K, typename V, typename ...R, typename O, typename Cx>
        inline bool write_value(O& o, const json::ordered::object<K, V, R...>& t, Cx& cx) {
            using value_type = typename json::ordered::object<K, V, R...>::value_type;
            return chio::con::write_map<X>(o, t, cx, [&](const value_type& e) {
                return chio::write_key<X>(o, e.first, cx) && write_value<X>(o, e.second, cx);
            });
        }

}

namespace cxon { namespace json {

    struct ordered_node_traits : node_traits {
        template <typename K, typename V> using object_type = ordered::object<K, V>;
    };
    using ordered_node = basic_node<ordered_node_traits>;

}}

#endif // CXON_TEST_NODE_ORDERED_HXX_
