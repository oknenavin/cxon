// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMPOUND_HXX_
#define CXON_CBOR_COMPOUND_HXX_

#include "cxon/lang/common/bio/bio.hxx"

namespace cxon { // pointer

    template <typename X, typename T>
        struct read<CBOR<X>, T*> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(T*& t, II& i, II e, Cx& cx) {
                    auto const b = bio::peek(i, e);
                    if (b == X::nil)
                        return bio::get(i, e), t = nullptr, true;
                    auto ax = bio::allocator::value(cx.px, std::allocator<T>());
                    typename std::allocator_traits<decltype(ax)>::template rebind_alloc<T> at;
                        using al = std::allocator_traits<decltype(at)>;
                    T *const n = al::allocate(at, 1); al::construct(at, n);
                        if (!read_value<J>(*n, i, e, cx))
                            return al::destroy(at, n), al::deallocate(at, n, 1), false;
                    return t = n, true;
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, T*> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const T* t, Cx& cx) {
                    return t ? write_value<J>(o, *t, cx) : bio::poke<J>(o, X::nil, cx);
                }
        };

}

namespace cxon { // array

    namespace bits {

        template <typename X, typename II, typename Cx>
            inline bool read_size_(bio::byte m, size_t& s, II& i, II e, Cx& cx) {
                II const o = i;
                switch (bio::byte const b = bio::get(i, e) - m) {
                    case  0: case  1: case  2: case  3: case  4: case  5: case  6: case  7:
                    case  8: case  9: case 10: case 11: case 12: case 13: case 14: case 15:
                    case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
                        return  s = b, true;
                    case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
                        return  (bio::get(s, b - 23, i, e)) ||
                                (bio::rewind(i, o), cx|cbor::read_error::size_invalid)
                        ;
                    default:
                        return  (bio::rewind(i, o), cx|cbor::read_error::size_invalid);
                }
            }

        template <typename X, size_t N, typename II, typename Cx>
            inline bool read_size_n_(bio::byte m, size_t& s, II& i, II e, Cx& cx) {
                II const o = i;
                return  read_size_<X>(m, s, i, e, cx) &&
                        (s == N || (bio::rewind(i, o), cx|cbor::read_error::size_invalid))
                ;
            }

    }

    template <typename X, typename T, size_t N>
        struct read<CBOR<X>, T[N]> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(T (&t)[N], II& i, II e, Cx& cx) {
                    size_t s, j = 0;
                    if (bits::read_size_n_<J, N>(J::arr, s, i, e, cx))
                        for ( ; j != N && read_value<J>(t[j], i, e, cx); ++j) ;
                    return j == N;
                }
        };

    namespace bits {

        template <typename X, typename O, typename Cx>
            inline bool write_size_(O& o, bio::byte m, size_t s, Cx& cx) {
                unsigned const n = bits::bytes_(s);
                return s > 23 ?
                    bio::poke<X>(o, bio::byte(m + 23 + n), cx) && bio::poke<X>(o, s, n, cx) :
                    bio::poke<X>(o, bio::byte(m + s), cx)
                ;
            }

    }

    template <typename X, typename T, size_t N>
        struct write<CBOR<X>, T[N]> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    size_t i = 0;
                    if (bits::write_size_<J>(o, J::arr, N, cx))
                        for ( ; i != N && write_value<J>(o, t[i], cx); ++ i) ;
                    return i == N;
                }
        };

}

namespace cxon { // character pointer & array

}

#endif // CXON_CBOR_COMPOUND_HXX_
