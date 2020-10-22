// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TEST_HXX_
#define CXON_TEST_HXX_

#include <string>
#include <vector>
#include <queue>
#include <valarray>

#include <cstring>

////////////////////////////////////////////////////////////////////////////////

namespace cxon { namespace test {

    using byte = unsigned char;
    using bytes = std::basic_string<byte>;

}}

namespace cxon { namespace test {

    struct suite {
        static std::vector<suite*>& get() {
            static std::vector<suite*> tests;
            return tests;
        }
        static unsigned& all() {
            static unsigned a = 0;
            return a;
        }
        static unsigned& err() {
            static unsigned e = 0;
            return e;
        }

        suite() { get().push_back(this); }

        virtual void test() const = 0;
    };

}}

#define TEST_CAT_(x, y) x##y
#define TEST_CAT(x, y)  TEST_CAT_(x, y)

#define TEST_BEG_(...)\
    static struct : cxon::test::suite {\
        using XXON = __VA_ARGS__;\
        void test() const override {\
        using namespace cxon;
#define TEST_END_()\
        }\
    }   TEST_CAT(TEST_, __COUNTER__);

#define TEST_BEG(...) TEST_BEG_(__VA_ARGS__)
#define TEST_END() TEST_END_()

#define TEST_CHECK(conditon)\
    do if (++suite::all(), !(conditon)) {\
        ++suite::err(), fprintf(stderr, "at %s:%li\n", __FILE__, (long)__LINE__), fflush(stderr);\
        CXON_ASSERT(false, "check failed");\
    } while (0)

#define R_TEST(ref, ...) TEST_CHECK(cxon::test::verify_read<XXON>(ref, __VA_ARGS__))
#define W_TEST(ref, ...) TEST_CHECK(cxon::test::verify_write<XXON>(ref, __VA_ARGS__))

#define QS(s) "\"" s "\""
#define BS(s) test::bytes((test::byte*)s, sizeof(s) - 1)

namespace cxon { namespace test {

    template <typename I>
        struct force_input_iterator;

    template <typename X, typename T>
        inline std::string to_string(const T& t);

    template <typename X, typename T>
        inline bool verify_read(const T& ref, const std::string& sbj);
    template <typename X, typename T, typename E>
        inline bool verify_read(const T& ref, const std::string& sbj, E err, int pos = -1);
    template <typename X, typename T>
        inline bool verify_read(const T& ref, const bytes& sbj);
    template <typename X, typename T, typename E>
        inline bool verify_read(const T& ref, const bytes& sbj, E err, int pos = -1);

    template <typename X, typename T>
        inline bool verify_write(const std::string& ref, const T& sbj);
    template <typename X, typename T, typename E>
        inline bool verify_write(const std::string& ref, const T& sbj, E err);
    template <typename X, typename T>
        inline bool verify_write(const bytes& ref, const T& sbj);
    template <typename X, typename T, typename E>
        inline bool verify_write(const bytes& ref, const T& sbj, E err);

}}

////////////////////////////////////////////////////////////////////////////////

namespace cxon { namespace test {

    template <typename X, typename T>
        inline std::string to_string(const T& t) {
            std::string s;
                to_bytes<X>(s, t);
            return s;
        }

    template <typename ...>
        struct match;

    template <typename T>
        struct match<T> {
            template <typename U = T>
                static auto values(const U& t0, const U& t1) -> enable_if_t<!std::is_floating_point<U>::value, bool> {
                    return t0 == t1;
                }
            template <typename U = T>
                static auto values(const U& t0, const U& t1) -> enable_if_t< std::is_floating_point<U>::value, bool> {
                    return !std::isgreater(t0, t1) && !std::isless(t0, t1);
                }
        };
    template <typename T>
        struct match<T*> {
            static bool values(const T* t0, const T* t1) {
                return t0 == t1 || (t0 && match<T>::values(*t0, *t1));
            }
        };
    template <>
        struct match<const char*> {
            static bool values(const char* t0, const char* t1) {
                return t0 == t1 || (t0 && std::strcmp(t0, t1) == 0);
            }
        };
    template <>
        struct match<char*> {
            static bool values(const char* t0, const char* t1) {
                return t0 == t1 || (t0 && std::strcmp(t0, t1) == 0);
            }
        };
    template <>
        struct match<const char16_t*> {
            static bool values(const char16_t* t0, const char16_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <>
        struct match<char16_t*> {
            static bool values(const char16_t* t0, const char16_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <>
        struct match<const char32_t*> {
            static bool values(const char32_t* t0, const char32_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <>
        struct match<char32_t*> {
            static bool values(const char32_t* t0, const char32_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <>
        struct match<const wchar_t*> {
            static bool values(const wchar_t* t0, const wchar_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <>
        struct match<wchar_t*> {
            static bool values(const wchar_t* t0, const wchar_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <typename T, size_t N>
        struct match<T[N]> {
            static bool values(const T (&t0)[N], const T (&t1)[N]) {
                return std::equal(std::begin(t0), std::end(t0), std::begin(t1), [] (const T& t0, const T& t1) { return match<T>::values(t0, t1); });
            }
        };
    template <typename T>
        struct match<std::valarray<T>> {
            static bool values(const std::valarray<T>& t0, const std::valarray<T>& t1) {
                return std::equal(std::begin(t0), std::end(t0), std::begin(t1), [] (const T& t0, const T& t1) { return match<T>::values(t0, t1); });
            }
        };
    template <typename T>
        struct match<std::priority_queue<T>> {
            static bool values(const std::priority_queue<T>& t0, const std::priority_queue<T>& t1) {
                auto r0 = t0; auto r1 = t1;
                    while (!r0.empty() && !r1.empty()) {
                        if (!match<T>::values(r0.top(), r1.top())) return false;
                        r0.pop(); r1.pop();
                    }
                return r0.empty() && r1.empty();
            }
        };

    template <typename I>
        struct force_input_iterator {
            using iterator_category = std::input_iterator_tag;
            using value_type        = typename std::iterator_traits<I>::value_type;
            using difference_type   = typename std::iterator_traits<I>::difference_type;
            using pointer           = typename std::iterator_traits<I>::pointer;
            using reference         = typename std::iterator_traits<I>::reference;

            I i_;

            force_input_iterator(I i) : i_(i) {}

            bool operator ==(const force_input_iterator& i) const { return i_ == i.i_; }
            bool operator !=(const force_input_iterator& i) const { return i_ != i.i_; }
            force_input_iterator& operator ++() { return ++i_, *this; }
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
        struct is_force_input_iterator<X, enable_if_t<X::force_input_iterator>> : std::true_type {};

    template <typename X, typename T, typename S>
        inline auto from_string(T& t, const S& s)
            -> enable_if_t< is_force_input_iterator<X>::value, from_bytes_result<decltype(std::begin(s))>>
        {
            auto b = make_force_input_iterator(std::begin(s)), e = make_force_input_iterator(std::end(s));
            auto const r =  from_bytes<X>(t, b, e);
            return { r.ec, r.end.i_ };
        }
    template <typename X, typename T, typename S>
        inline auto from_string(T& t, const S& s)
            -> enable_if_t<!is_force_input_iterator<X>::value, from_bytes_result<decltype(std::begin(s))>>
        {
            return from_bytes<X>(t, s);
        }

    template <typename X, typename T, typename C>
        static bool verify_read_(const T& ref, const C& sbj) {
            T res{};
                auto const r = from_string<X>(res, sbj);
            return r && r.end == std::end(sbj) && match<T>::values(res, ref);
        }
    template <typename X, typename T, typename C, typename E>
        static bool verify_read_(const T&, const C& sbj, E err, int pos) {
            T res{};
                auto const r = from_string<X>(res, sbj);
            return r.ec.value() == (int)err && (pos == -1 || std::distance(std::begin(sbj), r.end) == pos);
        }

    template <typename X, typename T>
        inline bool verify_read(const T& ref, const std::string& sbj)                   { return verify_read_<X>(ref, sbj); }
    template <typename X, typename T, typename E>
        inline bool verify_read(const T& ref, const std::string& sbj, E err, int pos)   { return verify_read_<X>(ref, sbj, err, pos); }
    template <typename X, typename T>
        inline bool verify_read(const T& ref, const bytes& sbj)                         { return verify_read_<X>(ref, sbj); }
    template <typename X, typename T, typename E>
        inline bool verify_read(const T& ref, const bytes& sbj, E err, int pos)         { return verify_read_<X>(ref, sbj, err, pos); }

    template <typename X, typename T, typename C>
        static bool verify_write_(const C& ref, const T& sbj) {
            C res;
                auto const r = to_bytes<X>(res, sbj);
            return r && ref == res;
        }
    template <typename X, typename T, typename C, typename E>
        static bool verify_write_(const C&, const T& sbj, E err) {
            C res;
                auto const r = to_bytes<X>(res, sbj);
            return r.ec.value() == (int)err;
        }

    template <typename X, typename T>
        inline bool verify_write(const std::string& ref, const T& sbj)          { return verify_write_<X>(ref, sbj); }
    template <typename X, typename T, typename E>
        inline bool verify_write(const std::string& ref, const T& sbj, E err)   { return verify_write_<X>(ref, sbj, err); }
    template <typename X, typename T>
        inline bool verify_write(const bytes& ref, const T& sbj)                { return verify_write_<X>(ref, sbj); }
    template <typename X, typename T, typename E>
        inline bool verify_write(const bytes& ref, const T& sbj, E err)         { return verify_write_<X>(ref, sbj, err); }

}}

#endif // CXON_TEST_HXX_
