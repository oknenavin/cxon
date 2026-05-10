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

#ifndef CXON_TEST_HXX_
#define CXON_TEST_HXX_

#include <string>
#include <map>
#include <queue>
#include <valarray>

#include <cstring>


////////////////////////////////////////////////////////////////////////////////

namespace cxon { namespace test {

    struct suite {
        char const*const category;
        suite(const char *category = "") : category(category) { info::get(category).suites.push_back(this); }

        virtual void test() const = 0;

        struct info {
            struct cell {
                std::vector<suite*> suites;
                unsigned all = 0;
                unsigned err = 0;
                using map = std::map<std::string, cell>;
            };

            static cell::map& get() {
                static cell::map info_;
                return info_;
            }

            static cell& get(const char *category)          { return get()[category]; }
            static unsigned& count(const char *category)    { return get(category).all; }
            static unsigned& errors(const char *category)   { return get(category).err; }

            static unsigned count() {
                unsigned c = 0;
                    for (auto& s : get()) c += s.second.all;
                return c;
            }
            static unsigned errors() {
                unsigned c = 0;
                    for (auto& s : get()) c += s.second.err;
                return c;
            }
        };
    };

}}

#define TEST_BEG_(Id, Tr, Ct) \
    namespace { \
        static struct test_##Id##_ : cxon::test::suite { \
            using X = Tr; \
            test_##Id##_() : suite(Ct) {} \
            void test() const override; \
        }   test_##Id##__; \
        void test_##Id##_::test() const { \
            using namespace cxon;
#define TEST_END_() \
        } \
    }

#define TEST_BEG(Id, Tr, Ct) TEST_BEG_(Id, Tr, Ct)
#define TEST_END() TEST_END_()

#define TEST_CHECK(cnd)\
    do if (++suite::info::count(category), !(cnd)) {\
        ++suite::info::errors(category), std::fprintf(stderr, "at %s:%li\n\t%s\n", __FILE__, (long)__LINE__, #cnd);\
        CXON_ASSERT(false, "check failed");\
    }   while (0)

#define TEST_VERIFY(cnd)\
    do if (++suite::info::count(category), !(cnd)) {\
        ++suite::info::errors(category), std::fprintf(stderr, "at %s:%li\n\t%s\n\t  ex: %s\n\t  rs: %s\n\t  in: %s\n", __FILE__, (long)__LINE__, #cnd, cnd.ex.c_str(), cnd.ot.c_str(), cnd.in.c_str());\
        CXON_ASSERT(false, "check failed");\
    }   while (0)

#define R_TEST(ref, ...) TEST_VERIFY(cxon::test::verify_read<X>(ref, __VA_ARGS__))
#define W_TEST(ref, ...) TEST_VERIFY(cxon::test::verify_write<X>(ref, __VA_ARGS__))

#define QS(s) "\"" s "\""

namespace cxon { namespace test {

    template <typename I>
        struct force_input_iterator;

    struct result {
        std::string ex; // expected
        std::string ot; // result
        std::string in; // input
        bool ok;
        result(bool ok) : ok(ok) {};
        operator bool() const noexcept { return ok; };
    };

    template <typename X, typename T>
        inline result verify_read(const T& ref, const std::string& sbj);
    template <typename X, typename T, typename E>
        inline result verify_read(const T& ref, const std::string& sbj, E err, int pos = -1);

    template <typename X, typename T>
        inline result verify_write(const std::string& ref, const T& sbj);
    template <typename X, typename T, typename E>
        inline result verify_write(const std::string& ref, const T& sbj, E err);

}}

////////////////////////////////////////////////////////////////////////////////

namespace cxon { namespace test {

    template <typename ...>
        struct match;

    template <typename T>
        struct match<T> {
            template <typename U = T>
                static auto values(const U& t0, const U& t1) -> std::enable_if_t<!std::is_floating_point<U>::value, bool> {
                    return t0 == t1;
                }
            template <typename U = T>
                static auto values(U t0, U t1) -> std::enable_if_t< std::is_floating_point<U>::value, bool> {
                    return !std::isgreater(t0, t1) && !std::isless(t0, t1);
                }
        };
    template <typename T, std::size_t N>
        struct match<T[N]> {
            static bool values(const T (&t0)[N], const T (&t1)[N]) {
                return std::equal(
                    std::begin(t0), std::end(t0), std::begin(t1),
                    [] (const T& t0, const T& t1) { return match<T>::values(t0, t1); }
                );
            }
        };
    template <typename T>
        struct match<T*> {
            template <typename U = T>
                static auto values(const T* t0, const T* t1) -> std::enable_if_t<!is_char<U>::value, bool> {
                    return t0 == t1 || (t0 && t1 && match<T>::values(*t0, *t1));
                }
            template <typename U = T>
                static auto values(const T* t0, const T* t1) -> std::enable_if_t< is_char<U>::value, bool> {
                    if (!t0 || !t1) return t0 == t1;
                    while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                    return *t0 == *t1;
                }
        };
    template <typename T>
        struct match<std::valarray<T>> {
            static bool values(const std::valarray<T>& t0, const std::valarray<T>& t1) {
#               ifdef _MSC_VER // std::begin/std::end broken for empty std::valarray
                    return  (t0.size() == 0 && t1.size() == 0) ||
                            (t0.size() && t1.size() && std::equal(
                                std::begin(t0), std::end(t0), std::begin(t1),
                                [] (const T& t0, const T& t1) { return match<T>::values(t0, t1); }
                            ))
                    ;
#               else
                    return std::equal(
                        std::begin(t0), std::end(t0), std::begin(t1),
                        [] (const T& t0, const T& t1) { return match<T>::values(t0, t1); }
                    );
#               endif
            }
        };
    template <typename T>
        struct match<std::priority_queue<T>> {
            static bool values(const std::priority_queue<T>& t0, const std::priority_queue<T>& t1) {
                auto r0 = t0, r1 = t1;
                    while (!r0.empty() && !r1.empty()) {
                        if (!match<T>::values(r0.top(), r1.top())) return false;
                        r0.pop(); r1.pop();
                    }
                return r0.empty() && r1.empty();
            }
        };
    template <typename T, typename D>
        struct match<std::unique_ptr<T, D>> {
            static bool values(const std::unique_ptr<T, D>& t0, const std::unique_ptr<T, D>& t1) {
                return t0 == t1 || (t0 && t1 && match<T>::values(*t0.get(), *t1.get()));
            }
        };
    template <typename T>
        struct match<std::shared_ptr<T>> {
            static bool values(const std::shared_ptr<T>& t0, const std::shared_ptr<T>& t1) {
                return t0 == t1 || (t0 && t1 && match<T>::values(*t0.get(), *t1.get()));
            }
        };

    template <typename T>
        inline bool equal(const T& t0, const T& t1) {
            return match<T>::values(t0, t1);
        }

    template <typename I>
        struct force_input_iterator {
            using iterator_category = std::input_iterator_tag;
            using value_type        = typename std::iterator_traits<I>::value_type;
            using difference_type   = typename std::iterator_traits<I>::difference_type;
            using pointer           = typename std::iterator_traits<I>::pointer;
            using reference         = typename std::iterator_traits<I>::reference;

            I i_;

            force_input_iterator(I i) : i_(i) {}

            force_input_iterator(const force_input_iterator& i) : i_(i.i_) {}
            force_input_iterator& operator =(const force_input_iterator&) = delete;

            bool operator ==(const force_input_iterator& i) const { return i_ == i.i_; }
            bool operator !=(const force_input_iterator& i) const { return i_ != i.i_; }
            force_input_iterator& operator ++() { return ++i_, *this; }
            force_input_iterator  operator ++(int) { auto i = *this; return ++i_, i; }
            reference operator *() { return *i_; }
            const reference operator *() const { return *i_; }
        };
    template <typename I>
        inline force_input_iterator<I> make_force_input_iterator(I i) {
            return force_input_iterator<I>(i);
        }

    template <typename, typename = void>
        struct is_force_input_iterator : std::false_type {};
    template <typename X>
        struct is_force_input_iterator<X, std::enable_if_t<X::force_input_iterator>> : std::true_type {};

    template <typename X, typename T, typename S, typename ...A>
        inline auto from_string(T& t, const S& s, A&&... as)
            -> std::enable_if_t< is_force_input_iterator<X>::value, from_bytes_result<decltype(std::begin(s))>>
        {
            auto b = make_force_input_iterator(std::begin(s)), e = make_force_input_iterator(std::end(s));
            auto const r =  from_bytes<X>(t, b, e, std::forward<A>(as)...);
            return { r.ec, r.end.i_ };
        }
    template <typename X, typename T, typename S, typename ...A>
        inline auto from_string(T& t, const S& s, A&&... as)
            -> std::enable_if_t<!is_force_input_iterator<X>::value, from_bytes_result<decltype(std::begin(s))>>
        {
            return from_bytes<X>(t, s, std::forward<A>(as)...);
        }

    template <typename T>
        struct clean        { clean(T&) {} };
    template <typename T, std::size_t N>
        struct clean<T[N]>  { clean(T (&)[N]) {} };
    template <typename T>
        struct clean<T*>    {
            T* t_;

            clean(T* t) : t_(t) {}
            ~clean()            { destroy(); }

            template <typename U = typename std::remove_const<T>::type>
                auto destroy() -> std::enable_if_t<!is_char<U>::value, void> {
                    auto al = alc::make_allocator<U>(std::allocator<U>());
                    al.release(const_cast<U*>(t_));
                }
            template <typename U = typename std::remove_const<T>::type>
                auto destroy() -> std::enable_if_t< is_char<U>::value, void> {
                    auto al = alc::make_allocator<U>(std::allocator<U>());
                    al.release(const_cast<U*>(t_), t_ ? std::char_traits<U>::length(t_) + 1 : 0);
                }
        };

    template <typename X, typename T, typename C>
        static result verify_read_(const T& ref, const C& sbj) {
            T res {};
                auto const r = from_string<X>(res, sbj);
                clean<T> clean__(res);
            result rs = r && r.end == std::end(sbj) && match<T>::values(res, ref);
                if (!rs) {
                    to_bytes<X>(rs.ex, ref);
                    to_bytes<X>(rs.ot, res);
                    rs.in = sbj;
                }
            return rs;
        }
    template <typename X, typename T, typename C, typename E>
        static result verify_read_(const T& ref, const C& sbj, E err, int pos) {
            T res {};
                auto const r = from_string<X>(res, sbj);
                clean<T> clean__(res);
            result rs = r.ec.value() == (int)err && (pos == -1 || std::distance(std::begin(sbj), r.end) == pos);
                if (!rs) {
                    to_bytes<X>(rs.ex, ref);
                    to_bytes<X>(rs.ot, res);
                    rs.in = sbj;
                }
            return rs;
        }

    template <typename X, typename T>
        inline result verify_read(const T& ref, const std::string& sbj)                   { return verify_read_<X>(ref, sbj); }
    template <typename X, typename T, typename E>
        inline result verify_read(const T& ref, const std::string& sbj, E err, int pos)   { return verify_read_<X>(ref, sbj, err, pos); }

    template <typename X, typename T, typename C>
        static result verify_write_(const C& ref, const T& sbj) {
            C res;
                auto const r = to_bytes<X>(res, sbj);
            result rs = r && ref == res;
                if (!rs) {
                    rs.ex = ref;
                    rs.ot = std::move(res);
                    to_bytes(rs.in, sbj);
                }
            return rs;
        }
    template <typename X, typename T, typename C, typename E>
        static result verify_write_(const C& ref, const T& sbj, E err) {
            C res;
                auto const r = to_bytes<X>(res, sbj);
            result rs = r.ec.value() == (int)err;
                if (!rs) {
                    rs.ex = ref;
                    rs.ot = std::move(res);
                    to_bytes(rs.in, sbj);
                }
            return rs;
        }

    template <typename X, typename T>
        inline result verify_write(const std::string& ref, const T& sbj)          { return verify_write_<X>(ref, sbj); }
    template <typename X, typename T, typename E>
        inline result verify_write(const std::string& ref, const T& sbj, E err)   { return verify_write_<X>(ref, sbj, err); }

}}

#endif // CXON_TEST_HXX_
