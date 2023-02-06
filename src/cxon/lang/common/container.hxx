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
#include <array>

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

    template <typename BI>
        struct buffered_back_inserter;
    template <typename BI>
        inline buffered_back_inserter<BI> make_buffered_back_inserter(BI& o) noexcept;

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
                        return imp::emplace_(option<5>(), std::allocator_arg, c.get_allocator(), std::forward<A>(as)...);
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
                return emplace(c) = std::move(t), true;
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
                CXON_ASSERT(e_ != l_, "overflow"); // LCOV_EXCL_LINE
                *e_ = std::move(t), ++e_;
            }

            template <typename II>
                auto append(II f, II l) noexcept
                    -> enable_if_t<is_random_access_iterator<II>::value, bool>
                {
                    auto const s = std::distance(f, l);
                    return s <= std::distance(e_, l_) && (std::copy(f, l, e_), e_ += s, true);
                }
            bool append(const value_type* t, std::size_t n) noexcept {
                return append(t, t + n);
            }
            bool append(value_type&& t) noexcept {
                return e_ != l_ && (push_back(std::move(t)), true);
            }
            bool append(const value_type& t) noexcept {
                return e_ != l_ && (push_back(t), true);
            }
            bool append(unsigned n, const value_type& t) noexcept {
                return n > static_cast<std::size_t>(std::distance(e_, l_)) ?
                    (std::fill_n(e_, std::distance(e_, l_), t), e_ = l_, false) :
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
                static bool append(range_container<FI>& c, T t) {
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

namespace cxon { namespace cnt {

    template <typename BI>
        struct buffered_back_inserter {
            using value_type = typename BI::value_type;
            using reference = value_type&;

            buffered_back_inserter(BI& o) noexcept : bf_(), o_(o), s_() {}
            ~buffered_back_inserter() { flush(); }

            typename BI::iterator begin() noexcept  { return o_.begin(); }
            typename BI::iterator end() noexcept    { return o_.end(); }

            void push_back(const value_type& t) {
                    if (s_ == bf_.size()) flush();
                bf_[s_++] = t;
            }
            void push_back(value_type&& t) {
                    if (s_ == bf_.size()) flush();
                bf_[s_++] = std::move(t);
            }

            void append(const value_type& t) {
                push_back(t);
            }
            void append(value_type&& t) {
                push_back(std::move(t));
            }

            template <typename II>
                void append(II f, II l) {
                    auto const s = std::distance(f, l);
                        if (s_ + s > bf_.size()) {
                            flush();
                            if (std::size_t(s) > bf_.size())
                                return o_.append(f, l), void();
                        }
                    std::copy(f, l, bf_.begin() + s_), s_ += s;
                }
            void append(const value_type* t, std::size_t n) {
                append(t, t + n);
            }
            void append(unsigned n, const value_type& t) {
                    if (s_ + n > bf_.size()) flush();
                    CXON_ASSERT(s_ + n <= bf_.size(), "TODO");
                std::fill_n(bf_.begin() + s_, n, t), s_ += n;
            }

            void flush() {
                imp::flush(option<1>(), o_, bf_, s_), s_ = 0;
            }

            private:
                using buffer_type = std::array<value_type, 4 * 1024>;

                struct imp {
                    template <typename U = BI>
                        static auto flush(option<1>, U& o, const buffer_type& b, typename buffer_type::size_type s)
                            -> decltype(o.append(b.data(), b.data() + s), void())
                        {
                            o.append(b.data(), b.data() + s);
                        }
                    template <typename U = BI>
                        static auto flush(option<0>, U& o, const buffer_type& b, typename buffer_type::size_type s)
                            -> decltype(o.insert(o.end(), b.data(), b.data() + s), void())
                        {
                            o.insert(o.end(), b.data(), b.data() + s);
                        }
                };

            private:
                buffer_type bf_;
                BI& o_;
                typename buffer_type::size_type s_;
        };
    template <typename BI>
        inline buffered_back_inserter<BI> make_buffered_back_inserter(BI& o) noexcept {
            return {o};
        }

    template <typename BI>
        struct traits<buffered_back_inserter<BI>> {
            template <typename II>
                static bool append(buffered_back_inserter<BI>& c, II f, II l) {
                    return c.append(f, l);
                }
            template <typename T = typename buffered_back_inserter<BI>::value_type>
                static bool append(buffered_back_inserter<BI>& c, T t) {
                    return c.append(std::forward<T>(t));
                }
        };

}}

#endif // CXON_CONTAINER_HXX_
