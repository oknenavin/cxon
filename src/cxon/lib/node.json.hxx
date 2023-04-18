// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_NODE_JSON_HXX_
#define CXON_LIB_NODE_JSON_HXX_

#include "cxon/lang/json/node/node.hxx"

#ifdef CXON_CBOR_DEFINED
#   include "cxon/lang/cbor/node/node.hxx"
#endif

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace node { namespace json { // context parameters

    CXON_PARAMETER(extract_nans, bool); // read/write: constexpr

}}}

namespace cxon {

    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename InIt, typename ...NaPa>
        inline auto     from_bytes(json::basic_node<Tr>& t, InIt b, InIt e, NaPa&&... p)        -> from_bytes_result<InIt>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename Iterable, typename ...NaPa>
        inline auto     from_bytes(json::basic_node<Tr>& t, const Iterable& i, NaPa&&... p)     -> from_bytes_result<decltype(std::begin(i))>;

    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename OutIt, typename ...NaPa>
        inline auto     to_bytes(OutIt o, const json::basic_node<Tr>& t, NaPa&&... p)           -> enable_if_t<is_output_iterator<OutIt>::value, to_bytes_result<OutIt>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename Insertable, typename ...NaPa>
        inline auto     to_bytes(Insertable& i, const json::basic_node<Tr>& t, NaPa&&... p)     -> enable_if_t<is_back_insertable<Insertable>::value, to_bytes_result<decltype(std::begin(i))>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename FwIt, typename ...NaPa>
        inline auto     to_bytes(FwIt b, FwIt e, const json::basic_node<Tr>& t, NaPa&&... p)    -> to_bytes_result<FwIt>;

}

// implementation //////////////////////////////////////////////////////////////

namespace cxon {

    template <typename X, typename Tr, typename II, typename ...NaPa>
        inline auto from_bytes(json::basic_node<Tr>& t, II b, II e, NaPa&&... p) -> from_bytes_result<II> {
            return interface::from_bytes<X>(t, b, e, node::recursion_guard::set(0), std::forward<NaPa>(p)...);
        }
    template <typename X, typename Tr, typename I, typename ...NaPa>
        inline auto from_bytes(json::basic_node<Tr>& t, const I& i, NaPa&&... p) -> from_bytes_result<decltype(std::begin(i))> {
            return interface::from_bytes<X>(t, i, node::recursion_guard::set(0), std::forward<NaPa>(p)...);
        }

    template <typename X, typename Tr, typename OI, typename ...NaPa>
        inline auto to_bytes(OI o, const json::basic_node<Tr>& t, NaPa&&... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
            return interface::to_bytes<X>(o, t, node::recursion_guard::set(0), std::forward<NaPa>(p)...);
        }
    template <typename X, typename Tr, typename I, typename ...NaPa>
        inline auto to_bytes(I& i, const json::basic_node<Tr>& t, NaPa&&... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
            return interface::to_bytes<X>(i, t, node::recursion_guard::set(0), std::forward<NaPa>(p)...);
        }
    template <typename X, typename Tr, typename FI, typename ...NaPa>
        inline auto to_bytes(FI b, FI e, const json::basic_node<Tr>& t, NaPa&&... p) -> to_bytes_result<FI> {
            return interface::to_bytes<X>(b, e, t, node::recursion_guard::set(0), std::forward<NaPa>(p)...);
        }

}

#define CXON_NODE_RG()\
    cxon::node::imp::scinc_<Cx> RG__(cx); \
    if (!RG__.check()) return cx/cxon::node::error::recursion_depth_exceeded

namespace cxon {

    namespace imp {

        template <unsigned N, typename II, bool E = is_random_access_iterator<II>::value>
            struct buffer_;
        template <unsigned N, typename II>
            struct buffer_<N, II, false> {
                constexpr explicit buffer_(II) : c_(std::begin(s_)), e_(std::end(s_)) {}
                          bool        put(char c) noexcept  { return c_ != e_ && (*c_ = c, ++c_); }
                constexpr const char* beg() const noexcept  { return std::begin(s_); }
                constexpr const char* end() const noexcept  { return c_; }
                private:
                    char s_[N]; char *c_, *e_;
            };
        template <unsigned N, typename II>
            struct buffer_<N, II, true> {
                constexpr explicit buffer_(II& b) : b_(b), e_(b) {}
                constexpr bool put(char) const noexcept { return true; }
                constexpr II   beg() const noexcept     { return b_; }
                constexpr II   end() const noexcept     { return e_; }
                private:
                    II b_; II& e_;
            };

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_number_value_(T& t, II i, II e, Cx& cx) {
                return read_value<X>(t, i, e, cx);
            }

        template <typename T, typename C, bool S = std::is_signed<T>::value>
            struct plm_;
        template <typename T, typename C>
            struct plm_<T, C, false> {
                static constexpr T max =   std::numeric_limits<T>::max() / 10;
                static constexpr C fst = C(std::numeric_limits<T>::max() - max * 10) + '0';
            };
        template <typename T, typename C>
            struct plm_<T, C, true> {
                static constexpr T max = -(std::numeric_limits<T>::min() / 10);
                static constexpr C fst = C(max * -10 - std::numeric_limits<T>::min()) + '0';
            };

#       define CXON_STORE(c) if (!bf.put(c)) return cx/X::read_error::overflow
            template <typename X, typename N, typename II, typename Cx>
                inline bool read_number_value_(N& n, typename N::real& v, II& i, II e, Cx& cx) {
                    using ittp = typename std::iterator_traits<II>::value_type;
                    using sint = typename N::sint;
                    using uint = typename N::uint;
                    buffer_<json::num_len_max::constant<napa_type<Cx>>(64), II> bf(i);
                    uint u = 0;
                    int r = 0;
                    auto c = cio::peek(i, e);

                    bool const s = c == '-';
                    if (s) {
                        CXON_STORE('-');
                        c = cio::next(i, e);
                    }

                    if (c == '0') {
                        CXON_STORE('0');
                        ++i;
                    }
                    else if (c >= '1' && c <='9') {
                        if (s)
                            for ( ; c >= '0' && c <= '9'; c = cio::next(i, e)) {
                                if (u >= plm_<sint, ittp>::max)
                                    if (u != plm_<sint, ittp>::max || c > plm_<sint, ittp>::fst) {
                                        r = 1; break;
                                    }
                                u = u * 10 + static_cast<unsigned>(c - '0');
                                CXON_STORE(c);
                            }
                        else
                            for ( ; c >= '0' && c <= '9'; c = cio::next(i, e)) {
                                if (u >= plm_<uint, ittp>::max)
                                    if (u != plm_<uint, ittp>::max || c > plm_<uint, ittp>::fst) {
                                        r = 1; break;
                                    }
                                u = u * 10 + static_cast<unsigned>(c - '0');
                                CXON_STORE(c);
                            }
                    }
                    else if (c == 'I' || c == 'N') {
                        return read_value<X>(v, i, e, cx) && (!s || (v = -v, true));
                    }
                    else
                        return cx/X::read_error::floating_point_invalid;

                    if (r) {
                        for ( ; c >= '0' && c <= '9'; c = cio::next(i, e)) {
                            CXON_STORE(c);
                        }
                    }

                    c = cio::peek(i, e);

                    if (c == '.') {
                        CXON_STORE('.');
                        r = 1; c = cio::next(i, e);
                        for ( ; c >= '0' && c <= '9'; c = cio::next(i, e)) {
                            CXON_STORE(c);
                        }
                    }

                    if (c == 'e' || c == 'E') {
                        CXON_STORE('e');
                        r = 1, c = cio::next(i, e);
                        if (c == '-' || c == '+') {
                            CXON_STORE(c);
                            c = cio::next(i, e);
                        }
                        for ( ; c >= '0' && c <= '9'; c = cio::next(i, e)) {
                            CXON_STORE(c);
                        }
                    }

                    if (r) {
                        CXON_STORE('\0');
                        return read_number_value_<X>(v, bf.beg(), bf.end(), cx);
                    }
                    s ? n.template imbue<sint>() = -sint(1) - (u - 1) :
                        n.template imbue<uint>() = u
                    ;

                    return true;
                }
#       undef CXON_STORE

        template <typename C>
            struct nmst_;
        template <>
            struct nmst_<char> {
                static constexpr char const*        pinf =  "inf";
                static constexpr char const*        ninf = "-inf";
                static constexpr char const*        nan =   "nan";
            };
#       if defined(__cpp_char8_t)
            template <>
                struct nmst_<char8_t> {
                    static constexpr char8_t const* pinf =  u8"inf";
                    static constexpr char8_t const* ninf = u8"-inf";
                    static constexpr char8_t const* nan =   u8"nan";
                };
#       endif
        template <>
            struct nmst_<char16_t> {
                static constexpr char16_t const*    pinf =  u"inf";
                static constexpr char16_t const*    ninf = u"-inf";
                static constexpr char16_t const*    nan =   u"nan";
            };
        template <>
            struct nmst_<char32_t> {
                static constexpr char32_t const*    pinf =  U"inf";
                static constexpr char32_t const*    ninf = U"-inf";
                static constexpr char32_t const*    nan =   U"nan";
            };
        template <>
            struct nmst_<wchar_t> {
                static constexpr wchar_t const*     pinf =  L"inf";
                static constexpr wchar_t const*     ninf = L"-inf";
                static constexpr wchar_t const*     nan =   L"nan";
            };

        template <typename X, typename N, typename S, typename II, typename Cx>
            static bool read_string_value_extract_nans_(N& n, S& s, II& i, II e, Cx& cx) {
                if (read_value<X>(s, i, e, cx)) {
                    using real = typename N::real;
                    using symbol = typename S::value_type;
                            if (s == nmst_<symbol>::pinf) n.template imbue<real>() =  std::numeric_limits<real>::infinity();
                    else if (s == nmst_<symbol>::ninf) n.template imbue<real>() = -std::numeric_limits<real>::infinity();
                    else if (s == nmst_<symbol>::nan ) n.template imbue<real>() =  std::numeric_limits<real>::quiet_NaN();
                    return true;
                }
                return false;
            }

        template <typename X, typename O, typename T, typename Cx>
            static bool write_number_key_(O& o, const T& t, Cx& cx) {
                if (std::isinf(t))
                    return std::signbit(t) ?
                            write_value<X>(o, "-inf", cx) : write_value<X>(o, "inf", cx)
                    ;
                if (std::isnan(t))
                    return  write_value<X>(o, "nan", cx);
                return cio::key::write_key<X>(o, t, cx);
            }

    }

    namespace cio { namespace key { // keys

        template <typename X, typename Tr>
            struct read<JSON<X>, json::basic_node<Tr>> {
                template <typename V, typename II, typename Cx, typename Y = JSON<X>>
                    static bool read_key_(json::basic_node<Tr>&, V& k, II& i, II e, Cx& cx) {
                        return read_value<Y>(k, i, e, cx);
                    }
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static auto read_key_(json::basic_node<Tr>& n, typename json::basic_node<Tr>::string& k, II& i, II e, Cx& cx)
                        -> enable_if_t<node::json::extract_nans::in<napa_type<Cx>>::value, bool>
                    {
                        return cxon::imp::read_string_value_extract_nans_<Y>(n, k, i, e, cx);
                    }
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool read_key_(json::basic_node<Tr>& n, typename json::basic_node<Tr>::real& k, II& i, II e, Cx& cx) {
                        return cxon::imp::read_number_value_<Y>(n, k, i, e, cx);
                    }
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static auto key(json::basic_node<Tr>& t, II& i, II e, Cx& cx)
                        -> enable_if_t<!Y::quote_unquoted_keys, bool>
                    {
                        if (!cio::consume<Y>(i, e, cx))
                            return false;
                        switch (cio::peek(i, e)) {
#                           define CXON_IMBUE(T) t.template imbue<typename json::basic_node<Tr>::T>()
                                case '{'                : { CXON_NODE_RG();     return read_key_(t, CXON_IMBUE(object),     i, e, cx); }
                                case '['                : { CXON_NODE_RG();     return read_key_(t, CXON_IMBUE(array),      i, e, cx); }
                                case '"': case '\''     :                       return read_key_(t, CXON_IMBUE(string), i, e, cx);
                                case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case 'I': case 'N'
                                                        :                       return read_key_(t, CXON_IMBUE(real),       i, e, cx);
                                case 't': case 'f'      :                       return read_key_(t, CXON_IMBUE(boolean),    i, e, cx);
                                case 'n'                :                       return read_key_(t, CXON_IMBUE(null),       i, e, cx);
#                           undef CXON_IMBUE
                        }
                        return cx/node::error::invalid;
                    }
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static auto key(json::basic_node<Tr>& t, II& i, II e, Cx& cx)
                        -> enable_if_t< Y::quote_unquoted_keys, bool>
                    {
#                       define CXON_IMBUE(T) t.template imbue<typename json::basic_node<Tr>::T>()
                            CXON_IF_CONSTEXPR (Y::unquote_quoted_keys) {
                                if (!cio::consume<Y>(i, e, cx))
                                    return false;
                                switch (cio::peek(i, e)) {
                                        case '{': { CXON_NODE_RG();     return read_key_(t, CXON_IMBUE(object), i, e, cx); }
                                        case '[': { CXON_NODE_RG();     return read_key_(t, CXON_IMBUE(array),  i, e, cx); }
                                }
                            }
                            return cio::key::read_key<Y>(CXON_IMBUE(string), i, e, cx);
#                       undef CXON_IMBUE
                    }
            };

        template <typename X, typename Tr>
            struct write<JSON<X>, json::basic_node<Tr>> {
                template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                    static auto write_key_(O& o, const T& t, Cx& cx)
                        -> enable_if_t<!Y::quote_unquoted_keys, bool>
                    {
                        return write_value<Y>(o, t, cx);
                    }
                template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                    static auto write_key_(O& o, const T& t, Cx& cx)
                        -> enable_if_t< Y::quote_unquoted_keys && !std::is_floating_point<T>::value, bool>
                    {
                        return cio::key::write_key<Y>(o, t, cx);
                    }
                template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                    static auto write_key_(O& o, const T& t, Cx& cx)
                        -> enable_if_t< Y::quote_unquoted_keys &&  std::is_floating_point<T>::value, bool>
                    {
                        return cxon::imp::write_number_key_<Y>(o, t, cx);
                    }
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool key(O& o, const json::basic_node<Tr>& t, Cx& cx) {
                        using json::node_kind;
                        switch (t.kind()) {
#                           define CXON_GET(T) t.template get<typename json::basic_node<Tr>::T>()
                                case node_kind::object  : { CXON_NODE_RG();     return write_key_(o, CXON_GET(object),  cx); }
                                case node_kind::array   : { CXON_NODE_RG();     return write_key_(o, CXON_GET(array),   cx); }
                                case node_kind::string  :                       return write_key_(o, CXON_GET(string),  cx);
                                case node_kind::real    :                       return write_key_(o, CXON_GET(real),    cx);
                                case node_kind::sint    :                       return write_key_(o, CXON_GET(sint),    cx);
                                case node_kind::uint    :                       return write_key_(o, CXON_GET(uint),    cx);
                                case node_kind::boolean :                       return write_key_(o, CXON_GET(boolean), cx);
                                case node_kind::null    :                       return write_key_(o, CXON_GET(null),    cx);
#                           undef CXON_GET
                        }
                        return false; // LCOV_EXCL_LINE
                    }
            };

    }}

    template <typename X, typename Tr>
        struct read<JSON<X>, json::basic_node<Tr>> {
            template <typename V, typename II, typename Cx, typename Y = JSON<X>>
                static bool read_value_(json::basic_node<Tr>&, V& v, II& i, II e, Cx& cx) {
                    return read_value<Y>(v, i, e, cx);
                }
            template <typename II, typename Cx, typename Y = JSON<X>>
                static auto read_value_(json::basic_node<Tr>& n, typename json::basic_node<Tr>::string& v, II& i, II e, Cx& cx)
                    -> enable_if_t<node::json::extract_nans::in<napa_type<Cx>>::value, bool>
                {
                    return imp::read_string_value_extract_nans_<Y>(n, v, i, e, cx);
                }
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool read_value_(json::basic_node<Tr>& n, typename json::basic_node<Tr>::real& v, II& i, II e, Cx& cx) {
                    return imp::read_number_value_<Y>(n, v, i, e, cx);
                }
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(json::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<Y>(i, e, cx))
                        return false;
                    switch (cio::peek(i, e)) {
#                       define CXON_IMBUE(T) t.template imbue<typename json::basic_node<Tr>::T>()
                            case '{'                : { CXON_NODE_RG();     return read_value_(t, CXON_IMBUE(object),   i, e, cx); }
                            case '['                : { CXON_NODE_RG();     return read_value_(t, CXON_IMBUE(array),    i, e, cx); }
                            case '"': case '\''     :                       return read_value_(t, CXON_IMBUE(string),   i, e, cx);
                            case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case 'I': case 'N'
                                                    :                       return read_value_(t, CXON_IMBUE(real),     i, e, cx);
                            case 't': case 'f'      :                       return read_value_(t, CXON_IMBUE(boolean),  i, e, cx);
                            case 'n'                :                       return read_value_(t, CXON_IMBUE(null),     i, e, cx);
#                       undef CXON_IMBUE
                    }
                    return cx/node::error::invalid;
                }
        };

    template <typename X, typename Tr>
        struct write<JSON<X>, json::basic_node<Tr>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const json::basic_node<Tr>& t, Cx& cx) {
                    using json::node_kind;
                    switch (t.kind()) {
#                       define CXON_GET(T) t.template get<typename json::basic_node<Tr>::T>()
                            case node_kind::object  : { CXON_NODE_RG();     return write_value<Y>(o, CXON_GET(object),  cx); }
                            case node_kind::array   : { CXON_NODE_RG();     return write_value<Y>(o, CXON_GET(array),   cx); }
                            case node_kind::string  :                       return write_value<Y>(o, CXON_GET(string),  cx);
                            case node_kind::real    :                       return write_value<Y>(o, CXON_GET(real),    cx);
                            case node_kind::sint    :                       return write_value<Y>(o, CXON_GET(sint),    cx);
                            case node_kind::uint    :                       return write_value<Y>(o, CXON_GET(uint),    cx);
                            case node_kind::boolean :                       return write_value<Y>(o, CXON_GET(boolean), cx);
                            case node_kind::null    :                       return write_value<Y>(o, CXON_GET(null),    cx);
#                       undef CXON_GET
                    }
                    return false; // LCOV_EXCL_LINE
                }
        };

}

#ifdef CXON_CBOR_DEFINED

    namespace cxon {

        namespace cio { namespace key { // keys

            template <typename X, typename Tr>
                struct read<JSON<X>, cbor::basic_node<Tr>> {
                    template <typename V, typename II, typename Cx, typename Y = JSON<X>>
                        static bool read_key_(cbor::basic_node<Tr>&, V& k, II& i, II e, Cx& cx) {
                            return read_value<Y>(k, i, e, cx);
                        }
                    template <typename II, typename Cx, typename Y = JSON<X>>
                        static auto read_key_(cbor::basic_node<Tr>& n, typename cbor::basic_node<Tr>::text& k, II& i, II e, Cx& cx)
                            -> enable_if_t<node::json::extract_nans::in<napa_type<Cx>>::value, bool>
                        {
                            return cxon::imp::read_string_value_extract_nans_<Y>(n, k, i, e, cx);
                        }
                    template <typename II, typename Cx, typename Y = JSON<X>>
                        static bool read_key_(cbor::basic_node<Tr>& n, typename cbor::basic_node<Tr>::real& k, II& i, II e, Cx& cx) {
                            return cxon::imp::read_number_value_<Y>(n, k, i, e, cx);
                        }
                    template <typename II, typename Cx, typename Y = JSON<X>>
                        static auto key(cbor::basic_node<Tr>& t, II& i, II e, Cx& cx)
                            -> enable_if_t<!Y::quote_unquoted_keys, bool>
                        {
                            if (!cio::consume<Y>(i, e, cx))
                                return false;
                            switch (cio::peek(i, e)) {
#                               define CXON_IMBUE(T) t.template imbue<typename cbor::basic_node<Tr>::T>()
                                    case '{'                : { CXON_NODE_RG();     return read_key_(t, CXON_IMBUE(map),        i, e, cx); }
                                    case '['                : { CXON_NODE_RG();     return read_key_(t, CXON_IMBUE(array),      i, e, cx); }
                                    case '"': case '\''     :                       return read_key_(t, CXON_IMBUE(text),       i, e, cx);
                                    case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case 'I': case 'N'
                                                            :                       return read_key_(t, CXON_IMBUE(real),       i, e, cx);
                                    case 't': case 'f'      :                       return read_key_(t, CXON_IMBUE(boolean),    i, e, cx);
                                    case 'n'                :                       return read_key_(t, CXON_IMBUE(null),       i, e, cx);
#                               undef CXON_IMBUE
                            }
                            return cx/node::error::invalid;
                        }
                    template <typename II, typename Cx, typename Y = JSON<X>>
                        static auto key(cbor::basic_node<Tr>& t, II& i, II e, Cx& cx)
                            -> enable_if_t< Y::quote_unquoted_keys, bool>
                        {
#                           define CXON_IMBUE(T) t.template imbue<typename cbor::basic_node<Tr>::T>()
                                CXON_IF_CONSTEXPR (Y::unquote_quoted_keys) {
                                    if (!cio::consume<Y>(i, e, cx))
                                        return false;
                                    switch (cio::peek(i, e)) {
                                        case '{': { CXON_NODE_RG();     return read_key_(t, CXON_IMBUE(map),    i, e, cx); }
                                        case '[': { CXON_NODE_RG();     return read_key_(t, CXON_IMBUE(array),  i, e, cx); }
                                    }
                                }
                                return cio::key::read_key<Y>(CXON_IMBUE(text), i, e, cx);
#                           undef CXON_IMBUE
                        }
                };

            template <typename X, typename Tr>
                struct write<JSON<X>, cbor::basic_node<Tr>> {
                    template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                        static auto write_key_(O& o, const T& t, Cx& cx)
                            -> enable_if_t<!Y::quote_unquoted_keys, bool>
                        {
                            return write_value<Y>(o, t, cx);
                        }
                    template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                        static auto write_key_(O& o, const T& t, Cx& cx)
                            -> enable_if_t< Y::quote_unquoted_keys && !std::is_floating_point<T>::value, bool>
                        {
                            return cio::key::write_key<Y>(o, t, cx);
                        }
                    template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                        static auto write_key_(O& o, const T& t, Cx& cx)
                            -> enable_if_t< Y::quote_unquoted_keys &&  std::is_floating_point<T>::value, bool>
                        {
                            return cxon::imp::write_number_key_<Y>(o, t, cx);
                        }
                    template <typename O, typename Cx, typename Y = JSON<X>>
                        static bool key(O& o, const cbor::basic_node<Tr>& t, Cx& cx) {
                            using cbor::node_kind;
                            switch (t.kind()) {
#                               define CXON_GET(T) t.template get<typename cbor::basic_node<Tr>::T>()
                                    case node_kind::map         : { CXON_NODE_RG();     return write_key_(o, CXON_GET(map),         cx); }
                                    case node_kind::array       : { CXON_NODE_RG();     return write_key_(o, CXON_GET(array),       cx); }
                                    case node_kind::tag         : { CXON_NODE_RG();     return write_key_(o, CXON_GET(tag),         cx); }
                                    case node_kind::bytes       :                       return write_key_(o, CXON_GET(bytes),       cx);
                                    case node_kind::text        :                       return write_key_(o, CXON_GET(text),        cx);
                                    case node_kind::real        :                       return write_key_(o, CXON_GET(real),        cx);
                                    case node_kind::sint        :                       return write_key_(o, CXON_GET(sint),        cx);
                                    case node_kind::uint        :                       return write_key_(o, CXON_GET(uint),        cx);
                                    case node_kind::simple      :                       return cio::key::write_key<Y>(o, CXON_GET(simple).value, cx);
                                    case node_kind::boolean     :                       return write_key_(o, CXON_GET(boolean),     cx);
                                    case node_kind::null        :                       return write_key_(o, CXON_GET(null),        cx);
                                    case node_kind::undefined   :                       return write_key_(o, CXON_GET(undefined),   cx);
#                               undef CXON_GET
                            }
                            return false; // LCOV_EXCL_LINE
                        }
                };

        }}

        template <typename X, typename Tr>
            struct read<JSON<X>, cbor::basic_node<Tr>> {
                template <typename V, typename II, typename Cx, typename Y = JSON<X>>
                    static bool read_value_(cbor::basic_node<Tr>&, V& v, II& i, II e, Cx& cx) {
                        return read_value<Y>(v, i, e, cx);
                    }
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static auto read_value_(cbor::basic_node<Tr>& n, typename cbor::basic_node<Tr>::text& v, II& i, II e, Cx& cx)
                        -> enable_if_t<node::json::extract_nans::in<napa_type<Cx>>::value, bool>
                    {
                        return imp::read_string_value_extract_nans_<Y>(n, v, i, e, cx);
                    }
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool read_value_(cbor::basic_node<Tr>& n, typename cbor::basic_node<Tr>::real& v, II& i, II e, Cx& cx) {
                        return imp::read_number_value_<Y>(n, v, i, e, cx);
                    }
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(cbor::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                        if (!cio::consume<Y>(i, e, cx))
                            return false;
                        switch (cio::peek(i, e)) {
#                           define CXON_IMBUE(T) t.template imbue<typename cbor::basic_node<Tr>::T>()
                                case '{'                    : { CXON_NODE_RG();     return read_value_(t, CXON_IMBUE(map),      i, e, cx); }
                                case '['                    : { CXON_NODE_RG();     return read_value_(t, CXON_IMBUE(array),    i, e, cx); }
                                case '"': case '\''         :                       return read_value_(t, CXON_IMBUE(text),     i, e, cx);
                                case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case 'I': case 'N'
                                                            :                       return read_value_(t, CXON_IMBUE(real),     i, e, cx);
                                case 't': case 'f'          :                       return read_value_(t, CXON_IMBUE(boolean),  i, e, cx);
                                case 'n'                    :                       return read_value_(t, CXON_IMBUE(null),     i, e, cx);
#                           undef CXON_IMBUE
                        }
                        return cx/node::error::invalid;
                    }
            };

        template <typename X, typename Tr>
            struct write<JSON<X>, cbor::basic_node<Tr>> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const cbor::basic_node<Tr>& t, Cx& cx) {
                        using cbor::node_kind;
                        switch (t.kind()) {
#                           define CXON_GET(T) t.template get<typename cbor::basic_node<Tr>::T>()
#                           define CXON_WRITE(T) write_value<Y>(o, t.template get<typename cbor::basic_node<Tr>::T>(), cx)
                                case node_kind::map         : { CXON_NODE_RG();     return write_value<Y>(o, CXON_GET(map),         cx); }
                                case node_kind::array       : { CXON_NODE_RG();     return write_value<Y>(o, CXON_GET(array),       cx); }
                                case node_kind::tag         : { CXON_NODE_RG();     return write_value<Y>(o, CXON_GET(tag),         cx); }
                                case node_kind::bytes       :                       return write_value<Y>(o, CXON_GET(bytes),       cx);
                                case node_kind::text        :                       return write_value<Y>(o, CXON_GET(text),        cx);
                                case node_kind::real        :                       return write_value<Y>(o, CXON_GET(real),        cx);
                                case node_kind::sint        :                       return write_value<Y>(o, CXON_GET(sint),        cx);
                                case node_kind::uint        :                       return write_value<Y>(o, CXON_GET(uint),        cx);
                                case node_kind::simple      :                       return write_value<Y>(o, CXON_GET(simple),      cx);
                                case node_kind::boolean     :                       return write_value<Y>(o, CXON_GET(boolean),     cx);
                                case node_kind::null        :                       return write_value<Y>(o, CXON_GET(null),        cx);
                                case node_kind::undefined   :                       return write_value<Y>(o, CXON_GET(undefined),   cx);
#                           undef CXON_GET
                        }
                        return false; // LCOV_EXCL_LINE
                    }
            };

        template <typename X>
            struct read<JSON<X>, cbor::undefined> {
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(cbor::undefined&, II& i, II e, Cx& cx) {
                        II const o = i;
                        return cio::consume<Y>(i, e, cx) && (cio::consume<Y>(Y::id::nil, i, e) || (cio::rewind(i, o), cx/json::read_error::unexpected));
                    }
            };

        template <typename X>
            struct write<JSON<X>, cbor::undefined> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const cbor::undefined&, Cx& cx) {
                        return cio::poke<Y>(o, Y::id::nil, cx);
                    }
            };

        template <typename X, typename T>
            struct read<JSON<X>, cbor::simple<T>> {
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(cbor::simple<T>& t, II& i, II e, Cx& cx) {
                        II const o = i;
                        return  (read_value<Y>(t.value, i, e) || // TODO: check simple-value values
                                (cio::rewind(i, o), cx/json::read_error::unexpected))
                        ;
                    }
            };

        template <typename X, typename T>
            struct write<JSON<X>, cbor::simple<T>> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const cbor::simple<T>& t, Cx& cx) {
                        return write_value<Y>(o, t.value, cx); // TODO: check simple-value values
                    }
            };

        template <typename X, typename N, typename T>
            struct read<JSON<X>, cbor::taggle<N, T>> {
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(cbor::taggle<N, T>& t, II& i, II e, Cx& cx) {
                        II const o = i;
                        return  (/*read_value<Y>(t.tag, i, e) && */read_value<Y>(t.value, i, e)) || // TODO: keep as an object?
                                (cio::rewind(i, o), cx/json::read_error::unexpected)
                        ;
                    }
            };

        template <typename X, typename N, typename T>
            struct write<JSON<X>, cbor::taggle<N, T>> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const cbor::taggle<N, T>& t, Cx& cx) {
                        return /*write_value<Y>(o, t.tag, cx) && */write_value<Y>(o, t.value, cx); // TODO: keep as an object?
                    }
            };

    }

#endif

#undef CXON_NODE_RG

#endif // CXON_LIB_NODE_JSON_HXX_
