// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMPOUND_HXX_
#define CXON_CBOR_COMPOUND_HXX_

#include "cxon/lang/cbor/common/container.hxx"
#include <cstring>

namespace cxon { // array/read

    namespace cbor { namespace bits {

        template <typename X, typename FI, typename II, typename Cx>
            inline bool read_bytes_(FI f, size_t n, II& i, II e, Cx& cx) {
                return  (bio::get(f, i, e, n)) ||
                        (cx|cbor::read_error::unexpected)
                ;
            }

        template <typename X, typename FI, typename II, typename Cx>
            inline bool read_elements_(FI f, size_t n, II& i, II e, Cx& cx) {
                while (n != 0 && read_value<X>(*f, i, e, cx)) 
                    --n, ++f;
                return n == 0;
            }

    }}

    template <typename X, typename T, size_t N>
        struct read<CBOR<X>, T[N]> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(T (&t)[N], II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<Y>(std::begin(t), std::end(t), i, e, cx);
                }
        };

}

namespace cxon { // array/write

    template <typename X, typename T, size_t N>
        struct write<CBOR<X>, T[N]> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    return cbor::cnt::write_array<J>(o, std::begin(t), std::end(t), cx);
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
                if (!cnt::read_size<X>(n, i, e, cx))
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

                        if (!cnt::read_size<X>(ns, i, e, cx))
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
                        cbor::cnt::write_array<Y>(o, t, t + std::char_traits<T>::length(t), cx) :
                        bio::poke<Y>(o, Y::nil, cx)
                    ;
                }
        };

}

#endif // CXON_CBOR_COMPOUND_HXX_
