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
#include "lang-fwd.hxx"

#include <utility>
#include <type_traits>
#include <system_error>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { // interface

    // format selectors

    template <typename T>
        struct format_selector : T { using traits = T; };

    template <typename X, template <typename> class S>
        using is_same_format = std::is_same<X, S<typename X::traits>>;

    template <typename X, template <typename> class S, typename R = void>
        using enable_for_t = enable_if_t<is_same_format<X, S>::value, R>;

    // read

    template <typename It>
        struct from_bytes_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept { return !ec; }
        };

    template <typename X = CXON_DEFAULT_FORMAT, typename T, typename InIt, typename ...CxPs>
        inline auto     from_bytes(T& t, InIt b, InIt e, CxPs... p)     -> from_bytes_result<InIt>;
    template <typename X = CXON_DEFAULT_FORMAT, typename T, typename Iterable, typename ...CxPs>
        inline auto     from_bytes(T& t, const Iterable& i, CxPs... p)  -> from_bytes_result<decltype(std::begin(i))>;

    // write

    template <typename It>
        struct to_bytes_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept { return !ec; }
        };

    template <typename X = CXON_DEFAULT_FORMAT, typename T, typename OutIt, typename ...CxPs>
        inline auto     to_bytes(OutIt o, const T& t, CxPs... p)        -> enable_if_t<is_output_iterator<OutIt>::value, to_bytes_result<OutIt>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename T, typename Insertable, typename ...CxPs>
        inline auto     to_bytes(Insertable& i, const T& t, CxPs... p)  -> enable_if_t<is_back_insertable<Insertable>::value, to_bytes_result<decltype(std::begin(i))>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename T, typename FwIt, typename ...CxPs>
        inline auto     to_bytes(FwIt b, FwIt e, const T& t, CxPs... p) -> to_bytes_result<FwIt>;

}

// implementation bridge //////////////////////////////////////////////////////

namespace cxon { // context

    template <typename ...Ps> // ... named parameters
        struct context {
            using napa_type = napa::pack<Ps...>;

            std::error_condition    ec;
            napa_type               ps;

            context(Ps&&... ps) :   ec(),
                                    ps(napa::make_pack(std::forward<Ps>(ps)...))
            {
            }

            template <typename E>
                auto operator |(E e) noexcept -> enable_if_t<std::is_error_condition_enum<E>::value, context&> {
                    return ec = e, *this;
                }
            operator bool() const noexcept { return !ec; }
        };

    template <typename ...P>
        using read_context  = context<P...>;
    template <typename ...P>
        using write_context = context<P...>;

    template <typename Cx>
        using napa_type = typename Cx::napa_type;

}

namespace cxon { // bridge

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

}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { // interface

    // read

    namespace interface {

        template <typename X, typename T, typename II, typename ...CxPs>
            inline auto from_bytes(T& t, II b, II e, CxPs... p) -> from_bytes_result<II> {
                read_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    bool const r = read_value<X>(t, b, e, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, b };
            }
        template <typename X, typename T, typename I, typename ...CxPs>
            inline auto from_bytes(T& t, const I& i, CxPs... p) -> from_bytes_result<decltype(std::begin(i))> {
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
            inline auto to_bytes(OI o, const T& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    bool const r = write_value<X>(o, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, o };
            }
        template <typename X, typename T, typename I, typename ...CxPs>
            inline auto to_bytes(I& i, const T& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
                write_context<CxPs...> cx(std::forward<CxPs>(p)...);
                    auto const s = std::distance(std::begin(i), std::end(i));
                    bool const r = write_value<X>(i, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                    auto b = std::begin(i); std::advance(b, s);
                return { cx.ec, b };
            }
        template <typename X, typename T, typename FI, typename ...CxPs>
            inline auto to_bytes(FI b, FI e, const T& t, CxPs... p) -> to_bytes_result<FI> {
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

}

#endif // CXON_CXON_HXX_
