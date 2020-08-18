// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CXON_HXX_
#define CXON_CXON_HXX_

#define CXON_VERSION_MAJOR 0
#define CXON_VERSION_MINOR 49
#define CXON_VERSION_PATCH 0

#include "utility.hxx"

#include <string>

#include <utility>
#include <type_traits>
#include <system_error>

#include <cstring> // strcmp

// interface //////////////////////////////////////////////////////////////////

namespace cxon { // interface

    // format selectors

    template <typename T = struct cxon_format_traits>
        struct CXON : T { using traits = T; };

    template <typename T = struct json_format_traits>
        struct JSON : T { using traits = T; };

    template <typename X, template <typename> class S, typename R = void>
        using enable_for_t = enable_if_t<std::is_same<X, S<typename X::traits>>::value, R>;

    // read

    template <typename It>
        struct from_bytes_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept { return !ec; }
        };

    template <typename X = JSON<>, typename T, typename InIt, typename ...CxPs>
        inline auto     from_bytes(T& t, InIt b, InIt e, CxPs... p)       -> from_bytes_result<InIt>;
    template <typename X = JSON<>, typename T, typename Iterable, typename ...CxPs>
        inline auto     from_bytes(T& t, const Iterable& i, CxPs... p)    -> from_bytes_result<decltype(std::begin(i))>;

    // write

    template <typename It>
        struct to_bytes_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept { return !ec; }
        };

    template <typename X = JSON<>, typename T, typename OutIt, typename ...CxPs>
        inline auto     to_bytes(OutIt o, const T& t, CxPs... p)          -> enable_if_t<is_output_iterator<OutIt>::value, to_bytes_result<OutIt>>;
    template <typename X = JSON<>, typename T, typename Insertable, typename ...CxPs>
        inline auto     to_bytes(Insertable& i, const T& t, CxPs... p)    -> enable_if_t<is_back_insertable<Insertable>::value, to_bytes_result<decltype(std::begin(i))>>;
    template <typename X = JSON<>, typename T, typename FwIt, typename ...CxPs>
        inline auto     to_bytes(FwIt b, FwIt e, const T& t, CxPs... p)   -> to_bytes_result<FwIt>;

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

namespace cxon { // contexts

    template <typename ...Ps> // prms
        struct context {
            using prms_type = prms::pack<Ps...>;

            std::error_condition    ec;
            prms_type               ps;

            context(Ps&&... ps) :   ec(),
                                    ps(prms::make_pack(std::forward<Ps>(ps)...))
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

namespace cxon { // context parameters

    CXON_PARAMETER(allocator, std::allocator<char>);    // read: T*
    CXON_PARAMETER(num_len_max, unsigned);              // read: constexpr: numbers
    CXON_PARAMETER(ids_len_max, unsigned);              // read: constexpr: object key
    CXON_PARAMETER(fp_precision, int);                  // write: constexpr: floating-points

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

}   // cxon implementation bridge

// implementation /////////////////////////////////////////////////////////////

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

namespace cxon { // interface implementation

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
            CXON_FORCE_INLINE auto from_bytes(T& t, II b, II e, CxPs... p) -> from_bytes_result<II> {
                    if (b == e) return { read_error::unexpected, b };
                read_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    bool const r = read_value<X>(t, b, e, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, b };
            }
        template <typename X, typename T, typename I, typename ...CxPs>
            CXON_FORCE_INLINE auto from_bytes(T& t, const I& i, CxPs... p) -> from_bytes_result<decltype(std::begin(i))> {
                auto const c = continuous<I>::range(i);
                auto const r = interface::from_bytes<X>(t, c.first, c.second, std::forward<CxPs>(p)...);
                auto b = std::begin(i); std::advance(b, std::distance(c.first, r.end));
                return { r.ec, b };
            }

    }

    template <typename X, typename T, typename II, typename ...CxPs>
        inline auto from_bytes(T& t, II b, II e, CxPs... p) -> from_bytes_result<II> {
            return interface::from_bytes<X>(t, b, e, std::forward<CxPs>(p)...);
        }
    template <typename X, typename T, typename I, typename ...CxPs>
        inline auto from_bytes(T& t, const I& i, CxPs... p) -> from_bytes_result<decltype(std::begin(i))> {
            return interface::from_bytes<X>(t, i, std::forward<CxPs>(p)...);
        }

    // write

    namespace interface {

        template <typename X, typename T, typename OI, typename ...CxPs>
            CXON_FORCE_INLINE auto to_bytes(OI o, const T& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    bool const r = write_value<X>(o, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, o };
            }
        template <typename X, typename T, typename I, typename ...CxPs>
            CXON_FORCE_INLINE auto to_bytes(I& i, const T& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    auto const s = std::distance(std::begin(i), std::end(i));
                    bool const r = write_value<X>(i, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                    auto b = std::begin(i); std::advance(b, s);
                return { cx.ec, b };
            }
        template <typename X, typename T, typename FI, typename ...CxPs>
            CXON_FORCE_INLINE auto to_bytes(FI b, FI e, const T& t, CxPs... p) -> to_bytes_result<FI> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    auto o = make_output_iterator(b, e);
                    bool const r = write_value<X>(o, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, o };
            }

    }

    template <typename X, typename T, typename OI, typename ...CxPs>
        inline auto to_bytes(OI o, const T& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
            return interface::to_bytes<X>(o, t, std::forward<CxPs>(p)...);
        }
    template <typename X, typename T, typename I, typename ...CxPs>
        inline auto to_bytes(I& i, const T& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
            return interface::to_bytes<X>(i, t, std::forward<CxPs>(p)...);
        }
    template <typename X, typename T, typename FI, typename ...CxPs>
        inline auto to_bytes(FI b, FI e, const T& t, CxPs... p) -> to_bytes_result<FI> {
            return interface::to_bytes<X>(b, e, t, std::forward<CxPs>(p)...);
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
                default:                                    return "unknown error";
            }
        }
        static const read_error_category& value() {
            static read_error_category const v{};
            return v;
        }
    };

    inline std::error_condition make_error_condition(read_error e) noexcept {
        return { static_cast<int>(e), read_error_category::value() };
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
                default:                            return "unknown error";
            }
        }
        static const write_error_category& value() {
            static write_error_category const v{};
            return v;
        }
    };

    inline std::error_condition make_error_condition(write_error e) noexcept {
        return { static_cast<int>(e), write_error_category::value() };
    }

}   // cxon errors

namespace std { // cxon errors
    template <> struct is_error_condition_enum<cxon::read_error> : true_type {};
    template <> struct is_error_condition_enum<cxon::write_error> : true_type {};
}   // std cxon errors

#endif // CXON_CXON_HXX_
