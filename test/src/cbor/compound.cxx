// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(cxon::CBOR<>)
    // T[]
        {   // signed char
            {   signed char a[] = "";
                R_TEST(a, BS("\x40"));      // definite byte string
                R_TEST(a, BS("\x5F\xFF"));  // indefinite byte string
                R_TEST(a, BS("\x60"));      // definite text string
                R_TEST(a, BS("\x7F\xFF"));  // indefinite text string
                R_TEST(a, BS("\x80"));      // definite array
                R_TEST(a, BS("\x9F\xFF"));  // indefinite array
            }
            {   signed char a[] = { (signed char)0x7F, (signed char)0x80, (signed char)0x81 };
                R_TEST(a, BS("\x43\x7F\x80\x81"));                  // definite byte string
                R_TEST(a, BS("\x63\x7F\x80\x81"));                  // definite text string
                R_TEST(a, BS("\x5F\x43\x7F\x80\x81\xFF"));          // indefinite byte string (1)
                R_TEST(a, BS("\x5F\x41\x7F\x42\x80\x81\xFF"));      // indefinite byte string (1|2)
                R_TEST(a, BS("\x5F\x42\x7F\x80\x41\x81\xFF"));      // indefinite byte string (2|1)
                R_TEST(a, BS("\x7F\x63\x7F\x80\x81\xFF"));          // indefinite text string (1)
                R_TEST(a, BS("\x7F\x61\x7F\x62\x80\x81\xFF"));      // indefinite text string (1|2)
                R_TEST(a, BS("\x7F\x62\x7F\x80\x61\x81\xFF"));      // indefinite text string (2|1)
                R_TEST(a, BS("\x83\x18\x7F\x18\x80\x18\x81"));      // definite array
                R_TEST(a, BS("\x9F\x18\x7F\x18\x80\x18\x81\xFF"));  // indefinite array
                W_TEST(BS("\x43\x7F\x80\x81"), a);                  // always written as definite
                {
                    R_TEST(a, BS("\xA3\x7F\x80\x81"), cbor::read_error::array_invalid, 0);                          // invalid type
                    R_TEST(a, BS("\x5F\x43\x7F\x80\x81"), cbor::read_error::array_invalid, 5);                      // invalid type (missing indefinite break)
                    R_TEST(a, BS("\x5F\x62\x7F\x80\x41\x81\xFF"), cbor::read_error::array_invalid, 1);              // indefinite byte/text string mix
                    R_TEST(a, BS("\x5F\x41\x7F\x62\x80\x81\xFF"), cbor::read_error::array_invalid, 3);              // indefinite byte/text string mix
                    R_TEST(a, BS("\x7F\x62\x7F\x80\x41\x81\xFF"), cbor::read_error::array_invalid, 4);              // indefinite byte/text string mix
                    R_TEST(a, BS("\x7F\x41\x7F\x62\x80\x81\xFF"), cbor::read_error::array_invalid, 1);              // indefinite byte/text string mix
                    R_TEST(a, BS("\x44\x7F\x80\x81\x82"), cbor::read_error::size_invalid, 0);                       // more, invalid size (4)
                    R_TEST(a, BS("\x84\x18\x7F\x18\x80\x18\x81\x18\x82"), cbor::read_error::size_invalid, 0);       // more, invalid size (4)
                    R_TEST(a, BS("\x9D\x18\x7F\x18\x80\x18\x81\x18\x82\xFF"), cbor::read_error::size_invalid, 0);   // more, invalid size (4)
                    R_TEST(a, BS("\x5F\x44\x7F\x80\x81\x82\xFF"), cbor::read_error::size_invalid, 1);               // more, invalid size (4)
                    R_TEST(a, BS("\x5F\x43\x7F\x80\x81\x41\x82\xFF"), cbor::read_error::size_invalid, 5);           // more, invalid size (3|1)
                    R_TEST(a, BS("\x5F\x41\x7F\x43\x80\x81\x82\xFF"), cbor::read_error::size_invalid, 3);           // more, invalid size (1|3)
                    R_TEST(a, BS("\x43\x7F\x80"), cbor::read_error::unexpected, 3);                                 // less, unexpected end (2)
                    R_TEST(a, BS("\x83\x18\x7F\x18\x80"), cbor::read_error::integer_invalid, 5);                    // less, invalid integer (2)
                    R_TEST(a, BS("\x5C\x7F\x80\x81"), cbor::read_error::size_invalid, 0);                           // invalid size (1C-1E reserved)
                    R_TEST(a, BS("\x5D\x7F\x80\x81"), cbor::read_error::size_invalid, 0);                           // invalid size (1C-1E reserved)
                    R_TEST(a, BS("\x5E\x7F\x80\x81"), cbor::read_error::size_invalid, 0);                           // invalid size (1C-1E reserved)
                    R_TEST(a, BS("\x5F\x5C\x7F\x80\x81"), cbor::read_error::size_invalid, 1);                       // invalid size (1C-1F reserved)
                    R_TEST(a, BS("\x5F\x5D\x7F\x80\x81"), cbor::read_error::size_invalid, 1);                       // invalid size (1C-1F reserved)
                    R_TEST(a, BS("\x5F\x5E\x7F\x80\x81"), cbor::read_error::size_invalid, 1);                       // invalid size (1C-1F reserved)
                    R_TEST(a, BS("\x5F\x5F\x7F\x80\x81"), cbor::read_error::size_invalid, 1);                       // invalid size (1C-1F reserved)
                    R_TEST(a, BS("\x83\x7F\x80\x81"), cbor::read_error::integer_invalid, 1);                        // invalid array element
                }
            }
            {   signed char a[][3] = { { 1, 2, 3 }, { 1, 2, 3} };
                R_TEST(a, BS("\x82\x43\x01\x02\x03\x63\x01\x02\x03"));              // definite array
                R_TEST(a, BS("\x82\x63\x01\x02\x03\x43\x01\x02\x03"));              // definite array
                R_TEST(a, BS("\x9F\x83\x01\x02\x03\x83\x01\x02\x03\xFF"));          // indefinite array
                R_TEST(a, BS("\x82\x9F\x01\x02\x03\xFF\x83\x01\x02\x03"));          // indefinite array
                R_TEST(a, BS("\x82\x83\x01\x02\x03\x9F\x01\x02\x03\xFF"));          // indefinite array
                R_TEST(a, BS("\x9F\x9F\x01\x02\x03\xFF\x9F\x01\x02\x03\xFF\xFF"));  // indefinite array
                W_TEST(BS("\x82\x43\x01\x02\x03\x43\x01\x02\x03"), a);              // always written as definite
            }
        }
        {   // unsigned char
            {   unsigned char a[] = "";
                R_TEST(a, BS("\x40"));      // definite byte string
                R_TEST(a, BS("\x5F\xFF"));  // indefinite byte string
                R_TEST(a, BS("\x60"));      // definite text string
                R_TEST(a, BS("\x7F\xFF"));  // indefinite text string
                R_TEST(a, BS("\x80"));      // definite array
                R_TEST(a, BS("\x9F\xFF"));  // indefinite array
            }
            {   unsigned char a[] = { (unsigned char)0x7F, (unsigned char)0x80, (unsigned char)0x81 };
                R_TEST(a, BS("\x43\x7F\x80\x81"));                  // definite byte string
                R_TEST(a, BS("\x63\x7F\x80\x81"));                  // definite text string
                R_TEST(a, BS("\x5F\x43\x7F\x80\x81\xFF"));          // indefinite byte string (1)
                R_TEST(a, BS("\x5F\x41\x7F\x42\x80\x81\xFF"));      // indefinite byte string (1|2)
                R_TEST(a, BS("\x5F\x42\x7F\x80\x41\x81\xFF"));      // indefinite byte string (2|1)
                R_TEST(a, BS("\x7F\x63\x7F\x80\x81\xFF"));          // indefinite text string (1)
                R_TEST(a, BS("\x7F\x61\x7F\x62\x80\x81\xFF"));      // indefinite text string (1|2)
                R_TEST(a, BS("\x7F\x62\x7F\x80\x61\x81\xFF"));      // indefinite text string (2|1)
                R_TEST(a, BS("\x83\x18\x7F\x18\x80\x18\x81"));      // definite array
                R_TEST(a, BS("\x9F\x18\x7F\x18\x80\x18\x81\xFF"));  // indefinite array
                W_TEST(BS("\x43\x7F\x80\x81"), a);                  // always written as definite
                {
                    R_TEST(a, BS("\xA3\x7F\x80\x81"), cbor::read_error::array_invalid, 0);                          // invalid type
                    R_TEST(a, BS("\x5F\x43\x7F\x80\x81"), cbor::read_error::array_invalid, 5);                      // invalid type (missing indefinite break)
                    R_TEST(a, BS("\x5F\x62\x7F\x80\x41\x81\xFF"), cbor::read_error::array_invalid, 1);              // indefinite byte/text string mix
                    R_TEST(a, BS("\x5F\x41\x7F\x62\x80\x81\xFF"), cbor::read_error::array_invalid, 3);              // indefinite byte/text string mix
                    R_TEST(a, BS("\x7F\x62\x7F\x80\x41\x81\xFF"), cbor::read_error::array_invalid, 4);              // indefinite byte/text string mix
                    R_TEST(a, BS("\x7F\x41\x7F\x62\x80\x81\xFF"), cbor::read_error::array_invalid, 1);              // indefinite byte/text string mix
                    R_TEST(a, BS("\x44\x7F\x80\x81\x82"), cbor::read_error::size_invalid, 0);                       // invalid size (4)
                    R_TEST(a, BS("\x84\x18\x7F\x18\x80\x18\x81\x18\x82"), cbor::read_error::size_invalid, 0);       // invalid size (4)
                    R_TEST(a, BS("\x9D\x18\x7F\x18\x80\x18\x81\x18\x82\xFF"), cbor::read_error::size_invalid, 0);   // invalid size (4)
                    R_TEST(a, BS("\x5F\x44\x7F\x80\x81\x82\xFF"), cbor::read_error::size_invalid, 1);               // invalid size (4)
                    R_TEST(a, BS("\x5F\x43\x7F\x80\x81\x41\x82\xFF"), cbor::read_error::size_invalid, 5);           // invalid size (3|1)
                    R_TEST(a, BS("\x5F\x41\x7F\x43\x80\x81\x82\xFF"), cbor::read_error::size_invalid, 3);           // invalid size (1|3)
                    R_TEST(a, BS("\x5C\x7F\x80\x81"), cbor::read_error::size_invalid, 0);                           // invalid size (1C-1E reserved)
                    R_TEST(a, BS("\x5D\x7F\x80\x81"), cbor::read_error::size_invalid, 0);                           // invalid size (1C-1E reserved)
                    R_TEST(a, BS("\x5E\x7F\x80\x81"), cbor::read_error::size_invalid, 0);                           // invalid size (1C-1E reserved)
                    R_TEST(a, BS("\x5F\x5C\x7F\x80\x81"), cbor::read_error::size_invalid, 1);                       // invalid size (1C-1F reserved)
                    R_TEST(a, BS("\x5F\x5D\x7F\x80\x81"), cbor::read_error::size_invalid, 1);                       // invalid size (1C-1F reserved)
                    R_TEST(a, BS("\x5F\x5E\x7F\x80\x81"), cbor::read_error::size_invalid, 1);                       // invalid size (1C-1F reserved)
                    R_TEST(a, BS("\x5F\x5F\x7F\x80\x81"), cbor::read_error::size_invalid, 1);                       // invalid size (1C-1F reserved)
                    R_TEST(a, BS("\x83\x7F\x80\x81"), cbor::read_error::integer_invalid, 1);                        // invalid array element
                }
            }
            {   unsigned char a[][3] = { { 1, 2, 3 }, { 1, 2, 3} };
                R_TEST(a, BS("\x82\x43\x01\x02\x03\x63\x01\x02\x03"));              // definite array
                R_TEST(a, BS("\x82\x63\x01\x02\x03\x43\x01\x02\x03"));              // definite array
                R_TEST(a, BS("\x9F\x83\x01\x02\x03\x83\x01\x02\x03\xFF"));          // indefinite array
                R_TEST(a, BS("\x82\x9F\x01\x02\x03\xFF\x83\x01\x02\x03"));          // indefinite array
                R_TEST(a, BS("\x82\x83\x01\x02\x03\x9F\x01\x02\x03\xFF"));          // indefinite array
                R_TEST(a, BS("\x9F\x9F\x01\x02\x03\xFF\x9F\x01\x02\x03\xFF\xFF"));  // indefinite array
                W_TEST(BS("\x82\x43\x01\x02\x03\x43\x01\x02\x03"), a);              // always written as definite
            }
        }
        {   // unsigned
            {   unsigned a[] = { 0 };
                R_TEST(a, BS("\x40"));      // definite byte string
                R_TEST(a, BS("\x5F\xFF"));  // indefinite byte string
                R_TEST(a, BS("\x60"));      // definite text string
                R_TEST(a, BS("\x7F\xFF"));  // indefinite text string
                R_TEST(a, BS("\x80"));      // definite array
                R_TEST(a, BS("\x9F\xFF"));  // indefinite array
            }
            {   unsigned a[] = { 1, 2, 3 };
                R_TEST(a, BS("\x83\x01\x02\x03"));          // definite array
                R_TEST(a, BS("\x9F\x01\x02\x03\xFF"));      // indefinite array
                R_TEST(a, BS("\x43\x01\x02\x03"));          // definite byte string
                R_TEST(a, BS("\x5F\x43\x01\x02\x03\xFF"));  // indefinite byte string
                W_TEST(BS("\x83\x01\x02\x03"), a);          // always written as definite
                {
                    R_TEST(a, BS("\xA3\x01\x02\x03"), cbor::read_error::array_invalid, 0);          // invalid type
                    R_TEST(a, BS("\x84\x01\x02\x03\x04"), cbor::read_error::size_invalid, 0);       // invalid size (4)
                    R_TEST(a, BS("\x9F\x01\x02\x03\x04\xFF"), cbor::read_error::size_invalid, 0);   // invalid size (4)
                    R_TEST(a, BS("\x83\x01\x02\xF5"), cbor::read_error::integer_invalid, 3);        // invalid array element
                    R_TEST(a, BS("\x9F\x01\x02\xF5\xFF"), cbor::read_error::integer_invalid, 3);    // invalid array element
                }
            }
            {   unsigned a[] = { 1, 2, 0 };
                R_TEST(a, BS("\x82\x01\x02"));      // read less
                R_TEST(a, BS("\x9F\x01\x02\xFF"));  // read less
            }
            {   signed a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 };
                R_TEST(a, BS("\x98\x19\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19"));
                R_TEST(a, BS("\x9F\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19\xFF"));
                W_TEST(BS("\x98\x19\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19"), a);
            }
            {   signed a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 0 };
                R_TEST(a, BS("\x98\x18\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18"));  // definite
                R_TEST(a, BS("\x9F\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\xFF"));  // indefinite
            }
        }
        {   // signed
            {   signed a[] = { 0 };
                R_TEST(a, BS("\x40"));      // definite byte string
                R_TEST(a, BS("\x5F\xFF"));  // indefinite byte string
                R_TEST(a, BS("\x60"));      // definite text string
                R_TEST(a, BS("\x7F\xFF"));  // indefinite text string
                R_TEST(a, BS("\x80"));      // definite array
                R_TEST(a, BS("\x9F\xFF"));  // indefinite array
            }
            {   signed a[] = { 1, 2, 3 };
                R_TEST(a, BS("\x83\x01\x02\x03"));          // definite array
                R_TEST(a, BS("\x9F\x01\x02\x03\xFF"));      // indefinite array
                R_TEST(a, BS("\x43\x01\x02\x03"));          // definite byte string
                R_TEST(a, BS("\x5F\x43\x01\x02\x03\xFF"));  // indefinite byte string
                W_TEST(BS("\x83\x01\x02\x03"), a);          // always written as definite
                {
                    R_TEST(a, BS("\xA3\x01\x02\x03"), cbor::read_error::array_invalid, 0);          // invalid type
                    R_TEST(a, BS("\x84\x01\x02\x03\x04"), cbor::read_error::size_invalid, 0);       // invalid size (4)
                    R_TEST(a, BS("\x9F\x01\x02\x03\x04\xFF"), cbor::read_error::size_invalid, 0);   // invalid size (4)
                    R_TEST(a, BS("\x83\x01\x02\xF5"), cbor::read_error::integer_invalid, 3);        // invalid array element
                    R_TEST(a, BS("\x9F\x01\x02\xF5\xFF"), cbor::read_error::integer_invalid, 3);    // invalid array element
                }
            }
            {   signed a[] = { 1, 2, 0 };
                R_TEST(a, BS("\x82\x01\x02"));      // read less
                R_TEST(a, BS("\x9F\x01\x02\xFF"));  // read less
            }
            {   signed a[] = { -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16, -17, -18, -19, -20, -21, -22, -23, -24, -25, -26 };
                R_TEST(a, BS("\x98\x1A\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\x34\x35\x36\x37\x38\x18\x38\x19"));
                R_TEST(a, BS("\x9F\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\x34\x35\x36\x37\x38\x18\x38\x19\xFF"));
                W_TEST(BS("\x98\x1A\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\x34\x35\x36\x37\x38\x18\x38\x19"), a);
            }
            {   signed a[] = { -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16, -17, -18, -19, -20, -21, -22, -23, -24, -25, 0 };
                R_TEST(a, BS("\x98\x19\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\x34\x35\x36\x37\x38\x18"));
                R_TEST(a, BS("\x9F\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\x34\x35\x36\x37\x38\x18\xFF"));
            }
        }
    // char[]
        R_TEST(/*u8*/"", BS("\x80"));      // definite
        R_TEST(/*u8*/"", BS("\x9F\xFF"));  // indefinite
        R_TEST(/*u8*/"\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80", BS("\x6A\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80")); // definite
        R_TEST(/*u8*/"\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80", BS("\x9F\x18\x20\x18\xC2\x18\xAE\x18\xE2\x18\x9C\x18\x88\x18\xF0\x18\x9F\x18\x9A\x18\x80\xFF")); // indefinite
        W_TEST(BS("\x6B\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80\x00"), /*u8*/"\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80");
        R_TEST(/*u8*/"xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA", // definite
                    BS("\x58\x1C\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\xF0\x9F\x8D\xBA"));
        R_TEST(/*u8*/"xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA", // definite
                    BS("\x78\x1C\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\xF0\x9F\x8D\xBA"));
        R_TEST(/*u8*/"xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA", // definite
                    BS("\x98\x1C\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\xF0\x18\x9F\x18\x8D\x18\xBA"));
        R_TEST(/*u8*/"xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA", // indefinite
                    BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\xF0\x18\x9F\x18\x8D\x18\xBA\xFF"));
        W_TEST(   BS("\x78\x1D\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\xF0\x9F\x8D\xBA\x00"),
                /*u8*/"xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA");
        {   char a[][3] = { {1, 2, 3}, {1, 2, 3} };
            R_TEST(a, BS("\x82\x63\x01\x02\x03\x63\x01\x02\x03"));  // definite
            W_TEST(BS("\x82\x63\x01\x02\x03\x63\x01\x02\x03"), a);  // definite    
            
            R_TEST(a, BS("\x9F\x63\x01\x02\x03\x63\x01\x02\x03\xFF"));                  // indefinite
            R_TEST(a, BS("\x82\x63\x01\x02\x03\x7F\x63\x01\x02\x03\xFF"));              // indefinite
            R_TEST(a, BS("\x9F\x7F\x63\x01\x02\x03\xFF\x63\x01\x02\x03\xFF"));          // indefinite
            R_TEST(a, BS("\x9F\x7F\x63\x01\x02\x03\xFF\x7F\x63\x01\x02\x03\xFF\xFF"));  // indefinite

            R_TEST(a, BS("\x9F\x83\x01\x02\x03\x83\x01\x02\x03\xFF"));          // indefinite
            R_TEST(a, BS("\x82\x9F\x01\x02\x03\xFF\x83\x01\x02\x03"));          // indefinite
            R_TEST(a, BS("\x82\x83\x01\x02\x03\x9F\x01\x02\x03\xFF"));          // indefinite
            R_TEST(a, BS("\x9F\x9F\x01\x02\x03\xFF\x9F\x01\x02\x03\xFF\xFF"));  // indefinite
        }
    // char16_t[]
        R_TEST(u"", BS("\x80"));        // definite
        R_TEST(u"", BS("\x9F\xFF"));    // indefinite
        R_TEST(u"\x00AE\x2708\xD83D\xDE80", BS("\x84\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80"));        // definite
        R_TEST(u"\x00AE\x2708\xD83D\xDE80", BS("\x9F\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80\xFF"));    // indefinite
        W_TEST(BS("\x85\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80\x00"), u"\x00AE\x2708\xD83D\xDE80");
        R_TEST(   u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A", // definite
                BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A"));
        R_TEST(   u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A", // indefinite
                BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A\xFF"));
        W_TEST( BS("\x98\x1B\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A\x00"),
                    u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A");
    // char32_t[]
        R_TEST(U"", BS("\x80"));        // definite
        R_TEST(U"", BS("\x9F\xFF"));    // indefinite
        R_TEST(U"\x000000AE\x00002708\x0001F680", BS("\x83\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80"));      // definite
        R_TEST(U"\x000000AE\x00002708\x0001F680", BS("\x9F\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80\xFF"));  // indefinite
        W_TEST(BS("\x84\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80\x00"), U"\x000000AE\x00002708\x0001F680");
        R_TEST(   U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A", // definite
                BS("\x98\x19\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A"));
        W_TEST( BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A\x00"),
                    U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A");
        R_TEST(   U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A", // indefinite
                BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A\xFF"));
    // wchar_t[]
        R_TEST(L"", BS("\x80"));        // definite
        R_TEST(L"", BS("\x9F\xFF"));    // indefinite
    // T*
        R_TEST((int*)nullptr, BS("\xF6"));
        W_TEST(BS("\xF6"), (int*)nullptr);
        R_TEST((int*)nullptr, BS("\xF7"), cbor::read_error::integer_invalid, 0);
        {   int n = 0x01010101;
            R_TEST(&n, BS("\x1A\x01\x01\x01\x01"));
            W_TEST(BS("\x1A\x01\x01\x01\x01"), &n);
        }
    // const char*
        R_TEST((const char*)"", BS("\x80"));        // definite
        R_TEST((const char*)"", BS("\x60"));        // definite
        R_TEST((const char*)"", BS("\x40"));        // definite
        R_TEST((const char*)"", BS("\x9F\xFF"));    // indefinite
        R_TEST((const char*)"", BS("\x7F\xFF"));    // indefinite
        R_TEST((const char*)"", BS("\x5F\xFF"));    // indefinite
            R_TEST((const char*)nullptr, BS("\xF7"), cbor::read_error::integer_invalid, 0);
        R_TEST((const char*)"\x03\x02\x01", BS("\x83\x03\x02\x01"));        // definite array
        R_TEST((const char*)"xyz",          BS("\x63xyz"));                 // definite test string
        R_TEST((const char*)"xyz",          BS("\x43xyz"));                 // definite byte string
        R_TEST((const char*)"\x03\x02\x01", BS("\x9F\x03\x02\x01\xFF"));    // indefinite array (3)
        R_TEST((const char*)"xyz",          BS("\x7F\x63xyz\xFF"));         // indefinite text string (3)
        R_TEST((const char*)"xyz",          BS("\x7F\x61x\x62yz\xFF"));     // indefinite test string (1|2)
        R_TEST((const char*)"xyz",          BS("\x5F\x43xyz\xFF"));         // indefinite byte string (3)
        R_TEST((const char*)"xyz",          BS("\x5F\x42xy\x41z\xFF"));     // indefinite byte string (2|1)
        W_TEST(BS("\x63xyz"), (const char*)"xyz");                          // always written as definite
            R_TEST((const char*)"", BS("\xA3\x03\x02\x01"), cbor::read_error::integer_invalid, 0);      // invalid type
            R_TEST((const char*)"", BS("\x83\x03\xA2\x01"), cbor::read_error::integer_invalid, 2);      // definite array, invalid element
            R_TEST((const char*)"", BS("\x9F\x03\xA2\x01\xFF"), cbor::read_error::integer_invalid, 2);  // indefinite array, invalid element
            R_TEST((const char*)"", BS("\x7F\x41x\x62yz\xFF"), cbor::read_error::array_invalid, 1);     // indefinite byte/text string mix
            R_TEST((const char*)"", BS("\x7F\x61x\x42yz\xFF"), cbor::read_error::array_invalid, 3);     // indefinite byte/text string mix
            R_TEST((const char*)"", BS("\x5F\x62xy\x41z\xFF"), cbor::read_error::array_invalid, 1);     // indefinite byte/text string mix
            R_TEST((const char*)"", BS("\x5F\x42xy\x61z\xFF"), cbor::read_error::array_invalid, 4);     // indefinite byte/text string mix
            R_TEST((const char*)"", BS("\x7F\x7Cx\x62yz\xFF"), cbor::read_error::size_invalid, 1);      // invalid size
            R_TEST((const char*)"", BS("\x7F\x7Dx\x62yz\xFF"), cbor::read_error::size_invalid, 1);      // invalid size
            R_TEST((const char*)"", BS("\x7F\x7Ex\x62yz\xFF"), cbor::read_error::size_invalid, 1);      // invalid size
            R_TEST((const char*)"", BS("\x7F\x7Fx\x62yz\xFF"), cbor::read_error::size_invalid, 1);      // invalid size
            R_TEST((const char*)"", BS("\x5F\x42xy\x5Cz\xFF"), cbor::read_error::size_invalid, 4);      // invalid size
            R_TEST((const char*)"", BS("\x5F\x42xy\x5Dz\xFF"), cbor::read_error::size_invalid, 4);      // invalid size
            R_TEST((const char*)"", BS("\x5F\x42xy\x5Ez\xFF"), cbor::read_error::size_invalid, 4);      // invalid size
            R_TEST((const char*)"", BS("\x5F\x42xy\x5Fz\xFF"), cbor::read_error::size_invalid, 4);      // invalid size
    // const char16_t*
        R_TEST((const char16_t*)u"", BS("\x80"));     // definite
        R_TEST((const char16_t*)u"", BS("\x9F\xFF")); // indefinite
        R_TEST((const char16_t*)u"\x00AE\x2708\xD83D\xDE80", BS("\x84\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80"));     // definite
        R_TEST((const char16_t*)u"\x00AE\x2708\xD83D\xDE80", BS("\x9F\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80\xFF")); // indefinite
        W_TEST(BS("\x84\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80"), (const char16_t*)u"\x00AE\x2708\xD83D\xDE80");
        R_TEST((const char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A", // definite
                        BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A"));
        R_TEST((const char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A", // indefinite
                        BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A\xFF"));
        W_TEST(         BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A"),
               (const char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A");
    // const char32_t*
        R_TEST((const char32_t*)U"", BS("\x80"));       // definite
        R_TEST((const char32_t*)U"", BS("\x9F\xFF"));   // indefinite
        R_TEST((const char32_t*)U"\x000000AE\x00002708\x0001F680", BS("\x83\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80"));     // definite
        R_TEST((const char32_t*)U"\x000000AE\x00002708\x0001F680", BS("\x9F\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80\xFF")); // indefinite
        W_TEST(BS("\x83\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80"), (const char32_t*)U"\x000000AE\x00002708\x0001F680");
        R_TEST((const char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A", // definite
                              BS("\x98\x19\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A"));
        W_TEST(               BS("\x98\x19\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A"),
               (const char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A");
        R_TEST((const char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A", // indefinite
                              BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A\xFF"));
    // const wchar_t*
        R_TEST((const wchar_t*)L"", BS("\x80"));        // definite
        R_TEST((const wchar_t*)L"", BS("\x9F\xFF"));    // indefinite
    // char*
        R_TEST((char*)"", BS("\x80"));      // definite
        R_TEST((char*)"", BS("\x60"));      // definite
        R_TEST((char*)"", BS("\x40"));      // definite
        R_TEST((char*)"", BS("\x9F\xFF"));  // indefinite
        R_TEST((char*)"", BS("\x7F\xFF"));  // indefinite
        R_TEST((char*)"", BS("\x5F\xFF"));  // indefinite
            R_TEST((char*)nullptr, BS("\xF7"), cbor::read_error::integer_invalid, 0);
        R_TEST((char*)"\x03\x02\x01", BS("\x83\x03\x02\x01"));      // definite array
        R_TEST((char*)"xyz",          BS("\x63xyz"));               // definite test string
        R_TEST((char*)"xyz",          BS("\x43xyz"));               // definite byte string
        R_TEST((char*)"\x03\x02\x01", BS("\x9F\x03\x02\x01\xFF"));  // indefinite array (3)
        R_TEST((char*)"xyz",          BS("\x7F\x63xyz\xFF"));       // indefinite text string (3)
        R_TEST((char*)"xyz",          BS("\x7F\x61x\x62yz\xFF"));   // indefinite test string (1|2)
        R_TEST((char*)"xyz",          BS("\x5F\x43xyz\xFF"));       // indefinite byte string (3)
        R_TEST((char*)"xyz",          BS("\x5F\x42xy\x41z\xFF"));   // indefinite byte string (2|1)
        W_TEST(BS("\x63xyz"), (char*)"xyz");                        // always written as definite
            R_TEST((char*)"", BS("\xA3\x03\x02\x01"), cbor::read_error::integer_invalid, 0);        // invalid type
            R_TEST((char*)"", BS("\x83\x03\xA2\x01"), cbor::read_error::integer_invalid, 2);        // definite array, invalid element
            R_TEST((char*)"", BS("\x9F\x03\xA2\x01\xFF"), cbor::read_error::integer_invalid, 2);    // indefinite array, invalid element
            R_TEST((char*)"", BS("\x7F\x41x\x62yz\xFF"), cbor::read_error::array_invalid, 1);       // indefinite byte/text string mix
            R_TEST((char*)"", BS("\x7F\x61x\x42yz\xFF"), cbor::read_error::array_invalid, 3);       // indefinite byte/text string mix
            R_TEST((char*)"", BS("\x5F\x62xy\x41z\xFF"), cbor::read_error::array_invalid, 1);       // indefinite byte/text string mix
            R_TEST((char*)"", BS("\x5F\x42xy\x61z\xFF"), cbor::read_error::array_invalid, 4);       // indefinite byte/text string mix
            R_TEST((char*)"", BS("\x7F\x7Cx\x62yz\xFF"), cbor::read_error::size_invalid, 1);        // invalid size
            R_TEST((char*)"", BS("\x7F\x7Dx\x62yz\xFF"), cbor::read_error::size_invalid, 1);        // invalid size
            R_TEST((char*)"", BS("\x7F\x7Ex\x62yz\xFF"), cbor::read_error::size_invalid, 1);        // invalid size
            R_TEST((char*)"", BS("\x7F\x7Fx\x62yz\xFF"), cbor::read_error::size_invalid, 1);        // invalid size
            R_TEST((char*)"", BS("\x5F\x42xy\x5Cz\xFF"), cbor::read_error::size_invalid, 4);        // invalid size
            R_TEST((char*)"", BS("\x5F\x42xy\x5Dz\xFF"), cbor::read_error::size_invalid, 4);        // invalid size
            R_TEST((char*)"", BS("\x5F\x42xy\x5Ez\xFF"), cbor::read_error::size_invalid, 4);        // invalid size
            R_TEST((char*)"", BS("\x5F\x42xy\x5Fz\xFF"), cbor::read_error::size_invalid, 4);        // invalid size
    // char16_t*
        R_TEST((char16_t*)u"", BS("\x80"));     // definite
        R_TEST((char16_t*)u"", BS("\x9F\xFF")); // indefinite
        R_TEST((char16_t*)u"\x00AE\x2708\xD83D\xDE80", BS("\x84\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80"));     // definite
        R_TEST((char16_t*)u"\x00AE\x2708\xD83D\xDE80", BS("\x9F\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80\xFF")); // indefinite
        W_TEST(BS("\x84\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80"), (char16_t*)u"\x00AE\x2708\xD83D\xDE80");
        R_TEST((char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A", // definite
                        BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A"));
        R_TEST((char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A", // indefinite
                        BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A\xFF"));
        W_TEST(         BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A"),
               (char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A");
    // char32_t*
        R_TEST((char32_t*)U"", BS("\x80"));     // definite
        R_TEST((char32_t*)U"", BS("\x9F\xFF")); // indefinite
        R_TEST((char32_t*)U"\x000000AE\x00002708\x0001F680", BS("\x83\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80"));       // definite
        R_TEST((char32_t*)U"\x000000AE\x00002708\x0001F680", BS("\x9F\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80\xFF"));   // indefinite
        W_TEST(BS("\x83\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80"), (char32_t*)U"\x000000AE\x00002708\x0001F680");
        R_TEST((char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A", // definite
                        BS("\x98\x19\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A"));
        W_TEST(         BS("\x98\x19\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A"),
               (char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A");
        R_TEST((char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A", // indefinite
                        BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A\xFF"));
    // wchar_t*
        R_TEST((wchar_t*)L"", BS("\x80"));      // definite
        R_TEST((wchar_t*)L"", BS("\x9F\xFF"));  // indefinite
TEST_END()
