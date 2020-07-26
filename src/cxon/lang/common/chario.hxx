// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHARIO_HXX_
#define CXON_CHARIO_HXX_

#include "cxon/utility.hxx"
#include "cxon.hxx"

namespace cxon { namespace chario {

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
            while (cxon::bits::is<X>::space(peek(i, e))) next(i, e);
        }

    template <typename X, typename II>
        inline bool consume(char c, II& i, II e) {
            consume<X>(i, e);
            return c == peek(i, e) && (next(i, e), true);
        }
    template <typename X, typename II, typename Cx>
        inline bool consume(char c, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            return (c == peek(i, e) && (next(i, e), true)) || (cx|read_error::unexpected);
        }

    template <typename X, typename II>
        inline bool consume(const char* s, II& i, II e) {
            consume<X>(i, e);
            for (char c = peek(i, e); *s && *s == c; c = next(i, e), ++s) ;
            return *s == '\0';
        }

    template <typename X, typename II>
        constexpr bool consume(std::nullptr_t, II&, II) { return true; }
    template <typename X, typename II, typename Cx>
        constexpr bool consume(std::nullptr_t, II&, II, Cx&) { return true; }

    // output

    template <typename O>
        inline bool poke(O& o, char c)                  { return bits::poke(o, c); }
    template <typename O>
        inline bool poke(O& o, const char* s)           { return bits::poke(o, s); }
    template <typename O>
        inline bool poke(O& o, const char* s, size_t n) { return bits::poke(o, s, n); }
    template <typename O>
        inline bool poke(O& o, unsigned n, char c)      { return bits::poke(o, n, c); }
    template <typename O>
        inline bool poke(O&, std::nullptr_t) noexcept   { return true; }

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, char c, Cx& cx)                  { return bits::poke<X>(o, cx, c); }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, Cx& cx)           { return bits::poke<X>(o, cx, s); }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, size_t n, Cx& cx) { return bits::poke<X>(o, cx, s, n); }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, unsigned n, char c, Cx& cx)      { return bits::poke<X>(o, cx, n, c); }
    template <typename X, typename O, typename Cx>
        constexpr bool poke(O&, std::nullptr_t, Cx&)            { return true; }

}}  //cxon::chario

#endif // CXON_CHARIO_HXX_
