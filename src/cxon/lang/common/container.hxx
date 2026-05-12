// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CXON_CONTAINER_HXX_
#define CXON_CONTAINER_HXX_

#include "allocator.hxx"
#include <utility>
#include <type_traits>
#include <iterator>
#include <algorithm>
#include <vector>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cnt { // container mutation

    template <typename C>
        struct traits;
        //  static auto emplace(C& c) -> typename C::reference;
        //  template <typename II>
        //      static bool append(C& c, II f, II l);
        //template <typename C, typename T = typename C::value_type>
        //    inline bool append(C& c, T&& t);

    template <typename C, typename ...A>
        inline auto emplace(C& c, A&&... as) -> typename C::reference;
    template <typename C, typename II>
        inline bool append(C& c, II f, II l);
    template <typename C, typename T = typename C::value_type>
        inline bool append(C& c, T&& t);

}}

namespace cxon { namespace cnt { // container adaptors container access

    template <typename A>
        inline auto container(      A& a) ->       typename A::container_type&;
    template <typename A>
        inline auto container(const A& a) -> const typename A::container_type&;

    template <typename C>
        struct continuous;
            // std::pair<auto, auto> range(const C& c);

}}

namespace cxon { namespace cnt { // container element read/write

    template <typename X, typename C>
        struct element_reader {
            template <typename II, typename Cx>
                static bool read(C& c, II& i, II e, Cx& cx);
        };
    template <typename X, typename C, typename II, typename Cx>
        inline bool element_read(C& c, II& i, II e, Cx& cx);

    template <typename X, typename C>
        struct element_writer {
            template <typename O, typename E, typename Cx>
                static inline bool write(O& o, const E& e, Cx& cx);
        };
    template <typename X, typename C, typename E, typename O, typename Cx>
        inline bool element_write(O& o, const E& e, Cx& cx);

}}

namespace cxon { namespace cnt { // buffers

    template <typename FI>
        struct range_container;
    template <typename FI>
        inline range_container<FI> make_range_container(FI f, FI l) noexcept;

    template <typename T, typename A>
        struct pointer_container;
    template <typename X, typename T, typename Cx>
        inline auto make_pointer_container(Cx& cx) -> pointer_container<T, alc::context_allocator_type<T, Cx>>;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cnt {

    // emplace

    namespace imp {

        template <typename C, typename ...A>
            inline auto emplace_(option<5>, C& c, A&&... as)
                -> std::enable_if_t<std::is_same<decltype(traits<C>::emplace(c, std::forward<A>(as)...)), typename C::reference>::value, typename C::reference>
            {
                return traits<C>::emplace(c, std::forward<A>(as)...);
            }
        template <typename C, typename ...A>
            inline auto emplace_(option<4>, C& c, A&&... as)
                -> std::enable_if_t<std::is_same<decltype(c.emplace_back(std::forward<A>(as)...)), typename C::reference>::value, typename C::reference>
            {
                return c.emplace_back(std::forward<A>(as)...);
            }
        template <typename C, typename ...A>
            inline auto emplace_(option<3>, C& c, A&&... as)
                -> std::enable_if_t<std::is_same<decltype(c.emplace_back()), void>::value && std::is_same<decltype(c.back()), typename C::reference>::value, typename C::reference>
            {
                return c.emplace_back(std::forward<A>(as)...), c.back();
            }
        template <typename C, typename ...A>
            inline auto emplace_(option<2>, C& c, A&&... as)
                -> std::enable_if_t<std::is_same<decltype(c.push_back(typename C::value_type {std::forward<A>(as)...})), void>::value && std::is_same<decltype(c.back()), typename C::reference>::value, typename C::reference>
            {
                return c.push_back(typename C::value_type {std::forward<A>(as)...}), c.back();
            }
        template <typename C, typename ...A>
            inline auto emplace_(option<1>, C& c, A&&... as)
                -> std::enable_if_t<std::is_same<decltype(c.emplace(std::forward<A>(as)...)), std::pair<typename C::iterator, bool>>::value, typename C::reference>
            {
                return *c.emplace(std::forward<A>(as)...).first;
            }
        template <typename C, typename ...A>
            inline auto emplace_(option<0>, C& c, A&&... as)
                -> std::enable_if_t<std::is_same<decltype(c.emplace(std::forward<A>(as)...)), typename C::iterator>::value, typename C::reference>
            {
                return *c.emplace(std::forward<A>(as)...);
            }

        template <typename T>
            struct using_allocator_of_ {
                template <typename C, typename ...A>
                    static auto emplace_(option<2>, C& c, A&&... as) -> decltype(T {std::forward<A>(as)..., c.get_allocator()})& {
                        return imp::emplace_(option<5>(), c, std::forward<A>(as)..., c.get_allocator());
                    }
                template <typename C, typename ...A>
                    static auto emplace_(option<1>, C& c, A&&... as) -> decltype(T {std::allocator_arg, c.get_allocator(), std::forward<A>(as)...})& {
                        return imp::emplace_(option<5>(), c, std::allocator_arg, c.get_allocator(), std::forward<A>(as)...);
                    }
                template <typename C, typename ...A>
                    static auto emplace_(option<0>, C& c, A&&... as) -> decltype(T {std::forward<A>(as)...})& {
                        return imp::emplace_(option<5>(), c, std::forward<A>(as)...);
                    }
                template <typename C, typename ...A>
                    static auto emplace(C& c, A&&... as) -> T& {
                        return emplace_(option<2>(), c, std::forward<A>(as)...);
                    }
            };
        template <typename F, typename S>
            struct using_allocator_of_<std::pair<F, S>> {
                template <typename C, typename ...A>
                    static auto emplace(C& c, A&&... as) -> typename C::reference {
                        return emplace_(option<5>(), c, alc::create_using_allocator_of<std::pair<F, S>>(c, std::forward<A>(as)...));
                    }
            };

    }
    template <typename C, typename ...A>
        inline auto emplace(C& c, A&&... as) -> typename C::reference {
            return imp::using_allocator_of_<typename C::value_type>::emplace(c, std::forward<A>(as)...);
        }

    // append range

    namespace imp {

        template <typename C, typename II>
            inline auto append_(option<2>, C& c, II f, II l)
                -> std::enable_if_t<std::is_same<decltype(traits<C>::append(c, f, l)), bool>::value, bool>
            {
                return traits<C>::append(c, f, l);
            }
        template <typename C, typename II>
            inline auto append_(option<1>, C& c, II f, II l)
                -> std::enable_if_t<std::is_same<decltype(c.append(f, l)), bool>::value, bool>
            {
                return c.append(f, l);
            }
        template <typename C, typename II>
            inline auto append_(option<0>, C& c, II f, II l)
                -> std::enable_if_t<std::is_same<decltype(append(c, *f)), bool>::value, bool>
            {
                for ( ; f != l && append(c, *f); ++f)
                    ;
                return f == l;
            }

    }
    template <typename C, typename II>
        inline bool append(C& c, II f, II l) {
            return imp::append_(option<1>(), c, f, l);
        }

    // append

    namespace imp {

        template <typename C, typename T = typename C::value_type>
            inline auto append_(option<3>, C& c, T&& t)
                -> std::enable_if_t<std::is_same<decltype(traits<C>::append(c, std::forward<T>(t))), bool>::value, bool>
            {
                return traits<C>::append(c, std::forward<T>(t));
            }
        template <typename C, typename T = typename C::value_type>
            inline auto append_(option<2>, C& c, T&& t)
                -> decltype(c.emplace(std::forward<T>(t)), bool())
            {
                return c.emplace(std::forward<T>(t)), true;
            }
        template <typename C, typename T = typename C::value_type>
            inline auto append_(option<1>, C& c, T&& t)
                -> decltype(c.push_back(std::forward<T>(t)), bool())
            {
                return c.push_back(std::forward<T>(t)), true;
            }
        template <typename C, typename T = typename C::value_type>
            inline auto append_(option<0>, C& c, T&& t)
                -> std::enable_if_t<std::is_same<decltype(emplace(c)), typename C::reference>::value, bool>
            {
                return emplace(c) = std::forward<T>(t), true;
            }

    }
    template <typename C, typename T>
        inline bool append(C& c, T&& t) {
            return imp::append_(option<3>(), c, std::forward<T>(t));
        }

    }}

namespace cxon { namespace cnt { // container adaptors container access

    namespace imp {

        template <typename A>
            struct adaptor_ : A {
                static const typename A::container_type& container(const A& a) noexcept { return ((adaptor_&)a).c; }
                static       typename A::container_type& container(      A& a) noexcept { return ((adaptor_&)a).c; }
            };

    }
    template <typename A>
        inline auto container(      A& a) ->       typename A::container_type& {
            return imp::adaptor_<A>::container(a);
        }
    template <typename A>
        inline auto container(const A& a) -> const typename A::container_type& {
            return imp::adaptor_<A>::container(a);
        }

    template <typename C>
        struct continuous {
            template <typename D = C>
                static constexpr auto range_(option<1>, const D& c) -> decltype(std::make_pair(c.data(), c.data() + c.size())) {
                    return std::make_pair(c.data(), c.data() + c.size());
                }
            template <typename D = C>
                static constexpr auto range_(option<0>, const D& c) -> decltype(std::make_pair(std::begin(c), std::end(c))) {
                    return std::make_pair(std::begin(c), std::end(c));
                }
            static constexpr auto range(const C& c) -> decltype(range_(option<1>(), c)) {
                return range_(option<1>(), c);
            }
        };

}}

namespace cxon { namespace cnt { // container element read/write

    template <typename X, typename C>
        template <typename II, typename Cx>
            inline bool element_reader<X, C>::read(C& c, II& i, II e, Cx& cx) {
                return read_value<X>(emplace(c), i, e, cx);
            }
    template <typename X, typename C, typename II, typename Cx>
        inline bool element_read(C& c, II& i, II e, Cx& cx) {
            return element_reader<X, C>::read(c, i, e, cx);
        }

    template <typename X, typename C>
        template <typename O, typename E, typename Cx>
            inline bool element_writer<X, C>::write(O& o, const E& e, Cx& cx) {
                return write_value<X>(o, e, cx);
            }
    template <typename X, typename C, typename E, typename O, typename Cx>
        inline bool element_write(O& o, const E& e, Cx& cx) {
            return element_writer<X, C>::write(o, e, cx);
        }

}}

namespace cxon { namespace cnt { // buffers / static

    template <typename FI>
        struct range_container {
            using value_type = typename std::iterator_traits<FI>::value_type;
            using reference = value_type&;

            range_container(FI f, FI l) noexcept : f_(f), l_(l), e_(f) {}

            std::size_t size() const noexcept       { return std::distance(f_, e_); }
            std::size_t max_size() const noexcept   { return std::distance(f_, l_); }

            FI begin() noexcept                     { return f_; }
            FI end() noexcept                       { return e_; }

            reference emplace_back() noexcept {
                CXON_ASSERT(e_ != l_, "overflow"); // LCOV_EXCL_LINE
                return *e_++;
            }

            void push_back(const value_type& t) noexcept {
                CXON_ASSERT(e_ != l_, "overflow"); // LCOV_EXCL_LINE
                *e_ = t, ++e_;
            }
            void push_back(value_type&& t) noexcept {
#               if defined(__GNUC__) && __GNUC__ >= 7 && !defined(__clang__)
#                   pragma GCC diagnostic push
                    // with -O3 only when called from append(value_type&&)
#                   pragma GCC diagnostic ignored "-Wstringop-overflow"
#               endif
                    CXON_ASSERT(e_ != l_, "overflow"); // LCOV_EXCL_LINE
                    *e_ = std::move(t), ++e_;
#               if defined(__GNUC__) && __GNUC__ >= 7 && !defined(__clang__)
#                   pragma GCC diagnostic pop
#               endif
            }

            bool append(value_type&& t) noexcept {
                return e_ != l_ && (push_back(std::move(t)), true);
            }
            bool append(const value_type& t) noexcept {
                return e_ != l_ && (push_back(t), true);
            }

            template <typename II>
                auto append(II f, II l) noexcept
                    -> std::enable_if_t<is_random_access_iterator<II>::value, bool>
                {
#                   if defined(__GNUC__) && __GNUC__ >= 7 && !defined(__clang__)
#                       pragma GCC diagnostic push
                        // if f == l we have this with -O3 and seems it's only for unsigned char
#                       pragma GCC diagnostic ignored "-Wstringop-overflow"
#                   endif
                        auto const s = std::distance(f, l);
                        return s <= std::distance(e_, l_) && (std::copy(f, l, e_), e_ += s, true);
#                   if defined(__GNUC__) && __GNUC__ >= 7 && !defined(__clang__)
#                       pragma GCC diagnostic pop
#                   endif
                }
            bool append(const value_type* t, std::size_t n) noexcept {
                return append(t, t + n);
            }

            bool append(unsigned n, const value_type& t) noexcept {
                return  n <= static_cast<std::size_t>(std::distance(e_, l_)) &&
                        (std::fill_n(e_, n, t), std::advance(e_, n), true)
                ;
            }

            private:
                FI f_, l_, e_;
        };
    template <typename FI>
        inline range_container<FI> make_range_container(FI f, FI l) noexcept {
            return {f, l};
        }

    template <typename FI>
        struct traits<range_container<FI>> {
            template <typename II>
                static bool append(range_container<FI>& c, II f, II l) {
                    return c.append(f, l);
                }
            template <typename T = typename range_container<FI>::value_type>
                static bool append(range_container<FI>& c, T&& t) {
                    return c.append(std::forward<T>(t));
                }
        };

}}

namespace cxon { namespace cnt { // buffers / dynamic

    template <typename T, typename A>
        struct pointer_container {
            using value_type = T;
            using pointer = value_type*;
            using reference = value_type&;

            pointer_container(const A& a) : b_(a) {}

            pointer release() {
                auto a = alc::make_allocator<T>(b_.get_allocator());
                auto const p = a.create(b_.size());
                    std::copy(b_.begin(), b_.end(), p);
                    b_.clear();
                return p;
            }

            std::size_t size() const noexcept       { return b_.size(); }
            std::size_t max_size() const noexcept   { return b_.max_size(); }

            pointer begin() noexcept                { return b_.begin(); }
            pointer end() noexcept                  { return b_.end(); }

            reference emplace_back()                { return b_.emplace_back(), b_.back(); }
            void push_back(const value_type& t)     { b_.push_back(t); }
            void push_back(value_type&& t)          { b_.push_back(std::forward<value_type>(t)); }

            template <typename II>
                bool append(II f, II l)             { return b_.insert(b_.end(), f, l), true; }

            private:
                std::vector<T, typename std::allocator_traits<A>::template rebind_alloc<T>> b_;
        };
    template <typename X, typename T, typename Cx>
        inline auto make_pointer_container(Cx& cx) -> pointer_container<T, alc::context_allocator_type<T, Cx>> {
            return { alc::context_allocator<T>(cx) };
        }

}}

#endif // CXON_CONTAINER_HXX_
