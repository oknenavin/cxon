// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TIDY_HXX_
#define CXON_JSON_TIDY_HXX_

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace json { // interface

    template <typename X, typename Out>
        struct indenter;

    template <typename X = format_traits, typename OutIt>
        constexpr auto make_indenter(OutIt o, unsigned tab = 1, char pad = '\t')        -> enable_if_t<is_output_iterator<OutIt>::value, indenter<JSON<X>, OutIt>>;
    template <typename X = format_traits, typename Insertable>
        constexpr auto make_indenter(Insertable& i, unsigned tab = 1, char pad = '\t')  -> enable_if_t<is_back_insertable<Insertable>::value, indenter<JSON<X>, Insertable&>>;
    template <typename X = format_traits, typename FwIt>
        constexpr auto make_indenter(FwIt b, FwIt e, unsigned tab = 1, char pad = '\t') -> indenter<JSON<X>, cnt::range_container<FwIt>>;

    template <typename X = format_traits, typename OutIt, typename InIt>
        inline auto tidy(OutIt o, InIt b, InIt e, unsigned tab = 1, char pad = '\t')    -> enable_if_t<is_output_iterator<OutIt>::value, bool>;
    template <typename X = format_traits, typename OutIt, typename Iterable>
        inline auto tidy(OutIt o, const Iterable& i, unsigned tab = 1, char pad = '\t') -> enable_if_t<is_output_iterator<OutIt>::value, bool>;
    template <typename X = format_traits, typename Result = std::string, typename InIt>
        inline auto tidy(InIt b, InIt e, unsigned tab = 1, char pad = '\t')             -> enable_if_t<is_back_insertable<Result>::value, Result>;
    template <typename X = format_traits, typename Result = std::string, typename Iterable>
        inline auto tidy(const Iterable& i, unsigned tab = 1, char pad = '\t')          -> enable_if_t<is_back_insertable<Result>::value, Result>;
    template <typename X = format_traits, typename FwIt, typename InIt>
        inline auto tidy(FwIt f, FwIt l, InIt b, InIt e, unsigned tab = 1, char pad = '\t')    -> bool;
    template <typename X = format_traits, typename FwIt, typename Iterable>
        inline auto tidy(FwIt f, FwIt l, const Iterable& i, unsigned tab = 1, char pad = '\t') -> bool;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace json {

    template <typename X, typename OI>
        constexpr auto make_indenter(OI o, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, indenter<JSON<X>, OI>> {
            return indenter<JSON<X>, OI>{o, tab, pad};
        }
    template <typename X, typename I>
        constexpr auto make_indenter(I& i, unsigned tab, char pad) -> enable_if_t<is_back_insertable<I>::value, indenter<JSON<X>, I&>> {
            return indenter<JSON<X>, I&>{i, tab, pad};
        }
    template <typename X, typename FI>
        constexpr auto make_indenter(FI b, FI e, unsigned tab, char pad) -> indenter<JSON<X>, cnt::range_container<FI>> {
            using O = cnt::range_container<FI>;
            return indenter<JSON<X>, O>{cnt::make_range_container(b, e), tab, pad};
        }

    template <typename X, typename OI, typename II>
        inline auto tidy(OI o, II b, II e, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, bool> {
            auto i = make_indenter<JSON<X>>(o, tab, pad);
            for ( ; b != e && cio::poke(i, *b); ++b)
                ;
            return b == e;
        }
    template <typename X, typename OI, typename I>
        inline auto tidy(OI o, const I& i, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, bool> {
            return tidy<X>(o, std::begin(i), std::end(i), tab, pad);
        }

    template <typename X, typename R, typename II>
        inline auto tidy(II b, II e, unsigned tab, char pad) -> enable_if_t<is_back_insertable<R>::value, R> {
            R r;
                auto i = make_indenter<X>(r, tab, pad);
                for ( ; b != e && cio::poke(i, *b); ++b)
                    ;
            return r;
        }   // LCOV_EXCL_LINE: not covered with g++ > 8 and lcov 1.15
    template <typename X, typename R, typename I>
        inline auto tidy(const I& i, unsigned tab, char pad) -> enable_if_t<is_back_insertable<R>::value, R> {
            return tidy<X, R>(std::begin(i), std::end(i), tab, pad);
        }
        
    template <typename X, typename FwIt, typename InIt>
        inline bool tidy(FwIt f, FwIt l, InIt b, InIt e, unsigned tab, char pad) {
            auto i = make_indenter<X>(f, l, tab, pad);
            for ( ; b != e && cio::poke(i, *b); ++b)
                ;
            return b == e;
        }
    template <typename X, typename FwIt, typename Iterable>
        inline bool tidy(FwIt f, FwIt l, const Iterable& i, unsigned tab, char pad) {
            return tidy<X>(f, l, std::begin(i), std::end(i), tab, pad);
        }

    template <typename X, typename O>
        struct indenter<JSON<X>, O> {
            using iterator_category = std::output_iterator_tag;
            using value_type        = char;
            using difference_type   = void;
            using pointer           = void;
            using reference         = void;
            using out_type = O;

            constexpr indenter(out_type o, unsigned tab = 1, char pad = '\t')
            :   o_(o), cur_(), pre_(), lvl_(), tab_(tab), pad_(pad)
            {}

            bool append(char c) {
                using cio::poke;
                switch (cur()) {
                    case stt::grn: break;
                    case stt::lst:
                        switch (c) {
                            case '\t': case '\n': case '\r': case ' ': case '\0':
                                return true;
                            case '}': case ']':
                                return mut(stt::scl),   poke(o_, c);
                        }
                        mut(stt::grn);
                        if (!poke(o_, '\n') || !poke(o_, lvl_ += tab_, pad_))
                            return false;
                        break;
                    case stt::str:
                        switch (c) {
                            case '"' : mut(stt::scl); break;
                            case '\\': mut(stt::esc); break;
                        }
                        return poke(o_, c);
                    case stt::scl:
                        switch (c) {
                            case '\t': case '\n': case '\r': case ' ': case '\0':
                                return mut(stt::grn),   true;
                            case '\\':
                                return mut(stt::esc),   poke(o_, '\\');
                            default:
                                return poke(o_, c);
                            case '}': case ']':// case '"':
                                break;
                            case ',': case ':':
                                mut(stt::grn); break;
                        }
                        break;
                    case stt::esc:
                        return mut(pre()),  poke(o_, c);
                    case stt::com:
                        switch (c) {
                            case '\t': case '\n': case '\r': case ' ': case '\0':
                                return true;
                            case '}': case ']':
                                break;
                            default:
                                mut(stt::grn),   poke(o_, '\n') && poke(o_, lvl_, pad_);
                        }
                        break;
                }
                stt nxt;
                switch (c) {
                    case '\t': case '\n': case '\r': case ' ': case '\0':
                        return true;
                    case '{': case '[':
                        nxt = stt::lst; goto scl;
                    case '}': case ']':
                        return mut(stt::scl),   poke(o_, '\n') && poke(o_, (lvl_ ? lvl_ -= tab_ : 0), pad_) && poke(o_, c);
                    case '"':
                        nxt = stt::str; goto scl;
                    case ',':
                        return mut(stt::com),   poke(o_, c);
                    case ':':
                        return mut(stt::grn),   poke(o_, c) && poke(o_, ' ');
                    default:
                        nxt = stt::scl;// goto scl;
                }
                scl: return pre() == stt::scl ?
                    (mut(nxt),  poke(o_, '\n') && poke(o_, lvl_, pad_)   && poke(o_, c)) :
                    (mut(nxt),                                              poke(o_, c))
                ;
            }

            template <typename A>
                bool append(A append) {
                    using cio::poke;
                    switch (cur()) {
                        case stt::lst:
                            return  (mut(stt::scl),     poke(o_, '\n') && poke(o_, lvl_ += tab_, pad_)   && append(o_));
                        case stt::str:
                            return  (                                                                       append(o_));
                        case stt::com:
                            return  (mut(stt::grn),     poke(o_, '\n') && poke(o_, lvl_, pad_)           && append(o_));
                        default:
                            return pre() == stt::scl ?
                                    (mut(stt::scl),     poke(o_, '\n') && poke(o_, lvl_, pad_)           && append(o_)) :
                                    (mut(stt::scl),                                                         append(o_))
                            ;
                    }
                }

            bool append(const char* f, const char* l) {
                return append([&](out_type o) {
                    return cio::poke(o, f, l);
                });
            }
            bool append(const char* s, std::size_t n) {
                return append([&](out_type o) {
                    return cio::poke(o, s, n);
                });
            }
            bool append(const char* s) {
                return append([&](out_type o) {
                    return cio::poke(o, s);
                });
            }
            bool append(unsigned n, char c) {
                return append([&](out_type o) {
                    return cio::poke(o, n, c);
                });
            }

            private:
                enum class stt { grn, lst, str, scl, esc, com };
                stt cur() const noexcept    { return cur_; }
                stt pre() const noexcept    { return pre_; }
                void mut(stt s) noexcept    { pre_ = cur_, cur_ = s; }

            private:
                out_type        o_;
                stt             cur_;
                stt             pre_;
                unsigned        lvl_;
                unsigned const  tab_;
                char const      pad_;
        };

}}

#endif // CXON_JSON_TIDY_HXX_
