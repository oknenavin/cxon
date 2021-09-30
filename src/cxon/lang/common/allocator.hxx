// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_ALLOCATOR_HXX_
#define CXON_ALLOCATOR_HXX_

#include "cxon/utility.hxx"
#include <memory>

namespace cxon {

    CXON_PARAMETER(allocator, std::allocator<char>);

    template <typename T, typename Ax>
        struct basic_allocator {
            using type = typename std::allocator_traits<Ax>::template rebind_alloc<T>;

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
    template <typename T, typename Cx, typename Ax = decltype(allocator::value(std::declval<Cx&>().px, std::allocator<T>()))>
        inline auto make_context_allocator(Cx& cx) -> basic_allocator<T, Ax> {
            return basic_allocator<T, Ax>(allocator::value(cx.px, std::allocator<T>()));
        }
    template <typename T, typename Ax>
        inline auto make_allocator(const Ax& ax) -> basic_allocator<T, Ax> {
            return basic_allocator<T, Ax>(ax);
        }

    template <typename T, typename E = void_t<>>
        struct is_allocator_aware                                           : std::false_type {};
    template <typename T>
        struct is_allocator_aware<T, void_t<typename T::allocator_type>>    : std::true_type {};

    namespace imp {

        template <typename T, typename C, bool E = is_allocator_aware<T>::value && is_allocator_aware<C>::value>
            struct value_in_context_ {
                template <typename ...A>
                    static constexpr T make(A&&... as) {
                        return T { std::forward<A>(as)... };
                    }
            };
        template <typename T, typename C>
            struct value_in_context_<T, C, true> {
                template <typename ...A>
                    static constexpr T make(A&&... as) {
                        using al = typename std::allocator_traits<typename C::allocator_type>::template rebind_alloc<typename T::value_type>;
                        return T { std::forward<A>(as)..., al{} };
                    }
            };

    }
    template <typename T, typename C, typename ...A>
        constexpr T make_value_in_context(A&&... as) {
            return imp::value_in_context_<T, C>::make(std::forward<A>(as)...);
        }
    template <typename T, typename C, typename ...A>
        constexpr T make_value_in_context(const C&, A&&... as) {
            return imp::value_in_context_<T, C>::make(std::forward<A>(as)...);
        }

}

#endif // CXON_ALLOCATOR_HXX_
