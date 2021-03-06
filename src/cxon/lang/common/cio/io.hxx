// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_IO_HXX_
#define CXON_CIO_IO_HXX_

#include "cxon/utility.hxx"
#include "chcls.hxx"

#include <cstring>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio {

    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t<!cxon::is_forward_iterator<II>::value>;
    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t< cxon::is_forward_iterator<II>::value>;

    // input

    template <typename II>
        inline char peek(II i, II e);
    template <typename II>
        inline char next(II& i, II e);

    template <typename X, typename II>
        inline void consume(II& i, II e);

    template <typename X, typename II>
        inline bool consume(char c, II& i, II e);
    template <typename X, typename II, typename Cx>
        inline bool consume(char c, II& i, II e, Cx& cx);

    template <typename X, typename II>
        inline bool consume(const char* s, II& i, II e);

    template <typename X, typename II>
        constexpr bool consume(std::nullptr_t, II&, II);
    template <typename X, typename II, typename Cx>
        constexpr bool consume(std::nullptr_t, II&, II, Cx&);

    // output

    template <typename O>
        inline bool poke(O& o, char c);
    template <typename O>
        inline bool poke(O& o, const char* f, const char* l);
    template <typename O>
        inline bool poke(O& o, const char* s, std::size_t n);
    template <typename O>
        inline bool poke(O& o, const char* s);
    template <typename O>
        inline bool poke(O& o, unsigned n, char c);
    template <typename O>
        inline bool poke(O&, std::nullptr_t) noexcept;

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, char c, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* f, const char* l, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, std::size_t n, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, unsigned n, char c, Cx& cx);
    template <typename X, typename O, typename Cx>
        constexpr bool poke(O&, std::nullptr_t, Cx&);

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio {

    template<typename II>
        inline auto rewind(II&, II) noexcept     -> enable_if_t<!cxon::is_forward_iterator<II>::value> {}
    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t< cxon::is_forward_iterator<II>::value> { i = o; }

    // input

    template <typename II>
        inline char peek(II i, II e) {
            return i != e ? *i : '\xFF';
        }
    template <typename II>
        inline char next(II& i, II e) {
            return ++i, peek(i, e);
        }

    template <typename X, typename II>
        inline void consume(II& i, II e) {
            while (chr::is<X>::space(peek(i, e))) next(i, e);
        }

    template <typename X, typename II>
        inline bool consume(char c, II& i, II e) {
            consume<X>(i, e);
            return c == peek(i, e) && (next(i, e), true);
        }
    template <typename X, typename II, typename Cx>
        inline bool consume(char c, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            return (c == peek(i, e) && (next(i, e), true)) || cx/X::read_error::unexpected;
        }

    template <typename X, typename II>
        inline bool consume(const char* s, II& i, II e) {
            consume<X>(i, e);
            for (char c = peek(i, e); *s && *s == c; c = next(i, e), ++s) ;
            return *s == '\0';
        }

    template <typename X, typename II>
        constexpr bool consume(std::nullptr_t, II&, II)         { return true; }
    template <typename X, typename II, typename Cx>
        constexpr bool consume(std::nullptr_t, II&, II, Cx&)    { return true; }

    // output

    namespace imp {

        template <typename O>
            inline auto push_(O& o, char c) -> enable_if_t<is_output_iterator<O>::value> {
                *o = c, ++o;
            }
        template <typename O>
            inline auto push_(O& o, char c) -> enable_if_t<is_back_insertable<O>::value> {
                o.push_back(c);
            }

        template <typename O>
            inline auto push_(option<1>, O& o, const char* f, const char* l) -> decltype(o.append(f, l), void()) {
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

        template <typename O>
            inline auto push_(option<1>, O& o, unsigned n, char c) -> decltype(o.append(n, c), void()) {
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

        template <typename O, typename ...P>
            inline auto poke_(option<2>, O& o, P... p) -> enable_if_t<std::is_same<decltype(o.append(p...)), bool>::value, bool> {
                return o.append(p...);
            }
        template <typename O, typename ...P>
            inline auto poke_(option<1>, O& o, P... p) -> decltype(o.good()) {
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
        inline bool poke(O& o, char c)                          { return imp::poke_(o, c); }
    template <typename O>
        inline bool poke(O& o, const char* f, const char* l)    { return imp::poke_(o, f, l); }
    template <typename O>
        inline bool poke(O& o, const char* s)                   { return imp::poke_(o, s, s + std::strlen(s)); }
    template <typename O>
        inline bool poke(O& o, const char* s, std::size_t n)    { return imp::poke_(o, s, s + n); }
    template <typename O>
        inline bool poke(O& o, unsigned n, char c)              { return imp::poke_(o, n, c); }
    template <typename O>
        inline bool poke(O&, std::nullptr_t) noexcept           { return true; }

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, char c, Cx& cx)                          { return poke(o, c)     || cx/X::write_error::output_failure; }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* f, const char* l, Cx& cx)    { return poke(o, f, l)  || cx/X::write_error::output_failure; }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, std::size_t n, Cx& cx)    { return poke(o, s, n)  || cx/X::write_error::output_failure; }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, Cx& cx)                   { return poke(o, s)     || cx/X::write_error::output_failure; }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, unsigned n, char c, Cx& cx)              { return poke(o, n, c)  || cx/X::write_error::output_failure; }
    template <typename X, typename O, typename Cx>
        constexpr bool poke(O&, std::nullptr_t, Cx&)                    { return true; }

}}

#endif // CXON_CIO_IO_HXX_
