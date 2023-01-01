// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_ALLOCATOR_HXX_
#define CXON_ALLOCATOR_HXX_

#include "cxon/utility.hxx"
#include <memory>


// interface ///////////////////////////////////////////////////////////////////

namespace cxon {

    CXON_PARAMETER(allocator, std::allocator<char>);

}

namespace cxon { namespace alc {

    template <typename Al, typename U>
        using rebind_t = typename std::allocator_traits<Al>::template rebind_alloc<U>;

    template <typename T, typename Al, typename ...A> // C++20
        constexpr T* uninitialized_construct_using_allocator(T* t, const Al& al, A&&... as);

    template <typename T, typename Al, typename ...A>
        constexpr T create_using_allocator(const Al& al, A&&... as);
    template <typename T, typename C, typename ...A>
        constexpr T create_using_allocator_of(const C& c, A&&... as);

    template <typename T, typename Al>
        struct basic_allocator;

    template <typename T, typename Cx, typename Al = decltype(allocator::value(std::declval<Cx&>().px, std::allocator<T>()))>
        inline auto make_context_allocator(Cx& cx) -> basic_allocator<T, Al>;
    template <typename T, typename Al>
        inline auto make_allocator(const Al& al) -> basic_allocator<T, Al> ;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace alc {

    namespace imp {

        template <typename T>
            struct uninitialized_ {
                template <typename Al, typename ...A>
                    static constexpr auto construct_using_allocator_(option<2>, T* t, const Al& al, A&&... as) -> decltype(new T(std::forward<A>(as)..., al)) {
                        return new (t) T(std::forward<A>(as)..., al);
                    }
                template <typename Al, typename ...A>
                    static constexpr auto construct_using_allocator_(option<1>, T* t, const Al& al, A&&... as) -> decltype(new T(std::allocator_arg, al, std::forward<A>(as)...)) {
                        return new (t) T(std::allocator_arg, al, std::forward<A>(as)...);
                    }
                template <typename Al, typename ...A>
                    static constexpr auto construct_using_allocator_(option<0>, T* t, const Al&, A&&... as) -> decltype(new T(std::forward<A>(as)...)) {
                        return new (t) T(std::forward<A>(as)...);
                    }
                template <typename Al, typename ...A>
                    static constexpr T* construct_using_allocator(T* t, const Al& al, A&&... as) {
                        return construct_using_allocator_(option<2>(), t, al, std::forward<A>(as)...);
                    }
            };
#       if defined(__GNUC__)
            // warning: '*(std::nullptr_t*'nullptr_type' not supported by direct_abstract_declarator)...' may be used uninitialized [-Wmaybe-uninitialized]
            template <>
                struct uninitialized_<std::nullptr_t> {
                    template <typename Al>
                        static constexpr std::nullptr_t* construct_using_allocator(std::nullptr_t* t, const Al&) {
                            return *t = nullptr, t;
                        }
                    template <typename Al>
                        static constexpr std::nullptr_t* construct_using_allocator(std::nullptr_t* t, const Al&, std::nullptr_t a) {
                            return *t = a, t;
                        }
                };
#       endif
        template <typename F, typename S>
            struct uninitialized_<std::pair<F, S>> {
                template <typename Al>
                    static constexpr std::pair<F, S>* construct_using_allocator(std::pair<F, S>* t, const Al& al) {
                        return  uninitialized_<F>::construct_using_allocator(&t->first, al),
                                uninitialized_<S>::construct_using_allocator(&t->second, al),
                                t
                        ;
                    }
                template <typename Al, typename AF, typename AS>
                    static constexpr std::pair<F, S>* construct_using_allocator(std::pair<F, S>* t, const Al& al, AF&& af, AS&& as) {
                        return  uninitialized_<F>::construct_using_allocator(&t->first, al, std::forward<AF>(af)),
                                uninitialized_<S>::construct_using_allocator(&t->second, al, std::forward<AS>(as)),
                                t
                        ;
                    }
                template <typename T, typename Al, typename Tp, std::size_t ...I>
                    static constexpr T* construct_with_(T* t, const Al& al, Tp&& tp, index_sequence<I...>) {
                        return uninitialized_<T>::construct_using_allocator(t, al, std::forward<typename std::tuple_element<I, Tp>::type>(std::get<I>(tp))...);
                    }
                template <typename T, typename Al, typename Tp>
                    static constexpr T* construct_with_(T* t, const Al& al, Tp&& tp) {
                        return construct_with_<T>(t, al, std::forward<Tp>(tp), make_index_sequence<std::tuple_size<Tp>::value>());
                    }
                template <typename Al, typename ...AF, typename ...AS>
                    static constexpr std::pair<F, S>* construct_using_allocator(std::pair<F, S>* t, const Al& al, std::piecewise_construct_t, std::tuple<AF...>&& af, std::tuple<AS...>&& as) {
                        return  construct_with_<F>(&t->first, al, std::forward<std::tuple<AF...>>(af)),
                                construct_with_<S>(&t->second, al, std::forward<std::tuple<AS...>>(as)),
                                t
                        ;
                    }
            };

    }
    template <typename T, typename Al, typename ...A>
        constexpr T* uninitialized_construct_using_allocator(T* t, const Al& al, A&&... as) {
            return imp::uninitialized_<T>::construct_using_allocator(t, al, std::forward<A>(as)...);
        }

    namespace imp {

        template <typename T>
            struct using_allocator_ {
                template <typename Al, typename ...A>
                    static constexpr auto create_(option<2>, const Al& al, A&&... as) -> decltype(T (std::forward<A>(as)..., al)) {
                        return T {std::forward<A>(as)..., al};
                    }
                template <typename Al, typename ...A>
                    static constexpr auto create_(option<1>, const Al& al, A&&... as) -> decltype(T (std::allocator_arg, al, std::forward<A>(as)...)) {
                        return T {std::allocator_arg, al, std::forward<A>(as)...};
                    }
                template <typename Al, typename ...A>
                    static constexpr auto create_(option<0>, const Al&, A&&... as) -> decltype(T (std::forward<A>(as)...)) {
                        return T {std::forward<A>(as)...};
                    }
                template <typename Al, typename ...A>
                    static constexpr T create(const Al& al, A&&... as) {
                        return create_(option<2>(), al, std::forward<A>(as)...);
                    }
            };
        template <typename F, typename S>
            struct using_allocator_<std::pair<F, S>> {
                template <typename Al>
                    static constexpr std::pair<F, S> create(const Al& al) {
                        return std::make_pair(
                            using_allocator_<F>::create(al),
                            using_allocator_<S>::create(al)
                        );
                    }
                template <typename Al, typename AF, typename AS>
                    static constexpr std::pair<F, S> create(const Al& al, AF&& af, AS&& as) {
                        return std::make_pair(
                            using_allocator_<F>::create(al, std::forward<AF>(af)),
                            using_allocator_<S>::create(al, std::forward<AS>(as))
                        );
                    }
                template <typename T, typename Al, typename Tp, std::size_t ...I>
                    static constexpr T create_with_(const Al& al, Tp&& tp, index_sequence<I...>) {
                        return using_allocator_<T>::create(al, std::forward<typename std::tuple_element<I, Tp>::type>(std::get<I>(tp))...);
                    }
                template <typename T, typename Al, typename Tp>
                    static constexpr T create_with_(const Al& al, Tp&& tp) {
                        return create_with_<T>(al, std::forward<Tp>(tp), make_index_sequence<std::tuple_size<Tp>::value>());
                    }
                template <typename Al, typename ...AF, typename ...AS>
                    static constexpr std::pair<F, S> create(const Al& al, std::piecewise_construct_t, std::tuple<AF...>&& af, std::tuple<AS...>&& as) {
                        return std::make_pair(
                            create_with_<F>(al, std::forward<std::tuple<AF...>>(af)),
                            create_with_<S>(al, std::forward<std::tuple<AS...>>(as))
                        );
                    }
            };

    }
    template <typename T, typename Al, class... A>
        constexpr T create_using_allocator(const Al& al, A&&... as) {
            return imp::using_allocator_<T>::create(al, std::forward<A>(as)...);
        }

    namespace imp {

        template <typename T, typename C, class... A>
            constexpr auto create_using_allocator_of_(option<1>, const C& c, A&&... as) -> decltype(c.get_allocator(), T()) {
                return using_allocator_<T>::create(c.get_allocator(), std::forward<A>(as)...);
            }
        template <typename T, typename C, class... A>
            constexpr auto create_using_allocator_of_(option<0>, const C&, A&&... as) -> T {
                return T {std::forward<A>(as)...};
            }

    }
    template <typename T, typename C, class... A>
        constexpr T create_using_allocator_of(const C& c, A&&... as) {
            return imp::create_using_allocator_of_<T>(option<1>(), c, std::forward<A>(as)...);
        }

    template <typename T, typename Al>
        struct basic_allocator {
            using type = rebind_t<Al, T>;

            basic_allocator(const type& al) : al_(al) {}

            T* create() {
                auto t = tr::allocate(al_, 1);
                return t ? construct(t), t : t;
            }
            T* create(std::size_t n) {
                auto p = tr::allocate(al_, n);
                    if (p) for (T *t = p, *te = t + n; t != te; ++t)
                        construct(t);
                return p;
            }

            void release(T* t) {
                if (t) destroy(t), tr::deallocate(al_, t, 1);
            }
            void release(T* p, std::size_t n) {
                if (p) {
                    for (T *t = p, *te = t + n; t != te; ++t)
                        destroy(t);
                    tr::deallocate(al_, p, n);
                }
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
                using tr = std::allocator_traits<type>;
                type al_;
        };

    template <typename T, typename Cx, typename Al>
        inline auto make_context_allocator(Cx& cx) -> basic_allocator<T, Al> {
            return basic_allocator<T, Al>(allocator::value(cx.px, std::allocator<T>()));
        }
    template <typename T, typename Al>
        inline auto make_allocator(const Al& al) -> basic_allocator<T, Al> {
            return basic_allocator<T, Al>(al);
        }

}}

#endif // CXON_ALLOCATOR_HXX_
