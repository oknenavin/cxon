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

    template <unsigned N, typename T, typename II>
        inline auto get(T& t, II& i, II e)
            -> enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value, bool>;

    // output

    template <typename O>
        inline bool poke(O& o, byte b);

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, byte b, Cx& cx);

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
            inline auto get_n(byte (&bs)[N], II& i, II e) -> enable_if_t< is_random_access_iterator<II>::value, bool> {
                using dt = typename std::iterator_traits<II>::difference_type;
                return  std::distance(i, e) >= static_cast<dt>(N) &&
                        (std::copy(i, e, std::begin(bs)), std::advance(i, N), true)
                ;
            }
        template <typename II>
            inline auto get_n(byte (&bs)[2], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get<1>(bs[0], i, e) && get<1>(bs[1], i, e)
                ;
            }
        template <typename II>
            inline auto get_n(byte (&bs)[4], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get<1>(bs[0], i, e) && get<1>(bs[1], i, e) &&
                        get<1>(bs[2], i, e) && get<1>(bs[3], i, e)
                ;
            }
        template <typename II>
            inline auto get_n(byte (&bs)[8], II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, bool> {
                return  get<1>(bs[0], i, e) && get<1>(bs[1], i, e) &&
                        get<1>(bs[2], i, e) && get<1>(bs[3], i, e) &&
                        get<1>(bs[4], i, e) && get<1>(bs[5], i, e) &&
                        get<1>(bs[6], i, e) && get<1>(bs[7], i, e)
                ;
            }

        template <typename T>
            inline T be_to_t(const byte (&bs)[2]) {
                using R = unsigned short;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing!");
                return  (R(bs[1])<< 0) | (R(bs[0])<< 8);
            }
        template <typename T>
            inline T be_to_t(const byte (&bs)[4]) {
                using R = unsigned long;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing!");
                return  (R(bs[3])<< 0) | (R(bs[2])<< 8) |
                        (R(bs[1])<<16) | (R(bs[0])<<24)
                ;
            }
        template <typename T>
            inline T be_to_t(const byte (&bs)[8]) {
                using R = unsigned long long;
                CXON_ASSERT(sizeof(R) <= sizeof(T), "narrowing!");
                return  (R(bs[7])<< 0) | (R(bs[6])<< 8) |
                        (R(bs[5])<<16) | (R(bs[4])<<24) |
                        (R(bs[3])<<32) | (R(bs[2])<<40) |
                        (R(bs[1])<<48) | (R(bs[0])<<56)
                ;
            }

        template <unsigned N, typename T, typename II>
            inline auto get(T& t, II& i, II e) -> enable_if_t<N == 1, bool> {
                return i != e && (t = T(*i++), true);
            }
        template <unsigned N, typename T, typename II>
            inline auto get(T& t, II& i, II e) -> enable_if_t<N != 1, bool> {
                byte bs[N];
                return get_n(bs, i, e) && (t = be_to_t<T>(bs), true);
            }

    }

    template <unsigned N, typename T, typename II>
        inline auto get(T& t, II& i, II e)
            -> enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value, bool>
        {
            return bits::get<N>(t, i, e);
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

        template <typename X, typename O, typename Cx, typename ...P>
            inline bool poke_(O& o, Cx& cx, P... p) {
                return poke_(option<2>(), o, p...) || (cx|X::write_error::output_failure);
            }

    }

    template <typename O>
        inline bool poke(O& o, byte b)              { return bits::poke_(o, b); }

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, byte b, Cx& cx)      { return bits::poke_<X>(o, cx, b); }

}}

#endif // CXON_BIO_IO_HXX_
