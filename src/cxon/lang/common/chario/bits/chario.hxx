// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_BITS_CHARIO_HXX_
#define CXON_BITS_CHARIO_HXX_

namespace cxon { namespace chario { namespace bits { // output write with error handling

    template <typename O>
        inline auto push(O& o, char c) -> enable_if_t<is_output_iterator<O>::value> {
            *o = c, ++o;
        }
    template <typename O>
        inline auto push(O& o, char c) -> enable_if_t<is_back_insertable<O>::value> {
            o.push_back(c);
        }

    template <typename O>
        inline auto push_(option<1>, O& o, const char* s) -> decltype(o.append(s), void()) {
            o.append(s);
        }
    template <typename O>
        inline void push_(option<0>, O& o, const char* s) {
            while (*s) push(o, *s), ++s;
        }
    template <typename O>
        inline void push(O& o, const char* s) {
            push_(option<1>(), o, s);
        }

    template <typename O>
        inline auto push_(option<1>, O& o, const char* s, size_t n) -> decltype(o.append(s, n), void()) {
            o.append(s, n);
        }
    template <typename O>
        inline void push_(option<0>, O& o, const char* s, size_t n) {
            while (n) push(o, *s), ++s, --n;
        }
    template <typename O>
        inline void push(O& o, const char* s, size_t n) {
            push_(option<1>(), o, s, n);
        }

    template <typename O>
        inline auto push_(option<1>, O& o, unsigned n, char c) -> decltype(o.append(n, c), void()) {
            o.append(n, c);
        }
    template <typename O>
        inline void push_(option<0>, O& o, unsigned n, char c) {
            while (n) push(o, c), --n;
        }
    template <typename O>
        inline void push(O& o, unsigned n, char c) {
            push_(option<1>(), o, n, c);
        }

    template <typename O, typename ...P>
        inline auto poke_(option<2>, O& o, P... p) -> decltype((bool)o) {
            return push(o, p...), o;
        }
    template <typename O, typename ...P>
        inline auto poke_(option<1>, O& o, P... p) -> decltype(o.good()) {
            return push(o, p...), o.good();
        }
    template <typename O, typename ...P>
        inline bool poke_(option<0>, O& o, P... p) {
            return push(o, p...), true;
        }
    template <typename O, typename ...P>
        inline bool poke(O& o, P... p) {
            return poke_(option<2>(), o, p...);
        }

    template <typename X, typename O, typename Cx, typename ...P>
        inline bool poke(O& o, Cx& cx, P... p) {
            return poke_(option<2>(), o, p...) || (cx|write_error::output_failure);
        }

}}} //cxon::chario::bits output write with error handling

#endif // CXON_BITS_CHARIO_HXX_
