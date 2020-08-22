// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_PRETTY_HXX_
#define CXON_PRETTY_HXX_

// interface //////////////////////////////////////////////////////////////////

namespace cxon { // interface

    namespace bits {
        template <typename X, typename Out>
            struct indent_iterator;
    }
    template <typename X, typename Out>
        using indent_iterator = bits::indent_iterator<X, Out>;

    template <typename X = CXON_DEFAULT_FORMAT, typename OutIt>
        constexpr auto make_indenter(OutIt o, unsigned tab = 1, char pad = '\t')        -> enable_if_t<is_output_iterator<OutIt>::value, indent_iterator<X, OutIt>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Insertable>
        constexpr auto make_indenter(Insertable& i, unsigned tab = 1, char pad = '\t')  -> enable_if_t<is_back_insertable<Insertable>::value, indent_iterator<X, Insertable&>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename FwIt>
        constexpr auto make_indenter(FwIt b, FwIt e, unsigned tab = 1, char pad = '\t') -> indent_iterator<X, range_output_iterator<FwIt>>;

    template <typename X = CXON_DEFAULT_FORMAT, typename OutIt, typename InIt>
        inline auto pretty(OutIt o, InIt b, InIt e, unsigned tab = 1, char pad = '\t')      -> enable_if_t<is_output_iterator<OutIt>::value, void>;
    template <typename X = CXON_DEFAULT_FORMAT, typename OutIt, typename Iterable>
        inline auto pretty(OutIt o, const Iterable& i, unsigned tab = 1, char pad = '\t')   -> enable_if_t<is_output_iterator<OutIt>::value, void>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Result = std::string, typename InIt>
        inline auto pretty(InIt b, InIt e, unsigned tab = 1, char pad = '\t')               -> enable_if_t<is_back_insertable<Result>::value, Result>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Result = std::string, typename Iterable>
        inline auto pretty(const Iterable& i, unsigned tab = 1, char pad = '\t')            -> enable_if_t<is_back_insertable<Result>::value, Result>;

}

// implementation /////////////////////////////////////////////////////////////

namespace cxon {

    namespace bits {

        template <typename X, typename O>
            struct indent_iterator {
                using iterator_category = std::output_iterator_tag;
                using value_type        = char;
                using difference_type   = void;
                using pointer           = void;
                using reference         = void;

                using out_type          = O;

                constexpr indent_iterator(out_type o, unsigned tab = 1, char pad = '\t') : o_(o), stt_(grn), lvl_(), tab_(tab), pad_(pad) {}

                indent_iterator& operator ++() noexcept { return *this; }
                indent_iterator& operator *() noexcept { return *this; }

#               if defined(__GNUG__) && !defined(__clang__) && __cplusplus < 201703L
#                   pragma GCC diagnostic push
#                   pragma GCC diagnostic ignored "-Wimplicit-fallthrough="
#               endif

                indent_iterator& operator =(char c) {
                    using chio::poke;
                    switch (stt()) {
                        case quo:
                                              poke(o_, c);
                            if (is::que(c)) { mut(grn);     break; }
                            if (is::esc(c)) { mut(qes);     break; }
                            break;
                        case qes:
                                              mut(quo), poke(o_, c);
                            break;
                        case ges:
                                              mut(grn), poke(o_, c);
                            break;
                        case con:
                            if (is::spc(c))                                                     break;
                                              mut(grn);
                            if (is::end(c)) { poke(o_, c);                                      break; }
                                        if  (!poke(o_, '\n') || !poke(o_, lvl_ += tab_, pad_))  break;
#                           if __cplusplus >= 201703L
                                [[fallthrough]];
#                           endif
                        default:
                            if (is::beg(c)) { mut(con), poke(o_, c);                                                                break; }
                            if (is::end(c)) {           poke(o_, '\n') && poke(o_, (lvl_ ? lvl_ -= tab_ : 0), pad_) && poke(o_, c); break; }
                            if (is::sep(c)) {           poke(o_, c) && poke(o_, '\n') && poke(o_, lvl_, pad_);                      break; }
                            if (is::map(c)) {           poke(o_, c) && poke(o_, ' ');                                               break; }
                            if (is::qub(c)) { mut(quo), poke(o_, c);                                                                break; }
                            if (is::esc(c)) { mut(ges), poke(o_, c);                                                                break; }
                            if (is::nsp(c)) {           poke(o_, c);                                                                break; }
                    }
                    return *this;
                }

#               if defined(__GNUG__) && !defined(__clang__) && __cplusplus < 201703L
#                   pragma GCC diagnostic pop
#               endif

                template <typename VI>
                    bool indent_value(VI indent_value) {
                        if (stt() == con) {
                            mut(grn), chio::poke(o_, '\n') && chio::poke(o_, lvl_ += tab_, pad_);
                        }
                        return indent_value(o_, lvl_, tab_, pad_);
                    }

#               define HAS_METH_DEF(name, M)\
                    template <typename, typename = void>\
                        struct has_##name : std::false_type {};\
                    template <typename T>\
                        struct has_##name<T, enable_if_t<std::is_same<void, decltype(std::declval<T>().M, void())>::value>> : std::true_type {}

                HAS_METH_DEF(append_sn, append(std::declval<char*>(), 0));
                    template <typename S = out_type>
                        auto append(const char* s, size_t n) -> enable_if_t<has_append_sn<S>::value> {
                            indent_value([&](out_type out, unsigned&, unsigned, char) {
                                return chio::poke(out, s, n);
                            });
                        }
                HAS_METH_DEF(append_s, append(std::declval<char*>()));
                    template <typename S = out_type>
                        auto append(const char* s) -> enable_if_t<has_append_s<S>::value> {
                            indent_value([&](out_type out, unsigned&, unsigned, char) {
                                return chio::poke(out, s);
                            });
                        }

                HAS_METH_DEF(good, good());
                HAS_METH_DEF(bool, operator bool());
                    template <typename S = out_type>
                        auto good() const noexcept -> enable_if_t<!has_bool<S>::value && has_good<S>::value, bool> { return o_.good(); }
                    template <typename S = out_type>
                        auto good() const noexcept -> enable_if_t< has_bool<S>::value, bool> { return o_; }

#               undef HAS_METH_DEF

            private:
                struct is {
                    static bool beg(char c) { return c == X::map::beg || c == X::list::beg; }
                    static bool end(char c) { return c == X::map::end || c == X::list::end; }
                    static bool sep(char c) { return c == X::map::sep || c == X::list::sep; }
                    static bool map(char c) { return c == X::map::div; }
                    static bool qub(char c) { return c == X::string::beg; }
                    static bool que(char c) { return c == X::string::end; }
                    static bool esc(char c) { return c == '\\'; }
                    static bool spc(char c) { return chio::is<X>::space(c); }
                    static bool nsp(char c) { return !spc(c); }
                };

                enum { grn, con, quo, qes, ges };
                auto stt() const -> decltype(grn)   { return stt_; }
                void mut(decltype(grn) s)           { stt_ = s; }

            private:
                out_type        o_;
                decltype(grn)   stt_;
                unsigned        lvl_;
                unsigned const  tab_;
                char const      pad_;
            };

    }

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
        inline auto pretty(OI o, II b, II e, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, void> {
            auto i = make_indenter<X>(o, tab, pad);
            for ( ; b != e; ++b) *i = *b;
        }
    template <typename X, typename OI, typename I>
        inline auto pretty(OI o, const I& i, unsigned tab, char pad) -> enable_if_t<is_output_iterator<OI>::value, void> {
            pretty<X>(o, std::begin(i), std::end(i), tab, pad);
        }
    template <typename X, typename R, typename II>
        inline auto pretty(II b, II e, unsigned tab, char pad) -> enable_if_t<is_back_insertable<R>::value, R> {
            R r;
                auto i = make_indenter<X>(r, tab, pad);
                for ( ; b != e; ++b) *i = *b;
            return r;
        }
    template <typename X, typename R, typename I>
        inline auto pretty(const I& i, unsigned tab, char pad) -> enable_if_t<is_back_insertable<R>::value, R> {
            return pretty<X, R>(std::begin(i), std::end(i), tab, pad);
        }

}

#endif // CXON_PRETTY_HXX_
