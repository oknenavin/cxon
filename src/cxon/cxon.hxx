// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CXON_HXX_
#define CXON_CXON_HXX_

#define CXON_VERSION_MAJOR 0
#define CXON_VERSION_MINOR 63
#define CXON_VERSION_PATCH 0

#ifndef CXON_DEFAULT_FORMAT
#   error "Should only be included by format-specific headers."
#   define CXON_DEFAULT_FORMAT void
#endif

#include "utility.hxx"

#include <utility>
#include <type_traits>
#include <system_error>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { // interface

    // format selectors

    template <typename T>
        struct format_selector : T {};

    template <typename X, template <typename> class S>
        using is_same_format = has_traits<X, S>;

    template <typename X, template <typename> class S, typename R = bool>
        using enable_for_t = enable_if_t<is_same_format<X, S>::value, R>;

    // read

    template <typename It>
        struct from_bytes_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept { return !ec; }
        };

    template <typename X = CXON_DEFAULT_FORMAT, typename T, typename InIt, typename ...NaPa>
        inline auto     from_bytes(T& t, InIt b, InIt e, NaPa&&... p)       -> from_bytes_result<InIt>;
    template <typename X = CXON_DEFAULT_FORMAT, typename T, typename Iterable, typename ...NaPa>
        inline auto     from_bytes(T& t, const Iterable& i, NaPa&&... p)    -> from_bytes_result<decltype(std::begin(i))>;

    // write

    template <typename It>
        struct to_bytes_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept { return !ec; }
        };

    template <typename X = CXON_DEFAULT_FORMAT, typename T, typename OutIt, typename ...NaPa>
        inline auto     to_bytes(OutIt o, const T& t, NaPa&&... p)          -> enable_if_t<is_output_iterator<OutIt>::value, to_bytes_result<OutIt>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename T, typename Insertable, typename ...NaPa>
        inline auto     to_bytes(Insertable& i, const T& t, NaPa&&... p)    -> enable_if_t<is_back_insertable<Insertable>::value, to_bytes_result<decltype(std::begin(i))>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename T, typename FwIt, typename ...NaPa>
        inline auto     to_bytes(FwIt b, FwIt e, const T& t, NaPa&&... p)   -> to_bytes_result<FwIt>;

}

// implementation bridge //////////////////////////////////////////////////////

namespace cxon { // context

    template <typename X, typename ...NaPa> // ... named parameters
        struct context {
            using traits_type   = X;
            using napa_type     = decltype(napa::make_pack(std::declval<NaPa&&>()...));

            std::error_condition    ec;
            traits_type             tx;
            napa_type               px;

            context(NaPa&&... px) : ec(),
                                    tx(),
                                    px(napa::make_pack(std::forward<NaPa>(px)...))
            {
            }

            template <typename E>
                auto operator /(E e) noexcept -> enable_if_t<std::is_error_condition_enum<E>::value, bool> {
                    return ec = e, !ec;
                }
            operator bool() const noexcept { return !ec; }
        };

    template <typename X, typename ...P>
        using read_context  = context<X, P...>;
    template <typename X, typename ...P>
        using write_context = context<X, P...>;

    template <typename Cx>
        using napa_type = typename Cx::napa_type;

}

namespace cxon { // bridge

    namespace imp {
        template <typename E, typename T, typename R = E>
            using enable_if_same_t_ = enable_if_t< std::is_same<E, T>::value, R>;
        template <typename T>
            using enable_for_bridge_ = enable_if_same_t_<bool, T>;
    }

    // read value

    template <typename X, typename T, typename E = void>
        struct read {
            //template <typename II, typename Cx> static bool value(T& t, II& i, II e, Cx& cx);
        };

    template <typename X, typename T, typename II, typename Cx> // struct read
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> imp::enable_for_bridge_<decltype(read<X, T>::value(t, i, e, cx))>
        {
            return read<X, T>::value(t, i, e, cx);
        }
    template <typename X, typename T, typename II, typename Cx> // read_value static method of T
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> imp::enable_for_bridge_<decltype(T::template read_value<X>(t, i, e, cx))>
        {
            return T::template read_value<X>(t, i, e, cx);
        }
    template <typename X, typename T, typename II, typename Cx> // read_value method of T
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> imp::enable_for_bridge_<decltype(t.template read_value<X>(i, e, cx))>
        {
            return t.template read_value<X>(i, e, cx);
        }

    // write value

    template <typename X, typename T, typename E = void>
        struct write {
            //template <typename O, typename Cx> static bool value(O& o, const T& t, Cx& cx);
        };

    template <typename X, typename T, typename O, typename Cx> // struct write
        inline auto write_value(O& o, const T& t, Cx& cx)
            -> imp::enable_for_bridge_<decltype(write<X, T>::value(o, t, cx))>
        {
            return write<X, T>::value(o, t, cx);
        }
    template <typename X, typename T, typename O, typename Cx> // write_value static method of T
        inline auto write_value(O& o, const T& t, Cx& cx)
            -> imp::enable_for_bridge_<decltype(T::template write_value<X>(o, t, cx))>
        {
            return T::template write_value<X>(o, t, cx);
        }
    template <typename X, typename T, typename O, typename Cx> // write_value method of T
        inline auto write_value(O& o, const T& t, Cx& cx)
            -> imp::enable_for_bridge_<decltype(t.template write_value<X>(o, cx))>
        {
            return t.template write_value<X>(o, cx);
        }

}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { // interface

    // read

    namespace cnt {
        template <typename C> struct continuous;
    }

    namespace interface {

        template <typename X, typename T, typename II, typename ...NaPa>
            inline auto from_bytes(T& t, II b, II e, NaPa&&... p) -> from_bytes_result<II> {
                read_context<X, NaPa...> cx(std::forward<NaPa>(p)...);
                    bool const r = read_value<X>(t, b, e, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, b };
            }
        template <typename X, typename T, typename I, typename ...NaPa>
            inline auto from_bytes(T& t, const I& i, NaPa&&... p) -> from_bytes_result<decltype(std::begin(i))> {
                auto const c = cnt::continuous<I>::range(i);
                auto const r = interface::from_bytes<X>(t, c.first, c.second, std::forward<NaPa>(p)...);
                auto b = std::begin(i); std::advance(b, std::distance(c.first, r.end));
                return { r.ec, b };
            }

    }

    template <typename X, typename T, typename II, typename ...NaPa>
        inline auto from_bytes(T& t, II b, II e, NaPa&&... p) -> from_bytes_result<II> {
            return interface::from_bytes<X>(t, b, e, std::forward<NaPa>(p)...);
        }
    template <typename X, typename T, typename I, typename ...NaPa>
        inline auto from_bytes(T& t, const I& i, NaPa&&... p) -> from_bytes_result<decltype(std::begin(i))> {
            return interface::from_bytes<X>(t, i, std::forward<NaPa>(p)...);
        }

    // write

    namespace cnt {
        template <typename FI> struct range_container;
        template <typename FI> inline range_container<FI> make_range_container(FI f, FI l) noexcept;
    }

    namespace interface {

        template <typename X, typename T, typename OI, typename ...NaPa>
            inline auto to_bytes(OI o, const T& t, NaPa&&... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
                write_context<X, NaPa...> cx(std::forward<NaPa>(p)...);
                    bool const r = write_value<X>(o, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, o };
            }
        template <typename X, typename T, typename I, typename ...NaPa>
            inline auto to_bytes(I& i, const T& t, NaPa&&... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
                write_context<X, NaPa...> cx(std::forward<NaPa>(p)...);
                    auto const s = std::distance(std::begin(i), std::end(i));
                    bool const r = write_value<X>(i, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                    auto b = std::begin(i); std::advance(b, s);
                return { cx.ec, b };
            }
        template <typename X, typename T, typename FI, typename ...NaPa>
            inline auto to_bytes(FI b, FI e, const T& t, NaPa&&... p) -> to_bytes_result<FI> {
                write_context<X, NaPa...> cx(std::forward<NaPa>(p)...);
                    auto c = cnt::make_range_container(b, e);
                    bool const r = write_value<X>(c, t, cx); CXON_ASSERT(!r != !cx.ec, "result discrepant");
                return { cx.ec, c.end() };
            }

    }

    template <typename X, typename T, typename OI, typename ...NaPa>
        inline auto to_bytes(OI o, const T& t, NaPa&&... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
            return interface::to_bytes<X>(o, t, std::forward<NaPa>(p)...);
        }
    template <typename X, typename T, typename I, typename ...NaPa>
        inline auto to_bytes(I& i, const T& t, NaPa&&... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
            return interface::to_bytes<X>(i, t, std::forward<NaPa>(p)...);
        }
    template <typename X, typename T, typename FI, typename ...NaPa>
        inline auto to_bytes(FI b, FI e, const T& t, NaPa&&... p) -> to_bytes_result<FI> {
            return interface::to_bytes<X>(b, e, t, std::forward<NaPa>(p)...);
        }

}

#endif // CXON_CXON_HXX_
