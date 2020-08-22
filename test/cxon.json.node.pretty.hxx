// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TEST_JSON_PRETTY_HXX_
#define CXON_TEST_JSON_PRETTY_HXX_

namespace cxon {

#   define CXON_JSON_NODE_RG()\
        cxon::json::bits::scinc<Cx> RG__(cx);\
        if (!RG__.check()) return cx|error::recursion_depth_exceeded, false
#   define CXON_JSON_NODE_CHECK(e) if (!(e)) return false

    template <typename X, typename Tr, typename O, typename Cx> // pretty write
        static bool write_value(test::indent_iterator<X, O>& o, const basic_node<Tr>& t, Cx& cx) {
            return o.indent_value([&](typename test::indent_iterator<X, O>::out_type out, unsigned& lvl, unsigned tab, char pad) {
                using json = basic_node<Tr>;
                switch (t.kind()) {
                    case node_kind::object: {
                        CXON_JSON_NODE_RG();
                        auto& j = get<typename json::object>(t);
                        if (j.empty()) return chio::poke(out, "{}");
                        auto i = std::begin(j);
                        lvl += tab;
                            CXON_JSON_NODE_CHECK((
                                chio::poke(out, "{\n") && chio::poke(out, lvl, pad) &&
                                chio::write_key<X>(out, i->first, cx), chio::poke(out, ' ') &&
                                    write_value<X>(o, i->second, cx)
                            ));
                            if (j.size() > 1) {
                                auto const e = std::end(j);
                                while (++i != e) {
                                    CXON_JSON_NODE_CHECK((
                                        chio::poke(out, ",\n") && chio::poke(out, lvl, pad) &&
                                        chio::write_key<X>(out, i->first, cx), chio::poke(out, ' ') &&
                                            write_value<X>(o, i->second, cx)
                                    ));
                                }
                            }
                        lvl -= tab;
                        return chio::poke(out, '\n') && chio::poke(out, lvl, pad) && chio::poke(out, '}');
                    }
                    case node_kind::array: {
                        CXON_JSON_NODE_RG();
                        auto& j = get<typename json::array>(t);
                        if (j.empty()) return chio::poke(out, "[]");
                        auto i = std::begin(j);
                        lvl += tab;
                            CXON_JSON_NODE_CHECK((
                                chio::poke(out, "[\n") && chio::poke(out, lvl, pad) &&
                                    write_value<X>(o, *i, cx)
                            ));
                            if (j.size() > 1) {
                                auto const e = std::end(j);
                                while (++i != e) {
                                    CXON_JSON_NODE_CHECK((
                                        chio::poke(out, ",\n") && chio::poke(out, lvl, pad) &&
                                            write_value<X>(o, *i, cx)
                                    ));
                                }
                            }
                        lvl -= tab;
                        return chio::poke(out, '\n') && chio::poke(out, lvl, pad) && chio::poke(out, ']');
                    }
                    case node_kind::string:
                        return write_value<X>(out, get<typename json::string>(t), cx);
                    case node_kind::number:
                        return write_value<X>(out, get<typename json::number>(t), cx);
                    case node_kind::boolean:
                        return write_value<X>(out, get<typename json::boolean>(t), cx);
                    case node_kind::null:
                        return write_value<X>(out, get<typename json::null>(t), cx);
                }
                return true; // LCOV_EXCL_LINE
            });
        }

#   undef CXON_JSON_NODE_CHECK
#   undef CXON_JSON_NODE_RG

}

#endif // CXON_TEST_JSON_PRETTY_HXX_
