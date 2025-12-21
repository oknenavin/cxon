// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CXON_CBOR_CBOR_HXX_
#define CXON_CBOR_CBOR_HXX_

#include "error.hxx"
#include "fundamental.hxx"
#include "compound.hxx"
#include "enum.hxx"
#include "class.hxx"

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // format traits

    struct format_traits {
        using read_error = cbor::read_error;
        using write_error = cbor::write_error;
        
        static constexpr bio::byte  mjr     = 0xE0; // major
        static constexpr bio::byte  mnr     = 0x1F; // minor

        static constexpr bio::byte  pint    = 0x00; // 0, unsigned integer
        static constexpr bio::byte  nint    = 0x20; // 1, negative integer
        static constexpr bio::byte  bstr    = 0x40; // 2, byte string
        static constexpr bio::byte  tstr    = 0x60; // 3, text string

        static constexpr bio::byte  arr     = 0x80; // 4, array of data items
        static constexpr bio::byte  map     = 0xA0; // 5, map of pairs of data items
        static constexpr bio::byte  tag     = 0xC0; // 6, optional semantic tagging of other major types

        static constexpr bio::byte  svn     = 0xE0; // 7, floating-point numbers and simple values
                                                    // 7, 0xE0-0xF3, unassigned
        static constexpr bio::byte  neg     = 0xF4; // 7, false
        static constexpr bio::byte  pos     = 0xF5; // 7, true
        static constexpr bio::byte  nil     = 0xF6; // 7, null
        static constexpr bio::byte  und     = 0xF7; // 7, undefined
                                                    // 7, 0xF8 - simple value (value 32..255 in following byte)
        static constexpr bio::byte  fp16    = 0xF9; // 7, half-precision float (two-byte IEEE 754)
        static constexpr bio::byte  fp32    = 0xFA; // 7, single-precision float (four-byte IEEE 754)
        static constexpr bio::byte  fp64    = 0xFB; // 7, double-precision float (eight-byte IEEE 754)
                                                    // 7, 0xFC-0xFE, unassigned (reserved?)
        static constexpr bio::byte  brk     = 0xFF; // 7, break stop code for indefinite length items

        static constexpr bool strict = false;

        using tag_type = std::uint_fast64_t;
    };

}}

namespace cxon { // format selector

    template <typename T>
        struct CBOR : format_selector<T> {};

}

namespace cxon { namespace cbor { // named parameters

    using allocator = allocator;            // read: T*
    using ids_len_max = bio::ids_len_max;   // read: constexpr: object key

}}

#endif // CXON_CBOR_CBOR_HXX_
