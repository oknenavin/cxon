// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXJSON_PRETTY_HXX_
#define CXJSON_PRETTY_HXX_

// interface //////////////////////////////////////////////////////////////////

namespace cxjson {

    template <typename X = cxon::JSON<struct format_traits>, typename R = std::string, typename Tr>
        inline auto pretty(const basic_node<Tr>& n, unsigned tab = 1, char pad = '\t') -> cxon::enable_if_t<cxon::is_back_insertable<R>::value, R>;

}

// implementation /////////////////////////////////////////////////////////////

namespace cxjson {

    namespace bits {

        template <typename X, typename O>
            struct pretty {
                template <typename Tr>
                    static bool value(O o, const basic_node<Tr>& n, unsigned tab, char pad, unsigned lvl = 0) {
#                       define CXJSON_CHECK(e) if (!(e)) return false
                        using namespace cxon;
                        using json = basic_node<Tr>;
                        static wctx<X> ctx;
                        switch (n.type()) {
                            case node_type::object: {
                                auto& j = n.template get<typename json::object>();
                                if (j.empty()) return io::poke(o, "{}");
                                auto i = std::begin(j);
                                CXJSON_CHECK((
                                    io::poke(o, "{\n") &&
                                    io::poke(o, lvl + 1, pad) &&
                                        write_key<X>(o, i->first, ctx), io::poke(o, ' ') &&
                                        pretty<X, O>::value(o, i->second, tab, pad, lvl + 1)
                                ));
                                if (j.size() > 1) {
                                    auto const e = std::end(j);
                                    while (++i != e) {
                                        CXJSON_CHECK((
                                            io::poke(o, ",\n") &&
                                            io::poke(o, lvl + 1, pad) &&
                                                write_key<X>(o, i->first, ctx), io::poke(o, ' ') &&
                                                pretty<X, O>::value(o, i->second, tab, pad, lvl + 1)
                                        ));
                                    }
                                }
                                return io::poke(o, '\n') && io::poke(o, lvl, pad) && io::poke(o, '}');
                            }
                            case node_type::array: {
                                auto& j = n.template get<typename json::array>();
                                if (j.empty()) return io::poke(o, "[]");
                                auto i = std::begin(j);
                                CXJSON_CHECK((
                                    io::poke(o, "[\n") &&
                                    io::poke(o, lvl + 1, pad) &&
                                        pretty<X, O>::value(o, *i, tab, pad, lvl + 1)
                                ));
                                if (j.size() > 1) {
                                    auto const e = std::end(j);
                                    while (++i != e) {
                                        CXJSON_CHECK((
                                            io::poke(o, ",\n") &&
                                            io::poke(o, lvl + 1, pad) &&
                                                pretty<X, O>::value(o, *i, tab, pad, lvl + 1)
                                        ));
                                    }
                                }
                                return io::poke(o, '\n') && io::poke(o, lvl, pad) && io::poke(o, ']');
                            }
                            case node_type::string:
                                return write_value<X>(o, n.template get<typename json::string>(), ctx);
                            case node_type::number:
                                return write_value<X>(o, n.template get<typename json::number>(), ctx);
                            case node_type::boolean:
                                return write_value<X>(o, n.template get<typename json::boolean>(), ctx);
                            case node_type::null:
                                return write_value<X>(o, n.template get<typename json::null>(), ctx);
                            default:
                                return true;
                        }
#                       undef CXJSON_CHECK
                    }
            };

    }

    template <typename X, typename R, typename Tr>
        inline auto pretty(const basic_node<Tr>& n, unsigned tab, char pad) -> cxon::enable_if_t<cxon::is_back_insertable<R>::value, R> {
            R r;
                bits::pretty<X, R&>::value(r, n, tab, pad);
            return r;
        }

}

#endif // CXJSON_PRETTY_HXX_
