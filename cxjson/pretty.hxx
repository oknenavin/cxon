// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXJSON_PRETTY_HXX_
#define CXJSON_PRETTY_HXX_

// interface //////////////////////////////////////////////////////////////////

namespace cxjson {

    template <typename X = cxon::JSON<>, typename R = std::string, typename Tr, typename ...CxPs>
        inline auto pretty(const basic_node<Tr>& n, CxPs... p) -> cxon::enable_if_t<cxon::is_back_insertable<R>::value, R>;

    CXON_PARAMETER(tab); // unsigned/constexpr [1U]: number of pads to indent with
    CXON_PARAMETER(pad); // char/constexpr ['\t']: indent character

}

// implementation /////////////////////////////////////////////////////////////

namespace cxjson {

    namespace bits {

#       define CXJSON_CHECK(e) if (!(e)) return false

            template <typename X, typename O>
                struct pretty {
                    template <typename Tr, typename Cx>
                        static bool value(O o, const basic_node<Tr>& n, unsigned lvl, Cx& cx) {
                            using namespace cxon;
                            using json = basic_node<Tr>;
                            constexpr auto tab = tab::constant<prms_type<Cx>>(1);
                            constexpr auto pad = pad::constant<prms_type<Cx>>('\t');
                            switch (n.kind()) {
                                case node_kind::object: {
                                    auto& j = get<typename json::object>(n);
                                    if (j.empty()) return io::poke(o, "{}");
                                    auto i = std::begin(j);
                                    CXJSON_CHECK((
                                        io::poke(o, "{\n") &&
                                        io::poke(o, lvl + tab, pad) &&
                                            write_key<X>(o, i->first, cx), io::poke(o, ' ') &&
                                            pretty<X, O>::value(o, i->second, lvl + tab, cx)
                                    ));
                                    if (j.size() > 1) {
                                        auto const e = std::end(j);
                                        while (++i != e) {
                                            CXJSON_CHECK((
                                                io::poke(o, ",\n") &&
                                                io::poke(o, lvl + tab, pad) &&
                                                    write_key<X>(o, i->first, cx), io::poke(o, ' ') &&
                                                    pretty<X, O>::value(o, i->second, lvl + tab, cx)
                                            ));
                                        }
                                    }
                                    return io::poke(o, '\n') && io::poke(o, lvl, pad) && io::poke(o, '}');
                                }
                                case node_kind::array: {
                                    auto& j = get<typename json::array>(n);
                                    if (j.empty()) return io::poke(o, "[]");
                                    auto i = std::begin(j);
                                    CXJSON_CHECK((
                                        io::poke(o, "[\n") &&
                                        io::poke(o, lvl + tab, pad) &&
                                            pretty<X, O>::value(o, *i, lvl + tab, cx)
                                    ));
                                    if (j.size() > 1) {
                                        auto const e = std::end(j);
                                        while (++i != e) {
                                            CXJSON_CHECK((
                                                io::poke(o, ",\n") &&
                                                io::poke(o, lvl + tab, pad) &&
                                                    pretty<X, O>::value(o, *i, lvl + tab, cx)
                                            ));
                                        }
                                    }
                                    return io::poke(o, '\n') && io::poke(o, lvl, pad) && io::poke(o, ']');
                                }
                                case node_kind::string:
                                    return write_value<X>(o, get<typename json::string>(n), cx);
                                case node_kind::number:
                                    return write_value<X>(o, get<typename json::number>(n), cx);
                                case node_kind::boolean:
                                    return write_value<X>(o, get<typename json::boolean>(n), cx);
                                case node_kind::null:
                                    return write_value<X>(o, get<typename json::null>(n), cx);
                            }
                            return true; // LCOV_EXCL_LINE
                        }
                };

#           undef CXJSON_CHECK

    }

    template <typename X, typename R, typename Tr, typename ...CxPs>
        inline auto pretty(const basic_node<Tr>& n, CxPs... p) -> cxon::enable_if_t<cxon::is_back_insertable<R>::value, R> {
            R r;
                cxon::write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                bits::pretty<X, R&>::value(r, n, 0, cx);
            return r;
        }

}

#endif // CXJSON_PRETTY_HXX_
