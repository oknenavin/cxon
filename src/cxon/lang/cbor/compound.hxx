// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMPOUND_HXX_
#define CXON_CBOR_COMPOUND_HXX_

#include "cxon/lang/common/bio/bio.hxx"

namespace cxon { // array

    namespace bits {

        template <typename X, typename II, typename Cx>
            inline bool read_size_(size_t& s, II& i, II e, Cx& cx) {
                return read_unsigned_<X>(s, i, e, cx) || cx|cbor::read_error::size_invalid;
            }
        template <typename X, typename II, typename Cx>
            inline bool read_size_(size_t& s, size_t n, II& i, II e, Cx& cx) {
                II const o = i; size_t t;
                return  (read_size_<X>(t, i, e, cx) && t <= n && (s = t, true)) ||
                        (bio::rewind(i, o), cx|cbor::read_error::size_invalid)
                ;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_bytes_(T*& t, size_t n, II& i, II e, Cx& cx) {
                return  (bio::get(t, i, e, n) && (t += n, true)) ||
                        (cx|cbor::read_error::unexpected)
                ;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_fix_(T*& f, T* l, II& i, II e, Cx& cx) {
                size_t n;
                return  read_size_<X>(n, std::distance(f, l), i, e, cx) &&
                        read_bytes_<X>(f, n, i, e, cx)
                ;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_var_(T* f, T* l, II& i, II e, Cx& cx) {
                bio::byte const m = bio::get(i, e) & X::mjr;
                    for (auto b = bio::peek(i, e, 0); b != X::brk; b = bio::peek(i, e, 0)) {
                        if (m != (b & X::mjr))
                            return cx|cbor::read_error::array_invalid;
                        if (!read_array_b_fix_<X>(f, l, i, e, cx))
                            return false;
                    }
                return bio::get(i, e, 0), true;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_(T* f, T* l, bio::byte m, II& i, II e, Cx& cx) {
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx|cbor::read_error::size_invalid;
                    case 0x1F:                          return read_array_b_var_<X>(f, l, i, e, cx);
                    default:                            return read_array_b_fix_<X>(f, l, i, e, cx);
                }
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_fix_(T* f, size_t n, II& i, II e, Cx& cx) {
                while (n != 0 && read_value<X>(*f, i, e, cx)) 
                    --n, ++f;
                return n == 0;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_fix_(T* f, T* l, II& i, II e, Cx& cx) {
                size_t n;
                return read_size_<X>(n, std::distance(f, l), i, e, cx) && read_array_w_fix_<X>(f, n, i, e, cx);
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_var_(T* f, T* l, II& i, II e, Cx& cx) {
                II const o = i;
                    for (bio::get(i, e); f != l && bio::peek(i, e, 0) != X::brk; ++f)
                        if (!read_value<X>(*f, i, e, cx))
                            return false;
                return  (bio::get(i, e, 0) == X::brk) ||
                        (bio::rewind(i, o), cx|cbor::read_error::size_invalid)
                ;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_(T* f, T* l, bio::byte m, II& i, II e, Cx& cx) {
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx|cbor::read_error::size_invalid;
                    case 0x1F:                          return read_array_w_var_<X>(f, l, i, e, cx);
                    default:                            return read_array_w_fix_<X>(f, l, i, e, cx);
                }
            }

        template <typename X, typename T, typename II, typename Cx>
            inline auto read_array_(T* f, T* l, II& i, II e, Cx& cx)
                -> enable_if_t< std::is_assignable<decltype(*f), decltype(*i)>::value, bool>
            {
                auto const m = bio::peek(i, e) ;
                switch (m & X::mjr) {
                    case 0x40: case 0x60:   return read_array_b_<X>(f, l, m, i, e, cx);
                    case 0x80:              return read_array_w_<X>(f, l, m, i, e, cx);
                    default:                return cx|cbor::read_error::array_invalid;
                }
            }
        template <typename X, typename T, typename II, typename Cx>
            inline auto read_array_(T* f, T* l, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_assignable<decltype(*f), decltype(*i)>::value, bool>
            {
                auto const m = bio::peek(i, e);
                switch (m & X::mjr) {
                    case 0x80:  return read_array_w_<X>(f, l, m, i, e, cx);
                    default:    return cx|cbor::read_error::array_invalid;
                }
            }

    }

    template <typename X, typename T, size_t N>
        struct read<CBOR<X>, T[N]> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(T (&t)[N], II& i, II e, Cx& cx) {
                    return bits::read_array_<Y>(std::begin(t), std::end(t), i, e, cx);
                }
        };

    namespace bits {

        template <typename X, typename O, typename Cx>
            inline bool write_count_(O& o, bio::byte m, size_t s, Cx& cx) {
                unsigned const p = bits::power_(s);
                return s > 0x17 ?
                    bio::poke<X>(o, bio::byte(m + 0x18 + p), cx) && bio::poke<X>(o, s, 1 << p, cx) :
                    bio::poke<X>(o, bio::byte(m + s), cx)
                ;
            }

        template <typename X, typename T, typename O, typename Cx>
            inline auto write_array_(O& o, const T* f, const T* l, Cx& cx) -> enable_if_t<sizeof(T) == 1 &&  is_char<T>::value, bool> {
                size_t const n = std::distance(f, l);
                return  write_count_<X>(o, X::ustr, n, cx) &&
                        bio::poke<X>(o, f, n, cx)
                ;
            }
        template <typename X, typename T, typename O, typename Cx>
            inline auto write_array_(O& o, const T* f, const T* l, Cx& cx) -> enable_if_t<sizeof(T) == 1 && !is_char<T>::value, bool> {
                size_t const n = std::distance(f, l);
                return  write_count_<X>(o, X::bstr, n, cx) &&
                        bio::poke<X>(o, f, n, cx)
                ;
            }
        template <typename X, typename T, typename O, typename Cx>
            inline auto write_array_(O& o, const T* f, const T* l, Cx& cx) -> enable_if_t<sizeof(T) != 1, bool> {
                if (write_count_<X>(o, X::arr, std::distance(f, l), cx))
                    for ( ; f != l && write_value<X>(o, *f, cx); ++f) ;
                return f == l;
            }

    }

    template <typename X, typename T, size_t N>
        struct write<CBOR<X>, T[N]> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    return bits::write_array_<J>(o, std::begin(t), std::end(t), cx);
                }
        };

}

namespace cxon { // pointer

    namespace bits {

        template <typename X, typename T, typename Ax>
            struct allocator_ {
                T* create() {
                    auto t = tr::allocate(al_, 1);
                    return tr::construct(al_, t), t;
                }
                T* create(size_t n) {
                    auto p = tr::allocate(al_, n);
                        for (T *t = p, *te = t + n; t != te; ++t)
                            tr::construct(al_, t);
                    return p;
                }

                void release(T* t) {
                    tr::destroy(al_, t), tr::deallocate(al_, t, 1);
                }
                void release(T* p, size_t n) {
                    for (T *t = p, *te = t + n; t != te; ++t)
                        tr::destroy(al_, t);
                    tr::deallocate(al_, p, n);
                }

                private:
                    using al = typename std::allocator_traits<Ax>::template rebind_alloc<T>;
                    using tr = std::allocator_traits<al>;
                    al al_;
            };

        template <typename X, typename T, typename Cx>
            inline auto make_allocator_(Cx& cx)
                -> allocator_<X, T, decltype(bio::allocator::value(cx.px, std::allocator<T>()))>
            {
                return allocator_<X, T, decltype(bio::allocator::value(cx.px, std::allocator<T>()))>{};
            }

    }

    template <typename X, typename T>
        struct read<CBOR<X>, T*> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(T*& t, II& i, II e, Cx& cx) {
                    auto const b = bio::peek(i, e);
                    if (b == Y::nil)
                        return bio::get(i, e), t = nullptr, true;

                    auto al = bits::make_allocator_<X, T>(cx);

                    switch (b & Y::mjr) {
                        case Y::arr: {
                            size_t n;
                            if (!bits::read_size_<Y>(n, i, e, cx))
                                return false;
                        
                            T *const a = al.create(n);
                                if (!a || !bits::read_array_w_fix_<Y>(a, n, i, e, cx)) {
                                    return al.release(a, n), false;
                                }
                            return t = a, true;
                        }
                        default: {
                            T *const v = al.create();
                                if (!v || !read_value<Y>(*v, i, e, cx))
                                    return al.release(v), false;
                            return t = v, true;
                        }
                    }
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, T*> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const T* t, Cx& cx) {
                    return t ? write_value<Y>(o, *t, cx) : bio::poke<Y>(o, Y::nil, cx);
                }
        };

}

#endif // CXON_CBOR_COMPOUND_HXX_
