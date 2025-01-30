// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_IO_HXX_
#define CXON_CIO_IO_HXX_

#include "cxon/utility.hxx"
#include "chcls.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { // input

    template <typename II>
        inline char peek(II  i, II e);
    template <typename II>
        inline char next(II& i, II e);

    template <typename X, typename II>
        inline bool consume(II& i, II e);
    template <typename X, typename II, typename Cx>
        inline bool consume(II& i, II e, Cx& cx);

    template <typename X, typename II>
        inline bool consume(char c, II& i, II e);
    template <typename X, typename II, typename Cx>
        inline bool consume(char c, II& i, II e, Cx& cx);
    template <typename X, typename II>
        inline bool consume(const char* s, II& i, II e);
    template <typename X, typename II, typename Cx>
        inline bool consume(const char* s, II& i, II e, Cx& cx);

#   if defined(__GNUC__) && __GNUC__ > 11 && !defined(__clang__) // gcc does not like this forward declaration
        template <typename X, typename II, typename RD, typename Cx>
            inline auto consume(RD read, II& i, II e, Cx& cx)
                -> enable_if_t<std::is_same<decltype(read(i, e)), bool>::value, bool>;
#   endif

    template<typename II>
        inline void rewind(II& i, II o) noexcept;

}}

namespace cxon { namespace cio { // output

    template <typename O>
        inline bool poke(O& o, char c);
    template <typename O>
        inline bool poke(O& o, const char* s, std::size_t n);
    template <typename O>
        inline bool poke(O& o, const char* f, const char* l);
    template <typename O>
        inline bool poke(O& o, const char* s);
    template <typename O>
        inline bool poke(O& o, unsigned n, char c);

#   if defined(__GNUC__) && __GNUC__ > 11 && !defined(__clang__) // gcc does not like this forward declaration
        template <typename O, typename WR>
            inline auto poke(O& o, WR write)
                -> enable_if_t<std::is_same<decltype(write(o)), bool>::value, bool>;
#   endif

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, char c, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, std::size_t n, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* f, const char* l, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, unsigned n, char c, Cx& cx);

#   if defined(__GNUC__) && __GNUC__ > 11 && !defined(__clang__) // gcc does not like this forward declaration
        template <typename X, typename O, typename WR, typename Cx>
            inline auto poke(O& o, WR write, Cx& cx)
                -> enable_if_t<std::is_same<decltype(write(o)), bool>::value, bool>;
#   endif

}}

namespace cxon { namespace cio { // output adapter

    template <typename C, typename B>
        struct buffered_back_insert_iterator;
    template <typename C, typename B>
        inline auto buffered_back_inserter(C& c, B& b) noexcept -> buffered_back_insert_iterator<C, B>;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { // input

    template <typename II>
        inline char peek(II i, II e) {
            return i != e ? *i : '\xFF';
        }
    template <typename II>
        inline char next(II& i, II e) {
            CXON_ASSERT(i != e, "unexpected");
            return peek(++i, e);
        }

    namespace imp {

        template <typename X, typename II>
            inline void consume_ws_(II& i, II e) {
                for ( ; i != e && chr::is<X>::space(*i); ++i) ;
            }

        template <typename X, typename II>
            inline auto consume_(II& i, II e) -> enable_if_t<!X::allow_comments, bool> {
                return consume_ws_<X>(i, e), true;
            }
        template <typename X, typename II>
            inline auto consume_(II& i, II e) -> enable_if_t< X::allow_comments, bool> {
                consume_ws_<X>(i, e);
                if (peek(i, e) == '/') {
                    switch (next(i, e)) {
                        case '*':
                            for (++i; i != e; )
                                     if (        *i != '*') { ++i; continue; }
                                else if (next(i, e) == '/') { ++i; break;    }
                            break;
                        case '/':
                            while (++i != e && *i != '\n')
                                ;
                            break;
                        default:
                            return false;
                    }
                    consume_<X>(i, e);
                }
                return true;
            }

    }
    template <typename X, typename II>
        inline bool consume(II& i, II e) {
            return imp::consume_<X>(i, e);
        }
    template <typename X, typename II, typename Cx>
        inline bool consume(II& i, II e, Cx& cx) {
            return imp::consume_<X>(i, e) || cx/X::read_error::unexpected;
        }

    template <typename X, typename II>
        inline bool consume(char c, II& i, II e) {
            return i != e && c == *i && (++i, true);
        }
    template <typename X, typename II, typename Cx>
        inline bool consume(char c, II& i, II e, Cx& cx) {
            return consume<X>(i, e, cx) && (consume<X>(c, i, e) || cx/X::read_error::unexpected);
        }
    template <typename X, typename II>
        inline bool consume(const char* s, II& i, II e) {
            for ( ; i != e && *s && *s == *i; ++i, ++s) ;
            return *s == '\0';
        }
    template <typename X, typename II, typename Cx>
        inline bool consume(const char* s, II& i, II e, Cx& cx) {
            return consume(s, i, e) || cx/X::read_error::unexpected;
        }

    template <typename X, typename II, typename RD, typename Cx>
        inline auto consume(RD read, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<decltype(read(i, e)), bool>::value, bool>
        {
            return consume<X>(i, e, cx) && (read(i, e) || cx/X::read_error::unexpected);
        }

    namespace imp {
        template<typename II>
            inline auto rewind_(II&  , II  ) noexcept -> enable_if_t<!cxon::is_forward_iterator<II>::value> {        }
        template<typename II>
            inline auto rewind_(II& i, II o) noexcept -> enable_if_t< cxon::is_forward_iterator<II>::value> { i = o; }
    }
    template<typename II>
        inline void rewind(II& i, II o) noexcept {
            return imp::rewind_(i, o);
        }

}}

namespace cxon { namespace cio { // output

    namespace imp {
        // char
            template <typename O>
                inline auto push_(option<2>, O& o, char c)
                    -> decltype(o.append(c), void())
                {
                    o.append(c);
                }
            template <typename O>
                inline auto push_(option<1>, O& o, char c)
                    -> enable_if_t<is_back_insertable<O>::value>
                {
                    o.push_back(c);
                }
            template <typename O>
                inline auto push_(option<0>, O& o, char c)
                    -> enable_if_t<is_output_iterator<O>::value>
                {
                    *o = c, ++o;
                }
        template <typename O>
            inline void push_(O& o, char c) {
                push_(option<2>(), o, c);
            }
        // char[]
            template <typename O>
                inline auto push_(option<1>, O& o, const char* s, std::size_t n)
                    -> decltype(o.append(s, n), void())
                {
                    o.append(s, n);
                }
            template <typename O>
                inline void push_(option<0>, O& o, const char* s, std::size_t n) {
                    for (auto const e = s + n; s != e; ++s) push_(o, *s);
                }
        template <typename O>
            inline void push_(O& o, const char* s, std::size_t n) {
                push_(option<1>(), o, s, n);
            }
        // char*
            template <typename O>
                inline auto push_(option<1>, O& o, const char* f, const char* l)
                    -> decltype(o.append(f, l), void())
                {
                    o.append(f, l);
                }
            template <typename O>
                inline void push_(option<0>, O& o, const char* f, const char* l) {
                    for ( ; f != l; ++f) push_(o, *f);
                }
        template <typename O>
            inline void push_(O& o, const char* f, const char* l) {
                push_(option<1>(), o, f, l);
            }
        // x char
            template <typename O>
                inline auto push_(option<1>, O& o, unsigned n, char c)
                    -> decltype(o.append(n, c), void())
                {
                    o.append(n, c);
                }
            template <typename O>
                inline void push_(option<0>, O& o, unsigned n, char c) {
                    while (n) push_(o, c), --n;
                }
        template <typename O>
            inline void push_(O& o, unsigned n, char c) {
                push_(option<1>(), o, n, c);
            }
        // bool bridge
            template <typename O, typename ...P>
                inline auto poke_(option<2>, O& o, P... p)
                    -> enable_if_t<std::is_same<decltype(o.append(p...)), bool>::value, bool>
                {
                    return o.append(p...);
                }
            template <typename O, typename ...P>
                inline auto poke_(option<1>, O& o, P... p)
                    -> enable_if_t<std::is_same<decltype(o.good()), bool>::value, bool>
                {
                    return push_(o, p...), o.good();
                }
            template <typename O, typename ...P>
                inline bool poke_(option<0>, O& o, P... p) {
                    return push_(o, p...), true;
                }
        template <typename O, typename ...P>
            inline bool poke_(O& o, P... p) {
                return poke_(option<2>(), o, p...);
            }

    }

    template <typename O>
        inline bool poke(O& o, char c)                                          { return imp::poke_(o, c); }
    template <typename O>
        inline bool poke(O& o, const char* s, std::size_t n)                    { return imp::poke_(o, s, n); }
    template <typename O>
        inline bool poke(O& o, const char* f, const char* l)                    { return imp::poke_(o, f, l); }
    template <typename O>
        inline bool poke(O& o, const char* s)                                   { return imp::poke_(o, s, std::char_traits<char>::length(s)); }
    template <typename O>
        inline bool poke(O& o, unsigned n, char c)                              { return imp::poke_(o, n, c); }

    template <typename O, typename WR>
        inline auto poke(O& o, WR write)
            -> enable_if_t<std::is_same<decltype(write(o)), bool>::value, bool> { return write(o); }

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, char c, Cx& cx)                                  { return poke(o, c)     || cx/X::write_error::output_failure; }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, std::size_t n, Cx& cx)            { return poke(o, s, n)  || cx/X::write_error::output_failure; }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* f, const char* l, Cx& cx)            { return poke(o, f, l)  || cx/X::write_error::output_failure; }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, Cx& cx)                           { return poke(o, s)     || cx/X::write_error::output_failure; }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, unsigned n, char c, Cx& cx)                      { return poke(o, n, c)  || cx/X::write_error::output_failure; }

    template <typename X, typename O, typename WR, typename Cx>
        inline auto poke(O& o, WR write, Cx& cx)
            -> enable_if_t<std::is_same<decltype(write(o)), bool>::value, bool> { return write(o)       || cx/X::write_error::output_failure; }

}}

namespace cxon { namespace cio {

    template <typename C, typename B>
        struct buffered_back_insert_iterator {
            using iterator_category = std::output_iterator_tag;
            using value_type        = void;
            using difference_type   = std::ptrdiff_t;
            using pointer           = void;
            using reference         = void;
            using container_type    = C;
            using T                 = typename C::value_type;
            using buffer_type       = B;

            static_assert(std::is_scalar<T>::value, "not implemented");

            buffered_back_insert_iterator(container_type& c, buffer_type& b) noexcept : c_(c), b_(b), s_() {}
            ~buffered_back_insert_iterator() { if (s_) flush(); }

            buffered_back_insert_iterator& operator *()     noexcept    { return *this; }
            buffered_back_insert_iterator& operator ++()    noexcept    { return *this; }
            buffered_back_insert_iterator  operator ++(int) noexcept    { return *this; }
            buffered_back_insert_iterator& operator =(T t)              { return append(t), *this; }

            void append(T t) {
                if (s_ == b_.size())
                    flush();
                b_[s_++] = t;
            }

            void append(const T* f, const T* l) {
                append(f, std::distance(f, l));
            }
            void append(const T* t, std::size_t n) {
                if (s_ + n > b_.size()) {
                    flush();
                    if (n > b_.size())
                        return poke(c_, t, n), void();
                }
                std::copy_n(t, n, b_.data() + s_), s_ += n;
            }

            void append(unsigned n, T t) {
                if (s_ + n > b_.size()) {
                    flush();
                    if (n > b_.size())
                        return poke(c_, n, t), void();
                }
                std::fill_n(b_.data() + s_, n, t), s_ += n;
            }

            void flush() {
                poke(c_, b_.data(), /*b_.data() + */s_), s_ = 0;
            }

            private:
                container_type& c_;
                buffer_type& b_;
                typename buffer_type::size_type s_;
        };
    template <typename C, typename B>
        inline auto buffered_back_inserter(C& c, B& b) noexcept -> buffered_back_insert_iterator<C, B> {
            return {c, b};
        }

}}

#endif // CXON_CIO_IO_HXX_
