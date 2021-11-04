// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_ALLOCATOR_HXX_
#define CXON_ALLOCATOR_HXX_

#include "cxon/utility.hxx"
#include <memory>


// interface ///////////////////////////////////////////////////////////////////

namespace cxon {

#   if __cplusplus > 201703L /* C++20 */
        CXON_PARAMETER(allocator, std::allocator<void>);
#   else
        CXON_PARAMETER(allocator, std::allocator<char>);
#   endif

}

namespace cxon { namespace alc {

    template <typename Al, typename U>
        using rebind_t = typename std::allocator_traits<Al>::template rebind_alloc<U>;

    template <typename T, typename Al, typename ...A> // C++20
        constexpr T* uninitialized_construct_using_allocator(T* t, const Al& al, A&&... as);

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

        template <typename T, typename Al, typename ...A>
            constexpr auto uninitialized_construct_using_allocator_(option<2>, T* t, const Al& al, A&&... as) -> decltype(new T(std::forward<A>(as)..., al)) {
                return new (t) T(std::forward<A>(as)..., al);
            }
        template <typename T, typename Al, typename ...A>
            constexpr auto uninitialized_construct_using_allocator_(option<1>, T* t, const Al& al, A&&... as) -> decltype(new T(std::allocator_arg, al, std::forward<A>(as)...)) {
                return new (t) T(std::allocator_arg, al, std::forward<A>(as)...);
            }
        template <typename T, typename Al, typename ...A>
            constexpr auto uninitialized_construct_using_allocator_(option<0>, T* t, const Al&, A&&... as) -> decltype(new T(std::forward<A>(as)...)) {
                return new (t) T(std::forward<A>(as)...);
            }

    }
    template <typename T, typename Al, typename ...A>
        constexpr T* uninitialized_construct_using_allocator(T* t, const Al& al, A&&... as) {
            return imp::uninitialized_construct_using_allocator_<T>(option<2>(), t, al, std::forward<A>(as)...);
        }

    namespace imp {

        template <typename T, typename C, typename ...A>
            constexpr auto create_using_allocator_of_(option<2>, const C& c, A&&... as) -> decltype(T(std::forward<A>(as)..., c.get_allocator())) {
                return T {std::forward<A>(as)..., c.get_allocator()};
            }
        template <typename T, typename C, typename ...A>
            constexpr auto create_using_allocator_of_(option<1>, const C& c, A&&... as) -> decltype(T(std::allocator_arg, c.get_allocator(), std::forward<A>(as)...)) {
                return T {std::allocator_arg, c.get_allocator(), std::forward<A>(as)...};
            }
        template <typename T, typename C, typename ...A>
            constexpr auto create_using_allocator_of_(option<0>, const C&, A&&... as) -> decltype(T(std::forward<A>(as)...)) {
                return T {std::forward<A>(as)...};
            }

    }
    template <typename T, typename C, class... A>
        constexpr T create_using_allocator_of(const C& c, A&&... as) {
            return imp::create_using_allocator_of_<T>(option<2>(), c, std::forward<A>(as)...);
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
