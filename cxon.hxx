// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CXON_HXX_
#define CXON_CXON_HXX_

#define CXON_VERSION_MAJOR 0
#define CXON_VERSION_MINOR 42
#define CXON_VERSION_PATCH 0

#include <string>

#include <tuple>
#include <valarray>
#include <array>
#include <queue>
#include <stack>
#include <deque>
#include <list>
#include <forward_list>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include <utility>
#include <algorithm>

#include <limits>
#include <type_traits>
#include <system_error>

#if defined(__has_include) && __has_include(<charconv>)
#   if __cplusplus >= 201703L
#       include <charconv>
#       ifdef __GLIBCXX__ // partial
            namespace std { enum class chars_format; }
#       endif
#       define HAVE_CHARCONV
#   endif
#endif
#include <cerrno>
#include <cstdlib> // strto*
#include <cstdio> // snprintf
#include <cmath> // isfinite

#include <cstring> // strcmp

#if defined(__GNUC__) || defined(__clang__)
#   define CXON_FORCE_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#   define CXON_FORCE_INLINE __forceinline
#else
#   define CXON_FORCE_INLINE inline
#endif

#ifndef NDEBUG
#   if !defined(_MSC_VER)
#       include <cassert>
#       define CXON_ASSERT(e, m) assert(e)
#   else
#       include <crtdbg.h>
#       define CXON_ASSERT(e, m) _ASSERT_EXPR((e), m)
#   endif
#else
#       define CXON_ASSERT(e, m) ((void)(e))
#endif

// interface //////////////////////////////////////////////////////////////////

namespace cxon { // interface

    template <bool B, typename T = void>
        using enable_if_t = typename std::enable_if<B, T>::type;

    // format selectors

    template <typename T = struct cxon_format_traits>
        struct CXON : T { using traits = T; };  

    template <typename T = struct json_format_traits>
        struct JSON : T { using traits = T; };

    template <typename X, template <typename> class S, typename R = void>
        using enable_for_t = enable_if_t<std::is_same<X, S<typename X::traits>>::value, R>;

    // interface helpers

        template <typename II>
            struct is_continuous_iterator;
        template <typename, typename = void>
            struct is_output_iterator;

        template <typename, typename = void>
            struct is_back_insertable;

        template <typename II, typename P = typename std::iterator_traits<II>::pointer>
            constexpr auto continuous_range(II b, II e) -> enable_if_t< is_continuous_iterator<II>::value, std::pair<P, P>>;
        template <typename II>
            constexpr auto continuous_range(II b, II e) -> enable_if_t<!is_continuous_iterator<II>::value, std::pair<II, II>>;

    // read

    template <typename It>
        struct from_chars_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept { return !ec; }
        };

    template <typename X = JSON<>, typename T, typename InIt, typename ...CxPs>
        inline auto     from_chars(T& t, InIt b, InIt e, CxPs... p)       -> from_chars_result<InIt>;
    template <typename X = JSON<>, typename T, typename Iterable, typename ...CxPs>
        inline auto     from_chars(T& t, const Iterable& i, CxPs... p)    -> from_chars_result<decltype(std::begin(i))>;

    // write

    template <typename It>
        struct to_chars_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept { return !ec; }
        };

    template <typename X = JSON<>, typename OutIt, typename T, typename ...CxPs>
        inline auto     to_chars(OutIt o, const T& t, CxPs... p)          -> enable_if_t<is_output_iterator<OutIt>::value, to_chars_result<OutIt>>;
    template <typename X = JSON<>, typename Insertable, typename T, typename ...CxPs>
        inline auto     to_chars(Insertable& i, const T& t, CxPs... p)    -> enable_if_t<is_back_insertable<Insertable>::value, to_chars_result<decltype(std::begin(i))>>;
    template <typename X = JSON<>, typename FwIt, typename T, typename ...CxPs>
        inline auto     to_chars(FwIt b, FwIt e, const T& t, CxPs... p)   -> to_chars_result<FwIt>;

}   // cxon interface

namespace cxon { // errors

    enum class read_error : int {
        ok,                     // no error
        unexpected,             // unexpected input
        character_invalid,      // character invalid
        integral_invalid,       // integral invalid or out of range
        floating_point_invalid, // floating-point invalid
        boolean_invalid,        // boolean invalid
        escape_invalid,         // invalid escape sequence
        surrogate_invalid,      // invalid surrogate
        overflow                // buffer overflow
    };

    enum class write_error : int {
        ok,                     // no error
        output_failure,         // output cannot be written
        argument_invalid        // argument invalid
    };

}   // cxon errors

namespace cxon { // format traits

    struct format_traits {
        struct map {
            static constexpr char                   beg             = '{';
            static constexpr char                   end             = '}';
            static constexpr char                   div             = ':';
            static constexpr char                   sep             = ',';
            static constexpr bool                   unquoted_keys   = true;
        };
        struct list {
            static constexpr char                   beg             = '{';
            static constexpr char                   end             = '}';
            static constexpr char                   sep             = ',';
        };
        struct string {
            static constexpr char                   beg             = '"';
            static constexpr char                   end             = '"';
        };
        struct number {
            static constexpr bool                   strict          = false;
        };
        struct id {
            static constexpr char const*            nil             = "null";
            static constexpr char const*            pos             = "true";
            static constexpr char const*            neg             = "false";
        };
    };
    static_assert(format_traits::string::beg == format_traits::string::end && (format_traits::string::beg == '"' || format_traits::string::beg == '\''), "not supported");

    struct cxon_format_traits : format_traits {};

    struct json_format_traits : format_traits {
        struct map : format_traits::map {
            static constexpr bool                   unquoted_keys   = false;
        };
        struct list : format_traits::list {
            static constexpr char                   beg             = '[';
            static constexpr char                   end             = ']';
        };
        static constexpr bool                       strict_js       = false;
    };

    // access

    template <typename X>
        using map = typename X::map;
    template <typename X>
        using list = typename X::list;

}   // cxon format traits

namespace cxon { // contexts

    namespace prms {

        namespace bits {

            template <typename Ta, typename Ty, Ty c>
                struct ctt {
                    using tag   = Ta;
                    using type  = Ty;
                    static constexpr type value = c;
                };

            template <typename Ta, typename Ty>
                struct stt {
                    using tag   = Ta;
                    using type  = Ty;
                    type value;
                    stt()               : value() {}
                    stt(type&& v)       : value(std::move(v)) {}
                    stt(const type& v)  : value(v) {}
                };

            template <typename ...>
                struct pack {
                    using head_type = void;
                    using tag       = void;
                    using type      = void;
                };
            template <typename P>
                struct pack<P> : pack<> {
                    using head_type = pack<>;
                    using tag       = typename P::tag;
                    using type      = P;
                    type prm;
                    constexpr pack(type&& p)        : prm(std::move(p)) {}
                    constexpr pack(const type& p)   : prm(p) {}
                };
            template <typename P, typename ...T>
                struct pack<P, T...> : pack<T...> {
                    using head_type = pack<T...>;
                    using tag       = typename P::tag;
                    using type      = P;
                    type prm;
                    constexpr pack(type&& p, T&&... t)      : head_type(std::forward<T>(t)...), prm(std::move(p)) {}
                    constexpr pack(const type& p, T&&... t) : head_type(std::forward<T>(t)...), prm(p) {}
                };

            template <typename T>
                struct unwrap_reference                             { using type = T; }; // C++20
            template <typename T>
                struct unwrap_reference<std::reference_wrapper<T>>  { using type = T&; }; // C++20
 
            template <typename T>
                using unwrap_ref_decay      = unwrap_reference<typename std::decay<T>::type>; // C++20
            template <typename T>
                using unwrap_ref_decay_t    = typename unwrap_ref_decay<T>::type; // C++20

            template <typename Ta, typename Pa, bool V = std::is_same<typename Pa::head_type, void>::value, bool T = std::is_same<typename Pa::tag, Ta>::value>
                struct pack_has                         { static constexpr bool value = T; };
            template <typename Ta, typename Pa>
                struct pack_has<Ta, Pa, false, false>   { static constexpr bool value = pack_has<Ta, typename Pa::head_type>::value; };

            template <typename Ta, typename Pa, bool V = std::is_same<typename Pa::head_type, void>::value, bool T = std::is_same<typename Pa::tag, Ta>::value>
                struct pack_sbt                         { using type = Pa; static_assert(T, "tag unknown"); };
            template <typename Ta, typename Pa>
                struct pack_sbt <Ta, Pa, false, false>  { using type = typename pack_sbt<Ta, typename Pa::head_type>::type; };

        }   // bits

        template <typename Ta, typename Ty>
            using stt = bits::stt<Ta, Ty>;
        template <typename Ta, typename Ty, Ty c>
            using ctt = bits::ctt<Ta, Ty, c>;
        template <typename ...P>
            using pack = bits::pack<bits::unwrap_ref_decay_t<P>...>;

        template <typename ...P>
            constexpr pack<P...> make(P&&... p) {
                return { std::forward<P>(p)... };
            }

        template <typename Ta>
            struct tag {
                template <typename Pa>
                    using in = bits::pack_has<Ta, Pa>;

                template <typename Ty, Ty c>
                    static constexpr auto set() -> ctt<Ta, Ty, c>
                        { return {}; }

                template <typename Ty>
                    static constexpr auto set(Ty&& v) -> stt<Ta, Ty>
                        { return stt<Ta, Ty>(std::forward<Ty>(v)); }
                template <typename Ty>
                    static constexpr auto set(const Ty& v) -> stt<Ta, Ty>
                        { return stt<Ta, Ty>(v); }

                template <typename Pa>
                    using pack_of_tag = typename bits::pack_sbt<Ta, Pa>::type;

                template <typename Pa, typename Ty>
                    static constexpr auto constant(Ty)      -> cxon::enable_if_t< in<Pa>::value, Ty>
                        { return pack_of_tag<Pa>::type::value; }
                template <typename Pa, typename Ty>
                    static constexpr auto constant(Ty dflt) -> cxon::enable_if_t<!in<Pa>::value, Ty>
                        { return dflt; }

                template <typename Pa>
                    static constexpr auto reference(Pa& p)      -> typename pack_of_tag<Pa>::type::type&
                        { return static_cast<pack_of_tag<Pa>&>(p).prm.value; }
                template <typename Pa>
                    static constexpr auto value(const Pa& p)    -> typename pack_of_tag<Pa>::type::type
                        { return static_cast<const pack_of_tag<Pa>&>(p).prm.value; }

                template <typename Pa, typename Ty>
                    static constexpr auto value(const Pa& p, Ty)    -> cxon::enable_if_t< in<Pa>::value, Ty>
                        { return static_cast<const pack_of_tag<Pa>&>(p).prm.value; }
                template <typename Pa, typename Ty>
                    static constexpr auto value(const Pa&, Ty dflt) -> cxon::enable_if_t<!in<Pa>::value, Ty>
                        { return dflt; }
            };

    }   // prms

    template <typename ...Ps> // prms
        struct context {
            using prms_type = prms::pack<Ps...>;

            std::error_condition    ec;
            prms_type               ps;

            context(Ps&&... ps) :   ec(),
                                    ps(prms::make(std::forward<Ps>(ps)...))
            {
            }

            template <typename E>
                auto operator |(E e) noexcept -> enable_if_t<std::is_enum<E>::value, context&> {
                    return ec = e, *this;
                }
        };

    template <typename ...P>
        using read_context  = context<P...>;
    template <typename ...P>
        using write_context = context<P...>;

    template <typename Cx>
        using prms_type = typename Cx::prms_type;

}   // cxon contexts

#define CXON_PARAMETER(P) struct P : cxon::prms::tag<P> {}

namespace cxon { // context parameters

    CXON_PARAMETER(allocator);      // read: std::Allocator [default = std::allocator<T>()]: T*
    CXON_PARAMETER(num_len_max);    // read: unsigned/constexpr [32U (integral), 64U (floating point)]: numbers
    CXON_PARAMETER(ids_len_max);    // read: unsigned/constexpr [64U]: map, object key
    CXON_PARAMETER(fp_precision);   // write: int/constexpr [std::numeric_limits<T>::max_digits10]: floating-points

}   // cxon context parameters

namespace cxon { // implementation bridge

    template <typename E, typename T, typename R = E>
        using enable_if_same_t = enable_if_t< std::is_same<E, T>::value, R>;

    // read value

    template <typename X, typename T>
        struct read {
            //template <typename II, typename Cx> static bool value(T& t, II& i, II e, Cx& cx);
        };

    template <typename X, typename T, typename II, typename Cx> // struct read
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_same_t<bool, decltype(read<X, T>::value(t, i, e, cx))>
        {
            return read<X, T>::value(t, i, e, cx);
        }
    template <typename X, typename T, typename II, typename Cx> // read_value static method of T
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_same_t<bool, decltype(T::template read_value<X>(t, i, e, cx))>
        {
            return T::template read_value<X>(t, i, e, cx);
        }
    template <typename X, typename T, typename II, typename Cx> // read_value method of T
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_same_t<bool, decltype(t.template read_value<X>(i, e, cx))>
        {
            return t.template read_value<X>(i, e, cx);
        }

    // read key

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx);

    // write value

    template <typename X, typename T>
        struct write {
            //template <typename O, typename Cx> static bool value(O& o, const T& t, Cx& cx);
        };

    template <typename X, typename T, typename O, typename Cx> // struct write
        inline auto write_value(O& o, const T& t, Cx& cx)
            -> enable_if_same_t<bool, decltype(write<X, T>::value(o, t, cx))>
        {
            return write<X, T>::value(o, t, cx);
        }
    template <typename X, typename T, typename O, typename Cx> // write_value static method of T
        inline auto write_value(O& o, const T& t, Cx& cx)
            -> enable_if_same_t<bool, decltype(T::template write_value<X>(o, t, cx))>
        {
            return T::template write_value<X>(o, t, cx);
        }
    template <typename X, typename T, typename O, typename Cx> // write_value method of T
        inline auto write_value(O& o, const T& t, Cx& cx)
            -> enable_if_same_t<bool, decltype(t.template write_value<X>(o, cx))>
        {
            return t.template write_value<X>(o, cx);
        }

    // write key

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx);

}   // cxon implementation bridge

namespace cxon { namespace io { // I/O

    // input

    template <typename II>
        inline char peek(II i, II e); // *i or 0xFF 
    template <typename II>
        inline char next(II& i, II e); // *++i or 0xFF

    template <typename X, typename II>
        inline void consume(II& i, II e); // skip spaces

    template <typename X, typename II>
        inline bool consume(char c, II& i, II e); // try to read a character (skip spaces first)
    template <typename X, typename II, typename Cx>
        inline bool consume(char c, II& i, II e, Cx& cx); // try to read a character, error on failure (skip spaces first)

    template <typename X, typename II>
        inline bool consume(const char* s, II& i, II e); // try to read a string (skip spaces first)

    template <typename X, typename II>
        constexpr bool consume(std::nullptr_t t, II& i, II e); // read nothing
    template <typename X, typename II, typename Cx>
        constexpr bool consume(std::nullptr_t t, II& i, II e, Cx& cx); // read nothing

    // output

    template <typename O>
        inline bool poke(O& o, char c); // write a character
    template <typename O>
        inline bool poke(O& o, const char* s); // write a string
    template <typename O>
        inline bool poke(O& o, const char* s, size_t n); // write first n characters of a string
    template <typename O>
        inline bool poke(O& o, unsigned n, char c); // write a character n times
    template <typename O>
        inline bool poke(O& o, std::nullptr_t t) noexcept; // write nothing

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, char c, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, size_t n, Cx& cx);
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, unsigned n, char c, Cx& cx);
    template <typename X, typename O, typename Cx>
        constexpr bool poke(O& o, std::nullptr_t t, Cx& cx);

}}   // cxon::io I/O

namespace cxon { namespace container { // list read/write helpers

    // read

    template <typename X, typename Cr, typename II, typename Cx, typename EA>
        inline bool read(II& i, II e, Cx& cx, EA element_add);

    // write

    template <typename X, typename Cr, typename O, typename II, typename Cx, typename L>
        inline bool write(O& o, II b, II e, Cx& cx, L element_write);
    template <typename X, typename Cr, typename O, typename T, typename Cx, typename L>
        inline bool write(O& o, const T& t, Cx& cx, L element_write);
    template <typename X, typename Cr, typename O, typename II, typename Cx>
        inline bool write(O& o, II b, II e, Cx& cx);
    template <typename X, typename Cr, typename O, typename T, typename Cx>
        inline bool write(O& o, const T& t, Cx& cx);

}}  // cxon::container list read/write helpers

namespace cxon { namespace enums { // enum reader/writer construction helpers

    template <typename E>
        struct value {
            char const*const name;
            E const value;
        };
    template <typename E>
        constexpr value<E> make_value(const char* name, E value) {
            return { name, value };
        }

    template <typename X, typename E, typename V, typename II, typename Cx>
        inline bool read_value(E& t, V vb, V ve, II& i, II e, Cx& cx);
    template <typename X, typename E, typename V, typename O, typename Cx>
        inline bool write_value(O& o, E t, V vb, V ve, Cx& cx);

}}  // cxon::enums enum reader/writer construction helpers

namespace cxon { namespace structs { // structured types reader/writer construction helpers

    struct skip_type {};

    template <typename D>
        struct field;
    template <typename D = skip_type>
        constexpr field<D> make_field(const char* name, D d = {}) { return { name, d }; }

    template <typename ...>
        struct fields {};
    template <typename ...T> 
        constexpr fields<T...> make_fields(T... t) { return { t... }; }

    template <typename X, typename S, typename ...F, typename II, typename Cx>
        inline bool read_fields(S& s, const fields<F...>& f, II& i, II e, Cx& cx);
    template <typename X, typename S, typename ...F, typename O, typename Cx>
        inline bool write_fields(O& o, const S& s, const fields<F...>& f, Cx& cx);

}}  // cxon::structs structured types reader/writer construction helpers

// implementation /////////////////////////////////////////////////////////////

namespace cxon { // interface implementation

    template <typename II>
        struct is_continuous_iterator {
            static const bool value =
                std::is_same<II,                             typename std::iterator_traits<II>::pointer>        ::                 value ||
                std::is_same<II, typename std::basic_string <typename std::iterator_traits<II>::value_type>     ::      iterator>::value ||
                std::is_same<II, typename std::basic_string <typename std::iterator_traits<II>::value_type>     ::const_iterator>::value ||
                std::is_same<II, typename std::vector       <typename std::iterator_traits<II>::value_type>     ::      iterator>::value ||
                std::is_same<II, typename std::vector       <typename std::iterator_traits<II>::value_type>     ::const_iterator>::value ||
                std::is_same<II, typename std::array        <typename std::iterator_traits<II>::value_type, 0>  ::      iterator>::value ||
                std::is_same<II, typename std::array        <typename std::iterator_traits<II>::value_type, 0>  ::const_iterator>::value
            ;
        };

    template <typename, typename>
        struct is_output_iterator : std::false_type {};
    template <typename I>
        struct is_output_iterator<I, enable_if_t<std::is_same<typename I::iterator_category, std::output_iterator_tag>::value>> : std::true_type {};

    template <typename, typename>
        struct is_back_insertable : std::false_type {};
    template <typename C>
        struct is_back_insertable<C, decltype(C().push_back(' '))> : std::true_type {};

    template <typename II, typename P>
        constexpr auto continuous_range(II b, II e) -> enable_if_t< is_continuous_iterator<II>::value, std::pair<P, P>>     { return std::make_pair(&*b, &*b + std::distance(b, e)); }
    template <typename II>
        constexpr auto continuous_range(II b, II e) -> enable_if_t<!is_continuous_iterator<II>::value, std::pair<II, II>>   { return std::make_pair(b, e); }

    // read

    namespace interface {

        template <typename X, typename T, typename II, typename ...CxPs>
            CXON_FORCE_INLINE auto from_chars(T& t, II b, II e, CxPs... p) -> from_chars_result<II> {
                    if (b == e) return { read_error::unexpected, b };
                read_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    auto g = continuous_range(b, e); auto const o = g.first;
                    bool const r = read_value<X>(t, g.first, g.second, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, (std::advance(b, std::distance(o, g.first)), b) };
            }
        template <typename X, typename T, typename I, typename ...CxPs>
            CXON_FORCE_INLINE auto from_chars(T& t, const I& i, CxPs... p) -> from_chars_result<decltype(std::begin(i))> {
                return interface::from_chars<X>(t, std::begin(i), std::end(i), p...);
            }

    }

    template <typename X, typename T, typename II, typename ...CxPs>
        inline auto from_chars(T& t, II b, II e, CxPs... p) -> from_chars_result<II> {
            return interface::from_chars<X>(t, b, e, p...);
        }
    template <typename X, typename T, typename I, typename ...CxPs>
        inline auto from_chars(T& t, const I& i, CxPs... p) -> from_chars_result<decltype(std::begin(i))> {
            return interface::from_chars<X>(t, i, p...);
        }

    // write

    namespace bits {

        template <typename FwIt>
            struct range_write_iterator {
                using iterator_category = std::output_iterator_tag;
                using value_type        = char;
                using difference_type   = void;
                using pointer           = void;
                using reference         = void;

                range_write_iterator& operator ++() noexcept       { return *this; }
                range_write_iterator& operator *() noexcept        { return *this; }

                constexpr range_write_iterator(FwIt b, FwIt e)
                :   b_(b), e_(e)
                {
#                   if __cplusplus >= 201402L
                        CXON_ASSERT(std::distance(b_, e_) >= 0, "unexpected range");
#                   endif
                }

                range_write_iterator& operator =(char c) {
                    CXON_ASSERT(*this, "unexpected state");
                    *b_ = c, ++b_;
                    return *this;
                }
                void append(const char* s) {
                    CXON_ASSERT(*this, "unexpected state");
                    for ( ; b_ != e_ && *s; ++s, ++b_) *b_ = *s;
                }
                void append(const char* s, size_t n) {
                    CXON_ASSERT(*this, "unexpected state");
                    n = std::min<size_t>(n, e_ - b_);
                    std::copy_n(s, n, b_), std::advance(b_, n);
                }
                void append(size_t n, char c) {
                    CXON_ASSERT(*this, "unexpected state");
                    n = std::min<size_t>(n, e_ - b_);
                    std::fill_n(b_, n, c), std::advance(b_, n);
                }

                operator bool() const noexcept { return b_ != e_; }
                operator FwIt() const noexcept { return b_; }

            private:
                FwIt        b_;
                FwIt const  e_;
            };
        template <typename FwIt>
            constexpr range_write_iterator<FwIt> make_range_writer(FwIt b, FwIt e) {
                return range_write_iterator<FwIt>(b, e);
            }

    }

    namespace interface {

        template <typename X, typename OI, typename T, typename ...CxPs>
            CXON_FORCE_INLINE auto to_chars(OI o, const T& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_chars_result<OI>> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    bool const r = write_value<X>(o, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, o };
            }
        template <typename X, typename I, typename T, typename ...CxPs>
            CXON_FORCE_INLINE auto to_chars(I& i, const T& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_chars_result<decltype(std::begin(i))>> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    auto const s = std::distance(std::begin(i), std::end(i));
                    bool const r = write_value<X>(i, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                    auto b = std::begin(i); std::advance(b, s);
                return { cx.ec, b };
            }
        template <typename X, typename FwIt, typename T, typename ...CxPs>
            CXON_FORCE_INLINE auto to_chars(FwIt b, FwIt e, const T& t, CxPs... p) -> to_chars_result<FwIt> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    auto o = bits::make_range_writer(b, e);
                    bool const r = write_value<X>(o, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, o };
            }

    }

    template <typename X, typename OI, typename T, typename ...CxPs>
        inline auto to_chars(OI o, const T& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_chars_result<OI>> {
            return interface::to_chars<X>(o, t, p...);
        }
    template <typename X, typename I, typename T, typename ...CxPs>
        inline auto to_chars(I& i, const T& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_chars_result<decltype(std::begin(i))>> {
            return interface::to_chars<X>(i, t, p...);
        }
    template <typename X, typename FwIt, typename T, typename ...CxPs>
        inline auto to_chars(FwIt b, FwIt e, const T& t, CxPs... p) -> to_chars_result<FwIt> {
            return interface::to_chars<X>(b, e, t, p...);
        }

}   // cxon interface implementation

namespace cxon { // errors

    struct read_error_category : std::error_category {
        const char* name() const noexcept override {
            return "cxon/read";
        }
        std::string message(int ev) const override {
            switch (static_cast<read_error>(ev)) {
                case read_error::ok:                        return "no error";
                case read_error::unexpected:                return "unexpected input";
                case read_error::character_invalid:         return "invalid character";
                case read_error::integral_invalid:          return "invalid integral or value out of range";
                case read_error::floating_point_invalid:    return "invalid floating point";
                case read_error::boolean_invalid:           return "invalid boolean";
                case read_error::escape_invalid:            return "invalid escape sequence";
                case read_error::surrogate_invalid:         return "invalid surrogate";
                case read_error::overflow:                  return "buffer overflow";
            }
            CXON_ASSERT(0, "unexpected");
            return "unknown error";
        }
        static read_error_category const value;
    };
    read_error_category const read_error_category::value {};

    inline std::error_condition make_error_condition(read_error e) noexcept {
        return { static_cast<int>(e), read_error_category::value };
    }

    struct write_error_category : std::error_category {
        const char* name() const noexcept override {
            return "cxon/write";
        }
        std::string message(int ev) const override {
            switch (static_cast<write_error>(ev)) {
                case write_error::ok:               return "no error";
                case write_error::output_failure:   return "output cannot be written";
                case write_error::argument_invalid: return "invalid argument";
            }
            CXON_ASSERT(0, "unexpected");
            return "unknown error";
        }
        static write_error_category const value;
    };
    write_error_category const write_error_category::value {};

    inline std::error_condition make_error_condition(write_error e) noexcept {
        return { static_cast<int>(e), write_error_category::value };
    }

}   // cxon errors

namespace std { // cxon errors
    template <> struct is_error_condition_enum<cxon::read_error> : true_type {};
    template <> struct is_error_condition_enum<cxon::write_error> : true_type {};
}   // std cxon errors

namespace cxon { // I/O

    namespace bits { // character classes

        enum : unsigned char {
            CTRL    = 1 << 0,       // control
            REAL    = 1 << 1,       // character
            CXSP    = 1 << 2,       // cxon space
            JSSP    = 1 << 3,       // json space
            DGOC    = 1 << 4,       // octal
            DGDC    = 1 << 5,       // decimal
            DGHX    = 1 << 6,       // hexadecimal
            ALPH    = 1 << 7,       // alpha
            ALNM    = ALPH|DGDC,    // alphanumeric
            C0      = CTRL,
            C1      = REAL,
            C2      = CXSP|CTRL,
            C3      = JSSP|CXSP|CTRL,
            C4      = JSSP|CXSP|REAL,
            C5      = DGOC|DGDC|DGHX|REAL,
            C6      = DGDC|DGHX|REAL,
            C7      = DGHX|ALPH|REAL,
            C8      = ALPH|REAL
        };
        static constexpr unsigned char is_x_[] = { 
            /*  0*/  C0,C0,C0,C0,C0,C0,C0,C0,C0,C3,C3,C2,C2,C3,C0,C0,
            /* 16*/  C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,
            /* 32*/  C4,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
            /* 48*/  C5,C5,C5,C5,C5,C5,C5,C5,C6,C6,C1,C1,C1,C1,C1,C1,
            /* 64*/  C1,C7,C7,C7,C7,C7,C7,C8,C8,C8,C8,C8,C8,C8,C8,C8,
            /* 80*/  C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C1,C1,C1,C1,C1,
            /* 96*/  C1,C7,C7,C7,C7,C7,C7,C8,C8,C8,C8,C8,C8,C8,C8,C8,
            /*112*/  C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C1,C1,C1,C1,C1,
            /*128*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
            /*144*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
            /*160*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
            /*176*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
            /*192*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
            /*208*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
            /*224*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
            /*240*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,00
        };

        template <typename X>
            struct is {
                static bool ctrl   (char c) noexcept { return CTRL & is_x_[(unsigned char)c]; }
                static bool real   (char c) noexcept { return REAL & is_x_[(unsigned char)c]; }
                static bool space  (char c) noexcept { return CXSP & is_x_[(unsigned char)c]; }
                static bool digit8 (char c) noexcept { return DGOC & is_x_[(unsigned char)c]; }
                static bool digit10(char c) noexcept { return DGDC & is_x_[(unsigned char)c]; }
                static bool digit16(char c) noexcept { return DGHX & is_x_[(unsigned char)c]; }
                static bool alpha  (char c) noexcept { return ALPH & is_x_[(unsigned char)c]; }
                static bool alnum  (char c) noexcept { return ALNM & is_x_[(unsigned char)c]; }
            };
        template <typename X>
            struct is<JSON<X>> : is<X> {
                static bool space  (char c) noexcept { return JSSP & is_x_[(unsigned char)c]; }
            };

    }   // bits // character classes

    namespace bits {

        template <typename, typename = void>
            struct is_forward_iterator : std::false_type {};
        template <typename I>
            struct is_forward_iterator<I, enable_if_t<
                    !std::is_same<typename std::iterator_traits<I>::iterator_category, std::input_iterator_tag>::value
                >> : std::true_type {};

        template<typename II>
            inline auto rewind(II&, II) noexcept     -> enable_if_t<!is_forward_iterator<II>::value> {}
        template<typename II>
            inline auto rewind(II& i, II o) noexcept -> enable_if_t< is_forward_iterator<II>::value> { i = o; }

    }

    namespace io {

        // input

        template <typename II>
            inline char peek(II i, II e) {
                return i != e ? *i : '\xFF';
            }
        template <typename II>
            inline char next(II& i, II e) {
                return ++i, peek(i, e);
            }

        template <typename X, typename II>
            inline void consume(II& i, II e) {
                while (cxon::bits::is<X>::space(peek(i, e))) next(i, e);
            }

        template <typename X, typename II>
            inline bool consume(char c, II& i, II e) {
                consume<X>(i, e);
                return c == peek(i, e) ? (next(i, e), true) : false;
            }
        template <typename X, typename II, typename Cx>
            inline bool consume(char c, II& i, II e, Cx& cx) {
                consume<X>(i, e);
                if (c == peek(i, e)) return next(i, e), true;
                return cx|read_error::unexpected, false;
            }

        template <typename X, typename II>
            inline bool consume(const char* s, II& i, II e) {
                consume<X>(i, e);
                for (char c = peek(i, e); *s && *s == c; c = next(i, e), ++s) ;
                return *s == '\0';
            }

        template <typename X, typename II>
            constexpr bool consume(std::nullptr_t, II&, II) { return true; }
        template <typename X, typename II, typename Cx>
            constexpr bool consume(std::nullptr_t, II&, II, Cx&) { return true; }

        // output

        namespace bits {

            template <typename O>
                inline auto push(O& o, char c) -> enable_if_t<is_output_iterator<O>::value> {
                    *o = c, ++o;
                }
            template <typename O>
                inline auto push(O& o, char c) -> enable_if_t<is_back_insertable<O>::value> {
                    o.push_back(c);
                }

#           define HAS_METH_DEF(name, M)\
                template <typename, typename = void>\
                    struct has_##name : std::false_type {};\
                template <typename T>\
                    struct has_##name<T, enable_if_t<std::is_same<void, decltype(std::declval<T>().M, void())>::value>> : std::true_type {}

            HAS_METH_DEF(append_s, append(std::declval<char*>()));
            template <typename O>
                inline auto push(O& o, const char* s) -> enable_if_t<!has_append_s<O>::value> {
                    while (*s) push(o, *s), ++s;
                }
            template <typename O>
                inline auto push(O& o, const char* s) -> enable_if_t< has_append_s<O>::value> {
                    o.append(s);
                }

            HAS_METH_DEF(append_sn, append(std::declval<char*>(), 0));
            template <typename O>
                inline auto push(O& o, const char* s, size_t n) -> enable_if_t<!has_append_sn<O>::value> {
                    while (n) push(o, *s), ++s, --n;
                }
            template <typename O>
                inline auto push(O& o, const char* s, size_t n) -> enable_if_t< has_append_sn<O>::value> {
                    o.append(s, n);
                }

            HAS_METH_DEF(append_nc, append(0, ' '));
            template <typename O>
                inline auto push(O& o, unsigned n, char c) -> enable_if_t<!has_append_nc<O>::value> {
                    while (n) push(o, c), --n;
                }
            template <typename O>
                inline auto push(O& o, unsigned n, char c) -> enable_if_t< has_append_nc<O>::value> {
                    o.append(n, c);
                }

            HAS_METH_DEF(bool, operator bool());
            HAS_METH_DEF(good, good());

            template <typename O, typename ...P>
                inline auto poke(O& o, P... p)     -> enable_if_t<!has_bool<O>::value && !has_good<O>::value, bool> {
                    return push(o, p...), true;
                }
            template <typename O, typename ...P>
                inline auto poke(O& o, P... p) -> enable_if_t<!has_bool<O>::value &&  has_good<O>::value, bool> {
                    return push(o, p...), o.good();
                }
            template <typename O, typename ...P>
                inline auto poke(O& o, P... p) -> enable_if_t< has_bool<O>::value, bool> {
                    return push(o, p...), o;
                }

            template <typename X, typename O, typename Cx, typename ...P>
                inline auto poke(O& o, Cx&, P... p)     -> enable_if_t<!has_bool<O>::value && !has_good<O>::value, bool> {
                    return push(o, p...), true;
                }
            template <typename X, typename O, typename Cx, typename ...P>
                inline auto poke(O& o, Cx& cx, P... p) -> enable_if_t<!has_bool<O>::value &&  has_good<O>::value, bool> {
                    return push(o, p...), (!o.good() ? (cx|write_error::output_failure), false : true);
                }
            template <typename X, typename O, typename Cx, typename ...P>
                inline auto poke(O& o, Cx& cx, P... p) -> enable_if_t< has_bool<O>::value, bool> {
                    return push(o, p...), (!o ? (cx|write_error::output_failure), false : true);
                }

#           undef HAS_METH_DEF

        }

        template <typename O>
            inline bool poke(O& o, char c)                  { return bits::poke(o, c); }
        template <typename O>
            inline bool poke(O& o, const char* s)           { return bits::poke(o, s); }
        template <typename O>
            inline bool poke(O& o, const char* s, size_t n) { return bits::poke(o, s, n); }
        template <typename O>
            inline bool poke(O& o, unsigned n, char c)      { return bits::poke(o, n, c); }
        template <typename O>
            inline bool poke(O&, std::nullptr_t) noexcept   { return true; }

        template <typename X, typename O, typename Cx>
            inline bool poke(O& o, char c, Cx& cx)                    { return bits::poke<X>(o, cx, c); }
        template <typename X, typename O, typename Cx>
            inline bool poke(O& o, const char* s, Cx& cx)             { return bits::poke<X>(o, cx, s); }
        template <typename X, typename O, typename Cx>
            inline bool poke(O& o, const char* s, size_t n, Cx& cx)   { return bits::poke<X>(o, cx, s, n); }
        template <typename X, typename O, typename Cx>
            inline bool poke(O& o, unsigned n, char c, Cx& cx)        { return bits::poke<X>(o, cx, n, c); }
        template <typename X, typename O, typename Cx>
            constexpr bool poke(O&, std::nullptr_t, Cx&)               { return true; }

    }   // io

}   // cxon I/O

namespace cxon { // list read/write helpers

    namespace  bits {

        template <typename X, typename Cr, typename II, typename EA>
            inline void list_read(II& i, II e, EA element_add) {
                // expects non-empty list
                while (element_add() && io::consume<X>(Cr::sep, i, e)) ;
            }

        template <typename X, typename Cr, typename O, typename II, typename Cx, typename L>
            inline void list_write(O& o, II b, II e, Cx& cx, L element_write) {
                if (b != e && element_write(*b)) {
                    while (++b != e && io::poke<X>(o, Cr::sep, cx) && element_write(*b)) ;
                }
            }

    }   // bits

    namespace container {

        // read

        template <typename X, typename Cr, typename II, typename Cx, typename EA>
            inline bool read(II& i, II e, Cx& cx, EA element_add) {
                if (!io::consume<X>(Cr::beg, i, e, cx)) return false;
                if ( io::consume<X>(Cr::end, i, e))      return true;
                return bits::list_read<X, Cr>(i, e, element_add), !cx.ec && io::consume<X>(Cr::end, i, e, cx);
            }

        // write

        template <typename X, typename Cr, typename O, typename II, typename Cx, typename L>
            inline bool write(O& o, II b, II e, Cx& cx, L element_write) {
                if (!io::poke<X>(o, Cr::beg, cx)) return false;
                return bits::list_write<X, Cr>(o, b, e, cx, element_write), !cx.ec && io::poke<X>(o, Cr::end, cx);
            }
        template <typename X, typename Cr, typename O, typename T, typename Cx, typename L>
            inline bool write(O& o, const T& t, Cx& cx, L element_write) {
                return write<X, Cr>(o, std::begin(t), std::end(t), cx, element_write);
            }
        template <typename X, typename Cr, typename O, typename II, typename Cx>
            inline bool write(O& o, II b, II e, Cx& cx) {
                return write<X, Cr>(o, b, e, cx, [&](const decltype(*b)& e) {
                    return write_value<X>(o, e, cx);
                });
            }
        template <typename X, typename Cr, typename O, typename T, typename Cx>
            inline bool write(O& o, const T& t, Cx& cx) {
                return write<X, Cr>(o, std::begin(t), std::end(t), cx);
            }

    }   // container

}  // cxon list read/write helpers

namespace cxon { namespace bits { // <charconv>

    namespace charconv {

        using namespace std;

        enum class chars_format {
            scientific  = 1,
            fixed       = 2,
            hex         = 4,
            general     = fixed|scientific
        };

        struct from_chars_result {
            const char* ptr;
            errc ec;
        };

        namespace bits {

            template <typename T, typename F>
                constexpr auto clamp(F t)       -> enable_if_t<sizeof(T) == sizeof(F), T> {
                    return t;
                }
            template <typename T, typename F>
                inline auto clamp(F f) noexcept -> enable_if_t<sizeof(T) < sizeof(F) && std::is_signed<T>(), T> {
                            if (f < numeric_limits<T>::min()) errno = ERANGE, f = numeric_limits<T>::min();
                    else    if (f > numeric_limits<T>::max()) errno = ERANGE, f = numeric_limits<T>::max();
                    return (T)f;
                }
            template <typename T, typename F>
                inline auto clamp(F f) noexcept -> enable_if_t<sizeof(T) < sizeof(F) && std::is_unsigned<T>(), T> {
                    if (numeric_limits<T>::max() < f) errno = ERANGE, f = numeric_limits<T>::max();
                    return (T)f;
                }

            template <typename T>
                inline auto is_sign_invalid(char c) noexcept -> enable_if_t<std::is_signed<T>::value, bool> {
                    return c == '+';
                }
            template <typename T>
                inline auto is_sign_invalid(char c) noexcept -> enable_if_t<std::is_unsigned<T>::value, bool> {
                    return c == '+' || c == '-';
                }

            template <typename T, typename Cv>
                inline from_chars_result number_from_chars(const char* first, const char*, Cv convert, T& value, int base) {
                    CXON_ASSERT(base == 2 || base == 8 || base == 10 || base == 16, "unsupported base");
                        if (is_sign_invalid<T>(*first)) return { first, errc::invalid_argument };
                    char* end;
                    errno = 0, value = clamp<T>(convert(first, &end, base));
                    return { end, end != first ? errno != ERANGE ? errc{} : errc::result_out_of_range : errc::invalid_argument };
                }
            template <typename T, typename Cv>
                inline from_chars_result number_from_chars(const char* first, const char*, Cv convert, T& value, chars_format) {
                        if (is_sign_invalid<T>(*first)) return { first, errc::invalid_argument };
                    char* end;
                    value = convert(first, &end);
                    return { end, end == first ? errc::invalid_argument : errc() };
                }

        }

#       define CXON_FROM_CHARS(T, C)\
            inline from_chars_result from_chars(const char* first, const char* last, T& value, int base = 10) {\
                return bits::number_from_chars(first, last, C, value, base);\
            }
            CXON_FROM_CHARS(char, strtol)
            CXON_FROM_CHARS(signed char, strtol)
            CXON_FROM_CHARS(unsigned char, strtoul)
            CXON_FROM_CHARS(short, strtol)
            CXON_FROM_CHARS(unsigned short, strtoul)
            CXON_FROM_CHARS(int, strtol)
            CXON_FROM_CHARS(unsigned int, strtoul)
            CXON_FROM_CHARS(long, strtol)
            CXON_FROM_CHARS(unsigned long, strtoul)
            CXON_FROM_CHARS(long long, strtoll)
            CXON_FROM_CHARS(unsigned long long, strtoull)
#       undef CXON_FROM_CHARS
#       define CXON_FROM_CHARS(T, C)\
            inline from_chars_result from_chars(const char* first, const char* last, T& value, chars_format fmt = chars_format::general) {\
                return bits::number_from_chars(first, last, C, value, fmt);\
            }
            CXON_FROM_CHARS(float, strtof)
            CXON_FROM_CHARS(double, strtod)
            CXON_FROM_CHARS(long double, strtold)
#       undef CXON_FROM_CHARS

        struct to_chars_result {
            char* ptr;
            errc ec;
        };

        namespace bits {

            template <typename> struct fmt;
            template <> struct fmt<char>                // %c writes a single character but, the argument is first converted to unsigned char
                                                        { static constexpr char const*const str = "%hhu";   }; // no char[], clang complains ...
            template <> struct fmt<signed char>         { static constexpr char const*const str = "%hhi";   };
            template <> struct fmt<unsigned char>       { static constexpr char const*const str = "%hhu";   };
            template <> struct fmt<short>               { static constexpr char const*const str = "%hi";    };
            template <> struct fmt<unsigned short>      { static constexpr char const*const str = "%hu";    };
            template <> struct fmt<int>                 { static constexpr char const*const str = "%i";     };
            template <> struct fmt<unsigned int>        { static constexpr char const*const str = "%u";     };
            template <> struct fmt<long>                { static constexpr char const*const str = "%li";    };
            template <> struct fmt<unsigned long>       { static constexpr char const*const str = "%lu";    };
            template <> struct fmt<long long>           { static constexpr char const*const str = "%lli";   };
            template <> struct fmt<unsigned long long>  { static constexpr char const*const str = "%llu";   };
            template <> struct fmt<float>               { static constexpr char const*const str = "%.*g";   };
            template <> struct fmt<double>              { static constexpr char const*const str = "%.*g";   };
            template <> struct fmt<long double>         { static constexpr char const*const str = "%.*Lg";  };

            // to_chars: on error, returns a value of type to_chars_result holding errc::value_too_large in ec,
            //  a copy of the value last in ptr, and leaves the contents of the range [first, last) in unspecified state,
            //  but we don't care, the buffer must be sufficient
            template <typename T>
                inline to_chars_result number_to_chars(char* first, char* last, T value) {
                    size_t const l = last - first;
                    int const w = snprintf(first, l, fmt<T>::str, value); CXON_ASSERT(w > 0 && (size_t)w < l, "conversion failed");
                    return { first + w, errc{} };
                }
            template <typename T>
                inline to_chars_result number_to_chars(char* first, char* last, T value, int precision) {
                    size_t const l = last - first;
                    int const w = snprintf(first, l, fmt<T>::str, precision, value); CXON_ASSERT(w > 0 && (size_t)w < l, "conversion failed");
                    return { first + w, errc{} };
                }

        }

#       define CXON_TO_CHARS(T)\
            inline to_chars_result to_chars(char* first, char* last, T value) {\
                return bits::number_to_chars<T>(first, last, value);\
            }
            CXON_TO_CHARS(char)
            CXON_TO_CHARS(signed char)
            CXON_TO_CHARS(unsigned char)
            CXON_TO_CHARS(short)
            CXON_TO_CHARS(unsigned short)
            CXON_TO_CHARS(int)
            CXON_TO_CHARS(unsigned int)
            CXON_TO_CHARS(long)
            CXON_TO_CHARS(unsigned long)
            CXON_TO_CHARS(long long)
            CXON_TO_CHARS(unsigned long long)
#       undef CXON_TO_CHARS
#       define CXON_TO_CHARS(T)\
            inline to_chars_result to_chars(char* first, char* last, T value, int precision) {\
                return bits::number_to_chars<T>(first, last, value, precision);\
            }
            CXON_TO_CHARS(float)
            CXON_TO_CHARS(double)
            CXON_TO_CHARS(long double)
#       undef CXON_TO_CHARS

    }   // charconv

#   ifdef HAVE_CHARCONV
#       define HAS_FUNC_DEF(name, R, F)\
            template <typename, typename = void>\
                struct has_##name : std::false_type {};\
            template <typename T>\
                struct has_##name<T, enable_if_t<std::is_same<R, decltype(F)>::value>> : std::true_type {}

        HAS_FUNC_DEF(from_chars_i, std::from_chars_result, std::from_chars(std::declval<char*>(), std::declval<char*>(), std::declval<T&>(), 0));
        template <typename T>
            inline auto from_chars(const char* f, const char* l, T& t, int base) noexcept   -> enable_if_t< has_from_chars_i<T>::value, std::from_chars_result> {
                return std::from_chars(f, l, t, base);
            }
        template <typename T>
            inline auto from_chars(const char* f, const char* l, T& t, int base)            -> enable_if_t<!has_from_chars_i<T>::value, std::from_chars_result> {
                auto const r = charconv::from_chars(f, l, t, base);
                return { r.ptr, r.ec };
            }

        HAS_FUNC_DEF(from_chars_f, std::from_chars_result, std::from_chars(std::declval<char*>(), std::declval<char*>(), std::declval<T&>()));
        template <typename T>
            inline auto from_chars(const char* f, const char* l, T& t) noexcept -> enable_if_t< has_from_chars_f<T>::value, std::from_chars_result> {
                return std::from_chars(f, l, t);
            }
        template <typename T>
            inline auto from_chars(const char* f, const char* l, T& t)          -> enable_if_t<!has_from_chars_f<T>::value, std::from_chars_result> {
                auto const r = charconv::from_chars(f, l, t);
                return { r.ptr, r.ec };
            }

        HAS_FUNC_DEF(to_chars, std::to_chars_result, std::to_chars(std::declval<char*>(), std::declval<char*>(), std::declval<T>()));
        HAS_FUNC_DEF(to_chars_p, std::to_chars_result, std::to_chars(std::declval<char*>(), std::declval<char*>(), std::declval<T>(), (std::chars_format)0, 0));

        template <typename T>
            inline auto to_chars(char* f, char* l, T t) noexcept    -> enable_if_t< has_to_chars<T>::value, std::to_chars_result> {
                return std::to_chars(f, l, t);
            }
        template <typename T>
            inline auto to_chars(char* f, char* l, T t)             -> enable_if_t<!has_to_chars<T>::value, std::to_chars_result> {
                auto const r = charconv::to_chars(f, l, t);
                return { r.ptr, r.ec };
            }

        template <typename, typename = void>
            struct general {
                static constexpr int    value = 7;
            };
        template <typename T>
            struct general<T, enable_if_t<sizeof(T::general) != 0>> {
                static constexpr T      value = T::general;
            };

        template <typename T>
            inline auto to_chars(char* f, char* l, T t, int precision) noexcept
                -> enable_if_t< has_to_chars_p<T>::value, std::to_chars_result>
            {
#               if defined(_MSC_VER) && _MSC_VER <= 1921
                    return std::to_chars(f, l, t);
#               else
                    return std::to_chars(f, l, t, general<std::chars_format>::value, precision);
#               endif
            }
        template <typename T>
            inline auto to_chars(char* f, char* l, T t, int) noexcept
                -> enable_if_t<!has_to_chars_p<T>::value &&  has_to_chars<T>::value, std::to_chars_result>
            {
                return std::to_chars(f, l, t);
            }
        template <typename T>
            inline auto to_chars(char* f, char* l, T t, int precision)
                -> enable_if_t<!has_to_chars_p<T>::value && !has_to_chars<T>::value, std::to_chars_result>
            {
                auto const r = charconv::to_chars(f, l, t, precision);
                return { r.ptr, r.ec };
            }

#       undef HAS_FUNC_DEF
#   else
        template <typename T>
            inline charconv::from_chars_result from_chars(const char* f, const char* l, T& t, int base) {
                return charconv::from_chars(f, l, t, base);
            }
        template <typename T>
            inline charconv::from_chars_result from_chars(const char* f, const char* l, T& t) {
                return charconv::from_chars(f, l, t);
            }

        template <typename T>
            inline charconv::to_chars_result to_chars(char* f, char* l, T t) {
                return charconv::to_chars(f, l, t);
            }
        template <typename T>
            inline charconv::to_chars_result to_chars(char* f, char* l, T t, int precision) {
                return charconv::to_chars(f, l, t, precision);
            }
#   endif

}}  //cxon::bits <charconv>

namespace cxon { namespace bits { // fundamental type decoding

    template <typename II, typename IsX>
        inline unsigned consume(char* f, const char* l, II& i, II e, IsX is_x) {
            unsigned n = 0;
                for (char c = io::peek(i, e); f != l && is_x(c); c = io::next(i, e), ++f, ++n) *f = c;
            return n;
        }

    inline char32_t oct_to_utf32(const char* b, const char* e) noexcept {
        static constexpr char32_t oct_to_utf32_[] = { 
            /*  0*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 16*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 32*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 48*/ 00, 1, 2, 3, 4, 5, 6, 7,00,00,00,00,00,00,00,00
        };
        CXON_ASSERT(b && b < e, "unexpected");
        char32_t c = 0; 
            for ( ; b != e; ++b) c = (c << 3) | oct_to_utf32_[(unsigned char)*b];
        return c; 
    }

    inline char32_t hex_to_utf32(const char* b, const char* e) noexcept {
        static constexpr char32_t hex_to_utf32_[] = { 
            /*  0*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 16*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 32*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 48*/ 00, 1, 2, 3, 4, 5, 6, 7, 8, 9,00,00,00,00,00,00,
            /* 64*/ 00,10,11,12,13,14,15,00,00,00,00,00,00,00,00,00,
            /* 80*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 96*/ 00,10,11,12,13,14,15,00,00,00,00,00,00,00,00,00
        };
        CXON_ASSERT(b && b < e, "unexpected");
        char32_t c = 0; 
            for ( ; b != e; ++b) c = (c << 4) | hex_to_utf32_[(unsigned char)*b];
        return c; 
    }

#   define CXON_ASS_U(t) if (!is<X>::digit16(io::next(i, e))) return 0xFFFFFFFF; t = io::peek(i, e)
        template <typename X>
            struct esc_to {
                template <typename II>
                    static char32_t utf32(II& i, II e) {
                        switch (io::peek(i, e)) {
                            case '\'': return ++i, '\'';
                            case '\"': return ++i, '\"';
                            case '\?': return ++i, '\?';
                            case '\\': return ++i, '\\';
                            case 'a' : return ++i, '\a';
                            case 'b' : return ++i, '\b';
                            case 'f' : return ++i, '\f';
                            case 'n' : return ++i, '\n';
                            case 'r' : return ++i, '\r';
                            case 't' : return ++i, '\t';
                            case 'v' : return ++i, '\v';
                            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                                char h[3]; // arbitrary length
                                    unsigned const w = consume(h, h + sizeof(h), i, e, is<X>::digit8);
                                        if (!w) return 0xFFFFFFFF;
                                return oct_to_utf32(h, h + w);
                            }
                            case 'x' : {
                                char h[2]; // arbitrary length
                                    unsigned const w = consume(h, h + sizeof(h), ++i, e, is<X>::digit16);
                                        if (!w) return 0xFFFFFFFF;
                                return hex_to_utf32(h, h + w);
                            }
                            case 'u' : {
                                char h[4];
                                    CXON_ASS_U(h[0]); CXON_ASS_U(h[1]); CXON_ASS_U(h[2]); CXON_ASS_U(h[3]);
                                return ++i, hex_to_utf32(h, h + sizeof(h));
                            }
                            case 'U' : {
                                char h[8];
                                    CXON_ASS_U(h[0]); CXON_ASS_U(h[1]); CXON_ASS_U(h[2]); CXON_ASS_U(h[3]);
                                    CXON_ASS_U(h[4]); CXON_ASS_U(h[5]); CXON_ASS_U(h[6]); CXON_ASS_U(h[7]);
                                return ++i, hex_to_utf32(h, h + sizeof(h));
                            }
                            default: return 0xFFFFFFFF;
                        }
                    }
            };
#   undef CXON_ASS_U

#   define CXON_ASS_U(t) if (!is<JSON<X>>::digit16(io::next(i, e))) return 0xFFFFFFFF; t = io::peek(i, e)
        template <typename X>
            struct esc_to<JSON<X>> {
                template <typename II>
                    static char32_t utf32(II& i, II e) {
                        switch (io::peek(i, e)) {
                            case '\"': return ++i, '\"';
                            case '\\': return ++i, '\\';
                            case '/' : return ++i, '/';
                            case 'b' : return ++i, '\b';
                            case 'f' : return ++i, '\f';
                            case 'n' : return ++i, '\n';
                            case 'r' : return ++i, '\r';
                            case 't' : return ++i, '\t';
                            case 'u' : {
                                char h[4];
                                    CXON_ASS_U(h[0]); CXON_ASS_U(h[1]); CXON_ASS_U(h[2]); CXON_ASS_U(h[3]);
                                return ++i, hex_to_utf32(h, h + sizeof(h));
                            }
                            default: return 0xFFFFFFFF;
                        }
                    }
        };
#   undef CXON_ASS_U

    template <typename X, typename II, typename Cx>
        inline char32_t esc_to_utf32(II& i, II e, Cx& cx) {
            char32_t const c32 = esc_to<X>::utf32(i, e);
                if (c32 == 0xFFFFFFFF) return cx|read_error::escape_invalid, 0xFFFFFFFF;
            if (c32 < 0xD800 || c32 > 0xDBFF) return c32;
            // surrogate
                if (io::peek(i, e) != '\\') return cx|read_error::surrogate_invalid, 0xFFFFFFFF;
            char32_t const s32 = (++i, esc_to<X>::utf32(i, e));
                if (s32 < 0xDC00 || s32 > 0xDFFF)
                    return (s32 == 0xFFFFFFFF ? cx|read_error::escape_invalid : cx|read_error::surrogate_invalid), 0xFFFFFFFF;
            return char32_t(0x10000 + (((c32 - 0xD800) << 10) | (s32 - 0xDC00)));
        }

#   define CXON_EXPECT(c) if (!(c)) return cx|read_error::character_invalid, 0xFFFFFFFF

        template <typename X, typename II, typename Cx>
            static char32_t str_to_utf32(II& i, II e, Cx& cx) {
                char32_t const c0 = io::peek(i, e);
                if (c0 != '\\') {
                    if ((c0 & 0x80) == 0)
                        return ++i, c0;
                    if ((c0 & 0xE0) == 0xC0) {
                        char32_t const c1 = io::next(i, e); CXON_EXPECT((c1 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x1F) << 6) | (c1 & 0x3F);
                    }
                    if ((c0 & 0xF0) == 0xE0) {
                        char32_t const c1 = io::next(i, e); CXON_EXPECT((c1 & 0xC0) == 0x80);
                        char32_t const c2 = io::next(i, e); CXON_EXPECT((c2 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
                    }
                    if ((c0 & 0xF8) == 0xF0) {
                        char32_t const c1 = io::next(i, e); CXON_EXPECT((c1 & 0xC0) == 0x80);
                        char32_t const c2 = io::next(i, e); CXON_EXPECT((c2 & 0xC0) == 0x80);
                        char32_t const c3 = io::next(i, e); CXON_EXPECT((c3 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                    }
                    CXON_EXPECT(false);
                }
                return esc_to_utf32<X>(++i, e, cx);
            }

#   undef CXON_EXPECT

    template <typename = void> // instantiate only if used
        static int utf32_to_utf8(char (&t)[4], char32_t c32) noexcept {
            if (c32 < 0x80)  // 0XXX XXXX
                return t[0] = char(c32), 1;
            if (c32 < 0x800) { // 110XXXXX
                t[0] = char(0xC0 | (c32 >> 6));
                t[1] = char(0x80 | (0x3F & c32));
                return 2;
            }
            if (c32 < 0x10000) { // 1110XXXX
                // error: 0xFFFE || 0xFFFF // not a char?
                    if (c32 >= 0xD800 && c32 <= 0xDBFF) return 0;
                t[0] = char(0xE0 | (c32 >> 12));
                t[1] = char(0x80 | (0x3F & (c32 >> 6)));
                t[2] = char(0x80 | (0x3F & c32));
                return 3;
            }
            if (c32 < 0x200000) { // 11110XXX
                    if (c32 > 0x10FFFF) return 0;
                t[0] = char(0xF0 | (c32 >> 18));
                t[1] = char(0x80 | (0x3F & (c32 >> 12)));
                t[2] = char(0x80 | (0x3F & (c32 >> 6)));
                t[3] = char(0x80 | (0x3F & c32));
                return 4;
            }
            return 0;
        }

    template <typename T>
        inline auto is_sign(char c) -> enable_if_t<std::is_signed<T>::value, bool> {
            return c == '-';
        }
    template <typename T>
        constexpr auto is_sign(char) -> enable_if_t<std::is_unsigned<T>::value, bool> {
            return false;
        }

#   define CXON_NEXT() { if (f == l) return -1; *f = c, c = io::next(i, e), ++f; }

        template <typename X, typename T>
            struct number_consumer {
                // integral
                template <typename II, typename N = T>
                    static auto consume(char* f, const char* l, II& i, II e)
                        -> enable_if_t<std::is_integral<N>::value, int>
                    {
                        CXON_ASSERT(f && f < l, "unexpected");
                        int b = 2;
                        char c = io::peek(i, e);
                            if (is_sign<T>(c))              CXON_NEXT()
                            if (c == '0')                   goto trap_oct;
                        // trap_dec:
                               if (!is<X>::digit10(c))      return 0;
                            while ( is<X>::digit10(c))      CXON_NEXT()
                            b = 10;                         goto trap_end;
                        trap_oct:
                            ;                               c = io::next(i, e);
                            if (c == 'x')                   goto trap_hex;
                            if (c == 'b')                   goto trap_bin;
                            if (!is<X>::digit8(c)) {        *f = '0', ++f;
                                                            goto trap_end; }
                            while (is<X>::digit8(c))        CXON_NEXT()
                            b = 8;                          goto trap_end;
                        trap_hex:
                            ;                               c = io::next(i, e);
                               if (!is<X>::digit16(c))      return 0;
                            while ( is<X>::digit16(c))      CXON_NEXT()
                            b = 16;                         goto trap_end;
                        trap_bin:
                            ;                               c = io::next(i, e);
                               if (c != '0' && c != '1')    return 0;
                            while (c == '0' || c == '1')    CXON_NEXT()
                        trap_end:
                            return f != l ? (*f = '\0', b) : 0;
                    }
                // floating point
                template <typename II, typename N = T>
                    static auto consume(char* f, const char* l, II& i, II e)
                        -> enable_if_t<!std::is_integral<N>::value, int>
                    {
                        CXON_ASSERT(f && f < l, "unexpected");
                        char c = io::peek(i, e);
                            if (is_sign<T>(c))          CXON_NEXT()
                            if (c == '.')               goto trap_fraction_0;
                            if (c == 'i')               goto trap_inf;
                            if (c == 'n')               goto trap_nan;
                        //trap_whole:
                               if (!is<X>::digit10(c))  return 0;
                            while ( is<X>::digit10(c))  CXON_NEXT()
                            if (c == '.')               goto trap_fraction_1;
                            if (c == 'e' || c == 'E')   goto trap_exponent;
                            ;                           goto trap_end;
                        trap_fraction_0:
                            ;                           CXON_NEXT()
                               if (!is<X>::digit10(c))  return 0;
                            while ( is<X>::digit10(c))  CXON_NEXT()
                            if (c == 'e' || c == 'E')   goto trap_exponent;
                            ;                           goto trap_end;
                        trap_fraction_1:
                            ;                           CXON_NEXT()
                            while (is<X>::digit10(c))   CXON_NEXT()
                            if (c == 'e' || c == 'E')   goto trap_exponent;
                            ;                           goto trap_end;
                        trap_exponent:
                            ;                           CXON_NEXT()
                            if (c == '-' || c == '+')   CXON_NEXT()
                               if (!is<X>::digit10(c))  return 0;
                            while ( is<X>::digit10(c))  CXON_NEXT()
                            ;                           goto trap_end;
                        trap_inf:
                            CXON_NEXT() if (c != 'n')   return 0;
                            CXON_NEXT() if (c != 'f')   return 0;
                            CXON_NEXT()                 goto trap_end;
                        trap_nan:
                            CXON_NEXT() if (c != 'a')   return 0;
                            CXON_NEXT() if (c != 'n')   return 0;
                            CXON_NEXT()
                        trap_end:
                            return f != l ? (*f = '\0', 1) : 0;
                    }
            };
        template <typename X, typename T>
            struct number_consumer<JSON<X>, T> {
            // integral
            template <typename II, typename N = T>
                static auto consume(char* f, const char* l, II& i, II e)
                    -> enable_if_t<std::is_integral<N>::value, int>
                {
                    CXON_ASSERT(f && f < l, "unexpected");
                    char c = io::peek(i, e);
                        if (is_sign<T>(c))                  CXON_NEXT()
                        if (c == '0') {                     CXON_NEXT()
                                                            goto trap_end; }
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                    trap_end:
                        return f != l ? (*f = '\0', 1) : 0;
                }
            template <typename N = T>
                static auto consume(const char*& i, const char* e)
                    -> enable_if_t<std::is_integral<N>::value, int>
                {
                    io::consume<JSON<X>>(i, e);
                    char c = io::peek(i, e);
                    if (is_sign<T>(c)) goto trap_neg;
                    // trap_pos
                        if (c == '0') return ++i, 1;
                        while (is<JSON<X>>::digit10(c)) c = io::next(i, e);
                        goto trap_end;
                    trap_neg:
                        c = io::next(i, e);
                            if (c == '0' || !is<JSON<X>>::digit10(c)) return 0;
                        while (is<JSON<X>>::digit10(c)) c = io::next(i, e);
                    trap_end:
                        return 1;
                }
            // floating point
            template <typename II, typename N = T>
                static auto consume(char* f, const char* l, II& i, II e)
                    -> enable_if_t<!std::is_integral<N>::value, int>
                {   // as in RFC7159
                    CXON_ASSERT(f && f < l, "unexpected");
                    char c = io::peek(i, e);
                        if (c == '"')                       goto trap_spec_beg;
                        if (is_sign<T>(c))                  CXON_NEXT()
                        if (c == '0')                       goto trap_zero;
                    //trap_whole:
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                        if (c == '.')                       goto trap_fraction;
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_zero:
                        ;                                   CXON_NEXT()
                        if (c == '.')                       goto trap_fraction;
                        ;                                   goto trap_end;
                    trap_fraction:
                        ;                                   CXON_NEXT()
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_exponent:
                        ;                                   CXON_NEXT()
                        if (c == '-' || c == '+')           CXON_NEXT()
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                        ;                                   goto trap_end;
                    trap_spec_beg:
                        ;                                   CXON_NEXT()
                        if (is_sign<T>(c))                  CXON_NEXT()
                        if (c == 'i')                       goto trap_inf;
                        if (c == 'n')                       goto trap_nan;
                        ;                                   return 0;
                    trap_inf:
                        CXON_NEXT() if (c != 'n')           return 0;
                        CXON_NEXT() if (c != 'f')           return 0;
                        ;                                   goto trap_spec_end;
                    trap_nan:
                        CXON_NEXT() if (c != 'a')           return 0;
                        CXON_NEXT() if (c != 'n')           return 0;
                    trap_spec_end:
                        CXON_NEXT() if (c != '"')           return 0;
                        CXON_NEXT()
                    trap_end:
                        return f != l ? (*f = '\0', 1) : 0;
                }
            template <typename N = T>
                static auto consume(const char*& i, const char* e)
                    -> enable_if_t<!std::is_integral<N>::value, int>
                {   // as in RFC7159
                    char c = io::peek(i, e);
                        if (c == '"')                       goto trap_spec_beg;
                        if (is_sign<T>(c))                  c = io::next(i, e);
                        if (c == '0')                       goto trap_zero;
                    //trap_whole:
                            if (!is<JSON<X>>::digit10(c))   return 0;
                        while (is<JSON<X>>::digit10(c))     c = io::next(i, e);
                        if (c == '.')                       goto trap_fraction;
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_zero:
                        ;                                   c = io::next(i, e);
                        if (c == '.')                       goto trap_fraction;
                        ;                                   goto trap_end;
                    trap_fraction:
                        ;                                   c = io::next(i, e);
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    c = io::next(i, e);
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_exponent:
                        ;                                   c = io::next(i, e);
                        if (c == '-' || c == '+')           c = io::next(i, e);
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    c = io::next(i, e);
                        ;                                   goto trap_end;
                    trap_spec_beg:
                        ;                                   c = io::next(i, e);
                        if (is_sign<T>(c))                  c = io::next(i, e);
                        if (c == 'i')                       goto trap_inf;
                        if (c == 'n')                       goto trap_nan;
                        ;                                   return 0;
                    trap_inf:
                        c = io::next(i, e); if (c != 'n')   return 0;
                        c = io::next(i, e); if (c != 'f')   return 0;
                        ;                                   goto trap_spec_end;
                    trap_nan:
                        c = io::next(i, e); if (c != 'a')   return 0;
                        c = io::next(i, e); if (c != 'n')   return 0;
                    trap_spec_end:
                        c = io::next(i, e); if (c != '"')   return 0;
                        ++i;
                    trap_end:
                        return 1;
                }
        };

#   undef CXON_NEXT

    template <typename X, typename T>
        struct number_reader {
            // integral
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(32U)];
                            int const b = number_consumer<X, T>::consume(s, s + sizeof(s), i, e);
                                if (b == -1) return cx|read_error::overflow, rewind(i, o), false;
                                if (b ==  0 || bits::from_chars(s, s + sizeof(s), t, b).ec != std::errc())
                                    return cx|read_error::integral_invalid, rewind(i, o), false;
                            return true;
                    }
                // no optimization for const char* because of numeric bases (0, 0b, 0x)
            // floating point
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<!std::is_integral<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(64U)];
                            int const b = number_consumer<X, T>::consume(s, s + sizeof(s), i, e);
                                if (b == -1) return cx|read_error::overflow, rewind(i, o), false;
                                if (b ==  0 || bits::from_chars(s, s + sizeof(s), t).ec != std::errc())
                                    return cx|read_error::floating_point_invalid, rewind(i, o), false;
                            return true;
                    }
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<!std::is_integral<N>::value, bool>
                    {
                        auto const r = bits::from_chars(i, e, t);
                            if (r.ec != std::errc()) return cx|read_error::floating_point_invalid, false;
                        return i = r.ptr, true;
                    }
        };
    template <typename X, typename T>
        struct number_reader<JSON<X>, T> {
            // integral
                // strict
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(32U)];
                            int const b = number_consumer<JSON<X>, T>::consume(s, s + sizeof(s), i, e);
                                if (b == -1) return cx|read_error::overflow, rewind(i, o), false;
                                if (b ==  0 || bits::from_chars(s, s + sizeof(s), t).ec != std::errc())
                                    return cx|read_error::integral_invalid, rewind(i, o), false;
                            return true;
                    }
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value && X::number::strict, bool>
                    {
                        auto const b = i;
                        if (number_consumer<JSON<X>, T>::consume(i, e)) {
                            if (*b == '0' && i - b == 1) return t = 0, true;
                            auto const r = bits::from_chars(b, i, t);
                            if (r.ec == std::errc() && r.ptr == i) return true;
                        }
                        return i = b, cx|read_error::integral_invalid, false;
                    }
                // not strict
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value && !X::number::strict, bool>
                    {
                        auto const r = bits::from_chars(i, e, t);
                            if (r.ec != std::errc()) return cx|read_error::integral_invalid, false;
                        return i = r.ptr, true;
                    }
            // floating point
                template <typename N>
                    static charconv::from_chars_result from_chars(const char* b, const char* e, N& t) {
                        if (*b == '"') {
                            if (b[1] == '-') {
                                if (b[2] == 'i') return b[3] == 'n' && b[4] == 'f' && b[5] == '"' ? t = -std::numeric_limits<N>::infinity(),
                                    charconv::from_chars_result{ b + 6, std::errc() } : charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                                return b[2] == 'n' && b[3] == 'a' && b[4] == 'n' && b[5] == '"' ? t =  std::numeric_limits<N>::quiet_NaN(),
                                    charconv::from_chars_result{ b + 6, std::errc() } : charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                            }
                            else {
                                if (b[1] == 'i') return b[2] == 'n' && b[3] == 'f' && b[4] == '"' ? t =  std::numeric_limits<N>::infinity(),
                                    charconv::from_chars_result{ b + 5, std::errc() } : charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                                return b[1] == 'n' && b[2] == 'a' && b[3] == 'n' && b[4] == '"' ? t =  std::numeric_limits<N>::quiet_NaN(),
                                    charconv::from_chars_result{ b + 5, std::errc() } : charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                            }
                        }
                        auto const r = bits::from_chars(b, e, t);
                        return { r.ptr, r.ec };
                    }
                // strict
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<!std::is_integral<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(64U)];
                            int const b = number_consumer<JSON<X>, T>::consume(s, s + sizeof(s), i, e);
                                if (b == -1) return cx|read_error::overflow, rewind(i, o), false;
                                if (b ==  0 || from_chars(s, s + sizeof(s), t).ec != std::errc())
                                    return cx|read_error::floating_point_invalid, rewind(i, o), false;
                            return true;
                    }
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<!std::is_integral<N>::value && X::number::strict, bool>
                    {
                        auto const b = i;
                        if (number_consumer<JSON<X>, T>::consume(i, e)) {
                            if (*b == '0' && i - b == 1) return t = 0, true;
                            auto const r = from_chars(b, i, t);
                            if (r.ec == std::errc() && r.ptr == i) return true;
                        }
                        return i = b, cx|read_error::floating_point_invalid, false;
                    }
                // not strict
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<!std::is_integral<N>::value && !X::number::strict, bool>
                    {
                        auto const r = from_chars(i, e, t);
                            if (r.ec != std::errc()) return cx|read_error::floating_point_invalid, false;
                        return i = r.ptr, true;
                    }
        };

    template <typename X, typename T, typename II, typename Cx>
        inline bool number_read(T& t, II& i, II e, Cx& cx) {
            io::consume<X>(i, e);
            return number_reader<X, T>::read(t, i, e, cx);
        }

}}  // cxon::bits fundamental type decoding

namespace cxon { // read, fundamental types

    template <typename X, typename II, typename Cx>
        inline bool read_value(bool& t, II& i, II e, Cx& cx) {
            static_assert(*X::id::pos != *X::id::neg, "boolean literals ambiguous"); // for input-iterator, id must be consumed
            II const o = i;
                char const c = (io::consume<X>(i, e), io::peek(i, e));
                     if (c == *X::id::pos && io::consume<X>(X::id::pos, i, e))  return t = true,  true;
                else if (c == *X::id::neg && io::consume<X>(X::id::neg, i, e))  return t = false, true;
            return cx|read_error::boolean_invalid, bits::rewind(i, o), false;
        }

    template <typename X, typename II, typename Cx>
        inline bool read_value(char& t, II& i, II e, Cx& cx) {
            if (!io::consume<X>(X::string::beg, i, e, cx)) return false;
                II const o = i;
                    char32_t const c32 = bits::str_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF)  return                                      bits::rewind(i, o), false;
                        if (c32 > 0XFF)         return cx|read_error::character_invalid,   bits::rewind(i, o), false;
            return io::consume<X>(X::string::end, i, e, cx) ? (t = char(c32), true) : false;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char16_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char16_t)), bool>
        {
            if (!io::consume<X>(X::string::beg, i, e, cx)) return false;
                II const o = i;
                    char32_t const c32 = bits::str_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF)  return                                      bits::rewind(i, o), false;
                        if (c32 > 0XFFFF)       return cx|read_error::character_invalid,   bits::rewind(i, o), false;
            return io::consume<X>(X::string::end, i, e, cx) ? (t = T(c32), true) : false;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char32_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char32_t)), bool>
        {
            if (!io::consume<X>(X::string::beg, i, e, cx)) return false;
                II const o = i;
                    char32_t const c32 = bits::str_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF) return bits::rewind(i, o), false;
            return io::consume<X>(X::string::end, i, e, cx) ? (t = T(c32), true) : false;
        }

#   define CXON_READ_DEF(T)\
        template <typename X, typename II, typename Cx>\
            inline bool read_value(T& t, II& i, II e, Cx& cx) {\
                return bits::number_read<X>(t, i, e, cx);\
            }
        CXON_READ_DEF(signed char)
        CXON_READ_DEF(unsigned char)
        CXON_READ_DEF(short)
        CXON_READ_DEF(unsigned short)
        CXON_READ_DEF(int)
        CXON_READ_DEF(unsigned int)
        CXON_READ_DEF(long)
        CXON_READ_DEF(unsigned long)
        CXON_READ_DEF(long long)
        CXON_READ_DEF(unsigned long long)
        CXON_READ_DEF(float)
        CXON_READ_DEF(double)
        CXON_READ_DEF(long double)
#   undef CXON_READ_DEF

    template <typename X, typename II, typename Cx>
        inline bool read_value(std::nullptr_t& t, II& i, II e, Cx& cx) {
            II const o = i;
                if (io::consume<X>(X::id::nil, i, e)) return t = nullptr, true;
            return cx|read_error::unexpected, bits::rewind(i, o), false;
        }

}   // cxon read, fundamental types

namespace cxon { namespace bits { // key quoting

    template <typename T> struct is_quoted : std::false_type {};
#   define CXON_QUOTED(T)\
        template <size_t N>         struct is_quoted<T[N]>                          : std::true_type  {};\
        template <size_t N>         struct is_quoted<const T[N]>                    : std::true_type  {};\
        template <>                 struct is_quoted<T*>                            : std::true_type  {};\
        template <>                 struct is_quoted<const T*>                      : std::true_type  {};\
        template <typename ...R>    struct is_quoted<std::basic_string<T, R...>>    : std::true_type  {};
        CXON_QUOTED(char)
        CXON_QUOTED(char16_t)
        CXON_QUOTED(char32_t)
        CXON_QUOTED(wchar_t)
#   undef CXON_QUOTED

    template <typename S> struct UQKEY : S { using X = S; };

}}  // cxon::bits key quoting

namespace cxon { namespace bits { // string quoting

    template <typename X>
        struct is_str {
            static           bool end(char c) noexcept  { return c == X::string::end; }
            static constexpr bool esc(char)             { return false; }
        };
    template <typename X, template <typename> class S>
        struct is_str<S<UQKEY<X>>> {
            static bool end(char c) noexcept { return is<S<X>>::space(c) || c == S<X>::map::div; }
            static bool esc(char c) noexcept { return end(c); }
        };

    template <typename X>
        struct consume_str {
            template <typename II, typename Cx> static bool      beg(II& i, II e, Cx& cx) { return io::consume<X>(X::string::beg, i, e, cx); }
            template <typename II, typename Cx> static bool      end(II& i, II e, Cx& cx) { return io::consume<X>(X::string::end, i, e, cx); }
            template <typename II, typename Cx> static char32_t  chr(II& i, II e, Cx& cx) {
                return  str_to_utf32<X>(i, e, cx);
            }
        };
    template <typename X, template <typename> class S>
        struct consume_str<S<UQKEY<X>>> {
            template <typename II, typename Cx> static constexpr bool    beg(II&, II, Cx&)          { return true; }
            template <typename II, typename Cx> static constexpr bool    end(II&, II, Cx&)          { return true; }
            template <typename II, typename Cx> static char32_t          chr(II& i, II e, Cx& cx)   {
                if (io::peek(i, e) == '\\') {
                        char const c = io::next(i, e);
                        return!is_str<S<UQKEY<X>>>::esc(c) ? esc_to_utf32<S<X>>(i, e, cx) : (++i, char32_t(c));
                }
                return  str_to_utf32<S<X>>(i, e, cx);
            }
        };

}}  // cxon::bits string quoting

namespace cxon { namespace bits { // char arrays

    template <typename X, typename II, typename Cx> // TODO: common with std::basic_string?
        inline bool array_char_read(char*& t, const char* te, II& i, II ie, Cx& cx) {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                char b[4]; int const n = utf32_to_utf8(b, c32);
                    if (n == 0 || t + n > te) return cx|read_error::overflow, false;
                std::copy_n(b, n, t);
            return t += n, true;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto array_char_read(T*& t, const T* te, II& i, II ie, Cx& cx)
            -> enable_if_t<std::is_same<T, char16_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char16_t)), bool>
        {
            II const o = i;
                char32_t c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                if (c32 > 0xFFFF) {
                    c32 -= 0x10000;
                    *t = T(0xD800 | (c32 >> 10));   if (++t == te) return cx|read_error::overflow, false;
                    *t = T(0xDC00 | (c32 & 0x3FF));
                }
                else {
                    *t = T(c32);
                }
            return ++t, true;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto array_char_read(T*& t, const T*, II& i, II ie, Cx& cx)
            -> enable_if_t<std::is_same<T, char32_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char32_t)), bool>
        {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
            return *t = T(c32), ++t, true;
        }

    template <typename X, typename T, typename II, typename Cx>
        inline bool array_read(T* t, const T* te, II& i, II ie, Cx& cx) {
            II const o = i;
                if (!consume_str<X>::beg(i, ie, cx)) return false;
                    while (t < te) {
                        if (!is<X>::real(io::peek(i, ie)))          return consume_str<X>::end(i, ie, cx);
                        if (is_str<X>::end(io::peek(i, ie)))        return *t = '\0', consume_str<X>::end(i, ie, cx);
                        if (!array_char_read<X>(t, te, i, ie, cx))  return cx.ec == read_error::overflow ? (rewind(i, o), false) : false;
                    }
                return !is_str<X>::end(io::peek(i, ie)) ?
                    (cx|read_error::overflow, rewind(i, o), false) :
                    consume_str<X>::end(i, ie, cx)
                ;
        }

    template <typename X>
        struct array {
            template <typename T, typename II, typename Cx>
                static bool read(T* t, const T* te, II& i, II ie, Cx& cx) {
                    io::consume<X>(i, ie);
                    return array_read<X>(t, te, i, ie, cx);
                }
        };
    template <typename X, template <typename> class S>
        struct array<S<UQKEY<X>>> {
            template <typename T, typename II, typename Cx>
                static bool read(T* t, const T* te, II& i, II ie, Cx& cx) {
                    io::consume<S<X>>(i, ie);
                    return io::peek(i, ie) == S<X>::string::beg ?
                        array_read<S<X>>(t, te, i, ie, cx) :
                        array_read<S<UQKEY<X>>>(t, te, i, ie, cx)
                    ;
                }
        };

    template <typename X, typename T, typename II, typename Cx>
        inline bool pointer_read(T*& t, II& i, II e, Cx& cx) {
            io::consume<X>(i, e);
            if (io::peek(i, e) == *X::id::nil && io::consume<X>(X::id::nil, i, e)) return true;
                if (!consume_str<X>::beg(i, e, cx)) return false;
            auto a = allocator::value(cx.ps, std::allocator<T>());
                using al = std::allocator_traits<decltype(a)>;
            T *b = al::allocate(a, 4), *p = b; T* be = b + 4;
            for ( ; ; ) {
                if (p + 4 > be) {
                    T *const n = al::allocate(a, 2 * (be - b));
                        std::copy_n(b, p - b, n);
                    p = n + (p - b);
                    al::deallocate(a, b, be - b);
                    be = n + 2 * (be - b), b = n;
                }
                if (!is<X>::real(io::peek(i, e)))           goto err;
                if (is_str<X>::end(io::peek(i, e)))         return *p = '\0', t = b, consume_str<X>::end(i, e, cx);
                if (!array_char_read<X>(p, be, i, e, cx))   goto err;
            }
            err: return al::deallocate(a, b, be - b), cx|read_error::unexpected, false;
        }

}}  // cxon::bits char arrays

namespace cxon { // read, compound types

    template <typename X, typename T>
        struct read<X, T*> {
            template <typename II, typename Cx>
                static bool value(T*& t, II& i, II e, Cx& cx) {
                    io::consume<X>(i, e);
                    if (io::peek(i, e) == *X::id::nil) { // TODO: not correct as T may start with *X::id::nil (e.g. 'nan'), but it's supposed to be used in structs anyway?
                        II const o = i;
                            if (!io::consume<X>(X::id::nil, i, e)) return cx|read_error::unexpected, bits::rewind(i, o), false;
                        return t = nullptr, true;
                    }
                    auto a = allocator::value(cx.ps, std::allocator<T>());
                        using al = std::allocator_traits<decltype(a)>;
                    T *const n = al::allocate(a, 1); al::construct(a, n);
                        if (!read_value<X>(*n, i, e, cx))
                            return al::destroy(a, n), al::deallocate(a, n, 1), false;
                    return t = n, true;
                }
        };

    template <typename X, typename T, size_t N>
        struct read<X, T[N]> {
            template <typename II, typename Cx>
                static bool value(T (&t)[N], II& i, II e, Cx& cx) {
                    II const o = i;
                        size_t p = 0;
                    return container::read<X, list<X>>(i, e, cx, [&] {
                            if (p == N) return cx|read_error::overflow, bits::rewind(i, o), false;
                        return read_value<X>(t[p++], i, e, cx);
                    });
                }
        };

#   define CXON_ARRAY(T)\
        template <typename X, size_t N>\
            struct read<X, T[N]> {\
                template <typename II, typename Cx>\
                    static bool value(T (&t)[N], II& i, II e, Cx& cx)     { return bits::array<X>::read(t, t + N, i, e, cx); }\
            };
        CXON_ARRAY(char)
        CXON_ARRAY(char16_t)
        CXON_ARRAY(char32_t)
        CXON_ARRAY(wchar_t)
#   undef CXON_ARRAY

#   define CXON_POINTER(T)\
        template <typename X>\
            struct read<X, const T*> {\
                template <typename II, typename Cx>\
                    static bool value(const T*& t, II& i, II e, Cx& cx)   { return read_value<X>((T*&)t, i, e, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

#   define CXON_POINTER(T)\
        template <typename X>\
            struct read<X, T*> {\
                template <typename II, typename Cx>\
                    static bool value(T*& t, II& i, II e, Cx& cx)         { return bits::pointer_read<X>(t, i, e, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

}   // cxon read, compound types

namespace cxon { namespace bits { // tuple read

    template <typename X, typename T, unsigned N, unsigned L>
        struct tuple_read {
            template <typename II, typename Cx>
                static bool value(T& t, II& i, II e, Cx& cx) {
                    if (!read_value<X>(std::get<N>(t), i, e, cx)) return false;
                    if (N + 1 != L) if (!io::consume<X>(X::list::sep, i, e, cx)) return false;
                    return tuple_read<X, T, N + 1, L>::value(t, i, e, cx);
                }
        };

    template <typename X, typename T, unsigned N>
        struct tuple_read<X, T, N, N> {
            template <typename II, typename Cx>
                static constexpr bool value(T&, II&, II, Cx&) { return true; }
        };

}}  // cxon::bits tuple read

namespace cxon {  // key read

    namespace bits {

        template <typename X> struct key_read;

        template <typename X, template <typename> class S>
            struct key_read<S<X>> {
                template <typename T, typename II, typename Cx, typename E = S<X>>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                    {
                        return read_value<S<UQKEY<X>>>(t, i, e, cx);
                    }
                template <typename T, typename II, typename Cx, typename E = S<X>>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value || !E::map::unquoted_keys, bool>
                    {
                        return read_value<E>(t, i, e, cx);
                    }
            };
        template <typename X>
            struct key_read<JSON<X>> {
                template <typename T, typename II, typename Cx, typename E = JSON<X>>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                    {
                        return read_value<JSON<UQKEY<X>>>(t, i, e, cx);
                    }
                template <typename T, typename II, typename Cx, typename E = JSON<X>>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value && !E::map::unquoted_keys, bool>
                    {
                        return read_value<E>(t, i, e, cx);
                    }
                template <typename T, typename II, typename Cx, typename E = JSON<X>>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                    {
                        return read_value<E>(t, i, e, cx);
                    }
                template <typename T, typename II, typename Cx, typename E = JSON<X>>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value && !E::map::unquoted_keys, bool>
                    {
                        return  io::consume<E>(E::string::beg, i, e, cx) &&
                                    read_value<E>(t, i, e, cx) &&
                                io::consume<E>(E::string::end, i, e, cx)
                        ;
                    }
            };

    }   // bits key read

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx) {
            return bits::key_read<X>::value(t, i, e, cx) && io::consume<X>(X::map::div, i, e, cx);
        }

}   // cxon key read

namespace cxon { namespace bits { // std::basic_string

    template <typename X, typename ...R, typename II, typename Cx> // TODO: common with arrays?
        inline bool basic_string_char_read(std::basic_string<char, R...>& t, II& i, II e, Cx& cx) {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                char b[4]; t.append(b, utf32_to_utf8(b, c32));
            return true;
        }
    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_char_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char16_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char16_t)), bool>
        {
            II const o = i;
                char32_t c32 = consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                if (c32 > 0xFFFF) {
                    c32 -= 0x10000;
                    t.push_back(char16_t(0xD800 | (c32 >> 10)));
                    t.push_back(char16_t(0xDC00 | (c32 & 0x3FF)));
                }
                else {
                    t.push_back(char16_t(c32));
                }
            return true;
        }
    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_char_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char32_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char32_t)), bool>
        {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
            return t.push_back(T(c32)), true;
        }

    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline bool basic_string_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
            if (!consume_str<X>::beg(i, e, cx)) return false;
                for (char c = io::peek(i, e); is<X>::real(c); c = io::peek(i, e)) {
                    if (is_str<X>::end(c))                          return consume_str<X>::end(i, e, cx);
                    if (!basic_string_char_read<X>(t, i, e, cx))   return false;
                }
            return cx|read_error::unexpected, false;
        }

}}  // cxon::bits std::basic_string

namespace cxon { // read, library types

    template <typename X, typename T, typename ...R>
        struct read<X, std::basic_string<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
                    return bits::basic_string_read<X>(t, i, e, cx);
                }
        };
    template <typename X, template <typename> class S, typename T, typename ...R>
        struct read<S<bits::UQKEY<X>>, std::basic_string<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
                    io::consume<S<X>>(i, e);
                    return io::peek(i, e) == S<X>::string::beg ?
                        bits::basic_string_read<S<X>>(t, i, e, cx) :
                        bits::basic_string_read<S<bits::UQKEY<X>>>(t, i, e, cx)
                    ;
                }
        };

    template <typename X, typename H, typename ...T>
        struct read<X, std::tuple<H, T...>> {
            template <typename II, typename Cx>
                static bool value(std::tuple<H, T...>& t, II& i, II e, Cx& cx) {
                    using U = std::tuple<H, T...>;
                    return  io::consume<X>(X::list::beg, i, e, cx) &&
                            bits::tuple_read<X, U, 0, std::tuple_size<U>::value>::value(t, i, e, cx) &&
                            io::consume<X>(X::list::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename F, typename S>
        struct read<X, std::pair<F, S>> {
            template <typename II, typename Cx>
                static bool value(std::pair<F, S>& t, II& i, II e, Cx& cx) {
                    return  io::consume<X>(X::list::beg, i, e, cx) &&
                            read_value<X>(t.first, i, e, cx) && io::consume<X>(X::list::sep, i, e, cx) && read_value<X>(t.second, i, e, cx) &&
                            io::consume<X>(X::list::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename T>
        struct read<X, std::valarray<T>> {
            template <typename II, typename Cx>
                static bool value(std::valarray<T>& t, II& i, II e, Cx& cx) { // no, it sucks
                    std::valarray<T> v(4);
                    size_t p = 0;
                    bool const r = container::read<X, list<X>>(i, e, cx, [&] {
                        if (p >= v.size()) {
                            std::valarray<T> n(std::move(v));
                            v.resize(p + p);
                                for (size_t i = 0; i != p; ++i) v[i] = n[i];
                        }
                        return read_value<X>(v[p], i, e, cx) ? (++p, true) : false;
                    });
                        if (!r) return false;
                    t.resize(p);
                        for (size_t j = 0; j != p; ++j) t[j] = v[j];
                    return true;
                }
        };

    template <typename X, typename T, size_t N>
        struct read<X, std::array<T, N>> {
            template <typename II, typename Cx>
                static bool value(std::array<T, N>& t, II& i, II e, Cx& cx) {
                    II const o = i;
                        size_t p = 0;
                    return container::read<X, list<X>>(i, e, cx, [&] {
                            if (p == N) return cx|read_error::overflow, bits::rewind(i, o), false;
                        return read_value<X>(t[p++], i, e, cx);
                    });
                }
        };
    template <typename X, typename T>
        struct read<X, std::array<T, 0>> {
            template <typename II, typename Cx>
                static bool value(std::array<T, 0>&, II& i, II e, Cx& cx) {
                    return io::consume<X>(X::list::beg, i, e, cx) && io::consume<X>(X::list::end, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<X, std::queue<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::queue<T, R...>& t, II& i, II e, Cx& cx) {
                    return container::read<X, list<X>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace();
                            return read_value<X>(t.back(), i, e, cx);
#                       else
                            return read_value<X>(t.emplace(), i, e, cx);
#                       endif
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<X, std::priority_queue<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::priority_queue<T, R...>& t, II& i, II e, Cx& cx) {
                    return container::read<X, list<X>>(i, e, cx, [&] {
                        T o{};
                        if (read_value<X>(o, i, e, cx)) {
                            t.emplace(std::move(o));
                            return true;
                        }
                        return false;
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<X, std::stack<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::stack<T, R...>& t, II& i, II e, Cx& cx) {
                    return container::read<X, list<X>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace();
                            return read_value<X>(t.top(), i, e, cx);
#                       else
                            return read_value<X>(t.emplace(), i, e, cx);
#                       endif
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<X, std::deque<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::deque<T, R...>& t, II& i, II e, Cx& cx) {
                    return container::read<X, list<X>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace_back();
                            return read_value<X>(t.back(), i, e, cx);
#                       else
                            return read_value<X>(t.emplace_back(), i, e, cx);
#                       endif
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<X, std::list<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::list<T, R...>& t, II& i, II e, Cx& cx) {
                    return container::read<X, list<X>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace_back();
                            return read_value<X>(t.back(), i, e, cx);
#                       else
                            return read_value<X>(t.emplace_back(), i, e, cx);
#                       endif
                    });
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<X, std::forward_list<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::forward_list<T, R...>& t, II& i, II e, Cx& cx) {
                    bool const ok = container::read<X, list<X>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace_front();
                            return read_value<X>(t.front(), i, e, cx);
#                       else
                            return read_value<X>(t.emplace_front(), i, e, cx);
#                       endif
                    });
                    if (ok) t.reverse();
                    return ok;
                }
        };

    template <typename X, typename T, typename ...R>
        struct read<X, std::vector<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::vector<T, R...>& t, II& i, II e, Cx& cx) {
                    return container::read<X, list<X>>(i, e, cx, [&] {
#                       if __cplusplus < 201703L
                            t.emplace_back();
                            return read_value<X>(t.back(), i, e, cx);
#                       else
                            return read_value<X>(t.emplace_back(), i, e, cx);
#                       endif
                    });
                }
        };

    namespace bits {

        template <typename X, typename S, typename II, typename Cx>
            inline bool read_set(S& t, II& i, II e, Cx& cx) {
                return container::read<X, list<X>>(i, e, cx, [&] {
                    typename S::value_type o{};
                    if (read_value<X>(o, i, e, cx)) {
                        t.emplace(std::move(o));
                        return true;
                    }
                    return false;
                });
            }

    }
#   define CXON_SET(S)\
        template <typename X, typename T, typename ...R>\
            struct read<X, S<T, R...>> {\
                template <typename II, typename Cx>\
                    static bool value(S<T, R...>& t, II& i, II e, Cx& cx) {\
                        return bits::read_set<X>(t, i, e, cx);\
                    }\
            };
        CXON_SET(std::set)
        CXON_SET(std::multiset)
        CXON_SET(std::unordered_set)
        CXON_SET(std::unordered_multiset)
#   undef CXON_SET

    namespace bits {

        template <typename X, typename M, typename II, typename Cx>
            inline bool read_map(M& t, II& i, II e, Cx& cx) {
                return container::read<X, map<X>>(i, e, cx, [&] {
                    typename M::key_type k{}; typename M::mapped_type v{};
                    if (read_key<X>(k, i, e, cx) && read_value<X>(v, i, e, cx)) {
                        t.emplace(std::move(k), std::move(v));
                        return true;
                    }
                    return false;
                });
            }

    }
#   define CXON_MAP(M)\
        template <typename X, typename K, typename V, typename ...R>\
            struct read<X, M<K, V, R...>> {\
                template <typename II, typename Cx>\
                    static bool value(M<K, V, R...>& t, II& i, II e, Cx& cx) {\
                        return bits::read_map<X>(t, i, e, cx);\
                    }\
            };
        CXON_MAP(std::map)
        CXON_MAP(std::multimap)
        CXON_MAP(std::unordered_map)
        CXON_MAP(std::unordered_multimap)
#   undef CXON_MAP

}   // cxon read, library types

namespace cxon { namespace bits { // fundamental type encoding

    template <typename X, typename T>
        struct encode;

    template <typename X>
        struct encode<X, char> {
            template <typename O, typename Cx>
                static bool value(O& o, char c, Cx& cx) {
                    static constexpr char const* encode_[] = {
                        /*  0*/  "\\0"  , "\\1"  , "\\2"  , "\\3"  , "\\4"  , "\\5"  , "\\6"  , "\\7",
                        /*  8*/  "\\b"  , "\\t"  , "\\n"  , "\\v"  , "\\f"  , "\\r"  , "\\16" , "\\17",
                        /* 16*/  "\\20" , "\\21" , "\\22" , "\\23" , "\\24" , "\\25" , "\\26" , "\\27",
                        /* 24*/  "\\30" , "\\31" , "\\32" , "\\33" , "\\34" , "\\35" , "\\36" , "\\37",
                        /* 32*/  " "    , "!"    , "\\\"" , "#"    , "$"    , "%"    , "&"    , "'"
                    };
                    if ('"' != X::string::end && X::string::end == c)   return io::poke<X>(o, '\\', cx) && io::poke<X>(o, c, cx);
                    else if (X::string::end != '"' && '"' == c)         return io::poke<X>(o, c, cx);
                    else if (0 <= c && c <= 39)                         return io::poke<X>(o, encode_[(unsigned char)c], cx);
                    else if (c == '\\')                                 return io::poke<X>(o, "\\\\", cx);
                    else                                                return io::poke<X>(o, c, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool value(O& o, II i, II, Cx& cx) {
                    return value(o, *i, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool range(O& o, II i, II e, Cx& cx) {
                    for ( ; i != e && value(o, *i, cx); ++i) ;
                    return i == e;
                }
        };
    template <typename X>
        struct encode<JSON<X>, char> {
            template <typename O, typename Cx>
                static bool value(O& o, char c, Cx& cx) {
                    static constexpr char const*const encode_[] = {
                        /*  0*/  "\\u0000", "\\u0001", "\\u0002", "\\u0003", "\\u0004", "\\u0005", "\\u0006", "\\u0007",
                        /*  8*/  "\\b"    , "\\t"    , "\\n"    , "\\u000b", "\\f"    , "\\r"    , "\\u000e", "\\u000f",
                        /* 16*/  "\\u0010", "\\u0011", "\\u0012", "\\u0013", "\\u0014", "\\u0015", "\\u0016", "\\u0017",
                        /* 24*/  "\\u0018", "\\u0019", "\\u001a", "\\u001b", "\\u001c", "\\u001d", "\\u001e", "\\u001f",
                        /* 32*/  " "      , "!"      , "\\\""   , "#"      , "$"      , "%"      , "&"      , "'"
                    };
                    if (0 <= c && c <= 39)  return io::poke<JSON<X>>(o, encode_[(unsigned char)c], cx);
                    else if (c == '\\')     return io::poke<JSON<X>>(o, "\\\\", cx);
                    else                    return io::poke<JSON<X>>(o, c, cx);
                }
            template <typename O, typename II, typename Cx, typename S = X>
                static auto value(O& o, II i, II, Cx& cx)      -> enable_if_t<!S::strict_js, bool> {
                    return value(o, *i, cx);
                }
            template <typename O, typename II, typename Cx, typename S = X>
                // escape U+2028 LINE SEPARATOR and U+2029 PARAGRAPH SEPARATOR are invalid JavaScript
                static auto value(O& o, II& i, II e, Cx& cx)   -> enable_if_t< S::strict_js, bool> {
                    if (*i == '\xE2') {
                        ++i; CXON_ASSERT(i != e, "unexpected");
                        if (*i == '\x80') {
                            ++i; CXON_ASSERT(i != e, "unexpected");
                            switch (*i) {
                                case '\xA8':    return io::poke<JSON<X>>(o, "\\u2028", cx);
                                case '\xA9':    return io::poke<JSON<X>>(o, "\\u2029", cx);
                                default:        return value(o, '\xE2', cx) && value(o, '\x80', cx) && value(o, *i, cx); } }
                        else                    return value(o, '\xE2', cx) && value(o, *i, cx); }
                    else                        return value(o, *i, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool range(O& o, II i, II e, Cx& cx) {
                    for ( ; i != e && value(o, i, e, cx); ++i) ;
                    return i == e;
                }
        };

    template <typename X>
        struct encode<X, char16_t> {
            template <typename O, typename Cx>
                static bool value(O& o, char16_t c, Cx& cx) {
                    CXON_ASSERT(c < 0xD800 || c > 0xDBFF, "unexpected surrogate");
                    return encode<X, char32_t>::value(o, c, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool value(O& o, II& i, II e, Cx& cx) {
                    char32_t c32 = *i;
                        if (c32 >= 0xD800 && c32 <= 0xDBFF) { // surrogate
                            ++i;                        CXON_ASSERT(i != e, "invalid surrogate");
                            char32_t const s32 = *i;    CXON_ASSERT(s32 >= 0xDC00 && s32 <= 0xDFFF, "invalid surrogate");
                            c32 = char32_t(0x10000 + (((c32 - 0xD800) << 10) | (s32 - 0xDC00)));
                        }
                    return encode<X, char32_t>::value(o, c32, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool range(O& o, II i, II e, Cx& cx) {
                    for ( ; i != e && value(o, i, e, cx); ++i) ;
                    return i == e;
                }
        };

    template <typename X>
        struct encode<X, char32_t> {
            template <typename O, typename Cx>
                static bool value(O& o, char32_t c, Cx& cx) {
                    if (c > 0x7F) {
                            char b[4]; int const n = bits::utf32_to_utf8(b, c);
                            return io::poke<X>(o, b, n, cx);
                    }
                    else    return encode<X, char>::value(o, char(c), cx);
                }
            template <typename O, typename II, typename Cx>
                static bool value(O& o, II i, II, Cx& cx) {
                    return value(o, *i, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool range(O& o, II i, II e, Cx& cx) {
                    for ( ; i != e && value(o, *i, cx); ++i) ;
                    return i == e;
                }
        };
    template <typename X>
        struct encode<JSON<X>, char32_t> {
            template <typename O, typename Cx, typename S = X>
                static auto value(O& o, char32_t c, Cx& cx) -> enable_if_t<!S::strict_js, bool> {
                    if (c > 0x7F) {
                            char b[4]; int const n = bits::utf32_to_utf8(b, c);
                            return io::poke<JSON<X>>(o, b, n, cx);
                    }
                    else    return encode<JSON<X>, char>::value(o, char(c), cx);
                }
            template <typename O, typename Cx, typename S = X>
                static auto value(O& o, char32_t c, Cx& cx) -> enable_if_t< S::strict_js, bool> {
                    if (c > 0x7F) {
                            if (c == 0x2028) return io::poke<JSON<X>>(o, "\\u2028", cx);
                            if (c == 0x2029) return io::poke<JSON<X>>(o, "\\u2029", cx);
                            char b[4]; int const n = bits::utf32_to_utf8(b, c);
                            return io::poke<JSON<X>>(o, b, n, cx);
                    }
                    else    return encode<JSON<X>, char>::value(o, char(c), cx);
                }
            template <typename O, typename II, typename Cx>
                static bool value(O& o, II i, II, Cx& cx) {
                    return value(o, *i, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool range(O& o, II i, II e, Cx& cx) {
                    for ( ; i != e && value(o, *i, cx); ++i) ;
                    return i == e;
                }
        };

    template <typename X>
        struct encode<X, wchar_t> {
            template <typename O, typename Cx, typename T = wchar_t>
                static auto value(O& o, T c, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char16_t), bool> {
                    return encode<X, char16_t>::value(o, char16_t(c), cx);
                }
            template <typename O, typename Cx, typename T = wchar_t>
                static auto value(O& o, T c, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char32_t), bool> {
                    return encode<X, char32_t>::value(o, char32_t(c), cx);
                }
            template <typename O, typename Cx, typename T = wchar_t, typename II>
                static auto value(O& o, II i, II e, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char16_t), bool> {
                    return encode<X, char16_t>::value(o, i, e, cx);
                }
            template <typename O, typename Cx, typename T = wchar_t, typename II>
                static auto value(O& o, II i, II e, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char32_t), bool> {
                    return encode<X, char32_t>::value(o, i, e, cx);
                }
            template <typename O, typename Cx, typename T = wchar_t, typename II>
                static auto range(O& o, II i, II e, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char16_t), bool> {
                    return encode<X, char16_t>::range(o, i, e, cx);
                }
            template <typename O, typename Cx, typename T = wchar_t, typename II>
                static auto range(O& o, II i, II e, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char32_t), bool> {
                    return encode<X, char32_t>::range(o, i, e, cx);
                }
        };

    template <typename X, typename T, typename O, typename Cx>
        inline bool character_write(O& o, T t, Cx& cx) {
            return  io::poke<X>(o, X::string::beg, cx) &&
                        encode<X, T>::value(o, t, cx) &&
                    io::poke<X>(o, X::string::end, cx)
            ;
        }

    template <typename X>
        struct opqt {
            static constexpr std::nullptr_t const beg = nullptr;
            static constexpr std::nullptr_t const end = nullptr;
        };
    template <typename X>
        struct opqt<JSON<X>> : JSON<X>::string {};

    template <typename X, typename T, typename O, typename Cx>
        inline auto number_write(O& o, const T& t, Cx& cx) -> enable_if_t<std::is_integral<T>::value, bool> {
            char s[std::numeric_limits<T>::digits10 + 3];
            auto const r = bits::to_chars(s, s + sizeof(s) / sizeof(char), t);
                if (r.ec != std::errc()) return cx|write_error::argument_invalid, false;
            return io::poke<X>(o, s, r.ptr - s, cx);
        }

    template <typename X, typename T, typename O, typename Cx>
        inline auto number_write(O& o, const T& t, Cx& cx) -> enable_if_t<std::is_floating_point<T>::value, bool> {
            if (std::isinf(t)) {
                if (!io::poke<X>(o, opqt<X>::beg, cx)) return false;
                if (std::signbit(t) && !io::poke<X>(o, '-', cx)) return false;
                return io::poke<X>(o, "inf", cx) && io::poke<X>(o, opqt<X>::end, cx);
            }
            if (std::isnan(t))
                return io::poke<X>(o, opqt<X>::beg, cx) && io::poke<X>(o, "nan", cx) && io::poke<X>(o, opqt<X>::end, cx);
            CXON_ASSERT(std::isfinite(t), "unexpected");
            char s[std::numeric_limits<T>::max_digits10 * 2];
            auto const r = bits::to_chars(s, s + sizeof(s) / sizeof(char), t,
                                            fp_precision::constant<prms_type<Cx>>(std::numeric_limits<T>::max_digits10));
                if (r.ec != std::errc()) return cx|write_error::argument_invalid, false;
            return io::poke<X>(o, s, r.ptr - s, cx);
        }

}}  // cxon::bits fundamental type encoding

namespace cxon { // write, fundamental types

    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, bool t, Cx& cx) {
            return io::poke<X>(o, t ? X::id::pos : X::id::neg, cx);
        }

    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, char t, Cx& cx)        { return bits::character_write<X>(o, t, cx); }
    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, char16_t t, Cx& cx)    { return bits::character_write<X>(o, t, cx); }
    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, char32_t t, Cx& cx)    { return bits::character_write<X>(o, t, cx); }
    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, wchar_t t, Cx& cx)     { return bits::character_write<X>(o, t, cx); }

#   define CXON_WRITE_DEF(T)\
        template <typename X, typename O, typename Cx>\
            inline bool write_value(O& o, const T& t, Cx& cx) {\
                return bits::number_write<X>(o, t, cx);\
            }
        CXON_WRITE_DEF(signed char)
        CXON_WRITE_DEF(unsigned char)
        CXON_WRITE_DEF(short)
        CXON_WRITE_DEF(unsigned short)
        CXON_WRITE_DEF(int)
        CXON_WRITE_DEF(unsigned int)
        CXON_WRITE_DEF(long)
        CXON_WRITE_DEF(unsigned long)
        CXON_WRITE_DEF(long long)
        CXON_WRITE_DEF(unsigned long long)
        CXON_WRITE_DEF(float)
        CXON_WRITE_DEF(double)
        CXON_WRITE_DEF(long double)
#   undef CXON_WRITE_DEF

    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, std::nullptr_t, Cx& cx) {
            return io::poke<X>(o, X::id::nil, cx);
        }

}   // cxon write, fundamental types

namespace cxon { namespace bits { // string quoting

    template <typename X>
        struct str {
            static constexpr char                   beg = X::string::beg;
            static constexpr char                   end = X::string::end;
        };
    template <typename X, template <typename> class S>
        struct str<S<UQKEY<X>>> {
            static constexpr std::nullptr_t         beg = nullptr;
            static constexpr std::nullptr_t         end = nullptr;
        };

}}  // cxon::bits string quoting

namespace cxon { namespace bits { // char arrays

    template <typename T>
        inline size_t ptrlen(const T* t) noexcept {
            const T* e = t;
                while (*e) ++e;
            return e - t;
        }

    template <typename X, typename O, typename T, typename Cx>
        inline bool array_write(O& o, const T* t, const T* te, Cx& cx) {
            if (!io::poke<X>(o, X::string::beg, cx)) return false;
                if (*(te - 1) == T(0)) --te;
            return encode<X, T>::range(o, t, te, cx) && io::poke<X>(o, X::string::end, cx);
        }

    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, size_t s, Cx& cx) {
            return  io::poke<X>(o, X::string::beg, cx) &&
                        encode<X, T>::range(o, t, t + s, cx) &&
                    io::poke<X>(o, X::string::end, cx)
            ;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, Cx& cx) {
            return t ?
                pointer_write<X>(o, t, ptrlen(t), cx) :
                io::poke<X>(o, X::id::nil, cx)
            ;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool uqkey_pointer_write(O& o, const T* t, size_t s, Cx& cx) {
            for (auto e = t + s; t != e; ++t) {
                switch (*t) {
                    case ' ':           if (!io::poke<X>(o, '\\', cx) || !io::poke<X>(o, ' ', cx))            return false;   break;
                    case '"':           if (!io::poke<X>(o, '"', cx))                                          return false;   break;
                    case '\'':          if (!io::poke<X>(o, '\'', cx))                                         return false;   break;
                    case X::map::div:   if (!io::poke<X>(o, '\\', cx) || !io::poke<X>(o, X::map::div, cx))    return false;   break;
                    default:            if (!encode<X, T>::value(o, t, e, cx))                                 return false;
                }
            }
            return true;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool uqkey_pointer_write(O& o, const T* t, Cx& cx) {
            return t ?
                uqkey_pointer_write<X>(o, t, ptrlen(t), cx) :
                io::poke<X>(o, X::id::nil, cx)
            ;
        }

    template <typename X>
        struct pointer {
            template <typename O, typename T, typename Cx>
                static bool write(O& o, const T* t, Cx& cx) {
                    return pointer_write<X>(o, t, cx);
                }
        };
    template <typename X, template <typename> class S>
        struct pointer<S<bits::UQKEY<X>>> {
            template <typename O, typename T, typename Cx>
                static bool write(O& o, const T* t, Cx& cx) {
                    return uqkey_pointer_write<S<X>>(o, t, cx);
                }
        };

}}  // cxon::bits

namespace cxon { // write, compound types

    template <typename X, typename T>
        struct write<X, T*> {
            template <typename O, typename Cx>
                static bool value(O& o, const T* t, Cx& cx) {
                    return t ? write_value<X>(o, *t, cx) : io::poke<X>(o, X::id::nil, cx);
                }
        };

    template <typename X, typename T, size_t N>
        struct write<X, T[N]> {
            template <typename O, typename Cx>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    return container::write<X, list<X>>(o, t, cx);
                }
        };

#   define CXON_ARRAY(T)\
        template <typename X, size_t N>\
            struct write<X, T[N]> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const T (&t)[N], Cx& cx)     { return bits::array_write<X>(o, t, t + N, cx); }\
            };
        CXON_ARRAY(char)
        CXON_ARRAY(char16_t)
        CXON_ARRAY(char32_t)
        CXON_ARRAY(wchar_t)
#   undef CXON_ARRAY

#   define CXON_POINTER(T)\
        template <typename X>\
            struct write<X, const T*> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const T* t, Cx& cx)          { return bits::pointer<X>::write(o, t, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

#   define CXON_POINTER(T)\
        template <typename X>\
            struct write<X, T*> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, T* t, Cx& cx)                { return write_value<X, const T*>(o, t, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

}   // cxon write, compound types

namespace cxon { namespace bits { // container helpers

    template <typename A>
        struct adaptor : A {
            static const typename A::container_type& container(const A& a) noexcept { return ((adaptor&)a).c; }
        };

}}  // cxon::bits container helpers

namespace cxon { namespace bits { // tuple write

    template <typename X, typename T, unsigned N, unsigned L>
        struct tuple_write {
            template <typename O, typename Cx>
                static bool value(O& o, const T& t, Cx& cx) {
                    return  write_value<X>(o, std::get<N>(t), cx) && io::poke<X>(o, X::list::sep, cx) &&
                            tuple_write<X, T, N + 1, L>::value(o, t, cx)
                    ;
                }
        };

    template <typename X, typename T, unsigned N>
        struct tuple_write<X, T, N, N> {
            template <typename O, typename Cx>
                static bool value(O& o, const T& t, Cx& cx) {
                    return write_value<X>(o, std::get<N>(t), cx);
                }
        };

}}  // cxon::bits tuple write

namespace cxon { // key write

    namespace bits {

        template <typename X> struct key_write;

        template <typename X, template <typename> class S>
            struct key_write<S<X>> {
                template <typename T, typename O, typename Cx, typename E = S<X>>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                    {
                        return write_value<S<UQKEY<X>>>(o, t, cx);
                    }
                template <typename T, typename O, typename Cx, typename E = S<X>>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value || !E::map::unquoted_keys, bool>
                    {
                        return write_value<E>(o, t, cx);
                    }
            };
        template <typename X>
            struct key_write<JSON<X>> {
                template <typename T, typename O, typename Cx, typename E = JSON<X>>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                    {
                        return write_value<JSON<UQKEY<X>>>(o, t, cx);
                    }
                template <typename T, typename O, typename Cx, typename E = JSON<X>>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value && !E::map::unquoted_keys, bool>
                    {
                        return write_value<E>(o, t, cx);
                    }
                template <typename T, typename O, typename Cx, typename E = JSON<X>>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                    {
                        return write_value<E>(o, t, cx);
                    }
                template <typename T, typename O, typename Cx, typename E = JSON<X>>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value && !E::map::unquoted_keys, bool>
                    {
                        return  io::poke<E>(o, E::string::beg, cx) &&
                                    write_value<E>(o, t, cx) &&
                                io::poke<E>(o, E::string::end, cx)
                        ;
                    }
            };

    }

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx) {
            return bits::key_write<X>::value(o, t, cx) && io::poke<X>(o, X::map::div, cx);
        }

}   // cxon key write

namespace cxon { // write, library types

    template <typename X, typename T, typename ...R>
        struct write<X, std::basic_string<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx)     { return bits::pointer_write<X>(o, t.data(), t.size(), cx); }
        };
    template <typename X, template <typename> class S, typename T, typename ...R>
        struct write<S<bits::UQKEY<X>>, std::basic_string<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx)  { return bits::uqkey_pointer_write<S<X>>(o, t.data(), t.size(), cx); }
        };

    template <typename X, typename ...T>
        struct write<X, std::tuple<T...>> {
            using U = std::tuple<T...>;
            template <typename O, typename Cx>
                static bool value(O& o, const U& t, Cx& cx) {
                    return  io::poke<X>(o, X::list::beg, cx) &&
                                bits::tuple_write<X, U, 0, std::tuple_size<U>::value - 1>::value(o, t, cx) &&
                            io::poke<X>(o, X::list::end, cx)
                    ;
                }
        };

    template <typename X, typename F, typename S>
        struct write<X, std::pair<F, S>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::pair<F, S>& t, Cx& cx) {
                    return  io::poke<X>(o, X::list::beg, cx) &&
                                write_value<X>(o, t.first, cx) && io::poke<X>(o, X::list::sep, cx) && write_value<X>(o, t.second, cx) &&
                            io::poke<X>(o, X::list::end, cx)
                    ;
                }
        };

    template <typename X, typename T>
        struct write<X, std::valarray<T>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::valarray<T>& t, Cx& cx) {
                    return container::write<X, list<X>>(o, t, cx);
                }
        };

    template <typename X, typename T, size_t S>
        struct write<X, std::array<T, S>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::array<T, S>& t, Cx& cx) {
                    return container::write<X, list<X>>(o, t, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::queue<T, R...>> {
            using A = std::queue<T, R...>;
            template <typename O, typename Cx>
                static bool value(O& o, const A& t, Cx& cx) {
                    auto const& c = bits::adaptor<A>::container(t);
                    return container::write<X, list<X>>(o, c.begin(), c.end(), cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::priority_queue<T, R...>> {
            using A = std::priority_queue<T, R...>;
            template <typename O, typename Cx>
                static bool value(O& o, const A& t, Cx& cx) {
                    auto const& c = bits::adaptor<A>::container(t);
                    return container::write<X, list<X>>(o, c.begin(), c.end(), cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::stack<T, R...>> {
            using A = std::stack<T, R...>;
            template <typename O, typename Cx>
                static bool value(O& o, const A& t, Cx& cx) {
                    auto const& c =  bits::adaptor<A>::container(t);
                    return container::write<X, list<X>>(o, c.rbegin(), c.rend(), cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::deque<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::deque<T, R...>& t, Cx& cx) {
                    return container::write<X, list<X>>(o, t, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::list<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::list<T, R...>& t, Cx& cx) {
                    return container::write<X, list<X>>(o, t, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::forward_list<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::forward_list<T, R...>& t, Cx& cx) {
                    return container::write<X, list<X>>(o, t, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::vector<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::vector<T, R...>& t, Cx& cx) {
                    return container::write<X, list<X>>(o, t, cx);
                }
        };

#   define CXON_SET(S)\
        template <typename X, typename T, typename ...R>\
            struct write<X, S<T, R...>> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const S<T, R...>& t, Cx& cx) {\
                        return container::write<X, list<X>>(o, t, cx);\
                    }\
            };
        CXON_SET(std::set)
        CXON_SET(std::multiset)
        CXON_SET(std::unordered_set)
        CXON_SET(std::unordered_multiset)
#   undef CXON_SET

    namespace bits {

        template <typename X, typename M, typename O, typename Cx>
            inline bool write_map(O& o, const M& t, Cx& cx) {
                return container::write<X, map<X>>(o, t, cx, [&](const typename M::value_type& e) {
                    return write_key<X>(o, e.first, cx) && write_value<X>(o, e.second, cx);
                });
            }

    }
#   define CXON_MAP(M)\
        template <typename X, typename K, typename V, typename ...R>\
            struct write<X, M<K, V, R...>> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const M<K, V, R...>& t, Cx& cx) {\
                        return bits::write_map<X>(o, t, cx);\
                    }\
            };
        CXON_MAP(std::map)
        CXON_MAP(std::multimap)
        CXON_MAP(std::unordered_map)
        CXON_MAP(std::unordered_multimap)
#   undef CXON_MAP

}   // write, library types

namespace cxon { namespace unquoted { // unquoted value

    namespace bits {

        template <typename X>
            struct value {
                template <typename BA, typename II>
                    static bool skip(BA& o, II& i, II e) {
                        if (!o.add(*i)) return false;
                        for (++i; i != e; ++i) {
                            switch (*i)  {
                                case '\\':              if (!o.add(*i)) return false;
                                                        if (++i == e)   return false;
                                                        break;
                                case X::string::end:    return o.add(*i);
                            }
                        }
                        return false;
                    }
                template <char CB, char CE, typename BA, typename II>
                    static bool skip(BA& o, II& i, II e) {
                        if (!o.add(*i)) return false;
                        unsigned r = 0;
                        for (++i; i != e; ++i) {
                            switch (*i) {
                                case CB:                if (!o.add(*i))     return false;
                                                        ++r; break;
                                case CE:                if (!o.add(*i))     return false;
                                                        if (r == 0)         return true;
                                                        --r; break;
                                case X::string::beg:    if (!skip(o, i, e)) return false;
                            }
                        }
                        return false;
                    }
                template <typename BA, typename II>
                    static bool read(BA o, II& i, II e) {
                        for (char c = *i; i != e; c = *++i) {
                                 if (c == X::map::beg)      { if (!skip<map<X>::beg, map<X>::end>(o, i, e))     return false; }
                            else if (c == X::list::beg)     { if (!skip<list<X>::beg, list<X>::end>(o, i, e))   return false; }
                            else if (c == X::string::beg)   { if (!skip(o, i, e))                               return false; }
                            else if (c == X::map::sep)                                                          break;
                            else if (c == X::map::end)                                                          break;
                            else if (c == X::list::end)                                                         break;
                            else                            { if (!o.add(*i))                                   return false; }
                        }
                        return o.add('\0');
                    }
            };

        template <typename T, typename Cx>
            struct array_adder {
                T *f, *l;
                Cx& cx;
                template <size_t N>
                    array_adder(T (&t)[N], Cx& cx) : f(t), l(t + N), cx(cx) {}
                bool add(T e) { return f != l ? (*f = e, ++f, true) : (cx|read_error::overflow, false); }
            };

        template <typename T>
            struct black_adder {
                constexpr bool add(T) const { return true; }
            };

    }

    template <typename X, typename T, size_t N, typename II, typename Cx>
        inline bool read_value(T (&t)[N], II& i, II e, Cx& cx) {
            II const o = i;
                if (!bits::value<X>::read(bits::array_adder<T, Cx>(t, cx), i, e)) {
                    return cx.ec == read_error::overflow ? (cxon::bits::rewind(i, o), false) : (cx|read_error::unexpected, false);
                }
            return true;
        }

    template <typename X, typename II, typename Cx>
        inline bool read_value(II& i, II e, Cx& cx) {
            return !bits::value<X>::read(bits::black_adder<decltype(*i)>(), i, e) ?
                (cx|read_error::unexpected, false) : true;
        }

}}  // cxon::unquoted value

namespace cxon { namespace enums { // enum reader/writer construction helpers

    namespace bits {

        template <typename X>
            struct read {
                template <size_t N, typename II, typename Cx>
                    static bool value(char (&t)[N], II& i, II e, Cx& cx) {
                        return unquoted::read_value<X>(t, i, e, cx);
                    }
            };
        template <typename X>
            struct read<JSON<X>> {
                template <size_t N, typename II, typename Cx>
                    static bool value(char (&t)[N], II& i, II e, Cx& cx) {
                        return cxon::read_value<JSON<X>>(t, i, e, cx);
                    }
            };

    }

    template <typename X, typename E, typename V, typename II, typename Cx>
        inline bool read_value(E& t, V vb, V ve, II& i, II e, Cx& cx) {
            io::consume<X>(i, e);
            II const o = i;
                char id[ids_len_max::constant<prms_type<Cx>>(64U)];
                    if (!bits::read<X>::value(id, i, e, cx)) return false;
                for ( ; vb != ve; ++vb) if (std::strcmp(vb->name, id) == 0)
                    return t = vb->value, true;
            return cx|read_error::unexpected, cxon::bits::rewind(i, o), false;
        }

    template <typename X, typename E, typename V, typename O, typename Cx>
        inline bool write_value(O& o, E t, V vb, V ve, Cx& cx) {
            for ( ; vb != ve; ++vb) if (t == vb->value)
                return io::poke<X>(o, cxon::bits::opqt<X>::beg, cx) && io::poke<X>(o, vb->name, cx) && io::poke<X>(o, cxon::bits::opqt<X>::end, cx);
            return false;
        }

}}  // cxon::enums enum reader/writer construction helpers

#if 1 // cxon enum read/write macros

#   define CXON_ENUM_VALUE(T, N, V)     enums::make_value(N, T::V)
#   define CXON_ENUM_VALUE_NAME(N, V)   CXON_ENUM_VALUE(T, N, V)
#   define CXON_ENUM_VALUE_ASIS(V)      CXON_ENUM_VALUE(T, #V, V)

#   define CXON_ENUM_READ(Type, ...)\
        namespace cxon {\
            template <typename X, typename II, typename Cx>\
                inline bool read_value(Type& t, II& i, II e, Cx& cx) {\
                    using T = Type;\
                    static constexpr enums::value<Type> v[] = { __VA_ARGS__ };\
                    return enums::read_value<X>(t, std::begin(v), std::end(v), i, e, cx);\
                }\
        }
#   define CXON_ENUM_WRITE(Type, ...)\
        namespace cxon {\
            template <typename X, typename O, typename Cx>\
                inline bool write_value(O& o, const Type& t, Cx& cx) {\
                    using T = Type;\
                    static constexpr enums::value<Type> v[] = { __VA_ARGS__ };\
                    return enums::write_value<X>(o, t, std::begin(v), std::end(v), cx);\
                }\
        }
#   define CXON_ENUM(Type, ...)\
        CXON_ENUM_READ(Type, __VA_ARGS__)\
        CXON_ENUM_WRITE(Type, __VA_ARGS__)

#endif // cxon enum read/write macros

namespace cxon { namespace structs { // structured types reader/writer construction helpers

    // field

    template <typename D>
        struct field {
            using type = D;
            char const*const name;
            D mptr;
        };
        template <typename X, typename S, typename F, typename II, typename Cx>
            static auto read_field(S& s, F f, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_same<typename F::type, skip_type>::value &&  std::is_member_pointer<typename F::type>::value, bool>
            {
                return read_value<X>(s.*f.mptr, i, e, cx);
            }
        template <typename X, typename S, typename F, typename II, typename Cx>
            static auto read_field(S&, F f, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_same<typename F::type, skip_type>::value && !std::is_member_pointer<typename F::type>::value, bool>
            {
                return read_value<X>(*f.mptr, i, e, cx);
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            static auto write_field(O& o, const S& s, F f, Cx& cx)
                -> enable_if_t<!std::is_same<typename F::type, skip_type>::value &&  std::is_member_pointer<typename F::type>::value, bool>
            {
                return write_key<X>(o, f.name, cx) && write_value<X>(o, s.*f.mptr, cx);
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            static auto write_field(O& o, const S&, F f, Cx& cx)
                -> enable_if_t<!std::is_same<typename F::type, skip_type>::value && !std::is_member_pointer<typename F::type>::value, bool>
            {
                return write_key<X>(o, f.name, cx) && write_value<X>(o, *f.mptr, cx);
            }
    template <>
        struct field<skip_type> {
            using type = skip_type;
            char const*const name;
            skip_type const _;
        };
        template <typename X, typename S, typename F, typename II, typename Cx>
            static auto read_field(S&, F, II& i, II e, Cx& cx)
                -> enable_if_t<std::is_same<typename F::type, skip_type>::value, bool>
            {
                return unquoted::read_value<X>(i, e, cx);
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            static auto write_field(O&, const S&, F, Cx&)
                -> enable_if_t<std::is_same<typename F::type, skip_type>::value, bool>
            {
                return true;
            }

    // fields

    template <typename H, typename ...T>
        struct fields<H, T...> {
            H const field;
            fields<T...> const next;
            template <typename ...U>
                constexpr fields(H e, U ...t) : field(e), next(t...) {}
        };
    template <typename T>
        struct fields<T> {
            T const field;
            fields<> const next;
            constexpr fields(T e) : field(e), next() {}
        };

    // read

    namespace bits {

        template <typename X, typename S, typename ...>
            struct read {
                template <typename II, typename Cx>
                    static constexpr bool fields(S&, const char*, const fields<>&, II&, II, Cx&) {
                        return false;
                    }
            };
        template <typename X, typename S, typename H, typename ...T>
            struct read<X, S, H, T...> {
                template <typename II, typename Cx>
                    static bool fields(S& t, const char* name, const fields<H, T...>& f, II& i, II e, Cx& cx) {
                        return std::strcmp(f.field.name, name) == 0 ?
                            read_field<X>(t, f.field, i, e, cx) :
                            read<X, S, T...>::fields(t, name, f.next, i, e, cx)
                        ;
                    }
            };

    }

    template <typename X, typename S, typename ...F, typename II, typename Cx>
        inline bool read_fields(S& s, const fields<F...>& f, II& i, II e, Cx& cx) {
            if (!io::consume<X>(X::map::beg, i, e, cx)) return false;
            if ( io::consume<X>(X::map::end, i, e)) return true;
            for (char id[ids_len_max::constant<prms_type<Cx>>(64U)]; ; ) {
                io::consume<X>(i, e);
                II const o = i;
                    if (!read_key<X>(id, i, e, cx)) return false;
                    if (!bits::read<X, S, F...>::fields(s, id, f, i, e, cx)) return !cx.ec ?
                        (cxon::bits::rewind(i, o), cx|read_error::unexpected, false) :
                        false
                    ;
                    if (io::consume<X>(X::map::sep, i, e)) continue;
                return io::consume<X>(X::map::end, i, e, cx);
            }
            return true;
        }

    // write

    namespace bits {

        template <typename X, typename S, typename H, typename ...T>
            struct write {
                template <typename O, typename Cx>
                    static bool fields(O& o, const S& t, const fields<H, T...>& f, Cx& cx) {
                        return  write_field<X>(o, t, f.field, cx) && io::poke<X>(o, X::map::sep, cx) &&
                                write<X, S, T...>::fields(o, t, f.next, cx)
                        ;
                    }
            };
        template <typename X, typename S, typename F>
            struct write<X, S, F> {
                template <typename O, typename Cx>
                    static bool fields(O& o, const S& t, const fields<F>& f, Cx& cx) {
                        return write_field<X>(o, t, f.field, cx);
                    }
            };

    }

    template <typename X, typename S, typename ...F, typename O, typename Cx>
        inline bool write_fields(O& o, const S& s, const fields<F...>& f, Cx& cx) {
            return  io::poke<X>(o, X::map::beg, cx) &&
                        bits::write<X, S, F...>::fields(o, s, f, cx) &&
                    io::poke<X>(o, X::map::end, cx)
            ;
        }

}} // cxon::structs structured types reader/writer construction helpers

namespace cxon { // structs::skip_type read

    template <typename X>
        struct read<X, structs::skip_type> {
            template <typename InIt, typename Cx>
                static bool value(structs::skip_type&, InIt& i, InIt e, Cx& cx) {
                    return unquoted::read_value<X>(i, e, cx);
                }
        };

}   // cxon structs::skip_type read

#if 1 // cxon struct read/write macros

#   define CXON_STRUCT_FIELD(T, N, F)   cxon::structs::make_field(N, &T::F)
#   define CXON_STRUCT_FIELD_NAME(N, F) CXON_STRUCT_FIELD(T, N, F)
#   define CXON_STRUCT_FIELD_ASIS(F)    CXON_STRUCT_FIELD(T, #F, F)
#   define CXON_STRUCT_FIELD_SKIP(N)    cxon::structs::make_field(N)

#   define CXON_STRUCT_READ(Type, ...)\
        namespace cxon {\
            template <typename X, typename II, typename Cx>\
                inline bool read_value(Type& t, II& i, II e, Cx& cx) {\
                    using T = Type;\
                    static constexpr auto f = structs::make_fields(__VA_ARGS__);\
                    return structs::read_fields<X>(t, f, i, e, cx);\
                }\
        }
#   define CXON_STRUCT_WRITE(Type, ...)\
        namespace cxon {\
            template <typename X, typename O, typename Cx>\
                static bool write_value(O& o, const Type& t, Cx& cx) {\
                    using T = Type;\
                    static constexpr auto f = structs::make_fields(__VA_ARGS__);\
                    return structs::write_fields<X>(o, t, f, cx);\
                }\
        }
#   define CXON_STRUCT(Type, ...)\
        CXON_STRUCT_READ(Type, __VA_ARGS__)\
        CXON_STRUCT_WRITE(Type, __VA_ARGS__)

#   define CXON_STRUCT_READ_MEMBER(Type, ...)\
        template <typename X, typename II, typename Cx>\
            static bool read_value(Type& t, II& i, II e, Cx& cx) {\
                using T = Type;\
                static constexpr auto f = cxon::structs::make_fields(__VA_ARGS__);\
                return cxon::structs::read_fields<X>(t, f, i, e, cx);\
            }
#   define CXON_STRUCT_WRITE_MEMBER(Type, ...)\
        template <typename X, typename O, typename Cx>\
            static bool write_value(O& o, const Type& t, Cx& cx) {\
                using T = Type;\
                static constexpr auto f = cxon::structs::make_fields(__VA_ARGS__);\
                return cxon::structs::write_fields<X>(o, t, f, cx);\
            }
#   define CXON_STRUCT_MEMBER(Type, ...)\
        CXON_STRUCT_READ_MEMBER(Type, __VA_ARGS__)\
        CXON_STRUCT_WRITE_MEMBER(Type, __VA_ARGS__)

#endif // cxon struct read/write macros

#endif // CXON_CXON_HXX_
