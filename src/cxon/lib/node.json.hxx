// Copyright (c) 2017-2022 oknenavin.
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

    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename InIt, typename ...CxPs>
        inline auto     from_bytes(json::basic_node<Tr>& t, InIt b, InIt e, CxPs&&... p)        -> from_bytes_result<InIt>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename Iterable, typename ...CxPs>
        inline auto     from_bytes(json::basic_node<Tr>& t, const Iterable& i, CxPs&&... p)     -> from_bytes_result<decltype(std::begin(i))>;

    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename OutIt, typename ...CxPs>
        inline auto     to_bytes(OutIt o, const json::basic_node<Tr>& t, CxPs&&... p)           -> enable_if_t<is_output_iterator<OutIt>::value, to_bytes_result<OutIt>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename Insertable, typename ...CxPs>
        inline auto     to_bytes(Insertable& i, const json::basic_node<Tr>& t, CxPs&&... p)     -> enable_if_t<is_back_insertable<Insertable>::value, to_bytes_result<decltype(std::begin(i))>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename FwIt, typename ...CxPs>
        inline auto     to_bytes(FwIt b, FwIt e, const json::basic_node<Tr>& t, CxPs&&... p)    -> to_bytes_result<FwIt>;

}

// implementation //////////////////////////////////////////////////////////////

namespace cxon {

    template <typename X, typename Tr, typename II, typename ...CxPs>
        inline auto from_bytes(json::basic_node<Tr>& t, II b, II e, CxPs&&... p) -> from_bytes_result<II> {
            return interface::from_bytes<X>(t, b, e, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
        }
    template <typename X, typename Tr, typename I, typename ...CxPs>
        inline auto from_bytes(json::basic_node<Tr>& t, const I& i, CxPs&&... p) -> from_bytes_result<decltype(std::begin(i))> {
            return interface::from_bytes<X>(t, i, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
        }

    template <typename X, typename Tr, typename OI, typename ...CxPs>
        inline auto to_bytes(OI o, const json::basic_node<Tr>& t, CxPs&&... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
            return interface::to_bytes<X>(o, t, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
        }
    template <typename X, typename Tr, typename I, typename ...CxPs>
        inline auto to_bytes(I& i, const json::basic_node<Tr>& t, CxPs&&... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
            return interface::to_bytes<X>(i, t, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
        }
    template <typename X, typename Tr, typename FI, typename ...CxPs>
        inline auto to_bytes(FI b, FI e, const json::basic_node<Tr>& t, CxPs&&... p) -> to_bytes_result<FI> {
            return interface::to_bytes<X>(b, e, t, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
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

#       define CXON_STORE(c) if (!bf.put(c)) return cx/X::read_error::overflow
            template <typename X, typename N, typename II, typename Cx>
                inline bool read_number_value_(N& n, typename N::real& v, II& i, II e, Cx& cx) {
                    unsigned u = 0;
                    unsigned long long U = 0;
                    unsigned d = 0;
                    buffer_<json::num_len_max::constant<napa_type<Cx>>(64), II> bf(i);
                    auto c = cio::peek(i, e);

                    bool s = c == '-';
                    if (s) {
                        CXON_STORE('-');
                        c = cio::next(i, e);
                    }

                    if (c == '0') {
                        CXON_STORE('0');
                        ++i;
                    }
                    else if (c >= '1' && c <='9') {
                        CXON_STORE(c);
                        u = static_cast<unsigned>(c - '0');
                        if (s)
                            for (c = cio::next(i, e); c >= '0' && c <= '9'; c = cio::next(i, e)) {
                                if (u >= 214748364) // 2^31 = 2147483648
                                    if (u != 214748364 || c > '8') {
                                        U = u; break;
                                    }
                                u = u * 10 + static_cast<unsigned>(c - '0');
                                CXON_STORE(c);
                            }
                        else
                            for (c = cio::next(i, e); c >= '0' && c <= '9'; c = cio::next(i, e)) {
                                if (u >= 429496729) // 2^32 - 1 = 4294967295
                                    if (u != 429496729 || c > '5') {
                                        U = u; break;
                                    }
                                u = u * 10 + static_cast<unsigned>(c - '0');
                                CXON_STORE(c);
                            }
                    }
                    else
                        return cx/X::read_error::floating_point_invalid;

                    if (U) {
                        if (s)
                            for (c = cio::peek(i, e); c >= '0' && c <= '9'; c = cio::next(i, e)) {
                                if (U >= 922337203685477580ULL) // 2^63 = 9223372036854775808
                                    if (U != 922337203685477580ULL || c > '8') {
                                        d = 1; break;
                                    }
                                U = U * 10 + static_cast<unsigned>(c - '0');
                                CXON_STORE(c);
                            }
                        else
                            for (c = cio::peek(i, e); c >= '0' && c <= '9'; c = cio::next(i, e)) {
                                if (U >= 1844674407370955161ULL) // 2^64 - 1 = 18446744073709551615
                                    if (U != 1844674407370955161ULL || c > '5') {
                                        d = 1; break;
                                    }
                                U = U * 10 + static_cast<unsigned>(c - '0');
                                CXON_STORE(c);
                            }
                    }

                    if (d) {
                        for (c = cio::peek(i, e); c >= '0' && c <= '9'; c = cio::next(i, e)) {
                            CXON_STORE(c);
                        }
                    }
                    if (cio::peek(i, e) == '.') {
                        CXON_STORE('.');
                        d = 1;
                        for (c = cio::next(i, e); c >= '0' && c <= '9'; c = cio::next(i, e)) {
                            CXON_STORE(c);
                        }
                    }

                    c = cio::peek(i, e);
                    if (c == 'e' || c == 'E') {
                        CXON_STORE('e');
                        d = 1, c = cio::next(i, e);
                        if (c == '-' || c == '+') {
                            CXON_STORE(c);
                            c = cio::next(i, e);
                        }
                        for ( ; c >= '0' && c <= '9'; c = cio::next(i, e)) {
                            CXON_STORE(c);
                        }
                    }

                    if (d) {
                        CXON_STORE('\0');
                        return read_number_value_<X>(v, bf.beg(), bf.end(), cx);
                    }

                    using sint = typename N::sint;
                    using uint = typename N::uint;
                    U ?
                        s ? n.template imbue<sint>() = U, n.template get<sint>() = -n.template get<sint>() :
                            n.template imbue<uint>() = U :
                        s ? n.template imbue<sint>() = u, n.template get<sint>() = -n.template get<sint>() :
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
#       if __cplusplus > 201703L /* C++20 */
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
                        -> enable_if_t< Y::unquoted_keys, bool>
                    {
                        cio::consume<Y>(i, e);
                        switch (cio::peek(i, e)) {
#                           define CXON_READ(T) read_key_(t, t.template imbue<typename json::basic_node<Tr>::T>(), i, e, cx)
                                case '{'                : { CXON_NODE_RG();     return CXON_READ(object); }
                                case '['                : { CXON_NODE_RG();     return CXON_READ(array);  }
                                case '\"'               :                       return CXON_READ(string);
                                case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
                                                        :                       return CXON_READ(real);
                                case 't': case 'f'      :                       return CXON_READ(boolean);
                                case 'n'                :                       return CXON_READ(null);
#                           undef CXON_READ
                        }
                        return cx/node::error::invalid;
                    }
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static auto key(json::basic_node<Tr>& t, II& i, II e, Cx& cx)
                        -> enable_if_t<!Y::unquoted_keys, bool>
                    {
                        return cio::key::read_key<Y>(t.template imbue<typename json::basic_node<Tr>::string>(), i, e, cx);
                    }
            };

        template <typename X, typename Tr>
            struct write<JSON<X>, json::basic_node<Tr>> {
                template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                    static auto write_key_(O& o, const T& t, Cx& cx)
                        -> enable_if_t< Y::unquoted_keys, bool>
                    {
                        return write_value<Y>(o, t, cx);
                    }
                template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                    static auto write_key_(O& o, const T& t, Cx& cx)
                        -> enable_if_t<!Y::unquoted_keys && !std::is_floating_point<T>::value, bool>
                    {
                        return cio::key::write_key<Y>(o, t, cx);
                    }
                template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                    static auto write_key_(O& o, const T& t, Cx& cx)
                        -> enable_if_t<!Y::unquoted_keys &&  std::is_floating_point<T>::value, bool>
                    {
                        return cxon::imp::write_number_key_<Y>(o, t, cx);
                    }
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool key(O& o, const json::basic_node<Tr>& t, Cx& cx) {
                        using json::node_kind;
                        switch (t.kind()) {
#                           define CXON_WRITE(T) write_key_(o, t.template get<typename json::basic_node<Tr>::T>(), cx)
                                case node_kind::object  : { CXON_NODE_RG();     return CXON_WRITE(object); }
                                case node_kind::array   : { CXON_NODE_RG();     return CXON_WRITE(array);  }
                                case node_kind::string  :                       return CXON_WRITE(string);
                                case node_kind::real    :                       return CXON_WRITE(real);
                                case node_kind::sint    :                       return CXON_WRITE(sint);
                                case node_kind::uint    :                       return CXON_WRITE(uint);
                                case node_kind::boolean :                       return CXON_WRITE(boolean);
                                case node_kind::null    :                       return CXON_WRITE(null);
#                           undef CXON_WRITE
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
                    cio::consume<Y>(i, e);
                    switch (cio::peek(i, e)) {
#                       define CXON_READ(T) read_value_(t, t.template imbue<typename json::basic_node<Tr>::T>(), i, e, cx)
                            case '{'                : { CXON_NODE_RG();     return CXON_READ(object); }
                            case '['                : { CXON_NODE_RG();     return CXON_READ(array);  }
                            case '\"'               :                       return CXON_READ(string);
                            case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
                                                    :                       return CXON_READ(real);
                            case 't': case 'f'      :                       return CXON_READ(boolean);
                            case 'n'                :                       return CXON_READ(null);
#                       undef CXON_READ
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
#                       define CXON_WRITE(T) write_value<Y>(o, t.template get<typename json::basic_node<Tr>::T>(), cx)
                            case node_kind::object  : { CXON_NODE_RG();     return CXON_WRITE(object); }
                            case node_kind::array   : { CXON_NODE_RG();     return CXON_WRITE(array);  }
                            case node_kind::string  :                       return CXON_WRITE(string);
                            case node_kind::real    :                       return CXON_WRITE(real);
                            case node_kind::sint    :                       return CXON_WRITE(sint);
                            case node_kind::uint    :                       return CXON_WRITE(uint);
                            case node_kind::boolean :                       return CXON_WRITE(boolean);
                            case node_kind::null    :                       return CXON_WRITE(null);
#                       undef CXON_WRITE
                    }
                    return false; // LCOV_EXCL_LINE
                }
        };

}

#ifdef CXON_CBOR_DEFINED

    namespace cxon {

        namespace cio { namespace key { // keys

            template <typename Tr> struct is_quoted<cbor::basic_node<Tr>> : std::true_type {};

            template <typename X, typename Tr>
                struct write<JSON<X>, cbor::basic_node<Tr>> {
                    template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                        static auto write_key_(O& o, const T& t, Cx& cx)
                            -> enable_if_t< Y::unquoted_keys, bool>
                        {
                            return write_value<Y>(o, t, cx);
                        }
                    template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                        static auto write_key_(O& o, const T& t, Cx& cx)
                            -> enable_if_t<!Y::unquoted_keys && !std::is_floating_point<T>::value, bool>
                        {
                            return cio::key::write_key<Y>(o, t, cx);
                        }
                    template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                        static auto write_key_(O& o, const T& t, Cx& cx)
                            -> enable_if_t<!Y::unquoted_keys &&  std::is_floating_point<T>::value, bool>
                        {
                            return cxon::imp::write_number_key_<Y>(o, t, cx);
                        }
                    template <typename O, typename Cx, typename Y = JSON<X>>
                        static bool key(O& o, const cbor::basic_node<Tr>& t, Cx& cx) {
                            using cbor::node_kind;
                            switch (t.kind()) {
#                               define CXON_WRITE(T) write_key_(o, t.template get<typename cbor::basic_node<Tr>::T>(), cx)
                                    case node_kind::map         : { CXON_NODE_RG();     return CXON_WRITE(map);   }
                                    case node_kind::array       : { CXON_NODE_RG();     return CXON_WRITE(array); }
                                    case node_kind::tag         : { CXON_NODE_RG();     return CXON_WRITE(tag);   }
                                    case node_kind::bytes       :                       return CXON_WRITE(bytes);
                                    case node_kind::text        :                       return CXON_WRITE(text);
                                    case node_kind::real        :                       return CXON_WRITE(real);
                                    case node_kind::sint        :                       return CXON_WRITE(sint);
                                    case node_kind::uint        :                       return CXON_WRITE(uint);
                                    case node_kind::simple      :                       return cio::key::write_key<Y>(o, t.template get<typename cbor::basic_node<Tr>::simple>().value, cx);
                                    case node_kind::boolean     :                       return CXON_WRITE(boolean);
                                    case node_kind::null        :                       return CXON_WRITE(null);
                                    case node_kind::undefined   :                       return CXON_WRITE(undefined);
#                               undef CXON_WRITE
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
                        cio::consume<Y>(i, e);
                        switch (cio::peek(i, e)) {
#                           define CXON_READ(T) read_value_(t, t.template imbue<typename cbor::basic_node<Tr>::T>(), i, e, cx)
                                case '{'                    : { CXON_NODE_RG();     return CXON_READ(map); }
                                case '['                    : { CXON_NODE_RG();     return CXON_READ(array);  }
                                case '\"'                   :                       return CXON_READ(text);
                                case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
                                                            :                       return CXON_READ(real);
                                case 't': case 'f'          :                       return CXON_READ(boolean);
                                case 'n'                    :                       return CXON_READ(null);
#                           undef CXON_READ
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
#                           define CXON_WRITE(T) write_value<Y>(o, t.template get<typename cbor::basic_node<Tr>::T>(), cx)
                                case node_kind::map         : { CXON_NODE_RG();     return CXON_WRITE(map);   }
                                case node_kind::array       : { CXON_NODE_RG();     return CXON_WRITE(array); }
                                case node_kind::tag         : { CXON_NODE_RG();     return CXON_WRITE(tag);   }
                                case node_kind::bytes       :                       return CXON_WRITE(bytes);
                                case node_kind::text        :                       return CXON_WRITE(text);
                                case node_kind::real        :                       return CXON_WRITE(real);
                                case node_kind::sint        :                       return CXON_WRITE(sint);
                                case node_kind::uint        :                       return CXON_WRITE(uint);
                                case node_kind::simple      :                       return CXON_WRITE(simple);
                                case node_kind::boolean     :                       return CXON_WRITE(boolean);
                                case node_kind::null        :                       return CXON_WRITE(null);
                                case node_kind::undefined   :                       return CXON_WRITE(undefined);
#                           undef CXON_WRITE
                        }
                        return false; // LCOV_EXCL_LINE
                    }
            };

        template <typename X>
            struct read<JSON<X>, cbor::undefined> {
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(cbor::undefined&, II& i, II e, Cx& cx) {
                        II const o = i;
                        cio::consume<Y>(i, e);
                        return  (cio::consume<Y>(Y::id::nil, i, e) ||
                                (cio::rewind(i, o), cx/json::read_error::unexpected))
                        ;
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
