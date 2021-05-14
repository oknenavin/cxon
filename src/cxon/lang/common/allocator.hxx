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

    template <typename X, typename T, typename Ax>
        struct context_allocator {
            T* create() {
                auto t = tr::allocate(al_, 1);
                return construct(t), t;
            }
            T* create(std::size_t n) {
                auto p = tr::allocate(al_, n);
                    for (T *t = p, *te = t + n; t != te; ++t)
                        construct(t);
                return p;
            }

            void release(T* t) {
                destroy(t), tr::deallocate(al_, t, 1);
            }
            void release(T* p, std::size_t n) {
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
        inline auto make_context_allocator(Cx& cx)
            -> context_allocator<X, T, decltype(allocator::value(cx.px, std::allocator<T>()))>
        {
            return context_allocator<X, T, decltype(allocator::value(cx.px, std::allocator<T>()))>{};
        }

}

#endif // CXON_ALLOCATOR_HXX_
