// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_NODE_VALUE_HXX_
#define CXON_NODE_VALUE_HXX_

#include "cxon/utility.hxx"
#include "allocator.hxx"

#include <type_traits>
#include <memory>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace value {

    // <lang>/node/node.hxx

    template <typename N>
        struct controller;

    // value access
        template <typename T, typename N>
            constexpr T& get(N& n) noexcept;

    // move construct & assign
        template <typename T, typename N>
            inline void move_construct(N& n, N&& o)
                noexcept(noexcept(controller<N>::template move_construct<T>(n, std::forward<N>(o))));
        template <typename T, typename N>
            inline N& move_assign(N& n, N&& o)
                noexcept(noexcept(controller<N>::template move_assign<T>(n, std::forward<N>(o))));

    // copy construct & assign
        template <typename T, typename N>
            inline void copy_construct(N& n, const N& o)
                noexcept(noexcept(controller<N>::template copy_construct<T>(n, o)));
        template <typename T, typename N>
            inline N& copy_assign(N& n, const N& o)
                noexcept(noexcept(controller<N>::template copy_assign<T>(n, o)));

    // construct & destruct
        template <typename T, typename N, typename ...A>
            inline void construct(N& n, A&&... t)
                noexcept(noexcept(controller<N>::template construct<T, A...>(n, std::forward<A>(t)...)));
        template <typename T, typename N>
            inline void destruct(N& n) noexcept;

    // swap
        template <typename N, typename F, typename S>
            inline void swap(N& f, N& s)
                noexcept(noexcept(controller<N>::template swap<F, S>(f, s)));

    // exception specification
        template <typename N, typename T, typename ...A>
            using is_nothrow_constructible      = typename controller<N>::template is_nothrow_constructible<T, A...>;

        template <typename N>
            using is_nothrow_move_constructible = typename controller<N>::is_nothrow_move_constructible;
        template <typename N>
            using is_nothrow_move_assignable    = typename controller<N>::is_nothrow_move_assignable;

        template <typename N>
            using is_nothrow_copy_constructible = typename controller<N>::is_nothrow_copy_constructible;
        template <typename N>
            using is_nothrow_copy_assignable    = typename controller<N>::is_nothrow_copy_assignable;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace value {

    template <typename N>
        struct controller {
            template <typename T> using is_dynamic_type     = typename N::template is_dynamic_type_<T>;
            using allocator_type                            = typename N::allocator_type;
            using propagate_on_container_move_assignment    = typename std::allocator_traits<allocator_type>::propagate_on_container_move_assignment;
            using propagate_on_container_copy_assignment    = typename std::allocator_traits<allocator_type>::propagate_on_container_copy_assignment;

            // value access
                template <typename T, typename M>
                    static constexpr auto get(M& n) noexcept -> enable_if_t<!is_dynamic_type<T>::value && !std::is_pointer<T>::value, T&> {
                        return *(T*)&n.value_;
                    }
                template <typename T, typename M>
                    static constexpr auto get(M& n) noexcept -> enable_if_t< is_dynamic_type<T>::value && !std::is_pointer<T>::value, T&> {
                        return **(T**)&n.value_;
                    }
                template <typename T, typename M>
                    static constexpr auto get(M& n) noexcept -> enable_if_t<!is_dynamic_type<T>::value &&  std::is_pointer<T>::value, T&> {
                        return (T)&n.value_;
                    }
                template <typename T, typename M>
                    static constexpr auto get(M& n) noexcept -> enable_if_t< is_dynamic_type<T>::value &&  std::is_pointer<T>::value, T&> {
                        return *(T*)&n.value_;
                    }
            // move construct
                template <typename T>
                    static auto move_construct(N& n, N&& o) noexcept(std::is_nothrow_move_constructible<T>::value)
                        -> enable_if_t<!is_dynamic_type<T>::value && !std::is_trivially_assignable<T, T>::value>
                    {
                        alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, std::move(get<T>(o)));
                    }
                template <typename T>
                    static auto move_construct(N& n, N&& o) noexcept
                        -> enable_if_t<!is_dynamic_type<T>::value &&  std::is_trivially_assignable<T, T>::value>
                    {
                        get<T>(n) = get<T>(o);
                    }
                template <typename T>
                    static auto move_construct(N& n, N&& o) noexcept
                        -> enable_if_t< is_dynamic_type<T>::value>
                    {
                        get<T*>(n) = get<T*>(o), o.kind_ = N::kind_default_;
                    }
            // move assign
                template <typename T>
                    static auto move_assign(N& n, N&& o) noexcept(std::is_nothrow_move_constructible<T>::value)
                        -> enable_if_t<!is_dynamic_type<T>::value &&  propagate_on_container_move_assignment::value, N&>
                    {
                        if (n.kind_ == o.kind_) {
                            get<T>(n).~T(), n.alloc_ = std::move(o.alloc_);
                            alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, std::move(get<T>(o)));
                        }
                        else {
                            n.~N(), n.kind_ = o.kind_, n.alloc_ = std::move(o.alloc_);
                            alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, std::move(get<T>(o)));
                        }
                        return n;
                    }
                template <typename T>
                    static auto move_assign(N& n, N&& o) noexcept
                        -> enable_if_t< is_dynamic_type<T>::value &&  propagate_on_container_move_assignment::value, N&>
                    {
                        if (n.kind_ == o.kind_) {
                            get<T>(n).~T(), n.alloc_ = std::move(o.alloc_);
                            get<T*>(n) = get<T*>(o), o.kind_ = N::kind_default_;
                        }
                        else {
                            n.~N(), n.kind_ = o.kind_, n.alloc_ = std::move(o.alloc_);
                            get<T*>(n) = get<T*>(o), o.kind_ = N::kind_default_;
                        }
                        return n;
                    }
                template <typename T>
                    static auto move_assign(N& n, N&& o)
                        noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_constructible<T>::value)
                        -> enable_if_t<!is_dynamic_type<T>::value && !propagate_on_container_move_assignment::value, N&>
                    {
                        if (n.kind_ == o.kind_) {
                            get<T>(n).~T();
                            if (n.alloc_ == o.alloc_) // TODO: likely
                                alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, std::move(get<T>(o)));
                            else
                                alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, get<T>(o));
                        }
                        else {
                            n.~N(), n.kind_ = o.kind_;
                            if (n.alloc_ == o.alloc_) // TODO: likely
                                alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, std::move(get<T>(o)));
                            else
                                alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, get<T>(o));
                        }
                        return n;
                    }
                template <typename T>
                    static auto move_assign(N& n, N&& o)
                        -> enable_if_t< is_dynamic_type<T>::value && !propagate_on_container_move_assignment::value, N&>
                    {
                        if (n.kind_ == o.kind_) {
                            get<T>(n).~T();
                            if (n.alloc_ == o.alloc_) // TODO: likely
                                alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, std::move(get<T>(o)));
                            else
                                alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, get<T>(o));
                        }
                        else {
                            n.~N(), n.kind_ = o.kind_;
                            if (n.alloc_ == o.alloc_) // TODO: likely
                                construct(n, std::move(get<T>(o)));
                            else
                                construct(n, get<T>(o));
                        }
                        return n;
                    }
            // copy construct
                template <typename T>
                    static auto copy_construct(N& n, const N& o) noexcept(std::is_nothrow_copy_constructible<T>::value)
                        -> enable_if_t<!is_dynamic_type<T>::value && !std::is_trivially_assignable<T, T>::value>
                    {
                        alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, get<T>(o));
                    }
                template <typename T>
                    static auto copy_construct(N& n, const N& o) noexcept
                        -> enable_if_t<!is_dynamic_type<T>::value &&  std::is_trivially_assignable<T, T>::value>
                    {
                        get<T>(n) = get<T>(o);
                    }
                template <typename T>
                    static auto copy_construct(N& n, const N& o)
                        -> enable_if_t< is_dynamic_type<T>::value>
                    {
                        construct<T>(n, get<T>(o));
                    }
            // copy assign
                template <typename T>
                    static auto copy_assign(N& n, const N& o) noexcept(std::is_nothrow_copy_constructible<T>::value)
                        -> enable_if_t<!is_dynamic_type<T>::value &&  propagate_on_container_copy_assignment::value, N&>
                    {
                        if (n.kind_ == o.kind_) {
                            get<T>(n).~T(), n.alloc_ = o.alloc_;
                            alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, get<T>(o));
                        }
                        else {
                            n.~N(), n.kind_ = o.kind_, n.alloc_ = o.alloc_;
                            alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, get<T>(o));
                        }
                        return n;
                    }
                template <typename T>
                    static auto copy_assign(N& n, const N& o)
                        -> enable_if_t< is_dynamic_type<T>::value &&  propagate_on_container_copy_assignment::value, N&>
                    {
                        if (n.kind_ == o.kind_) {
                            get<T>(n).~T(), n.alloc_ = o.alloc_;
                            alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, get<T>(o));
                        }
                        else {
                            n.~N(), n.kind_ = o.kind_, n.alloc_ = o.alloc_;
                            construct<T>(n, get<T>(o));
                        }
                        return n;
                    }
                template <typename T>
                    static auto copy_assign(N& n, const N& o) noexcept(std::is_nothrow_copy_constructible<T>::value)
                        -> enable_if_t<!is_dynamic_type<T>::value && !propagate_on_container_copy_assignment::value, N&>
                    {
                        if (n.kind_ == o.kind_) {
                            get<T>(n).~T();
                            alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, get<T>(o));
                        }
                        else {
                            n.~N(), n.kind_ = o.kind_;
                            alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, get<T>(o));
                        }
                        return n;
                    }
                template <typename T>
                    static auto copy_assign(N& n, const N& o)
                        -> enable_if_t< is_dynamic_type<T>::value && !propagate_on_container_copy_assignment::value, N&>
                    {
                        if (n.kind_ == o.kind_) {
                            get<T>(n).~T();
                            alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, get<T>(o));
                        }
                        else {
                            n.~N(), n.kind_ = o.kind_;
                            construct<T>(n, get<T>(o));
                        }
                        return n;
                    }
            // construct
                template <typename T, typename ...A>
                    static auto construct(N& n, A&&... t) noexcept(std::is_nothrow_constructible<T, A&&...>::value)
                        ->  enable_if_t<!is_dynamic_type<T>::value>
                    {
                        alc::uninitialized_construct_using_allocator<T>(&get<T>(n), n.alloc_, std::forward<A>(t)...);
                    }
                template <typename T, typename ...A>
                    static auto construct(N& n, A&&... t)
                        -> enable_if_t< is_dynamic_type<T>::value>
                    {
                        using at = alc::rebind_t<allocator_type, T>;
                        at    al(n.alloc_);
                        get<T*>(n) = std::allocator_traits<at>::allocate(al, 1);
                        alc::uninitialized_construct_using_allocator<T>(get<T*>(n), al, std::forward<A>(t)...);
                    }
            // destruct
                template <typename T>
                    static auto destruct(N& n) noexcept -> enable_if_t<!is_dynamic_type<T>::value> {
                        get<T>(n).~T();
                    }
                template <typename T>
                    static auto destruct(N& n) noexcept -> enable_if_t< is_dynamic_type<T>::value> {
                        using at = alc::rebind_t<allocator_type, T>;
                        at    al(n.alloc_);
                        std::allocator_traits<at>::destroy(al, get<T*>(n));
                        std::allocator_traits<at>::deallocate(al, get<T*>(n), 1);
                    }
            // swap
                template <typename F, typename S>
                    static void swap(N& f, N& s) {
                        F t = std::move(get<F>(f));
                        destruct<F>(f);
                        construct<S>(f, std::move(get<S>(s)));
                        destruct<S>(s);
                        construct<F>(s, std::move(t));
                        std::swap(f.kind_, s.kind_);
                        std::swap(f.alloc_, s.alloc_);
                    }

            // exception specification
                using dynamic_types = typename N::dynamic_types_;
                using has_dynamic_tpyes             = type_sequence_disjunction<N::template is_dynamic_type_, dynamic_types>;

                template <typename T, typename ...A>
                    using is_nothrow_constructible  = bool_constant<!is_dynamic_type<T>::value && std::is_nothrow_constructible<T, A&&...>::value>;

                using is_nothrow_move_constructible = bool_constant<type_sequence_conjunction<std::is_nothrow_move_constructible, dynamic_types>::value>;
                using is_nothrow_move_assignable    = bool_constant<
                    !has_dynamic_tpyes::value &&
                    type_sequence_conjunction<std::is_nothrow_move_constructible, dynamic_types>::value &&
                    type_sequence_conjunction<std::is_nothrow_copy_constructible, dynamic_types>::value
                >;

                using is_nothrow_copy_constructible = bool_constant<!has_dynamic_tpyes::value && type_sequence_conjunction<std::is_nothrow_copy_constructible, dynamic_types>::value>;
                using is_nothrow_copy_assignable    = bool_constant<!has_dynamic_tpyes::value && type_sequence_conjunction<std::is_nothrow_copy_constructible, dynamic_types>::value>;
        };

    // value access
        template <typename T, typename N>
            constexpr T& get(N& n) noexcept {
                return controller<N>::template get<T>(n);
            }

    // move construct & assign
        template <typename T, typename N>
            inline void move_construct(N& n, N&& o)
                noexcept(noexcept(controller<N>::template move_construct<T>(n, std::forward<N>(o))))
            {
                controller<N>::template move_construct<T>(n, std::forward<N>(o));
            }
        template <typename T, typename N>
            inline N& move_assign(N& n, N&& o)
                noexcept(noexcept(controller<N>::template move_assign<T>(n, std::forward<N>(o))))
            {
                return controller<N>::template move_assign<T>(n, std::forward<N>(o));
            }

    // copy construct & assign
        template <typename T, typename N>
            inline void copy_construct(N& n, const N& o)
                noexcept(noexcept(controller<N>::template copy_construct<T>(n, o)))
            {
                controller<N>::template copy_construct<T>(n, o);
            }
        template <typename T, typename N>
            inline N& copy_assign(N& n, const N& o)
                noexcept(noexcept(controller<N>::template copy_assign<T>(n, o)))
            {
                return controller<N>::template copy_assign<T>(n, o);
            }

    // construct & destruct
        template <typename T, typename N, typename ...A>
            inline void construct(N& n, A&&... t)
                noexcept(noexcept(controller<N>::template construct<T, A...>(n, std::forward<A>(t)...)))
            {
                controller<N>::template construct<T, A...>(n, std::forward<A>(t)...);
            }
        template <typename T, typename N>
            inline void destruct(N& n) noexcept {
                controller<N>::template destruct<T>(n);
            }

    // swap
        template <typename N, typename F, typename S>
            inline void swap(N& f, N& s)
                noexcept(noexcept(controller<N>::template swap<F, S>(f, s)))
            {
                controller<N>::template swap<F, S>(f, s);
            }

}}

#endif // CXON_NODE_VALUE_HXX_
