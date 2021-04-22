// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TIDY_HXX_
#define CXON_JSON_TIDY_HXX_

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace json { // interface

    template <typename X, typename Out>
        struct indent_iterator;

    template <typename X = CXON_DEFAULT_FORMAT, typename OutIt>
        constexpr auto make_indenter(OutIt o, unsigned tab = 1, char pad = '\t')        -> enable_if_t<is_output_iterator<OutIt>::value, indent_iterator<X, OutIt>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Insertable>
        constexpr auto make_indenter(Insertable& i, unsigned tab = 1, char pad = '\t')  -> enable_if_t<is_back_insertable<Insertable>::value, indent_iterator<X, Insertable&>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename FwIt>
        constexpr auto make_indenter(FwIt b, FwIt e, unsigned tab = 1, char pad = '\t') -> indent_iterator<X, range_output_iterator<FwIt>>;

    template <typename X = CXON_DEFAULT_FORMAT, typename OutIt, typename InIt>
        inline auto tidy(OutIt o, InIt b, InIt e, unsigned tab = 1, char pad = '\t')    -> enable_if_t<is_output_iterator<OutIt>::value, void>;
    template <typename X = CXON_DEFAULT_FORMAT, typename OutIt, typename Iterable>
        inline auto tidy(OutIt o, const Iterable& i, unsigned tab = 1, char pad = '\t') -> enable_if_t<is_output_iterator<OutIt>::value, void>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Result = std::string, typename InIt>
        inline auto tidy(InIt b, InIt e, unsigned tab = 1, char pad = '\t')             -> enable_if_t<is_back_insertable<Result>::value, Result>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Result = std::string, typename Iterable>
        inline auto tidy(const Iterable& i, unsigned tab = 1, char pad = '\t')          -> enable_if_t<is_back_insertable<Result>::value, Result>;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace json {

    template <typename X, typename O>
        struct indent_iterator {
            using iterator_category = std::output_iterator_tag;
            using value_type        = char;
            using difference_type   = void;
            using pointer           = void;
            using reference         = void;

            using out_type          = O;

            constexpr indent_iterator(out_type o, unsigned tab = 1, char pad = '\t')
            :   o_(o), stt_(grn), lvl_(), tab_(tab), pad_(pad)
            {}

            indent_iterator& operator ++() noexcept { return *this; }
            indent_iterator& operator *() noexcept  { return *this; }

            indent_iterator& operator =(char c) {
                using cio::poke;
                switch (stt()) {
                    case quo:
                        poke(o_, c);
                        switch (c) {
                            case '"':   mut(grn); break;
                            case '\\':  mut(qes); break;
                        }
                        break;
                    case qes:
                        mut(quo), poke(o_, c);
                        break;
                    case con:
                        switch (c) {
                            case 0x09: case 0x0A: case 0x0D: case 0x20:
                                goto ret;
                            case '}': case ']':
                                mut(grn), poke(o_, c);
                                goto ret;
                            default:
                                mut(grn);
                                if (!poke(o_, '\n') || !poke(o_, lvl_ += tab_, pad_))
                                    goto ret;
                        }
                        CXON_FALLTHROUGH;
                    default:
                        switch (c) {
                            case '{': case '[':
                                mut(con), poke(o_, c);
                                break;
                            case '}': case ']':
                                poke(o_, '\n') && poke(o_, (lvl_ ? lvl_ -= tab_ : 0), pad_) && poke(o_, c);
                                break;
                            case ',':
                                poke(o_, c) && poke(o_, '\n') && poke(o_, lvl_, pad_);
                                break;
                            case ':':
                                poke(o_, c) && poke(o_, ' ');
                                break;
                            case '"':
                                mut(quo), poke(o_, c);
                                break;
                            case 0x09: case 0x0A: case 0x0D: case 0x20:
                                break;
                            default:
                                poke(o_, c);
                        }
                }
                ret: return *this;
            }

            template <typename VI>
                bool indent_value(VI indent_value) {
                    return stt() == con ?
                        (mut(grn), cio::poke(o_, '\n') && cio::poke(o_, lvl_ += tab_, pad_)) && indent_value(o_, lvl_, tab_, pad_) :
                                                                                                indent_value(o_, lvl_, tab_, pad_)
                    ;
                }

            template <typename S = out_type>
                auto append(const char* s, size_t n)    -> decltype(std::declval<S>().append(s, n), void()) {
                    indent_value([&](out_type out, ...) {
                        return cio::poke(out, s, n);
                    });
                }
            template <typename S = out_type>
                auto append(const char* s)              -> decltype(std::declval<S>().append(s), void()) {
                    indent_value([&](out_type out, ...) {
                        return cio::poke(out, s);
                    });
                }

            template <typename S = out_type>
                auto good() const noexcept -> decltype(std::declval<S>().good(), bool())            { return o_.good(); }
            template <typename S = out_type>
                auto good() const noexcept -> decltype(std::declval<S>().operator bool(), bool())   { return o_; }

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

    template <typename X, typename OI>
        constexpr auto make_indenter(OI o, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, indent_iterator<X, OI>> {
            return indent_iterator<X, OI>{o, tab, pad};
        }
    template <typename X, typename I>
        constexpr auto make_indenter(I& i, unsigned tab, char pad) -> enable_if_t<is_back_insertable<I>::value, indent_iterator<X, I&>> {
            return indent_iterator<X, I&>{i, tab, pad};
        }
    template <typename X, typename FI>
        constexpr auto make_indenter(FI b, FI e, unsigned tab, char pad) -> indent_iterator<X, range_output_iterator<FI>> {
            using O = range_output_iterator<FI>;
            return indent_iterator<X, O>{make_output_iterator(b, e), tab, pad};
        }

    template <typename X, typename OI, typename II>
        inline auto tidy(OI o, II b, II e, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, void> {
            auto i = make_indenter<X>(o, tab, pad);
            for ( ; b != e && cio::poke(i, *b); ++b)
                ;
        }
    template <typename X, typename OI, typename I>
        inline auto tidy(OI o, const I& i, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, void> {
            tidy<X>(o, std::begin(i), std::end(i), tab, pad);
        }
    template <typename X, typename R, typename II>
        inline auto tidy(II b, II e, unsigned tab, char pad) -> enable_if_t<is_back_insertable<R>::value, R> {
            R r;
                auto i = make_indenter<X>(r, tab, pad);
                for ( ; b != e && cio::poke(i, *b); ++b)
                    ;
            return r;
        }
    template <typename X, typename R, typename I>
        inline auto tidy(const I& i, unsigned tab, char pad) -> enable_if_t<is_back_insertable<R>::value, R> {
            return tidy<X, R>(std::begin(i), std::end(i), tab, pad);
        }

}}

#ifdef CXON_JSON_NODE_DEFINED

    namespace cxon {

#       define CXON_JSON_NODE_RG()\
            cxon::node::imp::scinc<Cx> RG__(cx);\
            if (!RG__.check()) return cx/cxon::node::error::recursion_depth_exceeded
#       define CXON_JSON_NODE_CHECK(e) if (!(e)) return false

        template <typename X, typename Tr, typename O, typename Cx> // indented write
            static bool write_value(json::indent_iterator<X, O>& o, const json::basic_node<Tr>& t, Cx& cx) {
                return o.indent_value([&](typename json::indent_iterator<X, O>::out_type out, unsigned& lvl, unsigned tab, char pad) {
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
                        case json::node_kind::number:
                            return write_value<X>(out, json::get<typename node::number>(t), cx);
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
