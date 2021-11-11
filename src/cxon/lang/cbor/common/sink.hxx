// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_SINK_HXX_
#define CXON_CBOR_SINK_HXX_

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor {

    template <typename C = void>    struct sink;
    template <typename C>           struct is_sink;

    template <typename X, typename S, typename II, typename Cx>
        inline bool sink_read(S& s, II& i, II e, Cx& cx);

    template <typename X, typename O, typename S, typename Cx>
        inline bool sink_write(O& o, const S& s, Cx& cx);

}}

namespace cxon {

    template <typename X, typename T>
        struct read<CBOR<X>, cbor::sink<T>>;
    template <typename X, typename T>
        struct write<CBOR<X>, cbor::sink<T>>;

}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor {

    template <typename C>
        struct sink {
            using value_type = typename C::value_type;
            void push_back(value_type v) { value.push_back(v); }
            C value;
        };
    template <>
        struct sink<void> {
            using value_type = bio::byte;
            void push_back(value_type) const {}
        };

    template <typename T>
        struct is_sink          : std::false_type {};
    template <typename T>
        struct is_sink<sink<T>> : std::true_type {};

    namespace imp {
        template <typename X, typename S>
            struct sink_reader_;
    }
    template <typename X, typename S, typename II, typename Cx>
        inline bool sink_read(S& s, II& i, II e, Cx& cx) {
            return imp::sink_reader_<X, S>::read(s, i, e, cx);
        }

    template <typename X, typename O, typename S, typename Cx>
        inline bool sink_write(O& o, const S& s, Cx& cx) {
            for (auto e : s.value) 
                if (!bio::poke<X>(o, e, cx))
                    return false;
            return true;
        }

}}

namespace cxon {

    template <typename X, typename T>
        struct read<CBOR<X>, cbor::sink<T>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(cbor::sink<T>& s, II& i, II e, Cx& cx) {
                    return cbor::sink_read<Y>(s, i, e, cx);
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, cbor::sink<T>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const cbor::sink<T>& s, Cx& cx) {
                    return cbor::sink_write<Y>(o, s, cx);
                }
        };

}

namespace cxon { namespace cbor { namespace imp {
    
    template <typename X, typename S>
        struct sink_reader_<CBOR<X>, S> {
            template<typename II, typename Cx, typename Y = CBOR<X>>
                static bool read(S& s, II& i, II e, Cx& cx) {
                    auto t = std::back_inserter(s);
                    auto const b = bio::peek(i, e);
                    unsigned const mjr = b & Y::mjr;
                    unsigned const mnr = b & Y::mnr;
                    switch (mjr) {
                        case Y::pint: case Y::nint: case Y::tag: case Y::svn: {
                            II const o = i;
                            switch (mnr) {
                                case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
                                case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
                                case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
                                    return  (bio::get(t, i, e));
                                case 0x18: case 0x19: case 0x1A: case 0x1B:
                                    return  (bio::get(t, i, e, 1 + (1 << (mnr - 0x18)))) ||
                                            (bio::rewind(i, o), cx/cbor::read_error::unexpected)
                                    ;
                                case 0x1C: case 0x1D: case 0x1E: case 0x1F:
                                    return  (bio::rewind(i, o), cx/cbor::read_error::unexpected);
                                default:                                            // LCOV_EXCL_LINE
                                    return  CXON_ASSERT(0, "unexpected"), false;    // LCOV_EXCL_LINE
                            }
                        }
                        case Y::bstr: case Y::tstr: {
                            II const o = i;
                            switch (mnr) {
                                case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
                                case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
                                case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
                                    return  (bio::get(t, i, e, 1 + mnr)) ||
                                            (bio::rewind(i, o), cx/cbor::read_error::unexpected)
                                    ;
                                case 0x18: case 0x19: case 0x1A: case 0x1B: {
                                    std::size_t n;
                                    return  (
                                                bio::get(t, i, e) &&
                                                bio::get(n, 1U << (mnr - 0x18), i, e) &&
                                                bio::poke(t, n, 1U << (mnr - 0x18)) &&
                                                bio::get(t, i, e, n)
                                            )   ||
                                            (   bio::rewind(i, o), cx/cbor::read_error::unexpected
                                            )
                                    ;
                                }
                                case 0x1F:
                                    do      (bio::get(t, i, e)); // should not fail
                                    while   (bio::peek(i, e) != Y::brk);
                                    return  (bio::get(t, i, e) || (bio::rewind(i, o), cx/cbor::read_error::unexpected));
                                case 0x1C: case 0x1D: case 0x1E:
                                    return  (bio::rewind(i, o), cx/cbor::read_error::unexpected);
                                default:                                            // LCOV_EXCL_LINE
                                    return  CXON_ASSERT(0, "unexpected"), false;    // LCOV_EXCL_LINE
                            }
                        }
                        case Y::arr: case Y::map: {
                            II const o = i;
                            switch (mnr) {
                                case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
                                case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
                                case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
                                    bio::get(t, i, e); // should not fail
                                    for (unsigned j = 0; j != mnr; ++j)
                                        if (!read_element(s, mjr, i, e, cx))
                                            return (bio::rewind(i, o), cx/cbor::read_error::unexpected);
                                    return  true;
                                case 0x18: case 0x19: case 0x1A: case 0x1B: {
                                    std::size_t n;
                                    if (!bio::get(t, i, e) || !bio::get(n, 1U << (mnr - 0x18), i, e) || !bio::poke(t, n, 1U << (mnr - 0x18)))
                                        return (bio::rewind(i, o), cx/cbor::read_error::unexpected);
                                    for (unsigned j = 0; j != n; ++j)
                                        if (!read_element(s, mjr, i, e, cx))
                                            return (bio::rewind(i, o), cx/cbor::read_error::unexpected);
                                    return  true;
                                }
                                case 0x1F:
                                    bio::get(t, i, e); // should not fail
                                    while (bio::peek(i, e) != Y::brk)
                                        if (!read_element(s, mjr, i, e, cx))
                                            return (bio::rewind(i, o), cx/cbor::read_error::unexpected);
                                    return  (bio::get(t, i, e) || (bio::rewind(i, o), cx/cbor::read_error::unexpected));
                                case 0x1C: case 0x1D: case 0x1E:
                                    return  (bio::rewind(i, o), cx/cbor::read_error::unexpected);
                                default:                                            // LCOV_EXCL_LINE
                                    return  CXON_ASSERT(0, "unexpected"), false;    // LCOV_EXCL_LINE
                            }
                        }
                        default:                                        // LCOV_EXCL_LINE
                            return CXON_ASSERT(0, "unexpected"), false; // LCOV_EXCL_LINE
                    }
                }
            template<typename II, typename Cx, typename Y = CBOR<X>>
                static bool read_element(S& s, bio::byte m, II& i, II e, Cx& cx) {
                    switch (m) {
                        case Y::map:    return  sink_read<Y>(s, i, e, cx) &&
                                                sink_read<Y>(s, i, e, cx);
                        case Y::arr:    return  sink_read<Y>(s, i, e, cx);
                        default:        return  CXON_ASSERT(0, "unexpected"), false; // LCOV_EXCL_LINE
                    }
                }
        };

}}}

#endif // CXON_CBOR_SINK_HXX_
