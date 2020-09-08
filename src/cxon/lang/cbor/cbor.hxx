// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_CBOR_HXX_
#define CXON_CBOR_CBOR_HXX_

#ifndef CXON_DEFAULT_FORMAT
#   define CXON_DEFAULT_FORMAT CBOR<>
#endif

#include "cxon/cxon.hxx"

#include "error.hxx"
#include "fundamental.hxx"

#include "cxon/lang/common/bio/bio.hxx"

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // format traits

    struct format_traits {

        using read_error = read_error;
        using write_error = write_error;
        
        static constexpr bio::byte  uint    = 0x00; // 0, unsigned integer
        static constexpr bio::byte  nint    = 0x20; // 1, negative integer
        static constexpr bio::byte  bstr    = 0x40; // 2, byte string
        static constexpr bio::byte  ustr    = 0x60; // 3, text string

        static constexpr bio::byte  arr     = 0x80; // 4, array of data items
        static constexpr bio::byte  map     = 0xA0; // 5, map of pairs of data items
        static constexpr bio::byte  tag     = 0xC0; // 6, optional semantic tagging of other major types

                                                    // 7, 0xE0-0xF3 - undefined
        static constexpr bio::byte  neg     = 0xF4; // 7, false
        static constexpr bio::byte  pos     = 0xF5; // 7, true
        static constexpr bio::byte  nil     = 0xF6; // 7, null
        static constexpr bio::byte  und     = 0xF7; // 7, undefined
                                                    // 7, 0xF8 - simple value, one byte follows

        static constexpr bio::byte  fp16    = 0xF9; // 7, half-precision float (two-byte IEEE 754)
        static constexpr bio::byte  fp32    = 0xFA; // 7, single-precision float (four-byte IEEE 754)
        static constexpr bio::byte  fp64    = 0xFB; // 7, double-precision float (eight-byte IEEE 754)

                                                    // 7, 0xFC-0xFE, reserved
        static constexpr bio::byte  brk     = 0xFF; // 7, break stop code for indefinite-length items

        static constexpr bool strict = false;
    };

}}

namespace cxon { // format selector

    template <typename T>
        struct CBOR : format_selector<T> {};

}

namespace cxon { namespace cbor { // named parameters

}}

#endif // CXON_CBOR_CBOR_HXX_
