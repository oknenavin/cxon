// Copyright (c) 2017-2019 oknenavin.
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

    template <typename X = JSON<>, typename Out>
        constexpr auto make_indenter(Out o, unsigned tab = 1, char pad = '\t')          -> enable_if_t<is_output_iterator<Out>::value, indent_iterator<X, Out>>;
    template <typename X = JSON<>, typename C>
        constexpr auto make_indenter(C& c, unsigned tab = 1, char pad = '\t')           -> enable_if_t<is_back_insertable<C>::value, indent_iterator<X, C&>>;
    template <typename X = JSON<>, typename FwIt>
        constexpr auto make_indenter(FwIt b, FwIt e, unsigned tab = 1, char pad = '\t') -> indent_iterator<X, io::output_iterator<FwIt>>;

    template <typename X = JSON<>, typename R = std::string, typename InIt>
        inline auto pretty(InIt b, InIt e, unsigned tab = 1, char pad = '\t')   -> enable_if_t<is_back_insertable<R>::value, R>;
    template <typename X = JSON<>, typename R = std::string, typename C>
        inline auto pretty(const C& c, unsigned tab = 1, char pad = '\t')       -> enable_if_t<is_back_insertable<R>::value, R>;

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

                constexpr indent_iterator(O o, unsigned tab = 1, char pad = '\t') : o_(o), stt_(grn), lvl_(), tab_(tab), pad_(pad) {}

                indent_iterator& operator ++() noexcept { return *this; }
                indent_iterator& operator *() noexcept { return *this; }

                indent_iterator& operator =(char c) {
                    switch (stt()) {
                        case quo:
                                              io::poke(o_, c);
                            if (is::que(c)) { mut(grn);     break; }
                            if (is::esc(c)) { mut(qes);     break; }
                            break;
                        case qes:
                                              mut(quo), io::poke(o_, c);
                            break;
                        case ges:
                                              mut(grn), io::poke(o_, c);
                            break;
                        case con:
                            if (is::spc(c))                                                             break;
                                              mut(grn);
                            if (is::end(c)) { io::poke(o_, c);                                          break; }
                                        if  (!io::poke(o_, '\n') || !io::poke(o_, lvl_ += tab_, pad_))  break;
#                           if __cplusplus >= 201703L
                                [[fallthrough]];
#                           endif
                        default:
                            if (is::beg(c)) { mut(con), io::poke(o_, c);                                                                        break; }
                            if (is::end(c)) {           io::poke(o_, '\n') && io::poke(o_, (lvl_ ? lvl_ -= tab_ : 0), pad_) && io::poke(o_, c); break; }
                            if (is::sep(c)) {           io::poke(o_, c) && io::poke(o_, '\n') && io::poke(o_, lvl_, pad_);                      break; }
                            if (is::map(c)) {           io::poke(o_, c) && io::poke(o_, ' ');                                                   break; }
                            if (is::qub(c)) { mut(quo), io::poke(o_, c);                                                                        break; }
                            if (is::esc(c)) { mut(ges), io::poke(o_, c);                                                                        break; }
                            if (is::nsp(c)) {           io::poke(o_, c);                                                                        break; }
                    }
                    return *this;
                }

#               define HAS_METH_DEF(name, M)\
                    template <typename, typename = void>\
                        struct has_##name : std::false_type {};\
                    template <typename T>\
                        struct has_##name<T, enable_if_t<std::is_same<void, decltype(std::declval<T>().M, void())>::value>> : std::true_type {}

                HAS_METH_DEF(bool, operator bool());
                HAS_METH_DEF(good, good());
                template <typename S = O>
                    auto good() const noexcept -> enable_if_t<!has_bool<S>::value && has_good<S>::value, bool> { return o_.good(); }
                template <typename S = O>
                    auto good() const noexcept -> enable_if_t< has_bool<S>::value, bool> { return o_; }

#               undef HAS_METH_DEF

            protected:
                struct is {
                    static bool beg(char c) { return c == X::map::beg || c == X::list::beg; }
                    static bool end(char c) { return c == X::map::end || c == X::list::end; }
                    static bool sep(char c) { return c == X::map::sep || c == X::list::sep; }
                    static bool map(char c) { return c == X::map::div; }
                    static bool qub(char c) { return c == X::string::beg; }
                    static bool que(char c) { return c == X::string::end; }
                    static bool esc(char c) { return c == '\\'; }
                    static bool spc(char c) { return cxon::bits::is<X>::space(c); }
                    static bool nsp(char c) { return !spc(c); }
                };

                enum { grn, con, quo, qes, ges };
                auto stt() const -> decltype(grn)   { return stt_; }
                void mut(decltype(grn) s)           { stt_ = s; }

            //protected:
            public:
                O               o_;
                decltype(grn)   stt_;
                unsigned        lvl_;
                unsigned const  tab_;
                char const      pad_;
            };

    }

    template <typename X, typename O>
        constexpr auto make_indenter(O o, unsigned tab, char pad) -> enable_if_t<is_output_iterator<O>::value, indent_iterator<X, O>> {
            return indent_iterator<X, O>{o, tab, pad};
        }
    template <typename X, typename C>
        constexpr auto make_indenter(C& c, unsigned tab, char pad) -> enable_if_t<is_back_insertable<C>::value, indent_iterator<X, C&>> {
            return indent_iterator<X, C&>{c, tab, pad};
        }
    template <typename X, typename FwIt>
        constexpr auto make_indenter(FwIt b, FwIt e, unsigned tab, char pad) -> indent_iterator<X, io::output_iterator<FwIt>> {
            using O = io::output_iterator<FwIt>;
            return indent_iterator<X, O>{io::make_output_iterator(b, e), tab, pad};
        }

    template <typename X, typename R, typename InIt>
        inline auto pretty(InIt b, InIt e, unsigned tab, char pad) -> enable_if_t<is_back_insertable<R>::value, R> {
            std::string s;
                auto i = make_indenter<X>(s, tab, pad);
                for ( ; b != e; ++b) *i = *b;
            return s;
        }
    template <typename X, typename R, typename C>
        inline auto pretty(const C& c, unsigned tab, char pad) -> enable_if_t<is_back_insertable<R>::value, R> {
            return pretty<X, R>(std::begin(c), std::end(c), tab, pad);
        }

}

#endif // CXON_PRETTY_HXX_
