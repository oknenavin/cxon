// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_NODE_HXX_
#define CXON_LIB_NODE_HXX_

// interface ///////////////////////////////////////////////////////////////////

#if defined(CXON_JSON_DEFINED) || defined(CXON_CBOR_DEFINED)

    namespace cxon { namespace node { // errors

        enum class error : int {
            ok,                         // no error
            invalid,                    // invalid input
            recursion_depth_exceeded    // recursion depth exceeded
        };

        struct error_category;
        std::error_condition make_error_condition(error e) noexcept;

    }}

    namespace cxon { namespace node { // context parameters

        CXON_PARAMETER(recursion_guard, unsigned);  // read/write
        CXON_PARAMETER(recursion_depth, unsigned);  // read/write: constexpr

    }}

#endif

#ifdef CXON_JSON_DEFINED

#   include "cxon/lang/json/node/node.hxx"

    namespace cxon { namespace node { namespace json { // context parameters

        CXON_PARAMETER(arbitrary_keys, bool);   // read/write: constexpr
        CXON_PARAMETER(extract_nans, bool);      // read/write: constexpr

    }}}

    namespace cxon {

        template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename InIt, typename ...CxPs>
            inline auto     from_bytes(json::basic_node<Tr>& t, InIt b, InIt e, CxPs... p)      -> from_bytes_result<InIt>;
        template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename Iterable, typename ...CxPs>
            inline auto     from_bytes(json::basic_node<Tr>& t, const Iterable& i, CxPs... p)   -> from_bytes_result<decltype(std::begin(i))>;

        template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename OutIt, typename ...CxPs>
            inline auto     to_bytes(OutIt o, const json::basic_node<Tr>& t, CxPs... p)         -> enable_if_t<is_output_iterator<OutIt>::value, to_bytes_result<OutIt>>;
        template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename Insertable, typename ...CxPs>
            inline auto     to_bytes(Insertable& i, const json::basic_node<Tr>& t, CxPs... p)   -> enable_if_t<is_back_insertable<Insertable>::value, to_bytes_result<decltype(std::begin(i))>>;
        template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename FwIt, typename ...CxPs>
            inline auto     to_bytes(FwIt b, FwIt e, const json::basic_node<Tr>& t, CxPs... p)  -> to_bytes_result<FwIt>;

    }

#endif

#ifdef CXON_CBOR_DEFINED

#   include "cxon/lang/cbor/node/node.hxx"

    namespace cxon {

        template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename InIt, typename ...CxPs>
            inline auto     from_bytes(cbor::basic_node<Tr>& t, InIt b, InIt e, CxPs... p)      -> from_bytes_result<InIt>;
        template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename Iterable, typename ...CxPs>
            inline auto     from_bytes(cbor::basic_node<Tr>& t, const Iterable& i, CxPs... p)   -> from_bytes_result<decltype(std::begin(i))>;

        template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename OutIt, typename ...CxPs>
            inline auto     to_bytes(OutIt o, const cbor::basic_node<Tr>& t, CxPs... p)         -> enable_if_t<is_output_iterator<OutIt>::value, to_bytes_result<OutIt>>;
        template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename Insertable, typename ...CxPs>
            inline auto     to_bytes(Insertable& i, const cbor::basic_node<Tr>& t, CxPs... p)   -> enable_if_t<is_back_insertable<Insertable>::value, to_bytes_result<decltype(std::begin(i))>>;
        template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename FwIt, typename ...CxPs>
            inline auto     to_bytes(FwIt b, FwIt e, const cbor::basic_node<Tr>& t, CxPs... p)  -> to_bytes_result<FwIt>;

    }

#endif

// implementation //////////////////////////////////////////////////////////////

#if defined(CXON_JSON_DEFINED) || defined(CXON_CBOR_DEFINED)

    namespace cxon { namespace node { // errors

        struct error_category : std::error_category {
            const char* name() const noexcept override {
                return "cxon/node";
            }
            std::string message(int ev) const override {
                switch (static_cast<error>(ev)) {
                    case error::ok:                         return "no error";
                    case error::invalid:                    return "invalid input";
                    case error::recursion_depth_exceeded:   return "recursion depth limit exceeded";
                    default:                                return "unknown error";
                }
            }
            static const error_category& value() {
                static error_category const v{};
                return v;
            }
        };

        std::error_condition make_error_condition(error e) noexcept {
            return { static_cast<int>(e), error_category::value() };
        }

    }}

    namespace std { // cxon/node errors
        template <> struct is_error_condition_enum<cxon::node::error> : true_type {};
    }

    namespace cxon { namespace node { namespace imp {

        template <typename Cx, bool G = recursion_guard::in<napa_type<Cx>>::value>
            struct scinc_ {
                Cx& cx;
                scinc_(Cx& cx) : cx(cx)  { ++recursion_guard::reference(cx.px); }
                ~scinc_()                { --recursion_guard::reference(cx.px); }
                bool check() const      { return recursion_guard::value(cx.px) < recursion_depth::constant<napa_type<Cx>>(64); }
            };
        template <typename Cx>
            struct scinc_<Cx, false> {
                scinc_(Cx&)              {}
                ~scinc_()                {}
                bool check() const      { return true; }
            };

    }}}

#endif

#define CXON_NODE_RG()\
    cxon::node::imp::scinc_<Cx> RG__(cx);\
    if (!RG__.check()) return cx/cxon::node::error::recursion_depth_exceeded

#ifdef CXON_JSON_DEFINED

    namespace cxon {

        template <typename X, typename Tr, typename II, typename ...CxPs>
            inline auto from_bytes(json::basic_node<Tr>& t, II b, II e, CxPs... p) -> from_bytes_result<II> {
                return interface::from_bytes<X>(t, b, e, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
            }
        template <typename X, typename Tr, typename I, typename ...CxPs>
            inline auto from_bytes(json::basic_node<Tr>& t, const I& i, CxPs... p) -> from_bytes_result<decltype(std::begin(i))> {
                return interface::from_bytes<X>(t, i, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
            }

        template <typename X, typename Tr, typename OI, typename ...CxPs>
            inline auto to_bytes(OI o, const json::basic_node<Tr>& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
                return interface::to_bytes<X>(o, t, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
            }
        template <typename X, typename Tr, typename I, typename ...CxPs>
            inline auto to_bytes(I& i, const json::basic_node<Tr>& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
                return interface::to_bytes<X>(i, t, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
            }
        template <typename X, typename Tr, typename FI, typename ...CxPs>
            inline auto to_bytes(FI b, FI e, const json::basic_node<Tr>& t, CxPs... p) -> to_bytes_result<FI> {
                return interface::to_bytes<X>(b, e, t, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
            }

    }

    namespace cxon {

        namespace imp {

            template <unsigned N, typename II, bool E = is_random_access_iterator<II>::value>
                struct buffer_;
            template <unsigned N, typename II>
                struct buffer_<N, II, false> {
                    buffer_(II) : b_(std::begin(s)), e_(std::end(s)) {}
                    bool put(char c)            { return b_ != e_ && (*b_ = c, ++b_); }
                    char*& begin() noexcept     { e_ = b_, b_ = std::begin(s); return b_; }
                    char* end() const noexcept  { return e_; }
                    char s[N];
                    char *b_, *e_;
                };
            template <unsigned N, typename II>
                struct buffer_<N, II, true> {
                    buffer_(II& b) : b_(b), e_(b) {}
                    constexpr bool put(char) const  { return true; }
                    II& begin() noexcept            { return b_; }
                    II end() const noexcept         { return e_; }
                    II b_; II& e_;
                };

            template <typename X, typename Tr, typename II, typename Cx>
                inline bool read_value_(json::basic_node<Tr>& n, typename json::basic_node<Tr>::real& v, II& i, II e, Cx& cx) {
                    unsigned u = 0;
                    unsigned long long U = 0;
                    unsigned d = 0;
                    buffer_<json::num_len_max::constant<napa_type<Cx>>(64), II> bf(i);
                    auto c = cio::peek(i, e);

                    bool s = c == '-';
                    if (s) {
                        if (!bf.put('-')) return cx/X::read_error::overflow;
                        c = cio::next(i, e);
                    }

                    if (c == '0') {
                        if (!bf.put('0')) return cx/X::read_error::overflow;
                        c = cio::next(i, e);
                    }
                    else if (c >= '1' && c <='9') {
                        if (!bf.put(c)) return cx/X::read_error::overflow;
                        u = static_cast<unsigned>(c - '0');
                        if (s)
                            for (c = cio::next(i, e); c >= '0' && c <= '9'; c = cio::next(i, e)) {
                                if (u >= 214748364) // 2^31 = 2147483648
                                    if (u != 214748364 || cio::next(i, e) > '8') {
                                        U = u; break;
                                    }
                                u = u * 10 + static_cast<unsigned>(c - '0');
                                if (!bf.put(c)) return cx/X::read_error::overflow;
                            }
                        else
                            for (c = cio::next(i, e); c >= '0' && c <= '9'; c = cio::next(i, e)) {
                                if (u >= 429496729) // 2^32 - 1 = 4294967295
                                    if (u != 429496729 || cio::next(i, e) > '5') {
                                        U = u; break;
                                    }
                                u = u * 10 + static_cast<unsigned>(c - '0');
                                if (!bf.put(c)) return cx/X::read_error::overflow;
                            }
                    }
                    else
                        return cx/X::read_error::floating_point_invalid;

                    if (U) {
                        if (s)
                            for (c = cio::peek(i, e); c >= '0' && c <= '9'; c = cio::next(i, e)) {
                                if (U >= 922337203685477580ULL) // 2^63 = 9223372036854775808
                                    if (U != 922337203685477580ULL || cio::next(i, e) > '8') {
                                        d = 1; break;
                                    }
                                U = U * 10 + static_cast<unsigned>(c - '0');
                                if (!bf.put(c)) return cx/X::read_error::overflow;
                            }
                        else
                            for (c = cio::peek(i, e); c >= '0' && c <= '9'; c = cio::next(i, e)) {
                                if (U >= 1844674407370955161ULL) // 2^64 - 1 = 18446744073709551615
                                    if (U != 1844674407370955161ULL || cio::next(i, e) > '5') {
                                        d = 1; break;
                                    }
                                U = U * 10 + static_cast<unsigned>(c - '0');
                                if (!bf.put(c)) return cx/X::read_error::overflow;
                            }
                    }

                    if (d) {
                        for (c = cio::peek(i, e); c >= '0' && c <= '9'; c = cio::next(i, e))
                            if (!bf.put(c)) return cx/X::read_error::overflow;
                    }
                    if (cio::peek(i, e) == '.') {
                        if (!bf.put('.')) return cx/X::read_error::overflow;
                        d = 1;
                        for (c = cio::next(i, e); c >= '0' && c <= '9'; c = cio::next(i, e))
                            if (!bf.put(c)) return cx/X::read_error::overflow;
                    }

                    c = cio::peek(i, e);
                    if (c == 'e' || c == 'E') {
                        if (!bf.put('e')) return cx/X::read_error::overflow;
                        d = 1, c = cio::next(i, e);
                        if (c == '-' || c == '+') {
                            if (!bf.put(c)) return cx/X::read_error::overflow;
                            c = cio::next(i, e);
                        }
                        for (c = cio::peek(i, e); c >= '0' && c <= '9'; c = cio::next(i, e))
                            if (!bf.put(c)) return cx/X::read_error::overflow;
                    }

                    if (d)
                        return read_value<X>(v, bf.begin(), bf.end(), cx);

                    using sint = typename json::basic_node<Tr>::sint;
                    using uint = typename json::basic_node<Tr>::uint;
                    U ?
                        s ? n.template imbue<sint>() = U, n.template get<sint>() = -n.template get<sint>() :
                            n.template imbue<uint>() = U :
                        s ? n.template imbue<sint>() = u, n.template get<sint>() = -n.template get<sint>() :
                            n.template imbue<uint>() = u
                    ;
                    return true;
                }

            template <typename C>
                struct nmst_;
            template <>
                struct nmst_<char> {
                    static constexpr char const*        pinf =  "inf";
                    static constexpr char const*        ninf = "-inf";
                    static constexpr char const*        nan =   "nan";
                };
#           if __cplusplus > 201703L /* C++20 */
                template <>
                    struct nmst_<char8_t> {
                        static constexpr char8_t const* pinf =  u8"inf";
                        static constexpr char8_t const* ninf = u8"-inf";
                        static constexpr char8_t const* nan =   u8"nan";
                    };
#           endif
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
                            bool const r = read_value<Y>(k, i, e, cx);
                                using real = typename json::basic_node<Tr>::real;
                                using symbol = typename json::basic_node<Tr>::string::value_type;
                                     if (k == cxon::imp::nmst_<symbol>::pinf) n.template imbue<real>() =  std::numeric_limits<real>::infinity();
                                else if (k == cxon::imp::nmst_<symbol>::ninf) n.template imbue<real>() = -std::numeric_limits<real>::infinity();
                                else if (k == cxon::imp::nmst_<symbol>::nan ) n.template imbue<real>() =  std::numeric_limits<real>::quiet_NaN();
                            return r;
                        }
                    template <typename II, typename Cx, typename Y = JSON<X>>
                        static bool read_key_(json::basic_node<Tr>& n, typename json::basic_node<Tr>::real& k, II& i, II e, Cx& cx) {
                            return cxon::imp::read_value_<Y>(n, k, i, e, cx);
                        }
                    template <typename II, typename Cx, typename Y = JSON<X>>
                        static auto key(json::basic_node<Tr>& t, II& i, II e, Cx& cx)
                            -> enable_if_t< node::json::arbitrary_keys::in<napa_type<Cx>>::value, bool>
                        {
                            cio::consume<Y>(i, e);
                            switch (cio::peek(i, e)) {
#                               define CXON_READ(T) read_key_(t, t.template imbue<typename json::basic_node<Tr>::T>(), i, e, cx)
                                    case '{'                : { CXON_NODE_RG();     return CXON_READ(object); }
                                    case '['                : { CXON_NODE_RG();     return CXON_READ(array);  }
                                    case '\"'               :                       return CXON_READ(string);
                                    case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
                                                            :                       return CXON_READ(real);
                                    case 't': case 'f'      :                       return CXON_READ(boolean);
                                    case 'n'                :                       return CXON_READ(null);
#                               undef CXON_READ
                            }
                            return cx/node::error::invalid;
                        }
                    template <typename II, typename Cx, typename Y = JSON<X>>
                        static auto key(json::basic_node<Tr>& t, II& i, II e, Cx& cx)
                            -> enable_if_t<!node::json::arbitrary_keys::in<napa_type<Cx>>::value, bool>
                        {
                            return cio::key::read_key<Y>(t.template imbue<typename json::basic_node<Tr>::string>(), i, e, cx);
                        }
                };

            template <typename X, typename Tr>
                struct write<JSON<X>, json::basic_node<Tr>> {
                    template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                        static auto write_key_(O& o, const T& t, Cx& cx)
                            -> enable_if_t< node::json::arbitrary_keys::in<napa_type<Cx>>::value, bool>
                        {
                            return write_value<Y>(o, t, cx);
                        }
                    template <typename O, typename T, typename Cx, typename Y = JSON<X>>
                        static auto write_key_(O& o, const T& t, Cx& cx)
                            -> enable_if_t<!node::json::arbitrary_keys::in<napa_type<Cx>>::value, bool>
                        {
                            return cio::key::write_key<Y>(o, t, cx);
                        }
                    template <typename O, typename Cx, typename Y = JSON<X>>
                        static bool key(O& o, const json::basic_node<Tr>& t, Cx& cx) {
                            using json::node_kind;
                            switch (t.kind()) {
#                               define CXON_WRITE(T) write_key_(o, t.template get<typename json::basic_node<Tr>::T>(), cx)
                                    case node_kind::object  :                       return CXON_WRITE(object);
                                    case node_kind::array   :                       return CXON_WRITE(array);
                                    case node_kind::string  :                       return CXON_WRITE(string);
                                    case node_kind::sint    :                       return CXON_WRITE(sint);
                                    case node_kind::uint    :                       return CXON_WRITE(uint);
                                    case node_kind::real    :                       return CXON_WRITE(real);
                                    case node_kind::boolean : { CXON_NODE_RG();     return CXON_WRITE(boolean); }
                                    case node_kind::null    : { CXON_NODE_RG();     return CXON_WRITE(null);   }
#                               undef CXON_WRITE
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
                        bool const r = read_value<Y>(v, i, e, cx);
                            using real = typename json::basic_node<Tr>::real;
                            using symbol = typename json::basic_node<Tr>::string::value_type;
                                 if (v == imp::nmst_<symbol>::pinf) n.template imbue<real>() =  std::numeric_limits<real>::infinity();
                            else if (v == imp::nmst_<symbol>::ninf) n.template imbue<real>() = -std::numeric_limits<real>::infinity();
                            else if (v == imp::nmst_<symbol>::nan ) n.template imbue<real>() =  std::numeric_limits<real>::quiet_NaN();
                        return r;
                    }
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool read_value_(json::basic_node<Tr>& n, typename json::basic_node<Tr>::real& v, II& i, II e, Cx& cx) {
                        return imp::read_value_<Y>(n, v, i, e, cx);
                    }
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(json::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                        cio::consume<Y>(i, e);
                        switch (cio::peek(i, e)) {
#                           define CXON_READ(T) read_value_(t, t.template imbue<typename json::basic_node<Tr>::T>(), i, e, cx)
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
            };

        template <typename X, typename Tr>
            struct write<JSON<X>, json::basic_node<Tr>> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const json::basic_node<Tr>& t, Cx& cx) {
                        using json::node_kind;
                        switch (t.kind()) {
#                           define CXON_WRITE(T) write_value<Y>(o, t.template get<typename json::basic_node<Tr>::T>(), cx)
                                case node_kind::object  : { CXON_NODE_RG();     return CXON_WRITE(object); }
                                case node_kind::array   : { CXON_NODE_RG();     return CXON_WRITE(array);  }
                                case node_kind::string  :                       return CXON_WRITE(string);
                                case node_kind::sint    :                       return CXON_WRITE(sint);
                                case node_kind::uint    :                       return CXON_WRITE(uint);
                                case node_kind::real    :                       return CXON_WRITE(real);
                                case node_kind::boolean :                       return CXON_WRITE(boolean);
                                case node_kind::null    :                       return CXON_WRITE(null);
#                           undef CXON_WRITE
                        }
                        return false; // LCOV_EXCL_LINE
                    }
            };

#       ifdef CXON_CBOR_DEFINED

            namespace cio { namespace key { // keys

                template <typename Tr> struct is_quoted<cbor::basic_node<Tr>> : std::true_type {};

                template <typename X, typename Tr>
                    struct write<JSON<X>, cbor::basic_node<Tr>> {
                        template <typename O, typename Cx, typename Y = JSON<X>>
                            static bool key(O& o, const cbor::basic_node<Tr>& t, Cx& cx) {
                                using cbor::node_kind;
                                switch (t.kind()) {
#                                   define CXON_WRITE(T) cio::key::write_key<Y>(o, t.template get<typename cbor::basic_node<Tr>::T>(), cx)
                                        case node_kind::sint        :                       return CXON_WRITE(sint);
                                        case node_kind::uint        :                       return CXON_WRITE(uint);
                                        case node_kind::bytes       :                       return CXON_WRITE(bytes);
                                        case node_kind::text        :                       return CXON_WRITE(text);
                                        case node_kind::array       : { CXON_NODE_RG();     return CXON_WRITE(array); }
                                        case node_kind::map         : { CXON_NODE_RG();     return CXON_WRITE(map);   }
                                        case node_kind::tag         :                       return CXON_WRITE(tag);
                                        case node_kind::boolean     :                       return CXON_WRITE(boolean);
                                        case node_kind::null        :                       return CXON_WRITE(null);
                                        case node_kind::undefined   :                       return CXON_WRITE(undefined);
                                        case node_kind::real        :                       return CXON_WRITE(real);
                                        case node_kind::simple      :                       return cio::key::write_key<Y>(o, t.template get<typename cbor::basic_node<Tr>::simple>().value, cx);
#                                   undef CXON_WRITE
                                }
                                return false; // LCOV_EXCL_LINE
                            }
                    };

            }}

            template <typename X, typename Tr>
                struct read<JSON<X>, cbor::basic_node<Tr>> {
                    template <typename II, typename Cx, typename Y = JSON<X>>
                        static bool value(cbor::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                            cio::consume<Y>(i, e);
                            switch (cio::peek(i, e)) {
#                               define CXON_READ(T) read_value<Y>(t.template imbue<typename cbor::basic_node<Tr>::T>(), i, e, cx)
                                    case '{'                    : { CXON_NODE_RG();     return CXON_READ(map); }
                                    case '['                    : { CXON_NODE_RG();     return CXON_READ(array);  }
                                    case '\"'                   :                       return CXON_READ(text);
                                    case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
                                                                :                       return CXON_READ(real);
                                    case 't': case 'f'          :                       return CXON_READ(boolean);
                                    case 'n'                    :                       return CXON_READ(null);
#                               undef CXON_READ
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
#                               define CXON_WRITE(T) write_value<Y>(o, t.template get<typename cbor::basic_node<Tr>::T>(), cx)
                                    case node_kind::sint        :                       return CXON_WRITE(sint);
                                    case node_kind::uint        :                       return CXON_WRITE(uint);
                                    case node_kind::bytes       :                       return CXON_WRITE(bytes);
                                    case node_kind::text        :                       return CXON_WRITE(text);
                                    case node_kind::array       : { CXON_NODE_RG();     return CXON_WRITE(array); }
                                    case node_kind::map         : { CXON_NODE_RG();     return CXON_WRITE(map);   }
                                    case node_kind::tag         :                       return CXON_WRITE(tag);
                                    case node_kind::boolean     :                       return CXON_WRITE(boolean);
                                    case node_kind::null        :                       return CXON_WRITE(null);
                                    case node_kind::undefined   :                       return CXON_WRITE(undefined);
                                    case node_kind::real        :                       return CXON_WRITE(real);
                                    case node_kind::simple      :                       return CXON_WRITE(simple);
#                               undef CXON_WRITE
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

        template <typename X, typename N, typename T, typename A>
            struct read<JSON<X>, cbor::taggle<N, T, A>> {
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(cbor::taggle<N, T, A>& t, II& i, II e, Cx& cx) {
                        II const o = i;
                        return  (/*read_value<Y>(t.tag, i, e) && */read_value<Y>(t.value, i, e)) || // TODO: keep as an object?
                                (cio::rewind(i, o), cx/json::read_error::unexpected)
                        ;
                    }
            };

        template <typename X, typename N, typename T, typename A>
            struct write<JSON<X>, cbor::taggle<N, T, A>> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const cbor::taggle<N, T, A>& t, Cx& cx) {
                        return /*write_value<Y>(o, t.tag, cx) && */write_value<Y>(o, t.value, cx); // TODO: keep as an object?
                    }
            };

#       endif

    }

#endif

#ifdef CXON_CBOR_DEFINED

    namespace cxon {

        template <typename X, typename Tr, typename II, typename ...CxPs>
            inline auto from_bytes(cbor::basic_node<Tr>& t, II b, II e, CxPs... p) -> from_bytes_result<II> {
                return interface::from_bytes<X>(t, b, e, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
            }
        template <typename X, typename Tr, typename I, typename ...CxPs>
            inline auto from_bytes(cbor::basic_node<Tr>& t, const I& i, CxPs... p) -> from_bytes_result<decltype(std::begin(i))> {
                return interface::from_bytes<X>(t, i, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
            }

        template <typename X, typename Tr, typename OI, typename ...CxPs>
            inline auto to_bytes(OI o, const cbor::basic_node<Tr>& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
                return interface::to_bytes<X>(o, t, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
            }
        template <typename X, typename Tr, typename I, typename ...CxPs>
            inline auto to_bytes(I& i, const cbor::basic_node<Tr>& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
                return interface::to_bytes<X>(i, t, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
            }
        template <typename X, typename Tr, typename FI, typename ...CxPs>
            inline auto to_bytes(FI b, FI e, const cbor::basic_node<Tr>& t, CxPs... p) -> to_bytes_result<FI> {
                return interface::to_bytes<X>(b, e, t, node::recursion_guard::set(0), std::forward<CxPs>(p)...);
            }

    }

    namespace cxon {

        template <typename X, typename Tr>
            struct read<CBOR<X>, cbor::basic_node<Tr>> {
                template <typename II, typename Cx, typename Y = CBOR<X>>
                    static bool value(cbor::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                        bio::byte const b = bio::peek(i, e);
                        switch (b & X::mjr) {
#                           define CXON_READ(T)             read_value<Y>(t.template imbue<typename cbor::basic_node<Tr>::T>(), i, e, cx)
                                case X::pint            :                           return CXON_READ(uint);
                                case X::nint            :                           return CXON_READ(sint);
                                case X::bstr            :                           return CXON_READ(bytes);
                                case X::tstr            :                           return CXON_READ(text);
                                case X::arr             : { CXON_NODE_RG();         return CXON_READ(array); }
                                case X::map             : { CXON_NODE_RG();         return CXON_READ(map); }
                                case X::tag             :                           return CXON_READ(tag);
                                case X::svn:
                                    switch (b) {
                                        case X::neg: case X::pos
                                                        :                           return CXON_READ(boolean);
                                        case X::nil     :                           return CXON_READ(null);
                                        case X::und     :                           return CXON_READ(undefined);
                                        case X::fp16: case X::fp32: case X::fp64
                                                        :                           return CXON_READ(real);
                                        default         :                           return CXON_READ(simple);
                                    }
#                           undef CXON_READ
                        }
                        return false; // LCOV_EXCL_LINE
                    }
            };

        template <typename X, typename Tr>
            struct write<CBOR<X>, cbor::basic_node<Tr>> {
                template <typename O, typename Cx, typename Y = CBOR<X>>
                    static bool value(O& o, const cbor::basic_node<Tr>& t, Cx& cx) {
                        using cbor::node_kind;
                        switch (t.kind()) {
#                           define CXON_WRITE(T) write_value<Y>(o, t.template get<typename cbor::basic_node<Tr>::T>(), cx)
                                case node_kind::sint        :                       return CXON_WRITE(sint);
                                case node_kind::uint        :                       return CXON_WRITE(uint);
                                case node_kind::bytes       :                       return CXON_WRITE(bytes);
                                case node_kind::text        :                       return CXON_WRITE(text);
                                case node_kind::array       : { CXON_NODE_RG();     return CXON_WRITE(array); }
                                case node_kind::map         : { CXON_NODE_RG();     return CXON_WRITE(map);   }
                                case node_kind::boolean     :                       return CXON_WRITE(boolean);
                                case node_kind::tag         :                       return CXON_WRITE(tag);
                                case node_kind::null        :                       return CXON_WRITE(null);
                                case node_kind::undefined   :                       return CXON_WRITE(undefined);
                                case node_kind::real        :                       return CXON_WRITE(real);
                                case node_kind::simple      :                       return CXON_WRITE(simple);
#                           undef CXON_WRITE
                        }
                        return false; // LCOV_EXCL_LINE
                    }
            };

        template <typename X>
            struct read<CBOR<X>, cbor::undefined> {
                template <typename II, typename Cx, typename Y = CBOR<X>>
                    static bool value(cbor::undefined&, II& i, II e, Cx& cx) {
                        II const o = i;
                        return  (bio::get(i, e) == Y::und) ||
                                (bio::rewind(i, o), cx/cbor::read_error::unexpected)
                        ;
                    }
            };

        template <typename X>
            struct write<CBOR<X>, cbor::undefined> {
                template <typename O, typename Cx, typename Y = CBOR<X>>
                    static bool value(O& o, const cbor::undefined&, Cx& cx) {
                        return bio::poke<Y>(o, Y::und, cx);
                    }
            };

        template <typename X, typename T>
            struct read<CBOR<X>, cbor::simple<T>> {
                template <typename II, typename Cx, typename Y = CBOR<X>>
                    static bool value(cbor::simple<T>& t, II& i, II e, Cx& cx) {
                        II const o = i;
                        auto const b = bio::get(i, e);
                        switch (b & X::mjr) {
                            case X::svn:
                                switch (b & X::mnr) {
                                    case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07: case 0x08: case 0x09:
                                    case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F: case 0x10: case 0x11: case 0x12: case 0x13:
                                        return  (t = T(b & X::mnr), true);
                                    case 0x18:
                                        return  (bio::get(t.value, 1, i, e)) ||
                                                (bio::rewind(i, o), cx/cbor::read_error::unexpected)
                                        ;
                                    case 0x14: case 0x15: case 0x16: case 0x17:
                                    case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1E: case 0x1F:
                                        break;
                                }
                        }
                        return bio::rewind(i, o), cx/cbor::read_error::unexpected;
                    }
            };

        template <typename X, typename T>
            struct write<CBOR<X>, cbor::simple<T>> {
                template <typename O, typename Cx, typename Y = CBOR<X>>
                    static bool value(O& o, const cbor::simple<T>& t, Cx& cx) {
                        switch (t) {
                            case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07: case 0x08: case 0x09:
                            case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F: case 0x10: case 0x11: case 0x12: case 0x13:
                                return  bio::poke<Y>(o, bio::byte(X::svn + t), cx);
                            default:
                                CXON_ASSERT(t <= 255, "invalid simple value");
                                return  bio::poke<Y>(o, bio::byte(X::svn + 0x18), cx) && bio::poke<Y>(o, t.value, 1, cx);
                            case 0x14: case 0x15: case 0x16: case 0x17:
                            case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1E: case 0x1F:
                                return  cx/cbor::write_error::argument_invalid;
                        }
                    }
            };

        template <typename X, typename N, typename T, typename A>
            struct read<CBOR<X>, cbor::taggle<N, T, A>> {
                template <typename II, typename Cx, typename Y = CBOR<X>>
                    static bool value(cbor::taggle<N, T, A>& t, II& i, II e, Cx& cx) {
                        return cbor::tag::read<Y>(t.tag, i, e, cx) && read_value<Y>(t.value, i, e, cx); // TODO: check simple-value values
                    }
            };

        template <typename X, typename N, typename T, typename A>
            struct write<CBOR<X>, cbor::taggle<N, T, A>> {
                template <typename O, typename Cx, typename Y = CBOR<X>>
                    static bool value(O& o, const cbor::taggle<N, T, A>& t, Cx& cx) {
                        return write_value<Y>(o, t.tag, cx) && write_value<Y>(o, t.value, cx); // TODO: check simple-value values
                    }
            };

#       ifdef CXON_JSON_DEFINED

            template <typename X, typename Tr>
                struct read<CBOR<X>, json::basic_node<Tr>> {
                    template <typename II, typename Cx, typename Y = CBOR<X>>
                        static bool value(json::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                            bio::byte const b = bio::peek(i, e);
                            switch (b & X::mjr) {
#                               define CXON_READ(T) read_value<Y>(t.template imbue<typename json::basic_node<Tr>::T>(), i, e, cx)
                                    case X::pint            :                       return CXON_READ(uint);
                                    case X::nint            :                       return CXON_READ(sint);
                                    case X::bstr            :                       return CXON_READ(array);
                                    case X::tstr            :                       return CXON_READ(string);
                                    case X::arr             : { CXON_NODE_RG();     return CXON_READ(array); }
                                    case X::map             : { CXON_NODE_RG();     return CXON_READ(object); }
                                    case X::tag             :                       return cbor::tag::read<Y>(i, e, cx) && value(t, i, e, cx);
                                    case X::svn:
                                        switch (b) {
                                            case X::neg: case X::pos
                                                            :                       return CXON_READ(boolean);
                                            case X::nil     :                       return CXON_READ(null);
                                            case X::und     : {
                                                cbor::undefined u;
                                                                                    return  (read_value<Y>(u, i, e, cx)) &&
                                                                                            (t.template imbue<typename json::basic_node<Tr>::null>(), true)
                                                                                    ;
                                            }
                                            case X::fp16: case X::fp32: case X::fp64
                                                            :                       return CXON_READ(real);
                                            default         : {
                                                typename cbor::node::simple s;
                                                                                    return  (read_value<Y>(s, i, e, cx)) &&
                                                                                            (t.template imbue<typename json::basic_node<Tr>::uint>() = s, true)
                                                                                    ;
                                            }
                                        }
#                               undef CXON_READ
                            }
                            return false; // LCOV_EXCL_LINE
                        }
                };

            template <typename X, typename Tr>
                struct write<CBOR<X>, json::basic_node<Tr>> {
                    template <typename O, typename Cx, typename Y = CBOR<X>>
                        static bool value(O& o, const json::basic_node<Tr>& t, Cx& cx) {
                            using json::node_kind;
                            switch (t.kind()) {
#                               define CXON_WRITE(T) write_value<Y>(o, t.template get<typename json::basic_node<Tr>::T>(), cx)
                                    case node_kind::object  : { CXON_NODE_RG();     return CXON_WRITE(object); }
                                    case node_kind::array   : { CXON_NODE_RG();     return CXON_WRITE(array);  }
                                    case node_kind::string  :                       return CXON_WRITE(string);
                                    case node_kind::sint    :                       return CXON_WRITE(sint);
                                    case node_kind::uint    :                       return CXON_WRITE(uint);
                                    case node_kind::real    :                       return CXON_WRITE(real);
                                    case node_kind::boolean :                       return CXON_WRITE(boolean);
                                    case node_kind::null    :                       return CXON_WRITE(null);
#                               undef CXON_WRITE
                            }
                            return false; // LCOV_EXCL_LINE
                        }
                };

#       endif

    }

#endif

#undef CXON_NODE_RG

#endif // CXON_LIB_NODE_HXX_
