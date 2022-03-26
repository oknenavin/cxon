// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TIDY_HXX_
#define CXON_JSON_TIDY_HXX_

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace json { // interface

    template <typename Tr, typename Out>
        struct indenter;

    template <typename Tr = format_traits, typename OutIt>
        constexpr auto make_indenter(OutIt o, unsigned tab = 1, char pad = '\t')        -> enable_if_t<is_output_iterator<OutIt>::value, indenter<JSON<Tr>, OutIt>>;
    template <typename Tr = format_traits, typename Insertable>
        constexpr auto make_indenter(Insertable& i, unsigned tab = 1, char pad = '\t')  -> enable_if_t<is_back_insertable<Insertable>::value, indenter<JSON<Tr>, Insertable&>>;
    template <typename Tr = format_traits, typename FwIt>
        constexpr auto make_indenter(FwIt b, FwIt e, unsigned tab = 1, char pad = '\t') -> indenter<JSON<Tr>, cnt::range_container<FwIt>>;

    template <typename Tr = format_traits, typename OutIt, typename InIt>
        inline auto tidy(OutIt o, InIt b, InIt e, unsigned tab = 1, char pad = '\t')    -> enable_if_t<is_output_iterator<OutIt>::value, bool>;
    template <typename Tr = format_traits, typename OutIt, typename Iterable>
        inline auto tidy(OutIt o, const Iterable& i, unsigned tab = 1, char pad = '\t') -> enable_if_t<is_output_iterator<OutIt>::value, bool>;
    template <typename Tr = format_traits, typename Result = std::string, typename InIt>
        inline auto tidy(InIt b, InIt e, unsigned tab = 1, char pad = '\t')             -> enable_if_t<is_back_insertable<Result>::value, Result>;
    template <typename Tr = format_traits, typename Result = std::string, typename Iterable>
        inline auto tidy(const Iterable& i, unsigned tab = 1, char pad = '\t')          -> enable_if_t<is_back_insertable<Result>::value, Result>;
    template <typename Tr = format_traits, typename FwIt, typename InIt>
        inline auto tidy(FwIt f, FwIt l, InIt b, InIt e, unsigned tab = 1, char pad = '\t')    -> bool;
    template <typename Tr = format_traits, typename FwIt, typename Iterable>
        inline auto tidy(FwIt f, FwIt l, const Iterable& i, unsigned tab = 1, char pad = '\t') -> bool;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace json {

    template <typename Tr, typename O>
        struct indenter<JSON<Tr>, O> {
            // pretend to be an iterator to satisfy the to_bytes interface
            using iterator_category = std::output_iterator_tag;
            using value_type        = char;
            using difference_type   = void;
            using pointer           = void;
            using reference         = void;
            using out_type = O;

            constexpr indenter(out_type o, unsigned tab = 1, char pad = '\t')
            :   o_(o), stt_(grn), lvl_(), tab_(tab), pad_(pad)
            {}

            bool append(char c) {
                using cio::poke;
                switch (stt()) {
                    case quo:
                        switch (c) {
                            case '"':   mut(grn); break;
                            case '\\':  mut(qes); break;
                        }
                        return poke(o_, c);
                    case qes:
                        return mut(quo), poke(o_, c);
                    case con:
                        switch (c) {
                            case 0x09: case 0x0A: case 0x0D: case 0x20:
                                return true;
                            case '}': case ']':
                                return mut(grn), poke(o_, c);
                            default:
                                mut(grn);
                                if (!poke(o_, '\n') || !poke(o_, lvl_ += tab_, pad_))
                                    return false;
                        }
                        CXON_FALLTHROUGH;
                    default:
                        switch (c) {
                            case '{': case '[':
                                return mut(con), poke(o_, c);
                            case '}': case ']':
                                return poke(o_, '\n') && poke(o_, (lvl_ ? lvl_ -= tab_ : 0), pad_) && poke(o_, c);
                            case ',':
                                return poke(o_, c) && poke(o_, '\n') && poke(o_, lvl_, pad_);
                            case ':':
                                return poke(o_, c) && poke(o_, ' ');
                            case '"':
                                return mut(quo), poke(o_, c);
                            case 0x09: case 0x0A: case 0x0D: case 0x20:
                                return true;
                            default:
                                return poke(o_, c);
                        }
                }
            }

            template <typename IV>
                bool indent_value(IV indent_value) {
                    return stt() == con ?
                        (mut(grn), cio::poke(o_, '\n') && cio::poke(o_, lvl_ += tab_, pad_)) && indent_value(o_, lvl_, tab_, pad_) :
                                                                                                indent_value(o_, lvl_, tab_, pad_)
                    ;
                }

            bool append(const char* f, const char* l) {
                return indent_value([&](out_type o, ...) {
                    return cio::poke(o, f, l);
                });
            }
            bool append(const char* s, std::size_t n) {
                return indent_value([&](out_type o, ...) {
                    return cio::poke(o, s, n);
                });
            }
            bool append(const char* s) {
                return indent_value([&](out_type o, ...) {
                    return cio::poke(o, s);
                });
            }
            bool append(unsigned n, char c) {
                return indent_value([&](out_type o, ...) {
                    return cio::poke(o, n, c);
                });
            }

            private:
                enum { grn, con, quo, qes };
                auto stt() const -> decltype(grn)   { return stt_; }
                void mut(decltype(grn) s)           { stt_ = s; }

            private:
                out_type        o_;
                decltype(grn)   stt_;
                unsigned        lvl_;
                unsigned const  tab_;
                char const      pad_;
        };

    template <typename Tr, typename OI>
        constexpr auto make_indenter(OI o, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, indenter<JSON<Tr>, OI>> {
            return indenter<JSON<Tr>, OI>{o, tab, pad};
        }
    template <typename Tr, typename I>
        constexpr auto make_indenter(I& i, unsigned tab, char pad) -> enable_if_t<is_back_insertable<I>::value, indenter<JSON<Tr>, I&>> {
            return indenter<JSON<Tr>, I&>{i, tab, pad};
        }
    template <typename Tr, typename FI>
        constexpr auto make_indenter(FI b, FI e, unsigned tab, char pad) -> indenter<JSON<Tr>, cnt::range_container<FI>> {
            using O = cnt::range_container<FI>;
            return indenter<JSON<Tr>, O>{cnt::make_range_container(b, e), tab, pad};
        }

    template <typename Tr, typename OI, typename II>
        inline auto tidy(OI o, II b, II e, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, bool> {
            auto i = make_indenter<JSON<Tr>>(o, tab, pad);
            for ( ; b != e && cio::poke(i, *b); ++b)
                ;
            return b == e;
        }
    template <typename Tr, typename OI, typename I>
        inline auto tidy(OI o, const I& i, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, bool> {
            return tidy<Tr>(o, std::begin(i), std::end(i), tab, pad);
        }

    template <typename Tr, typename R, typename II>
        inline auto tidy(II b, II e, unsigned tab, char pad) -> enable_if_t<is_back_insertable<R>::value, R> {
            R r;
                auto i = make_indenter<Tr>(r, tab, pad);
                for ( ; b != e && cio::poke(i, *b); ++b)
                    ;
            return r;
        }   // LCOV_EXCL_LINE: not covered with g++ > 8 and lcov 1.15
    template <typename Tr, typename R, typename I>
        inline auto tidy(const I& i, unsigned tab, char pad) -> enable_if_t<is_back_insertable<R>::value, R> {
            return tidy<Tr, R>(std::begin(i), std::end(i), tab, pad);
        }
        
    template <typename Tr, typename FwIt, typename InIt>
        inline bool tidy(FwIt f, FwIt l, InIt b, InIt e, unsigned tab, char pad) {
            auto i = make_indenter<Tr>(f, l, tab, pad);
            for ( ; b != e && cio::poke(i, *b); ++b)
                ;
            return b == e;
        }
    template <typename Tr, typename FwIt, typename Iterable>
        inline bool tidy(FwIt f, FwIt l, const Iterable& i, unsigned tab, char pad) {
            return tidy<Tr>(f, l, std::begin(i), std::end(i), tab, pad);
        }

}}

#ifdef CXON_JSON_NODE_DEFINED

    namespace cxon {

#       define CXON_JSON_NODE_RG()\
            cxon::node::imp::scinc_<Cx> RG__(cx);\
            if (!RG__.check()) return cx/cxon::node::error::recursion_depth_exceeded
#       define CXON_JSON_NODE_CHECK(e) if (!(e)) return false

        template <typename X, typename Tr, typename O, typename Cx> // indented write
            static bool write_value(json::indenter<X, O>& o, const json::basic_node<Tr>& t, Cx& cx) {
                return o.indent_value([&](typename json::indenter<X, O>::out_type out, unsigned& lvl, unsigned tab, char pad) {
                    using node = json::basic_node<Tr>;
                    switch (t.kind()) {
                        case json::node_kind::object: {
                            CXON_JSON_NODE_RG();
                            auto& j = json::get<typename node::object>(t);
                            if (j.empty()) return cio::poke(out, "{}");
                            auto i = std::begin(j);
                            lvl += tab;
                                CXON_JSON_NODE_CHECK((
                                    cio::poke(out, "{\n") && cio::poke(out, lvl, pad) &&
                                    cio::write_key<X>(out, i->first, cx), cio::poke(out, ' ') &&
                                        write_value<X>(o, i->second, cx)
                                ));
                                if (j.size() > 1) {
                                    auto const e = std::end(j);
                                    while (++i != e) {
                                        CXON_JSON_NODE_CHECK((
                                            cio::poke(out, ",\n") && cio::poke(out, lvl, pad) &&
                                            cio::write_key<X>(out, i->first, cx), cio::poke(out, ' ') &&
                                                write_value<X>(o, i->second, cx)
                                        ));
                                    }
                                }
                            lvl -= tab;
                            return cio::poke(out, '\n') && cio::poke(out, lvl, pad) && cio::poke(out, '}');
                        }
                        case json::node_kind::array: {
                            CXON_JSON_NODE_RG();
                            auto& j = json::get<typename node::array>(t);
                            if (j.empty()) return cio::poke(out, "[]");
                            auto i = std::begin(j);
                            lvl += tab;
                                CXON_JSON_NODE_CHECK((
                                    cio::poke(out, "[\n") && cio::poke(out, lvl, pad) &&
                                        write_value<X>(o, *i, cx)
                                ));
                                if (j.size() > 1) {
                                    auto const e = std::end(j);
                                    while (++i != e) {
                                        CXON_JSON_NODE_CHECK((
                                            cio::poke(out, ",\n") && cio::poke(out, lvl, pad) &&
                                                write_value<X>(o, *i, cx)
                                        ));
                                    }
                                }
                            lvl -= tab;
                            return cio::poke(out, '\n') && cio::poke(out, lvl, pad) && cio::poke(out, ']');
                        }
                        case json::node_kind::string:
                            return write_value<X>(out, json::get<typename node::string>(t), cx);
                        case json::node_kind::sint:
                            return write_value<X>(out, json::get<typename node::sint>(t), cx);
                        case json::node_kind::uint:
                            return write_value<X>(out, json::get<typename node::uint>(t), cx);
                        case json::node_kind::real:
                            return write_value<X>(out, json::get<typename node::real>(t), cx);
                        case json::node_kind::boolean:
                            return write_value<X>(out, json::get<typename node::boolean>(t), cx);
                        case json::node_kind::null:
                            return write_value<X>(out, json::get<typename node::null>(t), cx);
                    }
                    return true; // LCOV_EXCL_LINE
                });
            }

#       undef CXON_JSON_NODE_CHECK
#       undef CXON_JSON_NODE_RG

    }

#endif // CXON_JSON_NODE_DEFINED

#endif // CXON_JSON_TIDY_HXX_
