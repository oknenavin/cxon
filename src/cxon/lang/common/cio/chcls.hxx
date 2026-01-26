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

#ifndef CXON_CIO_CHCLS_HXX_
#define CXON_CIO_CHCLS_HXX_

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace chr { // character classes

    template <typename X>
        struct is; // ctrl, print, space, digit2, digit8, digit10, digit16, alpha, alnum, invalid

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace chr { // character classes

    enum : unsigned char {
        CTRL    = 1 << 0,       // control
        VSBL    = 1 << 1,       // printable character
        CXSP    = 1 << 2,       // space
        JSSP    = 1 << 3,       // json space
        DGOC    = 1 << 4,       // octal
        DGDC    = 1 << 5,       // decimal
        DGHX    = 1 << 6,       // hexadecimal
        ALPH    = 1 << 7,       // alpha
        ALNM    = ALPH|DGDC,    // alphanumeric
        C0      = CTRL,
        C1      = VSBL,
        C2      = CXSP|CTRL,
        C3      = JSSP|CXSP|CTRL,
        C4      = JSSP|CXSP|VSBL,
        C5      = DGOC|DGDC|DGHX|VSBL,
        C6      = DGDC|DGHX|VSBL,
        C7      = DGHX|ALPH|VSBL,
        C8      = ALPH|VSBL
    };
    inline unsigned char is_x_(unsigned char c) {
        static constexpr unsigned char IS_X_[] = {
        //   0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F
            C0,C0,C0,C0,C0,C0,C0,C0, C0,C3,C3,C2,C2,C3,C0,C0, // 0
            C0,C0,C0,C0,C0,C0,C0,C0, C0,C0,C0,C0,C0,C0,C0,C0, // 1
            C4,C1,C1,C1,C1,C1,C1,C1, C1,C1,C1,C1,C1,C1,C1,C1, // 2
            C5,C5,C5,C5,C5,C5,C5,C5, C6,C6,C1,C1,C1,C1,C1,C1, // 3
            C1,C7,C7,C7,C7,C7,C7,C8, C8,C8,C8,C8,C8,C8,C8,C8, // 4
            C8,C8,C8,C8,C8,C8,C8,C8, C8,C8,C8,C1,C1,C1,C1,C1, // 5
            C1,C7,C7,C7,C7,C7,C7,C8, C8,C8,C8,C8,C8,C8,C8,C8, // 6
            C8,C8,C8,C8,C8,C8,C8,C8, C8,C8,C8,C1,C1,C1,C1,C0, // 7
            C1,C1,C1,C1,C1,C1,C1,C1, C1,C1,C1,C1,C1,C1,C1,C1, // 8
            C1,C1,C1,C1,C1,C1,C1,C1, C1,C1,C1,C1,C1,C1,C1,C1, // 9
            C1,C1,C1,C1,C1,C1,C1,C1, C1,C1,C1,C1,C1,C1,C1,C1, // A
            C1,C1,C1,C1,C1,C1,C1,C1, C1,C1,C1,C1,C1,C1,C1,C1, // B
            00,00,C1,C1,C1,C1,C1,C1, C1,C1,C1,C1,C1,C1,C1,C1, // C
            C1,C1,C1,C1,C1,C1,C1,C1, C1,C1,C1,C1,C1,C1,C1,C1, // D
            C1,C1,C1,C1,C1,C1,C1,C1, C1,C1,C1,C1,C1,C1,C1,C1, // E
            C1,C1,C1,C1,C1,00,00,00, 00,00,00,00,00,00,00,00  // F
        };
        return IS_X_[c];
    }

    template <typename X>
        struct is {
            static bool ctrl    (char c) noexcept { return (unsigned char)c < 0x20; }
            static bool print   (char c) noexcept { return VSBL & is_x_(c); }
            static bool space   (char c) noexcept { return CXSP & is_x_(c); }
            static bool digit2  (char c) noexcept { return c == '0' || c == '1'; }
            static bool digit8  (char c) noexcept { return DGOC & is_x_(c); }
            static bool digit10 (char c) noexcept { return DGDC & is_x_(c); }
            static bool digit16 (char c) noexcept { return DGHX & is_x_(c); }
            static bool alpha   (char c) noexcept { return ALPH & is_x_(c); }
            static bool alnum   (char c) noexcept { return ALNM & is_x_(c); }
            static bool invalid (char c) noexcept { return '\0' == is_x_(c); }
        };

}}}

#endif // CXON_CIO_CHCLS_HXX_
