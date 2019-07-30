// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CXON_HXX_
#define CXON_CXON_HXX_

#define CXON_VERSION_MAJOR 0
#define CXON_VERSION_MINOR 45
#define CXON_VERSION_PATCH 1

#include <string>

// TODO: decouple
#include <array>
#include <valarray> // std::begin
#include <vector>

#include <utility>
#include <algorithm>

#include <limits>
#include <type_traits>
#include <system_error>

#include <cmath> // isfinite, ...

#include <cstring> // strcmp

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

    template <bool C, typename T = void>
        using enable_if_t = typename std::enable_if<C, T>::type;

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

    template <typename X = JSON<>, typename T, typename OutIt, typename ...CxPs>
        inline auto     to_chars(OutIt o, const T& t, CxPs... p)          -> enable_if_t<is_output_iterator<OutIt>::value, to_chars_result<OutIt>>;
    template <typename X = JSON<>, typename T, typename Insertable, typename ...CxPs>
        inline auto     to_chars(Insertable& i, const T& t, CxPs... p)    -> enable_if_t<is_back_insertable<Insertable>::value, to_chars_result<decltype(std::begin(i))>>;
    template <typename X = JSON<>, typename T, typename FwIt, typename ...CxPs>
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
        
        template <typename Ta, typename Ty, Ty c>
            using ctt = bits::ctt<Ta, Ty, c>;
        template <typename Ta, typename Ty>
            using stt = bits::stt<Ta, Ty>;
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
            operator bool() const noexcept { return !ec; }
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

namespace cxon { namespace bits { // forward: iterator category
    template <typename, typename = void> struct is_forward_iterator;
}}

namespace cxon { namespace bits { // forward: output iterator from a range
    template <typename FwIt> struct output_iterator;
}}

namespace cxon { namespace io { // I/O

    template<typename II>
        inline auto rewind(II&, II) noexcept     -> enable_if_t<!cxon::bits::is_forward_iterator<II>::value>;
    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t< cxon::bits::is_forward_iterator<II>::value>;

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

    template <typename FwIt>
        using output_iterator = cxon::bits::output_iterator<FwIt>;

    template <typename FwIt>
        constexpr auto make_output_iterator(FwIt b, FwIt e) -> output_iterator<FwIt>;

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

#include "bits/cxon.hxx"

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

#   if defined(__GNUC__) || defined(__clang__)
#       define CXON_FORCE_INLINE __attribute__((always_inline)) inline
#   elif defined(_MSC_VER)
#       define CXON_FORCE_INLINE __forceinline
#   else
#       define CXON_FORCE_INLINE inline
#   endif

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
                return interface::from_chars<X>(t, std::begin(i), std::end(i), std::forward<CxPs>(p)...);
            }

    }

    template <typename X, typename T, typename II, typename ...CxPs>
        inline auto from_chars(T& t, II b, II e, CxPs... p) -> from_chars_result<II> {
            return interface::from_chars<X>(t, b, e, std::forward<CxPs>(p)...);
        }
    template <typename X, typename T, typename I, typename ...CxPs>
        inline auto from_chars(T& t, const I& i, CxPs... p) -> from_chars_result<decltype(std::begin(i))> {
            return interface::from_chars<X>(t, i, std::forward<CxPs>(p)...);
        }

    // write

    namespace interface {

        template <typename X, typename T, typename OI, typename ...CxPs>
            CXON_FORCE_INLINE auto to_chars(OI o, const T& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_chars_result<OI>> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    bool const r = write_value<X>(o, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, o };
            }
        template <typename X, typename T, typename I, typename ...CxPs>
            CXON_FORCE_INLINE auto to_chars(I& i, const T& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_chars_result<decltype(std::begin(i))>> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    auto const s = std::distance(std::begin(i), std::end(i));
                    bool const r = write_value<X>(i, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                    auto b = std::begin(i); std::advance(b, s);
                return { cx.ec, b };
            }
        template <typename X, typename T, typename FI, typename ...CxPs>
            CXON_FORCE_INLINE auto to_chars(FI b, FI e, const T& t, CxPs... p) -> to_chars_result<FI> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    auto o = io::make_output_iterator(b, e);
                    bool const r = write_value<X>(o, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, o };
            }

    }

    template <typename X, typename T, typename OI, typename ...CxPs>
        inline auto to_chars(OI o, const T& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_chars_result<OI>> {
            return interface::to_chars<X>(o, t, std::forward<CxPs>(p)...);
        }
    template <typename X, typename T, typename I, typename ...CxPs>
        inline auto to_chars(I& i, const T& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_chars_result<decltype(std::begin(i))>> {
            return interface::to_chars<X>(i, t, std::forward<CxPs>(p)...);
        }
    template <typename X, typename T, typename FI, typename ...CxPs>
        inline auto to_chars(FI b, FI e, const T& t, CxPs... p) -> to_chars_result<FI> {
            return interface::to_chars<X>(b, e, t, std::forward<CxPs>(p)...);
        }

#   undef CXON_FORCE_INLINE

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

namespace cxon { namespace io { // I/O

    template<typename II>
        inline auto rewind(II&, II) noexcept     -> enable_if_t<!cxon::bits::is_forward_iterator<II>::value> {}
    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t< cxon::bits::is_forward_iterator<II>::value> { i = o; }

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
            return c == peek(i, e) && (next(i, e), true);
        }
    template <typename X, typename II, typename Cx>
        inline bool consume(char c, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            return (c == peek(i, e) && (next(i, e), true)) || (cx|read_error::unexpected);
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

    template <typename FwIt>
        constexpr output_iterator<FwIt> make_output_iterator(FwIt b, FwIt e) {
            return output_iterator<FwIt>(b, e);
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
        inline bool poke(O& o, char c, Cx& cx)                  { return bits::poke<X>(o, cx, c); }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, Cx& cx)           { return bits::poke<X>(o, cx, s); }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, const char* s, size_t n, Cx& cx) { return bits::poke<X>(o, cx, s, n); }
    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, unsigned n, char c, Cx& cx)      { return bits::poke<X>(o, cx, n, c); }
    template <typename X, typename O, typename Cx>
        constexpr bool poke(O&, std::nullptr_t, Cx&)            { return true; }

}}   // cxon::io

namespace cxon { namespace container { // container read/write helpers

    // read

    template <typename X, typename Cr, typename II, typename Cx, typename EA>
        inline bool read(II& i, II e, Cx& cx, EA element_add) {
            if (!io::consume<X>(Cr::beg, i, e, cx)) return false;
            if ( io::consume<X>(Cr::end, i, e))     return true;
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

}}  // cxon::container read/write helpers

namespace cxon { // read, fundamental types

    template <typename X, typename II, typename Cx>
        inline bool read_value(bool& t, II& i, II e, Cx& cx) {
            static_assert(*X::id::pos != *X::id::neg, "boolean literals ambiguous"); // for input-iterator, id must be consumed
            II const o = i;
                char const c = (io::consume<X>(i, e), io::peek(i, e));
                     if (c == *X::id::pos && io::consume<X>(X::id::pos, i, e))  return t = true,  true;
                else if (c == *X::id::neg && io::consume<X>(X::id::neg, i, e))  return t = false, true;
            return io::rewind(i, o), cx|read_error::boolean_invalid;
        }

    template <typename X, typename II, typename Cx>
        inline bool read_value(char& t, II& i, II e, Cx& cx) {
            if (!io::consume<X>(X::string::beg, i, e, cx)) return false;
                II const o = i;
                    char32_t const c32 = bits::str_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF)  return io::rewind(i, o), false;
                        if (c32 > 0XFF)         return io::rewind(i, o), cx|read_error::character_invalid;
            return io::consume<X>(X::string::end, i, e, cx) && (t = char(c32), true);
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char16_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char16_t)), bool>
        {
            if (!io::consume<X>(X::string::beg, i, e, cx)) return false;
                II const o = i;
                    char32_t const c32 = bits::str_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF)  return io::rewind(i, o), false;
                        if (c32 > 0XFFFF)       return io::rewind(i, o), cx|read_error::character_invalid;
            return io::consume<X>(X::string::end, i, e, cx) && (t = T(c32), true);
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char32_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char32_t)), bool>
        {
            if (!io::consume<X>(X::string::beg, i, e, cx)) return false;
                II const o = i;
                    char32_t const c32 = bits::str_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF) return io::rewind(i, o), false;
            return io::consume<X>(X::string::end, i, e, cx) && (t = T(c32), true);
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
            return (io::consume<X>(X::id::nil, i, e) || (io::rewind(i, o), cx|read_error::unexpected)) &&
                    (t = nullptr, true)
            ;
        }

}   // cxon read, fundamental types

namespace cxon { // read, compound types

    template <typename X, typename T>
        struct read<X, T*> {
            template <typename II, typename Cx>
                static bool value(T*& t, II& i, II e, Cx& cx) {
                    io::consume<X>(i, e);
                    if (io::peek(i, e) == *X::id::nil) { // TODO: not correct as T may start with *X::id::nil (e.g. 'nan'), but it's supposed to be used in structs anyway?
                        II const o = i;
                        return  (io::consume<X>(X::id::nil, i, e) || (io::rewind(i, o), cx|read_error::unexpected)) &&
                                (t = nullptr, true)
                        ;
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
                        return (p != N || (io::rewind(i, o), cx|read_error::overflow)) &&
                                read_value<X>(t[p++], i, e, cx)
                        ;
                    });
                }
        };

#   define CXON_ARRAY(T)\
        template <typename X, size_t N>\
            struct read<X, T[N]> {\
                template <typename II, typename Cx>\
                    static bool value(T (&t)[N], II& i, II e, Cx& cx)   { return bits::array<X>::read(t, t + N, i, e, cx); }\
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
                    static bool value(const T*& t, II& i, II e, Cx& cx) { return read_value<X>((T*&)t, i, e, cx); }\
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
                    static bool value(T*& t, II& i, II e, Cx& cx)       { return bits::pointer_read<X>(t, i, e, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

}   // cxon read, compound types

namespace cxon {  // key read

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx) {
            return bits::key_read<X>::value(t, i, e, cx) && io::consume<X>(X::map::div, i, e, cx);
        }

}   // cxon key read

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

}   // cxon read, library types

namespace cxon { // write, fundamental types

    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, bool t, Cx& cx) {
            return io::poke<X>(o, t ? X::id::pos : X::id::neg, cx);
        }

    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, char t, Cx& cx)       { return bits::character_write<X>(o, t, cx); }
    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, char16_t t, Cx& cx)   { return bits::character_write<X>(o, t, cx); }
    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, char32_t t, Cx& cx)   { return bits::character_write<X>(o, t, cx); }
    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, wchar_t t, Cx& cx)    { return bits::character_write<X>(o, t, cx); }

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
                    static bool value(O& o, const T (&t)[N], Cx& cx)    { return bits::array_write<X>(o, t, t + N, cx); }\
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
                    static bool value(O& o, const T* t, Cx& cx)         { return bits::pointer<X>::write(o, t, cx); }\
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
                    static bool value(O& o, T* t, Cx& cx)               { return write_value<X, const T*>(o, t, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

}   // cxon write, compound types

namespace cxon { // key write

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx) {
            return bits::key_write<X>::value(o, t, cx) && io::poke<X>(o, X::map::div, cx);
        }

}   // cxon key write

namespace cxon { // write, library types

    template <typename X, typename T, typename ...R>
        struct write<X, std::basic_string<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx)    { return bits::pointer_write<X>(o, t.data(), t.size(), cx); }
        };
    template <typename X, template <typename> class S, typename T, typename ...R>
        struct write<S<bits::UQKEY<X>>, std::basic_string<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx)    { return bits::uqkey_pointer_write<S<X>>(o, t.data(), t.size(), cx); }
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
                bool add(T e) { return f != l ? (*f = e, ++f, true) : cx|read_error::overflow; }
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
                    return cx.ec == read_error::overflow ? (io::rewind(i, o), false) : cx|read_error::unexpected;
                }
            return true;
        }

    template <typename X, typename II, typename Cx>
        inline bool read_value(II& i, II e, Cx& cx) {
            return bits::value<X>::read(bits::black_adder<decltype(*i)>(), i, e) || (cx|read_error::unexpected);
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
            return io::rewind(i, o), cx|read_error::unexpected;
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
                    if (!bits::read<X, S, F...>::fields(s, id, f, i, e, cx))
                        return cx && (io::rewind(i, o), cx|read_error::unexpected);
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
