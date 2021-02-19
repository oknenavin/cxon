// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMMON_TAG_HXX_
#define CXON_CBOR_COMMON_TAG_HXX_

#include "cxon/lang/common/bio/bio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace tag {

    template <typename X, typename II, typename Cx>
        inline bool read(II& i, II e, Cx& cx);
    template <typename X, typename II, typename Cx>
        inline bool read(size_t& t, II& i, II e, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace tag {

    template <typename X, typename II, typename Cx>
        inline bool read(II& i, II e, Cx& cx) {
            auto b = bio::peek(i, e);
            switch (b & X::mjr) {
                case X::tag: {
                    II const o = i;
                    switch (b = b & X::mnr) {
                        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
                        case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
                        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
                            return  (bio::get(i, e), true);
                        case 0x18: case 0x19: case 0x1A: case 0x1B:
                            return  (bio::advance(i, e, 1 + (1 << (b - 0x18)))) ||
                                    (bio::rewind(i, o), cx|cbor::read_error::unexpected)
                            ;
                        case 0x1C: case 0x1D: case 0x1E: case 0x1F:
                            return  (bio::rewind(i, o), cx|cbor::read_error::tag_invalid);
                        default:                                            // LCOV_EXCL_LINE
                            return  CXON_ASSERT(0, "unexpected"), false;    // LCOV_EXCL_LINE
                    }
                }
                default:
                            return  true;
            }
        }

    template <typename X, typename II, typename Cx>
        inline bool read(size_t& t, II& i, II e, Cx& cx) {
                auto b = bio::peek(i, e);
                switch (b & X::mjr) {
                    case X::tag: {
                        II const o = i;
                        switch (b = b & X::mnr) {
                            case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
                            case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
                            case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
                                return  (bio::get(i, e), t = size_t(b), true);
                            case 0x18: case 0x19: case 0x1A: case 0x1B:
                                return  (bio::get(i, e), bio::get(t, 1 << (b - 0x18), i, e)) ||
                                        (bio::rewind(i, o), cx|cbor::read_error::integer_invalid)
                                ;
                            case 0x1C: case 0x1D: case 0x1E: case 0x1F:
                                return  (bio::rewind(i, o), cx|cbor::read_error::tag_invalid);
                            default:                                            // LCOV_EXCL_LINE
                                return  CXON_ASSERT(0, "unexpected"), false;    // LCOV_EXCL_LINE
                        }
                    }
                    default:
                                return  (t = 0xFFFFFFFFFFFFFFFF, true);
                }
        }

}}}

#endif // CXON_CBOR_COMMON_TAG_HXX_
