// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_BIO_IO_HXX_
#define CXON_BIO_IO_HXX_

#include "cxon/utility.hxx"
#include <cmath> // std::ldexp

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace bio {

    using byte = unsigned char;

    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t<!cxon::is_forward_iterator<II>::value>;
    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t< cxon::is_forward_iterator<II>::value>;

    // input

    template <typename II>
        inline byte peek(II& i, II e, byte d = 0xFF);

    template <typename II>
        inline byte get(II& i, II e, byte d = 0xFF);

    template <typename T, typename II>
        inline auto get(T& t, unsigned n, II& i, II e)
            -> enable_if_t<std::is_integral<T>::value, bool>;

    template <std::size_t N, typename T, typename II>
        inline auto get(T& t, II& i, II e)
            -> enable_if_t<std::is_floating_point<T>::value, bool>;

    template <typename OI, typename II>
        inline bool get(OI o, II& i, II e);
    template <typename OI, typename II>
        inline bool get(OI o, II& i, II e, std::size_t n);

    template <typename II>
        inline bool advance(II& i, II e, std::size_t n);

    // output

    template <typename O>
        inline bool poke(O& o, byte b);

    template <typename O, typename T>
        inline auto poke(O& o, T t, unsigned n)
            -> enable_if_t<std::is_integral<T>::value, bool>;

    template <typename O, typename FI>
        inline auto poke(O& o, FI f, FI l )
            -> enable_if_t<is_forward_iterator<FI>::value, bool>;
    template <typename O, typename FI>
        inline auto poke(O& o, FI i, std::size_t n)
            -> enable_if_t<is_forward_iterator<FI>::value, bool>;

    template <typename O, typename T>
        inline auto poke(O& o, T t)
            -> enable_if_t<std::is_floating_point<T>::value, bool>;

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, byte b, Cx& cx);

    template <typename X, typename O, typename T, typename Cx>
        inline auto poke(O& o, T t, unsigned n, Cx& cx)
            -> enable_if_t<std::is_integral<T>::value, bool>;

    template <typename X, typename O, typename FI, typename Cx>
        inline auto poke(O& o, FI f, FI l, Cx& cx)
            -> enable_if_t<is_forward_iterator<FI>::value, bool>;
    template <typename X, typename O, typename FI, typename Cx>
        inline auto poke(O& o, FI i, std::size_t n, Cx& cx)
            -> enable_if_t<is_forward_iterator<FI>::value, bool>;

    template <typename X, typename O, typename T, typename Cx>
        inline auto poke(O& o, T t, Cx& cx)
            -> enable_if_t<std::is_floating_point<T>::value, bool>;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace bio {

    template<typename II>
        inline auto rewind(II&, II) noexcept     -> enable_if_t<!cxon::is_forward_iterator<II>::value> {}
    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t< cxon::is_forward_iterator<II>::value> { i = o; }

    // input

    template <typename II>
        inline byte peek(II& i, II e, byte d/* = 0xFF*/) {
            return i != e ? *i : d;
        }

    template <typename II>
        inline byte get(II& i, II e, byte d/* = 0xFF*/) {
            return i != e ? *i++ : d;
        }

    namespace imp {

        template <typename II, unsigned N>
            inline auto get_n_(byte (&bs)[N], II& i, II e) -> enable_if_t< is_random_access_iterator<II>::value, bool> {
                using dt = typename std::iterator_traits<II>::difference_type;
                return  std::distance(i, e) >= static_cast<dt>(N) &&
                        (std::copy(i, i + N, std::begin(bs)), std::advance(i, N), true)
                ;
            }

        template <typename T, typename II>
            inline bool get_1_(T& t, II& i, II e) {
                return i != e ? (t = T(*i), ++i, true) : false;
            }
        template <typename II>
            inline auto get_n_(byte (&bs)[2], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get_1_(bs[0], i, e) && get_1_(bs[1], i, e)
                ;
            }
        template <typename II>
            inline auto get_n_(byte (&bs)[4], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get_1_(bs[0], i, e) && get_1_(bs[1], i, e) &&
                        get_1_(bs[2], i, e) && get_1_(bs[3], i, e)
                ;
            }
        template <typename II>
            inline auto get_n_(byte (&bs)[8], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get_1_(bs[0], i, e) && get_1_(bs[1], i, e) &&
                        get_1_(bs[2], i, e) && get_1_(bs[3], i, e) &&
                        get_1_(bs[4], i, e) && get_1_(bs[5], i, e) &&
                        get_1_(bs[6], i, e) && get_1_(bs[7], i, e)
                ;
            }

        // coverity[ -tainted_data_return ]
        // coverity[ -tainted_data_argument : arg-0 ]
        template <typename T>
            inline T be_to_i_(const byte (&bs)[2]) {
                using R = unsigned long;
                CXON_ASSERT(sizeof(T) >= 2, "narrowing");
                return  (R(bs[1])<< 0) | (R(bs[0])<< 8);
            }
        // coverity[ -tainted_data_return ]
        // coverity[ -tainted_data_argument : arg-0 ]
        template <typename T>
            inline T be_to_i_(const byte (&bs)[4]) {
                using R = unsigned long;
                CXON_ASSERT(sizeof(T) >= 4, "narrowing");
                return  (R(bs[3])<< 0) | (R(bs[2])<< 8) |
                        (R(bs[1])<<16) | (R(bs[0])<<24)
                ;
            }
        // coverity[ -tainted_data_return ]
        // coverity[ -tainted_data_argument : arg-0 ]
        template <typename T>
            inline T be_to_i_(const byte (&bs)[8]) {
                using R = unsigned long long;
                CXON_ASSERT(sizeof(T) >= 8, "narrowing");
                return  (R(bs[7])<< 0) | (R(bs[6])<< 8) |
                        (R(bs[5])<<16) | (R(bs[4])<<24) |
                        (R(bs[3])<<32) | (R(bs[2])<<40) |
                        (R(bs[1])<<48) | (R(bs[0])<<56)
                ;
            }

        template <typename T>
            inline T be_to_fp_(const byte (&bs)[2]) {
                // TODO: not correct, but as in RFC7049 Appendix D
                const unsigned half = (bs[0] << 8) + bs[1];
                const unsigned exp = (half >> 10) & 0x1f;
                const unsigned mant = half & 0x3ff;
                double val;
                     if (exp == 0)  val = std::ldexp(mant, -24);
                else if (exp != 31) val = std::ldexp(mant + 1024, exp - 25);
                else                val = mant == 0 ? INFINITY : NAN;
                return (T)(half & 0x8000 ? -val : val);
            }
        template <typename T>
            inline T be_to_fp_(const byte (&bs)[4]) {
                using R = unsigned long;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing");
                union { R i; float f; } u;
                u.i =   (R(bs[3])<< 0) | (R(bs[2])<< 8) |
                        (R(bs[1])<<16) | (R(bs[0])<<24)
                ;
                return static_cast<T>(u.f);
            }
        template <typename T>
            inline T be_to_fp_(const byte (&bs)[8]) {
                using R = unsigned long long;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing");
                union { R i; double f; } u;
                u.i =   (R(bs[7])<< 0) | (R(bs[6])<< 8) |
                        (R(bs[5])<<16) | (R(bs[4])<<24) |
                        (R(bs[3])<<32) | (R(bs[2])<<40) |
                        (R(bs[1])<<48) | (R(bs[0])<<56)
                ;
                return static_cast<T>(u.f);
            }

        template <unsigned N, typename T, typename II>
            inline auto get_(T& t, II& i, II e) -> enable_if_t<N == 1, bool> {
                return i != e && (t = T(static_cast<byte>(*i)), ++i, true);
            }
        template <unsigned N, typename T, typename II>
            inline auto get_(T& t, II& i, II e) -> enable_if_t<N != 1, bool> {
                byte bs[N];
                return get_n_(bs, i, e) && (t = be_to_i_<T>(bs), true);
            }

    }

    template <typename T, typename II>
        inline auto get(T& t, unsigned n, II& i, II e)
            -> enable_if_t<std::is_integral<T>::value, bool>
        {
            switch (n) {
                case 1:     return imp::get_<1>(t, i, e);
                case 2:     return imp::get_<2>(t, i, e);
                case 4:     return imp::get_<4>(t, i, e);
                case 8:     return imp::get_<8>(t, i, e);
                default:    return CXON_ASSERT(0, "unexpected"), false; // LCOV_EXCL_LINE
            }
        }

    template <std::size_t N, typename T, typename II>
        inline auto get(T& t, II& i, II e)
            -> enable_if_t<std::is_floating_point<T>::value, bool>
        {
            byte bs[N];
            return imp::get_n_(bs, i, e) && (t = imp::be_to_fp_<T>(bs), true);
        }
    
    template <typename OI, typename II>
        inline bool get(OI o, II& i, II e) {
            return i != e && poke(o, *i) && (++i, true);
        }

    namespace imp {

        template <typename OI, typename II>
            inline auto get_(OI o, II& i, II e, std::size_t n) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                for ( ; n != 0 && i != e && poke(o, *i); --n, ++i)
                    ;
                return n == 0;
            }
        template <typename OI, typename II>
            inline auto get_(OI o, II& i, II e, std::size_t n) -> enable_if_t< is_random_access_iterator<II>::value, bool> {
                auto const m = std::min(n, static_cast<std::size_t>(std::distance(i, e)));
                return std::copy(i, i + m, o), std::advance(i, m), m == n;
            }

    }
    template <typename OI, typename II>
        inline bool get(OI o, II& i, II e, std::size_t n) {
            return imp::get_(o, i, e, n);
        }

    namespace imp {

        template <typename II>
            inline auto advance_(II& i, II e, std::size_t n) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                for ( ; n != 0 && i != e; --n, ++i)
                    ;
                return n == 0;
            }
        template <typename II>
            inline auto advance_(II& i, II e, std::size_t n) -> enable_if_t< is_random_access_iterator<II>::value, bool> {
                return  (static_cast<std::size_t>(std::distance(i, e)) >= n) &&
                        (std::advance(i, n), true)
                ;
            }

    }
    template <typename II>
        inline bool advance(II& i, II e, std::size_t n) {
            return imp::advance_(i, e, n);
        }

    // output

    namespace imp {

        template <typename O>
            inline auto push_(O& o, byte b) -> enable_if_t<is_output_iterator<O>::value> {
                *o = b, ++o;
            }
        template <typename O>
            inline auto push_(O& o, byte b) -> enable_if_t<is_back_insertable<O>::value> {
                o.push_back(b);
            }

        template <typename O, typename FI>
            inline auto push_(option<1>, O& o, FI f, FI l) -> decltype(o.append(f, l), void()) {
                o.append(f, l);
            }
        template <typename O, typename FI>
            inline void push_(option<0>, O& o, FI f, FI l) {
                for ( ; f != l; ++f) push_(o, *f);
            }
        template <typename O, typename FI>
            inline void push_(O& o, FI f, FI l) {
                push_(option<1>(), o, f, l);
            }

        template <typename O, typename FI>
            inline auto push_(option<1>, O& o, FI i, std::size_t n) -> decltype(o.append(i, n), void()) {
                o.append(i, n);
            }
        template <typename O, typename FI>
            inline void push_(option<0>, O& o, FI i, std::size_t n) {
                while (n) push_(o, *i), ++i, --n;
            }
        template <typename O, typename FI>
            inline void push_(O& o, FI i, std::size_t n) {
                push_(option<1>(), o, i, n);
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

    namespace imp {

        template <typename O, typename T>
            inline auto put_(O& o, T t, unsigned  )
                -> enable_if_t<std::is_integral<T>::value && sizeof(T) == 1, bool>
            {
                return poke_(o, t);
            }
        template <typename T, typename O>
            inline auto put_(O& o, T t, unsigned n)
                -> enable_if_t<std::is_integral<T>::value && sizeof(T) == 2, bool>
            {
                byte bs[sizeof(T)];
                    switch (n) {
                        case 2: bs[n - 2] = byte(t >>  8); CXON_FALLTHROUGH;
                        case 1: bs[n - 1] = byte(t >>  0);
                    }
                return poke_(o, bs, n);
            }
        template <typename T, typename O>
            inline auto put_(O& o, T t, unsigned n)
                -> enable_if_t<std::is_integral<T>::value && sizeof(T) == 4, bool>
            {
                byte bs[sizeof(T)];
                    switch (n) {
                        case 4: bs[n - 4] = byte(t >> 24);
                                bs[n - 3] = byte(t >> 16); CXON_FALLTHROUGH;
                        case 2: bs[n - 2] = byte(t >>  8); CXON_FALLTHROUGH;
                        case 1: bs[n - 1] = byte(t >>  0);
                    }
                return poke_(o, bs, n);
            }
        template <typename T, typename O>
            inline auto put_(O& o, T t, unsigned n)
                -> enable_if_t<std::is_integral<T>::value && sizeof(T) == 8, bool>
            {
                byte bs[sizeof(T)];
                    switch (n) {
                        case 8: bs[n - 8] = byte(t >> 56);
                                bs[n - 7] = byte(t >> 48);
                                bs[n - 6] = byte(t >> 40);
                                bs[n - 5] = byte(t >> 32); CXON_FALLTHROUGH;
                        case 4: bs[n - 4] = byte(t >> 24);
                                bs[n - 3] = byte(t >> 16); CXON_FALLTHROUGH;
                        case 2: bs[n - 2] = byte(t >>  8); CXON_FALLTHROUGH;
                        case 1: bs[n - 1] = byte(t >>  0);
                    }
                return poke_(o, bs, n);
            }

        template <typename O, typename FI>
            inline auto put_(O& o, FI f, FI l)
                -> enable_if_t<is_forward_iterator<FI>::value, bool>
            {
                return poke_(o, f, l);
            }
        template <typename O, typename FI>
            inline auto put_(O& o, FI i, std::size_t n)
                -> enable_if_t<is_forward_iterator<FI>::value, bool>
            {
                return poke_(o, i, n);
            }

        template <typename O>
            inline bool put_(O& o, float t) {
                union { unsigned long i; float f; } u; u.f = t;
                byte bs[sizeof(float)];
                    bs[0] = byte(u.i >> 24), bs[1] = byte(u.i >> 16),
                    bs[2] = byte(u.i >>  8), bs[3] = byte(u.i >>  0);
                return poke_(o, bs, sizeof(bs));
            }
        template <typename O>
            inline bool put_(O& o, double t) {
                union { unsigned long long i; double f; } u; u.f = t;
                byte bs[sizeof(double)];
                    bs[0] = byte(u.i >> 56), bs[1] = byte(u.i >> 48),
                    bs[2] = byte(u.i >> 40), bs[3] = byte(u.i >> 32),
                    bs[4] = byte(u.i >> 24), bs[5] = byte(u.i >> 16),
                    bs[6] = byte(u.i >>  8), bs[7] = byte(u.i >>  0);
                return poke_(o, bs, sizeof(bs));
            }

    }

    template <typename O>
        inline bool poke(O& o, byte b) {
            return imp::put_(o, b, 1);
        }

    template <typename O, typename T>
        inline auto poke(O& o, T t, unsigned n)
            -> enable_if_t<std::is_integral<T>::value, bool>
        {
            return imp::put_(o, t, n);
        }

    template <typename O, typename FI>
        inline auto poke(O& o, FI f, FI l)
            -> enable_if_t<is_forward_iterator<FI>::value, bool>
        {
            return imp::put_(o, f, l);
        }
    template <typename O, typename FI>
        inline auto poke(O& o, FI i, std::size_t n)
            -> enable_if_t<is_forward_iterator<FI>::value, bool>
        {
            return imp::put_(o, i, n);
        }

    template <typename O, typename T>
        inline auto poke(O& o, T t)
            -> enable_if_t<std::is_floating_point<T>::value, bool>
        {
            return imp::put_(o, t);
        }

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, byte b, Cx& cx) {
            return imp::put_(o, b, 1) || cx/X::write_error::output_failure;
        }

    template <typename X, typename O, typename T, typename Cx>
        inline auto poke(O& o, T t, unsigned n, Cx& cx)
            -> enable_if_t<std::is_integral<T>::value, bool>
        {
            return imp::put_(o, t, n) || cx/X::write_error::output_failure;
        }

    template <typename X, typename O, typename FI, typename Cx>
        inline auto poke(O& o, FI f, FI l, Cx& cx)
            -> enable_if_t<is_forward_iterator<FI>::value, bool>
        {
            return imp::put_(o, f, l) || cx/X::write_error::output_failure;
        }
    template <typename X, typename O, typename FI, typename Cx>
        inline auto poke(O& o, FI i, std::size_t n, Cx& cx)
            -> enable_if_t<is_forward_iterator<FI>::value, bool>
        {
            return imp::put_(o, i, n) || cx/X::write_error::output_failure;
        }

    template <typename X, typename O, typename T, typename Cx>
        inline auto poke(O& o, T t, Cx& cx)
            -> enable_if_t<std::is_floating_point<T>::value, bool>
        {
            return imp::put_(o, t) || cx/X::write_error::output_failure;
        }

}}

#endif // CXON_BIO_IO_HXX_
