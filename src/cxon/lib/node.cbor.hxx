// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_NODE_CBOR_HXX_
#define CXON_LIB_NODE_CBOR_HXX_

#include "cxon/lang/cbor/node/node.hxx"

#ifdef CXON_JSON_DEFINED
#   include "cxon/lang/json/node/node.hxx"
#endif

// interface ///////////////////////////////////////////////////////////////////

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

// implementation //////////////////////////////////////////////////////////////

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

#define CXON_NODE_RG()\
    cxon::node::imp::scinc_<Cx> RG__(cx);\
    if (!RG__.check()) return cx/cxon::node::error::recursion_depth_exceeded

namespace cxon {

    template <typename X, typename Tr>
        struct read<CBOR<X>, cbor::basic_node<Tr>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(cbor::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                    bio::byte const b = bio::peek(i, e);
                    switch (b & X::mjr) {
#                       define CXON_READ(T)             read_value<Y>(t.template imbue<typename cbor::basic_node<Tr>::T>(), i, e, cx)
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
                                    case X::brk     :                           return cx/node::error::invalid;
                                    default         :                           return CXON_READ(simple);
                                }
#                       undef CXON_READ
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
#                       define CXON_WRITE(T) write_value<Y>(o, t.template get<typename cbor::basic_node<Tr>::T>(), cx)
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
#                       undef CXON_WRITE
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

    template <typename X, typename N, typename T>
        struct read<CBOR<X>, cbor::taggle<N, T>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(cbor::taggle<N, T>& t, II& i, II e, Cx& cx) {
                    return cbor::tag::read<Y>(t.tag, i, e, cx) && read_value<Y>(t.value, i, e, cx); // TODO: check simple-value values
                }
        };

    template <typename X, typename N, typename T>
        struct write<CBOR<X>, cbor::taggle<N, T>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const cbor::taggle<N, T>& t, Cx& cx) {
                    return write_value<Y>(o, t.tag, cx) && write_value<Y>(o, t.value, cx); // TODO: check simple-value values
                }
        };

}

#ifdef CXON_JSON_DEFINED

    namespace  cxon {

        template <typename X, typename Tr>
            struct read<CBOR<X>, json::basic_node<Tr>> {
                template <typename II, typename Cx, typename Y = CBOR<X>>
                    static bool value(json::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                        bio::byte const b = bio::peek(i, e);
                        switch (b & X::mjr) {
#                           define CXON_READ(T) read_value<Y>(t.template imbue<typename json::basic_node<Tr>::T>(), i, e, cx)
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
                                        case X::brk     :                       return cx/node::error::invalid;
                                        default         : {
                                            typename cbor::node::simple s;
                                                                                return  (read_value<Y>(s, i, e, cx)) &&
                                                                                        (t.template imbue<typename json::basic_node<Tr>::uint>() = s, true)
                                                                                ;
                                        }
                                    }
#                           undef CXON_READ
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
#                           define CXON_WRITE(T) write_value<Y>(o, t.template get<typename json::basic_node<Tr>::T>(), cx)
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

    }

#endif

#undef CXON_NODE_RG

#endif // CXON_LIB_NODE_CBOR_HXX_