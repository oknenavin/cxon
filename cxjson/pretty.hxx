// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXJSON_PRETTY_HXX_
#define CXJSON_PRETTY_HXX_

// implementation /////////////////////////////////////////////////////////////

namespace cxon {

#   define CXJSON_RG()\
        bits::scinc<Cx> RG__(cx);\
        if (!RG__.check()) return cx|error::recursion_depth_exceeded, false
#   define CXJSON_CHECK(e) if (!(e)) return false

    template <typename X, typename Tr, typename O, typename Cx> // pretty write
        static bool write_value(indent_iterator<X, O>& o, const cxjson::basic_node<Tr>& t, Cx& cx) {
            return o.indent_value([&](typename indent_iterator<X, O>::out_type out, unsigned& lvl, unsigned tab, char pad) {
                using json = cxjson::basic_node<Tr>;
                switch (t.kind()) {
                    case node_kind::object: {
                        CXJSON_RG();
                        auto& j = get<typename json::object>(t);
                        if (j.empty()) return io::poke(out, "{}");
                        auto i = std::begin(j);
                        lvl += tab;
                            CXJSON_CHECK((
                                io::poke(out, "{\n") && io::poke(out, lvl, pad) &&
                                write_key<X>(out, i->first, cx), io::poke(out, ' ') &&
                                    write_value<X>(o, i->second, cx)
                            ));
                            if (j.size() > 1) {
                                auto const e = std::end(j);
                                while (++i != e) {
                                    CXJSON_CHECK((
                                        io::poke(out, ",\n") && io::poke(out, lvl, pad) &&
                                        write_key<X>(out, i->first, cx), io::poke(out, ' ') &&
                                            write_value<X>(o, i->second, cx)
                                    ));
                                }
                            }
                        lvl -= tab;
                        return io::poke(out, '\n') && io::poke(out, lvl, pad) && io::poke(out, '}');
                    }
                    case node_kind::array: {
                        CXJSON_RG();
                        auto& j = get<typename json::array>(t);
                        if (j.empty()) return io::poke(out, "[]");
                        auto i = std::begin(j);
                        lvl += tab;
                            CXJSON_CHECK((
                                io::poke(out, "[\n") && io::poke(out, lvl, pad) &&
                                    write_value<X>(o, *i, cx)
                            ));
                            if (j.size() > 1) {
                                auto const e = std::end(j);
                                while (++i != e) {
                                    CXJSON_CHECK((
                                        io::poke(out, ",\n") && io::poke(out, lvl, pad) &&
                                            write_value<X>(o, *i, cx)
                                    ));
                                }
                            }
                        lvl -= tab;
                        return io::poke(out, '\n') && io::poke(out, lvl, pad) && io::poke(out, ']');
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

#   undef CXJSON_CHECK
#   undef CXJSON_RG

}   // cxon

#endif // CXJSON_PRETTY_HXX_
