// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CONTAINER_HXX_
#define CXON_CONTAINER_HXX_

#include "allocator.hxx"
#include <limits>
#include <utility>
#include <type_traits>
#include <iterator>
#include <algorithm>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cnt { // container mutation

    template <typename C>
        struct traits;
        //  static bool reserve(C& c, std::size_t s);
        //  static auto emplace(C& c) -> typename C::reference;
        //  template <typename II>
        //      static bool append(C& c, II f, II l);
        //template <typename C, typename T = typename C::value_type>
        //    inline bool append(C& c, T&& t);

    template <typename C>
        inline bool reserve(C& c, std::size_t s);
    template <typename C, typename ...A>
        inline auto emplace(C& c, A&&... as) -> typename C::reference;
    template <typename C, typename II>
        inline bool append(C& c, II f, II l);
    template <typename C, typename T = typename C::value_type>
        inline bool append(C& c, T&& t);

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

namespace cxon { namespace cnt { // adapters

    template <typename FI>
        struct range_container;
    template <typename FI>
        inline range_container<FI> make_range_container(FI f, FI l) noexcept;

    template <typename T, typename A>
        struct pointer_container;
    template <typename X, typename T, typename Cx>
        inline auto make_pointer_container(Cx& cx) -> pointer_container<T, decltype(alc::make_context_allocator<T>(cx))>;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cnt {

    namespace imp {

        template <typename C>
            inline auto reserve_(option<2>, C& c, std::size_t s)
                -> enable_if_t<std::is_same<decltype(traits<C>::reserve(c, s)), bool>::value, bool>
            {
                return traits<C>::reserve(c, s);
            }
        template <typename C>
            inline auto reserve_(option<1>, C& c, std::size_t s)
                -> decltype(c.reserve(s), bool())
            {
                return c.reserve(s), true;
            }
        template <typename C>
            inline constexpr bool reserve_(option<0>, C&, std::size_t) {
                return true;
            }

    }
    template <typename C>
        inline bool reserve(C& c, std::size_t s) {
            return imp::reserve_(option<1>(), c, s);
        }


    namespace imp {

        template <typename C, typename ...A>
            inline auto emplace_(option<5>, C& c, A&&... as)
                -> enable_if_t<std::is_same<decltype(traits<C>::emplace(c, std::forward<A>(as)...)), typename C::reference>::value, typename C::reference>
            {
                return traits<C>::emplace(c, std::forward<A>(as)...);
            }
        template <typename C, typename ...A>
            inline auto emplace_(option<4>, C& c, A&&... as)
                -> enable_if_t<std::is_same<decltype(c.emplace_back(std::forward<A>(as)...)), typename C::reference>::value, typename C::reference>
            {
                return c.emplace_back(std::forward<A>(as)...);
            }
        template <typename C, typename ...A>
            inline auto emplace_(option<3>, C& c, A&&... as)
                -> enable_if_t<std::is_same<decltype(c.emplace_back()), void>::value && std::is_same<decltype(c.back()), typename C::reference>::value, typename C::reference>
            {
                return c.emplace_back(std::forward<A>(as)...), c.back();
            }
        template <typename C, typename ...A>
            inline auto emplace_(option<2>, C& c, A&&... as)
                -> enable_if_t<std::is_same<decltype(c.push_back(typename C::value_type {std::forward<A>(as)...})), void>::value && std::is_same<decltype(c.back()), typename C::reference>::value, typename C::reference>
            {
                return c.push_back(typename C::value_type {std::forward<A>(as)...}), c.back();
            }
        template <typename C, typename ...A>
            inline auto emplace_(option<1>, C& c, A&&... as)
                -> enable_if_t<std::is_same<decltype(c.emplace(std::forward<A>(as)...)), std::pair<typename C::iterator, bool>>::value, typename C::reference>
            {
                return *c.emplace(std::forward<A>(as)...).first;
            }
        template <typename C, typename ...A>
            inline auto emplace_(option<0>, C& c, A&&... as)
                -> enable_if_t<std::is_same<decltype(c.emplace(std::forward<A>(as)...)), typename C::iterator>::value, typename C::reference>
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
                    static auto emplace(C& c, A&&... as) -> std::pair<F, S>& {
                        return emplace_(option<5>(), c, alc::create_using_allocator_of<std::pair<F, S>>(c, std::forward<A>(as)...));
                    }
            };

    }
    template <typename C, typename ...A>
        inline auto emplace(C& c, A&&... as) -> typename C::reference {
            return imp::using_allocator_of_<typename C::value_type>::emplace(c, std::forward<A>(as)...);
        }

    namespace imp {

        template <typename C, typename II>
            inline auto append_(option<1>, C& c, II f, II l)
                -> enable_if_t<std::is_same<decltype(traits<C>::append(c, f, l)), bool>::value, bool>
            {
                return traits<C>::append(c, f, l);
            }
        template <typename C, typename II>
            inline auto append_(option<0>, C& c, II f, II l)
                -> enable_if_t<std::is_same<decltype(append(c, *f)), bool>::value, bool>
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

    namespace imp {

        template <typename C, typename T = typename C::value_type>
            inline auto append_(option<3>, C& c, T&& t)
                -> enable_if_t<std::is_same<decltype(traits<C>::append(c, std::forward<T>(t))), bool>::value, bool>
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
                -> enable_if_t<std::is_same<decltype(emplace(c)), typename C::reference>::value, bool>
            {
                return emplace(c) = std::forward<T>(t), true;
            }

    }
    template <typename C, typename T>
        inline bool append(C& c, T&& t) {
            return imp::append_(option<3>(), c, std::forward<T>(t));
        }

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

namespace cxon { namespace cnt {

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

namespace cxon { namespace cnt {

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
                    -> enable_if_t<is_random_access_iterator<II>::value, bool>
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

namespace cxon { namespace cnt {

    template <typename T, typename A>
        struct pointer_container {
            using value_type = T;
            using pointer = value_type*;
            using reference = value_type&;

            pointer_container(const A& a)
            :   a_(a), f_(), l_(), e_()
            {
                // coverity[var_deref_model] - 'grow dereferences null this->f_' - it's about std::move(f_, e_, p), but in this case f_ == e_
                resize(8);
            }
            ~pointer_container()                    { a_.release(f_, l_ - f_); }

            pointer release()                       { pointer p = reduce(); return f_ = l_ = e_ = nullptr, p; }

            std::size_t size() const noexcept       { return std::distance(f_, e_); }
            std::size_t max_size() const noexcept   { return std::numeric_limits<std::size_t>::max(); }

            pointer begin() noexcept                { return f_; }
            pointer end() noexcept                  { return e_; }

            reference emplace_back()                { return grow(), *e_++; }
            void push_back(const value_type& t)     { grow(), *e_ = t, ++e_; }
            void push_back(value_type&& t)          { grow(), *e_ = std::move(t), ++e_; }

            void reserve(std::size_t n)             { n > std::size_t(l_ - f_) ? resize(n) : void(); }

            private:
                void grow()                         { e_ == l_ ? resize((l_ - f_) * 2) : void(); }
                    
                void resize(std::size_t n) {
                    CXON_ASSERT(n != 0 && n >= std::size_t(e_ - f_), "unexpected");
                    auto const p = a_.create(n);
                        std::move(f_, e_, p);
                        a_.release(f_, l_ - f_);
                    e_ = p + (e_ - f_), l_ = p + n, f_ = p;
                }

                pointer reduce() { return resize(e_ - f_), f_; }

            private:
                A a_;
                pointer f_, l_, e_;
        };
    template <typename X, typename T, typename Cx>
        inline auto make_pointer_container(Cx& cx) -> pointer_container<T, decltype(alc::make_context_allocator<T>(cx))> {
            return { alc::make_context_allocator<T>(cx) };
        }

}}

#endif // CXON_CONTAINER_HXX_
