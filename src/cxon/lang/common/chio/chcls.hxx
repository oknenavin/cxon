// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHIO_CHCLS_HXX_
#define CXON_CHIO_CHCLS_HXX_

namespace cxon { namespace chio {

    enum : unsigned char {
        CTRL    = 1 << 0,       // control
        REAL    = 1 << 1,       // character
        CXSP    = 1 << 2,       // cxon space
        JSSP    = 1 << 3,       // json space
        DGOC    = 1 << 4,       // octal
        DGDC    = 1 << 5,       // decimal
        DGHX    = 1 << 6,       // hexadecimal
        ALPH    = 1 << 7,       // alpha
        ALNM    = ALPH|DGDC,    // alphanumeric
        C0      = CTRL,
        C1      = REAL,
        C2      = CXSP|CTRL,
        C3      = JSSP|CXSP|CTRL,
        C4      = JSSP|CXSP|REAL,
        C5      = DGOC|DGDC|DGHX|REAL,
        C6      = DGDC|DGHX|REAL,
        C7      = DGHX|ALPH|REAL,
        C8      = ALPH|REAL
    };
    static constexpr unsigned char is_x_[] = { 
        /*  0*/  C0,C0,C0,C0,C0,C0,C0,C0,C0,C3,C3,C2,C2,C3,C0,C0,
        /* 16*/  C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,
        /* 32*/  C4,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /* 48*/  C5,C5,C5,C5,C5,C5,C5,C5,C6,C6,C1,C1,C1,C1,C1,C1,
        /* 64*/  C1,C7,C7,C7,C7,C7,C7,C8,C8,C8,C8,C8,C8,C8,C8,C8,
        /* 80*/  C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C1,C1,C1,C1,C1,
        /* 96*/  C1,C7,C7,C7,C7,C7,C7,C8,C8,C8,C8,C8,C8,C8,C8,C8,
        /*112*/  C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C1,C1,C1,C1,C1,
        /*128*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*144*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*160*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*176*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*192*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*208*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*224*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*240*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,00
    };

    template <typename X>
        struct is {
            static bool ctrl   (char c) noexcept { return CTRL & is_x_[(unsigned char)c]; }
            static bool real   (char c) noexcept { return REAL & is_x_[(unsigned char)c]; }
            static bool space  (char c) noexcept { return CXSP & is_x_[(unsigned char)c]; }
            static bool digit8 (char c) noexcept { return DGOC & is_x_[(unsigned char)c]; }
            static bool digit10(char c) noexcept { return DGDC & is_x_[(unsigned char)c]; }
            static bool digit16(char c) noexcept { return DGHX & is_x_[(unsigned char)c]; }
            static bool alpha  (char c) noexcept { return ALPH & is_x_[(unsigned char)c]; }
            static bool alnum  (char c) noexcept { return ALNM & is_x_[(unsigned char)c]; }
        };
    template <typename X>
        struct is<JSON<X>> : is<X> {
            static bool space  (char c) noexcept { return JSSP & is_x_[(unsigned char)c]; }
        };

}}  // cxon::chio

#endif // CXON_CHIO_CHCLS_HXX_
