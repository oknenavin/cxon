// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_BIO_IO_HXX_
#define CXON_BIO_IO_HXX_

#include "cxon/utility.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace bio {

    using byte = unsigned char;

    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t<!cxon::is_forward_iterator<II>::value>;
    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t< cxon::is_forward_iterator<II>::value>;

    // input

    template <typename II>
        inline byte get(II& i, II e);

    template <typename T, typename II>
        inline auto get(T& t, unsigned n, II& i, II e)
            -> enable_if_t<std::is_integral<T>::value, bool>;

    // output

    template <typename O>
        inline bool poke(O& o, byte b);

    template <typename O, typename T>
        inline auto poke(O& o, T t, unsigned n)
            -> enable_if_t<std::is_integral<T>::value, bool>;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace bio {

    template<typename II>
        inline auto rewind(II&, II) noexcept     -> enable_if_t<!cxon::is_forward_iterator<II>::value> {}
    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t< cxon::is_forward_iterator<II>::value> { i = o; }

    // input

    template <typename II>
        inline byte get(II& i, II e) {
            return i != e ? *i++ : 0xFF;
        }

    namespace bits {

        template <typename II, unsigned N>
            inline auto get_n_(byte (&bs)[N], II& i, II e) -> enable_if_t< is_random_access_iterator<II>::value, bool> {
                using dt = typename std::iterator_traits<II>::difference_type;
                return  std::distance(i, e) >= static_cast<dt>(N) &&
                        (std::copy(i, e, std::begin(bs)), std::advance(i, N), true)
                ;
            }
        template <typename II>
            inline auto get_n_(byte (&bs)[2], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get<1>(bs[0], i, e) && get<1>(bs[1], i, e)
                ;
            }
        template <typename II>
            inline auto get_n_(byte (&bs)[3], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get<1>(bs[0], i, e) && get<1>(bs[1], i, e) &&
                        get<1>(bs[2], i, e)
                ;
            }
        template <typename II>
            inline auto get_n_(byte (&bs)[4], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get<1>(bs[0], i, e) && get<1>(bs[1], i, e) &&
                        get<1>(bs[2], i, e) && get<1>(bs[3], i, e)
                ;
            }
        template <typename II>
            inline auto get_n_(byte (&bs)[5], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get<1>(bs[0], i, e) && get<1>(bs[1], i, e) &&
                        get<1>(bs[2], i, e) && get<1>(bs[3], i, e) &&
                        get<1>(bs[4], i, e)
                ;
            }
        template <typename II>
            inline auto get_n_(byte (&bs)[6], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get<1>(bs[0], i, e) && get<1>(bs[1], i, e) &&
                        get<1>(bs[2], i, e) && get<1>(bs[3], i, e) &&
                        get<1>(bs[4], i, e) && get<1>(bs[5], i, e)
                ;
            }
        template <typename II>
            inline auto get_n_(byte (&bs)[7], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get<1>(bs[0], i, e) && get<1>(bs[1], i, e) &&
                        get<1>(bs[2], i, e) && get<1>(bs[3], i, e) &&
                        get<1>(bs[4], i, e) && get<1>(bs[5], i, e) &&
                        get<1>(bs[6], i, e)
                ;
            }
        template <typename II>
            inline auto get_n_(byte (&bs)[8], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get<1>(bs[0], i, e) && get<1>(bs[1], i, e) &&
                        get<1>(bs[2], i, e) && get<1>(bs[3], i, e) &&
                        get<1>(bs[4], i, e) && get<1>(bs[5], i, e) &&
                        get<1>(bs[6], i, e) && get<1>(bs[7], i, e)
                ;
            }

        template <typename T>
            inline T be_to_t_(const byte (&bs)[2]) {
                using R = unsigned short;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing!");
                return  (R(bs[1])<< 0) | (R(bs[0])<< 8);
            }
        template <typename T>
            inline T be_to_t_(const byte (&bs)[3]) {
                using R = unsigned long;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing!");
                return  (R(bs[2])<< 0) | (R(bs[1])<< 8) |
                        (R(bs[0])<<16)
                ;
            }
        template <typename T>
            inline T be_to_t_(const byte (&bs)[4]) {
                using R = unsigned long;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing!");
                return  (R(bs[3])<< 0) | (R(bs[2])<< 8) |
                        (R(bs[1])<<16) | (R(bs[0])<<24)
                ;
            }
        template <typename T>
            inline T be_to_t_(const byte (&bs)[5]) {
                using R = unsigned long long;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing!");
                return  (R(bs[4])<< 0) | (R(bs[3])<< 8) |
                        (R(bs[2])<<16) | (R(bs[1])<<24) |
                        (R(bs[0])<<32)
                ;
            }
        template <typename T>
            inline T be_to_t_(const byte (&bs)[6]) {
                using R = unsigned long long;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing!");
                return  (R(bs[5])<< 0) | (R(bs[4])<< 8) |
                        (R(bs[3])<<16) | (R(bs[2])<<24) |
                        (R(bs[1])<<32) | (R(bs[0])<<40)
                ;
            }
        template <typename T>
            inline T be_to_t_(const byte (&bs)[7]) {
                using R = unsigned long long;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing!");
                return  (R(bs[6])<< 0) | (R(bs[5])<< 8) |
                        (R(bs[4])<<16) | (R(bs[3])<<24) |
                        (R(bs[2])<<32) | (R(bs[1])<<40) |
                        (R(bs[0])<<48)
                ;
            }
        template <typename T>
            inline T be_to_t_(const byte (&bs)[8]) {
                using R = unsigned long long;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing!");
                return  (R(bs[7])<< 0) | (R(bs[6])<< 8) |
                        (R(bs[5])<<16) | (R(bs[4])<<24) |
                        (R(bs[3])<<32) | (R(bs[2])<<40) |
                        (R(bs[1])<<48) | (R(bs[0])<<56)
                ;
            }

        template <unsigned N, typename T, typename II>
            inline auto get_(T& t, II& i, II e) -> enable_if_t<N == 1, bool> {
                return i != e && (t = T(*i++), true);
            }
        template <unsigned N, typename T, typename II>
            inline auto get_(T& t, II& i, II e) -> enable_if_t<N != 1, bool> {
                byte bs[N];
                return get_n_(bs, i, e) && (t = be_to_t_<T>(bs), true);
            }

    }

    template <typename T, typename II>
        inline auto get(T& t, unsigned n, II& i, II e)
            -> enable_if_t<std::is_integral<T>::value, bool>
        {
            switch (n) {
                case 1:     return bits::get_<1>(t, i, e);
                case 2:     return bits::get_<2>(t, i, e);
                case 3:     return bits::get_<3>(t, i, e);
                case 4:     return bits::get_<4>(t, i, e);
                case 5:     return bits::get_<5>(t, i, e);
                case 6:     return bits::get_<6>(t, i, e);
                case 7:     return bits::get_<7>(t, i, e);
                case 8:     return bits::get_<8>(t, i, e);
                default:    return CXON_ASSERT(0, "unexpected"), false;
            }
        }

    // output

    namespace bits {

        template <typename O>
            inline auto push_(O& o, byte b) -> enable_if_t<is_output_iterator<O>::value> {
                *o = b, ++o;
            }
        template <typename O>
            inline auto push_(O& o, byte b) -> enable_if_t<is_back_insertable<O>::value> {
                o.push_back(b);
            }

        template <typename O>
            inline auto push_(option<1>, O& o, const byte* s, unsigned n) -> decltype(o.append(s, n), void()) {
                o.append(s, n);
            }
        template <typename O>
            inline void push_(option<0>, O& o, const byte* s, unsigned n) {
                while (n) push_(o, *s), ++s, --n;
            }
        template <typename O>
            inline void push_(O& o, const byte* s, unsigned n) {
                push_(option<1>(), o, s, n);
            }

        template <typename O, typename ...P>
            inline auto poke_(option<2>, O& o, P... p) -> decltype((bool)o) {
                return push_(o, p...), o;
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

    namespace bits {

        template <typename T, typename O>
            inline auto put(O& o, T t, unsigned  ) -> enable_if_t<sizeof(T) == 1, bool> {
                return bits::poke_(o, t);
            }
        template <typename T, typename O> // TODO: unroll?
            inline auto put(O& o, T t, unsigned n) -> enable_if_t<sizeof(T) != 1, bool> {
                byte bs[sizeof(T)];
                    for (unsigned i = 0; i != n; ++i)
                        bs[i] = byte(t >> (n - i - 1) * 8);
                return bits::poke_(o, bs, n);
            }

    }

    template <typename O>
        inline bool poke(O& o, byte b) {
            return bits::put(o, b, 1);
        }

    template <typename O, typename T>
        inline auto poke(O& o, T t, unsigned n)
            -> enable_if_t<std::is_integral<T>::value, bool>
        {
            return bits::put(o, t, n);
        }

}}

#endif // CXON_BIO_IO_HXX_
