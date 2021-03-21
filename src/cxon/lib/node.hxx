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

    namespace cxon { namespace node { namespace bits {

        template <typename Cx, bool G = recursion_guard::in<napa_type<Cx>>::value>
            struct scinc {
                Cx& cx;
                scinc(Cx& cx) : cx(cx)  { ++recursion_guard::reference(cx.px); }
                ~scinc()                { --recursion_guard::reference(cx.px); }
                bool check() const      { return recursion_guard::value(cx.px) < recursion_depth::constant<napa_type<Cx>>(64); }
            };
        template <typename Cx>
            struct scinc<Cx, false> {
                scinc(Cx&)              {}
                ~scinc()                {}
                bool check() const      { return true; }
            };

    }}}

#endif

#define CXON_NODE_RG()\
    cxon::node::bits::scinc<Cx> RG__(cx);\
    if (!RG__.check()) return cx|cxon::node::error::recursion_depth_exceeded, false

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

        namespace cio { namespace key { // keys

            template <typename X, typename Tr>
                struct read<JSON<X>, json::basic_node<Tr>> {
                    template <typename II, typename Cx, typename Y = JSON<X>>
                        static bool key(json::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                            cio::consume<X>(i, e);
                            switch (cio::peek(i, e)) {
    #                           define CXON_READ(T) cio::key::read_key<Y>(t.template imbue<typename json::basic_node<Tr>::T>(), i, e, cx)
                                    //case '{'                : { CXON_NODE_RG();     return CXON_READ(object); }
                                    //case '['                : { CXON_NODE_RG();     return CXON_READ(array);  }
                                    case '\"'               :                       return CXON_READ(string);
                                    //case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
                                    //                        :                       return CXON_READ(number);
                                    //case 't': case 'f'      :                       return CXON_READ(boolean);
                                    //case 'n'                :                       return CXON_READ(null);
    #                           undef CXON_READ
                            }
                            return cx|node::error::invalid, false;
                        }
                };

            template <typename X, typename Tr>
                struct write<JSON<X>, json::basic_node<Tr>> {
                    template <typename O, typename Cx, typename Y = JSON<X>>
                        static bool key(O& o, const json::basic_node<Tr>& t, Cx& cx) {
                            using json::node_kind;
                            switch (t.kind()) {
#                               define CXON_WRITE(T) cio::key::write_key<Y>(o, t.template get<typename json::basic_node<Tr>::T>(), cx)
                                    case node_kind::object  :                       return CXON_WRITE(object);
                                    case node_kind::array   :                       return CXON_WRITE(array);
                                    case node_kind::string  :                       return CXON_WRITE(string);
                                    case node_kind::number  :                       return CXON_WRITE(number);
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
                template <typename II, typename Cx, typename Y = JSON<X>>
                    static bool value(json::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                        cio::consume<X>(i, e);
                        switch (cio::peek(i, e)) {
#                           define CXON_READ(T) read_value<Y>(t.template imbue<typename json::basic_node<Tr>::T>(), i, e, cx)
                                case '{'                : { CXON_NODE_RG();     return CXON_READ(object); }
                                case '['                : { CXON_NODE_RG();     return CXON_READ(array);  }
                                case '\"'               :                       return CXON_READ(string);
                                case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
                                                        :                       return CXON_READ(number);
                                case 't': case 'f'      :                       return CXON_READ(boolean);
                                case 'n'                :                       return CXON_READ(null);
#                           undef CXON_READ
                        }
                        return cx|node::error::invalid, false;
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
                                case node_kind::number  :                       return CXON_WRITE(number);
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
                                        case node_kind::boolean     :                       return CXON_WRITE(boolean);
                                        case node_kind::null        :                       return CXON_WRITE(null);
                                        case node_kind::undefined   :                       return CXON_WRITE(undefined);
                                        case node_kind::real        :                       return CXON_WRITE(real);
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
                            cio::consume<X>(i, e);
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
                            return cx|node::error::invalid, false;
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
                                    case node_kind::boolean     :                       return CXON_WRITE(boolean);
                                    case node_kind::null        :                       return CXON_WRITE(null);
                                    case node_kind::undefined   :                       return CXON_WRITE(undefined);
                                    case node_kind::real        :                       return CXON_WRITE(real);
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
                        cio::consume<X>(i, e);
                        return  (cio::consume<X>(X::id::nil, i, e) ||
                                (cio::rewind(i, o), cx|cio::read_error::unexpected))
                        ;
                    }
            };

        template <typename X>
            struct write<JSON<X>, cbor::undefined> {
                template <typename O, typename Cx, typename Y = JSON<X>>
                    static bool value(O& o, const cbor::undefined&, Cx& cx) {
                        return cio::poke<X>(o, X::id::nil, cx);
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
                        size_t tag;
                        if (!cbor::tag::read<X>(tag, i, e, cx))
                            return false;
                        bio::byte const b = bio::peek(i, e);
                        switch (b & X::mjr) {
#                           define CXON_READ(T)             read_value<Y>(t.template imbue<typename cbor::basic_node<Tr>::T>(), i, e, cx)
#                           define CXON_READ_ARR(T, tag)    cbor::cnt::read_array<Y>(t.template imbue<typename cbor::basic_node<Tr>::T>(), tag, i, e, cx)
                                case X::pint            :                           return CXON_READ(uint);
                                case X::nint            :                           return CXON_READ(sint);
                                case X::bstr            :                           return CXON_READ(bytes);
                                case X::tstr            :                           return CXON_READ(text);
                                case X::arr             : { CXON_NODE_RG();         return CXON_READ_ARR(array, tag); }
                                case X::map             : { CXON_NODE_RG();         return CXON_READ_ARR(map, tag); }
                                case X::tag             : { CXON_NODE_RG();         return value(t, i, e, cx); }
                                case X::svn:
                                    switch (b) {
                                        case X::neg: case X::pos
                                                        :                           return CXON_READ(boolean);
                                        case X::nil     :                           return CXON_READ(null);
                                        case X::und     :                           return CXON_READ(undefined);
                                        case X::fp16: case X::fp32: case X::fp64
                                                        :                           return CXON_READ(real);
                                    }
#                           undef CXON_READ_ARR
#                           undef CXON_READ
                        }
                        return cx|node::error::invalid, false;
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
                                case node_kind::null        :                       return CXON_WRITE(null);
                                case node_kind::undefined   :                       return CXON_WRITE(undefined);
                                case node_kind::real        :                       return CXON_WRITE(real);
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
                        return  (bio::get(i, e) == X::und) ||
                                (bio::rewind(i, o), cx|cbor::read_error::unexpected)
                        ;
                    }
            };

        template <typename X>
            struct write<CBOR<X>, cbor::undefined> {
                template <typename O, typename Cx, typename Y = CBOR<X>>
                    static bool value(O& o, const cbor::undefined&, Cx& cx) {
                        return bio::poke<X>(o, X::und, cx);
                    }
            };

#       ifdef CXON_JSON_DEFINED

            template <typename X, typename Tr>
                struct read<CBOR<X>, json::basic_node<Tr>> {
                    template <typename II, typename Cx, typename Y = CBOR<X>>
                        static bool value(json::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                            if (!cbor::tag::read<X>(i, e, cx))
                                return false;
                            bio::byte const b = bio::peek(i, e);
                            switch (b & X::mjr) {
#                               define CXON_READ(T) read_value<Y>(t.template imbue<typename json::basic_node<Tr>::T>(), i, e, cx)
                                    case X::pint            :                       return CXON_READ(number);
                                    case X::nint            :                       return CXON_READ(number);
                                    case X::bstr            :                       return CXON_READ(array);
                                    case X::tstr            :                       return CXON_READ(string);
                                    case X::arr             : { CXON_NODE_RG();     return CXON_READ(array); }
                                    case X::map             : { CXON_NODE_RG();     return CXON_READ(object); }
                                    case X::tag             : { CXON_NODE_RG();     return value(t, i, e, cx); }
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
                                                            :                       return CXON_READ(number);
                                        }
#                               undef CXON_READ
                            }
                            return cx|node::error::invalid, false;
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
                                    case node_kind::number  :                       return CXON_WRITE(number);
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
