// Copyright (c) 2017-2024 oknenavin.
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

    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename InIt, typename ...NaPa>
        inline auto     from_bytes(cbor::basic_node<Tr>& t, InIt b, InIt e, NaPa&&... p)        -> from_bytes_result<InIt>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename Iterable, typename ...NaPa>
        inline auto     from_bytes(cbor::basic_node<Tr>& t, const Iterable& i, NaPa&&... p)     -> from_bytes_result<decltype(std::begin(i))>;

    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename OutIt, typename ...NaPa>
        inline auto     to_bytes(OutIt o, const cbor::basic_node<Tr>& t, NaPa&&... p)           -> enable_if_t<is_output_iterator<OutIt>::value, to_bytes_result<OutIt>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename Insertable, typename ...NaPa>
        inline auto     to_bytes(Insertable& i, const cbor::basic_node<Tr>& t, NaPa&&... p)     -> enable_if_t<is_back_insertable<Insertable>::value, to_bytes_result<decltype(std::begin(i))>>;
    template <typename X = CXON_DEFAULT_FORMAT, typename Tr, typename FwIt, typename ...NaPa>
        inline auto     to_bytes(FwIt b, FwIt e, const cbor::basic_node<Tr>& t, NaPa&&... p)    -> to_bytes_result<FwIt>;

}

// implementation //////////////////////////////////////////////////////////////

namespace cxon {

    template <typename X, typename Tr, typename II, typename ...NaPa>
        inline auto from_bytes(cbor::basic_node<Tr>& t, II b, II e, NaPa&&... p) -> from_bytes_result<II> {
            return interface::from_bytes<X>(t, b, e, node::recursion_guard::set(0), std::forward<NaPa>(p)...);
        }
    template <typename X, typename Tr, typename I, typename ...NaPa>
        inline auto from_bytes(cbor::basic_node<Tr>& t, const I& i, NaPa&&... p) -> from_bytes_result<decltype(std::begin(i))> {
            return interface::from_bytes<X>(t, i, node::recursion_guard::set(0), std::forward<NaPa>(p)...);
        }

    template <typename X, typename Tr, typename OI, typename ...NaPa>
        inline auto to_bytes(OI o, const cbor::basic_node<Tr>& t, NaPa&&... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
            return interface::to_bytes<X>(o, t, node::recursion_guard::set(0), std::forward<NaPa>(p)...);
        }
    template <typename X, typename Tr, typename I, typename ...NaPa>
        inline auto to_bytes(I& i, const cbor::basic_node<Tr>& t, NaPa&&... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
            return interface::to_bytes<X>(i, t, node::recursion_guard::set(0), std::forward<NaPa>(p)...);
        }
    template <typename X, typename Tr, typename FI, typename ...NaPa>
        inline auto to_bytes(FI b, FI e, const cbor::basic_node<Tr>& t, NaPa&&... p) -> to_bytes_result<FI> {
            return interface::to_bytes<X>(b, e, t, node::recursion_guard::set(0), std::forward<NaPa>(p)...);
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
#                       define CXON_IMBUE(T) t.template imbue<typename cbor::basic_node<Tr>::T>()
                            case X::pint            :                           return read_value<Y>(CXON_IMBUE(uint),      i, e, cx);
                            case X::nint            :                           return read_value<Y>(CXON_IMBUE(sint),      i, e, cx);
                            case X::bstr            :                           return read_value<Y>(CXON_IMBUE(bytes),     i, e, cx);
                            case X::tstr            :                           return read_value<Y>(CXON_IMBUE(text),      i, e, cx);
                            case X::arr             : { CXON_NODE_RG();         return read_value<Y>(CXON_IMBUE(array),     i, e, cx); }
                            case X::map             : { CXON_NODE_RG();         return read_value<Y>(CXON_IMBUE(map),       i, e, cx); }
                            case X::tag             : { CXON_NODE_RG();         return read_value<Y>(CXON_IMBUE(tag),       i, e, cx); }
                            case X::svn:
                                switch (b) {
                                    case X::neg: case X::pos
                                                    :                           return read_value<Y>(CXON_IMBUE(boolean),   i, e, cx);
                                    case X::nil     :                           return read_value<Y>(CXON_IMBUE(null),      i, e, cx);
                                    case X::und     :                           return read_value<Y>(CXON_IMBUE(undefined), i, e, cx);
                                    case X::fp16: case X::fp32: case X::fp64
                                                    :                           return read_value<Y>(CXON_IMBUE(real),      i, e, cx);
                                    case X::brk     :                           return cx/node::error::invalid;
                                    default         :                           return read_value<Y>(CXON_IMBUE(simple),    i, e, cx);
                                }
#                       undef CXON_IMBUE
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
#                       define CXON_GET(T) t.template get<typename cbor::basic_node<Tr>::T>()
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
#                       undef CXON_GET
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
                    switch (b & Y::mjr) {
                        case Y::svn:
                            switch (b & X::mnr) {
                                case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07: case 0x08: case 0x09:
                                case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F: case 0x10: case 0x11: case 0x12: case 0x13:
                                    return  (t = T(b & Y::mnr), true);
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
#                           define CXON_IMBUE(T) t.template imbue<typename json::basic_node<Tr>::T>()
                                case X::pint            :                       return read_value<Y>(CXON_IMBUE(uint),          i, e, cx);
                                case X::nint            :                       return read_value<Y>(CXON_IMBUE(sint),          i, e, cx);
                                case X::bstr            :                       return read_value<Y>(CXON_IMBUE(array),         i, e, cx);
                                case X::tstr            :                       return read_value<Y>(CXON_IMBUE(string),        i, e, cx);
                                case X::arr             : { CXON_NODE_RG();     return read_value<Y>(CXON_IMBUE(array),         i, e, cx); }
                                case X::map             : { CXON_NODE_RG();     return read_value<Y>(CXON_IMBUE(object),        i, e, cx); }
                                case X::tag             :                       return cbor::tag::read<Y>(i, e, cx) && value(t, i, e, cx);
                                case X::svn:
                                    switch (b) {
                                        case X::neg: case X::pos
                                                        :                       return read_value<Y>(CXON_IMBUE(boolean),       i, e, cx);
                                        case X::nil     :                       return read_value<Y>(CXON_IMBUE(null),          i, e, cx);
                                        case X::und     : {
                                            cbor::undefined u;                  return read_value<Y>(u, i, e, cx) && (CXON_IMBUE(null), true);
                                        }
                                        case X::fp16: case X::fp32: case X::fp64
                                                        :                       return read_value<Y>(CXON_IMBUE(real),          i, e, cx);
                                        case X::brk     :                       return cx/node::error::invalid;
                                        default         : {
                                            typename cbor::node::simple s;      return read_value<Y>(s, i, e, cx) && (CXON_IMBUE(uint) = s, true);
                                        }
                                    }
#                           undef CXON_IMBUE
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
#                           define CXON_GET(T) t.template get<typename json::basic_node<Tr>::T>()
                                case node_kind::object  : { CXON_NODE_RG();     return write_value<Y>(o, CXON_GET(object),  cx); }
                                case node_kind::array   : { CXON_NODE_RG();     return write_value<Y>(o, CXON_GET(array),   cx); }
                                case node_kind::string  :                       return write_value<Y>(o, CXON_GET(string),  cx);
                                case node_kind::real    :                       return write_value<Y>(o, CXON_GET(real),    cx);
                                case node_kind::sint    :                       return write_value<Y>(o, CXON_GET(sint),    cx);
                                case node_kind::uint    :                       return write_value<Y>(o, CXON_GET(uint),    cx);
                                case node_kind::boolean :                       return write_value<Y>(o, CXON_GET(boolean), cx);
                                case node_kind::null    :                       return write_value<Y>(o, CXON_GET(null),    cx);
#                           undef CXON_GET
                        }
                        return false; // LCOV_EXCL_LINE
                    }
            };

    }

#endif // CXON_JSON_DEFINED

#undef CXON_NODE_RG

#endif // CXON_LIB_NODE_CBOR_HXX_
