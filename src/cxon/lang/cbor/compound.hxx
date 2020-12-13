// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMPOUND_HXX_
#define CXON_CBOR_COMPOUND_HXX_

#include "cxon/lang/common/bio/bio.hxx"
#include <cstring>

namespace cxon { // array/read

    namespace cbor { namespace bits {

        template <typename X, typename II, typename Cx>
            inline bool read_size_(size_t& s, II& i, II e, Cx& cx) {
                return read_unsigned_<X>(s, i, e, cx) || cx|cbor::read_error::size_invalid;
            }
        template <typename X, typename II, typename Cx>
            inline bool read_size_le_(size_t& s, size_t n, II& i, II e, Cx& cx) {
                II const o = i; size_t t;
                return  (read_size_<X>(t, i, e, cx) && t <= n && (s = t, true)) ||
                        (bio::rewind(i, o), cx|cbor::read_error::size_invalid)
                ;
            }
        template <typename X, typename II, typename Cx>
            inline bool read_size_eq_(size_t& s, size_t n, II& i, II e, Cx& cx) {
                II const o = i; size_t t;
                return  (read_size_<X>(t, i, e, cx) && t == n && (s = t, true)) ||
                        (bio::rewind(i, o), cx|cbor::read_error::size_invalid)
                ;
            }

        template <typename X, typename FI, typename II, typename Cx>
            inline bool read_bytes_(FI f, size_t n, II& i, II e, Cx& cx) {
                return  (bio::get(f, i, e, n)) ||
                        (cx|cbor::read_error::unexpected)
                ;
            }

        template <typename X, typename FI, typename II, typename Cx>
            inline bool read_array_b_fix_(FI& f, FI l, II& i, II e, Cx& cx) {
                size_t n;
                return  read_size_le_<X>(n, std::distance(f, l), i, e, cx) &&
                        read_bytes_<X>(f, n, i, e, cx) && (std::advance(f, n), true);
                ;
            }
        template <typename X, typename FI, typename II, typename Cx>
            inline bool read_array_b_var_(FI f, FI l, II& i, II e, Cx& cx) {
                bio::byte const m = bio::get(i, e) & X::mjr;
                    for (auto b = bio::peek(i, e, 0); b != X::brk; b = bio::peek(i, e, 0)) {
                        if (m != (b & X::mjr))
                            return cx|cbor::read_error::array_invalid;
                        if (!read_array_b_fix_<X>(f, l, i, e, cx))
                            return false;
                    }
                return bio::get(i, e, 0), true;
            }
        template <typename X, typename FI, typename II, typename Cx>
            inline bool read_array_b_(FI f, FI l, bio::byte m, II& i, II e, Cx& cx) {
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx|cbor::read_error::size_invalid;
                    case 0x1F:                          return read_array_b_var_<X>(f, l, i, e, cx);
                    default:                            return read_array_b_fix_<X>(f, l, i, e, cx);
                }
            }

        template <typename X, typename FI, typename II, typename Cx>
            inline bool read_elements_(FI f, size_t n, II& i, II e, Cx& cx) {
                while (n != 0 && read_value<X>(*f, i, e, cx)) 
                    --n, ++f;
                return n == 0;
            }

        template <typename X, typename FI, typename II, typename Cx>
            inline bool read_array_w_fix_(FI f, FI l, II& i, II e, Cx& cx) {
                size_t n;
                return read_size_le_<X>(n, std::distance(f, l), i, e, cx) && read_elements_<X>(f, n, i, e, cx);
            }
        template <typename X, typename FI, typename II, typename Cx>
            inline bool read_array_w_var_(FI f, FI l, II& i, II e, Cx& cx) {
                II const o = i;
                    for (bio::get(i, e); f != l && bio::peek(i, e, 0) != X::brk; ++f)
                        if (!read_value<X>(*f, i, e, cx))
                            return false;
                return  (bio::get(i, e, 0) == X::brk) ||
                        (bio::rewind(i, o), cx|cbor::read_error::size_invalid)
                ;
            }
        template <typename X, typename FI, typename II, typename Cx>
            inline bool read_array_w_(FI f, FI l, bio::byte m, II& i, II e, Cx& cx) {
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx|cbor::read_error::size_invalid;
                    case 0x1F:                          return read_array_w_var_<X>(f, l, i, e, cx);
                    default:                            return read_array_w_fix_<X>(f, l, i, e, cx);
                }
            }

        template <typename X, typename FI, typename II, typename Cx>
            inline auto read_array_(FI f, FI l, II& i, II e, Cx& cx)
                -> enable_if_t< std::is_assignable<decltype(*f), decltype(*i)>::value, bool>
            {
                auto const m = bio::peek(i, e) ;
                switch (m & X::mjr) {
                    case X::bstr: case X::tstr: return read_array_b_<X>(f, l, m, i, e, cx);
                    case X::arr:                return read_array_w_<X>(f, l, m, i, e, cx);
                    default:                    return cx|cbor::read_error::array_invalid;
                }
            }
        template <typename X, typename FI, typename II, typename Cx>
            inline auto read_array_(FI f, FI l, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_assignable<decltype(*f), decltype(*i)>::value, bool>
            {
                auto const m = bio::peek(i, e);
                switch (m & X::mjr) {
                    case X::arr:    return read_array_w_<X>(f, l, m, i, e, cx);
                    default:        return cx|cbor::read_error::array_invalid;
                }
            }

    }}

    template <typename X, typename T, size_t N>
        struct read<CBOR<X>, T[N]> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(T (&t)[N], II& i, II e, Cx& cx) {
                    return cbor::bits::read_array_<Y>(std::begin(t), std::end(t), i, e, cx);
                }
        };

}

namespace cxon { // array/write

    namespace cbor { namespace bits {

        template <typename X, typename O, typename Cx>
            inline bool write_size_(O& o, bio::byte m, size_t s, Cx& cx) {
                unsigned const p = bits::power_(s);
                return s > 0x17 ?
                    bio::poke<X>(o, bio::byte(m + 0x18 + p), cx) && bio::poke<X>(o, s, 1 << p, cx) :
                    bio::poke<X>(o, bio::byte(m + s), cx)
                ;
            }

        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) == 1 &&  is_char<T>::value, bool> {
                size_t const n = std::distance(f, l);
                return  write_size_<X>(o, X::tstr, n, cx) &&
                        bio::poke<X>(o, f, n, cx)
                ;
            }
        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) == 1 && !is_char<T>::value, bool> {
                size_t const n = std::distance(f, l);
                return  write_size_<X>(o, X::bstr, n, cx) &&
                        bio::poke<X>(o, f, n, cx)
                ;
            }
        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) != 1, bool> {
                if (write_size_<X>(o, X::arr, std::distance(f, l), cx))
                    for ( ; f != l && write_value<X>(o, *f, cx); ++f) ;
                return f == l;
            }

    }}

    template <typename X, typename T, size_t N>
        struct write<CBOR<X>, T[N]> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    return cbor::bits::write_array_<J>(o, std::begin(t), std::end(t), cx);
                }
        };

}

namespace cxon { // pointer/read

    namespace cbor { namespace bits {

        template <typename X, typename T, typename Ax>
            struct allocator_ {
                T* create() {
                    auto t = tr::allocate(al_, 1);
                    return construct(t), t;
                }
                T* create(size_t n) {
                    auto p = tr::allocate(al_, n);
                        for (T *t = p, *te = t + n; t != te; ++t)
                            construct(t);
                    return p;
                }

                void release(T* t) {
                    destroy(t), tr::deallocate(al_, t, 1);
                }
                void release(T* p, size_t n) {
                    for (T *t = p, *te = t + n; t != te; ++t)
                        destroy(t);
                    tr::deallocate(al_, p, n);
                }

                template <typename U = T> auto construct(U *u)
                    -> enable_if_t<!std::is_trivial<U>::value> { tr::construct(al_, u); }
                template <typename U = T> auto construct(U *)
                    -> enable_if_t< std::is_trivial<U>::value> {}

                template <typename U = T> auto destroy(U *u)
                    -> enable_if_t<!std::is_trivial<U>::value> { tr::destroy(al_, u); }
                template <typename U = T> auto destroy(U *)
                    -> enable_if_t< std::is_trivial<U>::value> {}

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

        template <typename X, typename T, typename RX, typename II, typename Cx>
            inline bool read_pointer_x_fix_(T*& t, RX rx_, II& i, II e, Cx& cx) {
                size_t n;
                if (!bits::read_size_<X>(n, i, e, cx))
                    return false;
                auto al = bits::make_allocator_<X, T>(cx);
                T *const a = al.create(n + 1);
                    if (!a || !rx_(a, n, i, e, cx))
                        return al.release(a, n + 1), false;
                return a[n] = {}, t = a, true;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_pointer_b_fix_(T*& t, II& i, II e, Cx& cx) {
                return read_pointer_x_fix_<X>(t, &read_bytes_<X, T*, II, Cx>, i, e, cx);
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_pointer_b_var_(T*& t, II& i, II e, Cx& cx) {
                auto al = bits::make_allocator_<X, T>(cx);
                {   bio::byte const m = bio::get(i, e) & X::mjr;
                    size_t cs = 0, ns;
                    T *p = nullptr;
                    for ( ; ; ) {
                        auto const b = bio::peek(i, e, 0);

                        if (b == X::brk)
                            return bio::get(i, e, 0), (p || (p = al.create())) && (p[cs] = {}, t = p, true);

                        if (m != (b & X::mjr))
                            return al.release(p, cs + 1), cx|cbor::read_error::array_invalid;

                        if (!bits::read_size_<X>(ns, i, e, cx))
                            return al.release(p, cs + 1), false;
                        T *const n = al.create(cs + ns + 1);
                            std::copy_n(p, cs, n);
                        al.release(p, cs + 1), p = n;

                        if (!read_bytes_<X>(p + cs, ns, i, e, cx))
                            return al.release(p, cs + ns + 1), false;

                        cs += ns;
                    }
                }
                return CXON_ASSERT(0, "unexpected"), false;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_pointer_b_(T*& t, bio::byte m, II& i, II e, Cx& cx) {
                if (m == X::nil)
                    return bio::get(i, e), t = nullptr, true;
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx|cbor::read_error::size_invalid;
                    case 0x1F:                          return read_pointer_b_var_<X>(t, i, e, cx);
                    default:                            return read_pointer_b_fix_<X>(t, i, e, cx);
                }
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_pointer_w_fix_(T*& t, II& i, II e, Cx& cx) {
                return read_pointer_x_fix_<X>(t, &read_elements_<X, T*, II, Cx>, i, e, cx);
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_pointer_w_var_(T*& t, II& i, II e, Cx& cx) {
                bio::get(i, e);
                {   auto al = bits::make_allocator_<X, T>(cx);
                    T *f = al.create(4), *l = f + 4;
                    for (T *p = f; ; ++p) {
                        if (p == l) {
                            T *const n = al.create(2 * (l - f));
                                std::copy_n(f, l - f, n);
                            p = n + (l - f), al.release(f, l - f);
                            l = n + 2 * (l - f), f = n;
                        }
                        if (bio::peek(i, e, 0) == X::brk)
                            return bio::get(i, e, 0), p && (*p = {}, t = f, true);
                        if (!p || !read_value<X>(*p, i, e, cx))
                            return al.release(f, l - f), (p || cx|cbor::read_error::unexpected, false);
                    }
                }
                return CXON_ASSERT(0, "unexpected"), false;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_pointer_w_(T*& t, bio::byte m, II& i, II e, Cx& cx) {
                if (m == X::nil)
                    return bio::get(i, e), t = nullptr, true;
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx|cbor::read_error::size_invalid;
                    case 0x1F:                          return read_pointer_w_var_<X>(t, i, e, cx);
                    default:                            return read_pointer_w_fix_<X>(t, i, e, cx);
                }
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_pointer_t_(T*& t, bio::byte m, II& i, II e, Cx& cx) {
                if (m == X::nil)
                    return bio::get(i, e), t = nullptr, true;
                auto al = bits::make_allocator_<X, T>(cx);
                T *const v = al.create();
                    if (!v || !read_value<X>(*v, i, e, cx))
                        return al.release(v), false;
                return t = v, true;
            }

    }}

    template <typename X, typename T>
        struct read<CBOR<X>, T*> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static auto value(T*& t, II& i, II e, Cx& cx)
                    -> enable_if_t< std::is_assignable<decltype(*t), decltype(*i)>::value, bool>
                {
                    auto const m = bio::peek(i, e);
                    switch (m & Y::mjr) {
                        case Y::bstr: case Y::tstr: return cbor::bits::read_pointer_b_<Y>(t, m, i, e, cx);
                        case Y::arr:                return cbor::bits::read_pointer_w_<Y>(t, m, i, e, cx);
                        default:                    return cbor::bits::read_pointer_t_<Y>(t, m, i, e, cx);
                    }
                }
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static auto value(T*& t, II& i, II e, Cx& cx)
                    -> enable_if_t<!std::is_assignable<decltype(*t), decltype(*i)>::value, bool>
                {
                    auto const m = bio::peek(i, e);
                    switch (m & Y::mjr) {
                        case Y::arr:    return cbor::bits::read_pointer_w_<Y>(t, m, i, e, cx);
                        default:        return cbor::bits::read_pointer_t_<Y>(t, m, i, e, cx);
                    }
                }
        };
    template <typename X, typename T>
        struct read<CBOR<X>, const T*> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(const T*& t, II& i, II e, Cx& cx) {
                    return read<CBOR<X>, T*>::value((T*&)t, i, e, cx);
                }
        };

}

namespace cxon { // pointer/write

    template <typename X, typename T>
        struct write<CBOR<X>, T*> {
            template <typename O, typename Cx, typename U = T, typename Y = CBOR<X>>
                static auto value(O& o, const T* t, Cx& cx)
                    -> enable_if_t<!is_char<U>::value, bool>
                {
                    return t ?
                        write_value<Y>(o, *t, cx) :
                        bio::poke<Y>(o, Y::nil, cx)
                    ;
                }
            template <typename O, typename Cx, typename U = T, typename Y = CBOR<X>>
                static auto value(O& o, const T* t, Cx& cx)
                    -> enable_if_t< is_char<U>::value, bool>
                {
                    return t ?
                        cbor::bits::write_array_<Y>(o, t, t + std::char_traits<T>::length(t), cx) :
                        bio::poke<Y>(o, Y::nil, cx)
                    ;
                }
        };

}

#endif // CXON_CBOR_COMPOUND_HXX_
