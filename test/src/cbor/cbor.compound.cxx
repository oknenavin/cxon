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
                // errors
                R_TEST(a, BS("\x00"), cbor::read_error::array_invalid, 0);
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
            R_TEST(a, BS("\x9F\x7F\x63\x01\x02\x03\xFF\x63\x01\x02\x03\xFF"));          // indefinitehar
            R_TEST(a, BS("\x9F\x7F\x63\x01\x02\x03\xFF\x7F\x63\x01\x02\x03\xFF\xFF"));  // indefinite

            R_TEST(a, BS("\x9F\x83\x01\x02\x03\x83\x01\x02\x03\xFF"));          // indefinite
            R_TEST(a, BS("\x82\x9F\x01\x02\x03\xFF\x83\x01\x02\x03"));          // indefinite
            R_TEST(a, BS("\x82\x83\x01\x02\x03\x9F\x01\x02\x03\xFF"));          // indefinite
            R_TEST(a, BS("\x9F\x9F\x01\x02\x03\xFF\x9F\x01\x02\x03\xFF\xFF"));  // indefinite
        }
    // wchar_t[]
        R_TEST(L"", BS("\x80"));        // definite
        R_TEST(L"", BS("\x9F\xFF"));    // indefinite
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
        W_TEST(BS("\x64xyz\x00"), (const char*)"xyz");                          // always written as definite
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
    // const wchar_t*
        R_TEST((const wchar_t*)L"", BS("\x80"));        // definite
        R_TEST((const wchar_t*)L"", BS("\x9F\xFF"));    // indefinite
    // const char16_t*
        R_TEST((const char16_t*)u"", BS("\x80"));     // definite
        R_TEST((const char16_t*)u"", BS("\x9F\xFF")); // indefinite
        R_TEST((const char16_t*)u"\x00AE\x2708\xD83D\xDE80", BS("\x84\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80"));     // definite
        R_TEST((const char16_t*)u"\x00AE\x2708\xD83D\xDE80", BS("\x9F\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80\xFF")); // indefinite
        W_TEST(BS("\x85\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80\x00"), (const char16_t*)u"\x00AE\x2708\xD83D\xDE80");
        R_TEST((const char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A", // definite
                        BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A"));
        R_TEST((const char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A", // indefinite
                        BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A\xFF"));
        W_TEST(         BS("\x98\x1B\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A\x00"),
               (const char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A");
    // const char32_t*
        R_TEST((const char32_t*)U"", BS("\x80"));       // definite
        R_TEST((const char32_t*)U"", BS("\x9F\xFF"));   // indefinite
        R_TEST((const char32_t*)U"\x000000AE\x00002708\x0001F680", BS("\x83\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80"));     // definite
        R_TEST((const char32_t*)U"\x000000AE\x00002708\x0001F680", BS("\x9F\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80\xFF")); // indefinite
        W_TEST(BS("\x84\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80\x00"), (const char32_t*)U"\x000000AE\x00002708\x0001F680");
        R_TEST((const char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A", // definite
                              BS("\x98\x19\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A"));
        W_TEST(               BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A\x00"),
               (const char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A");
        R_TEST((const char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A", // indefinite
                              BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A\xFF"));
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
        W_TEST(BS("\x64xyz\00"), (char*)"xyz");                     // always written as definite
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
    // wchar_t*
        R_TEST((wchar_t*)L"", BS("\x80"));      // definite
        R_TEST((wchar_t*)L"", BS("\x9F\xFF"));  // indefinite
    // char16_t*
        R_TEST((char16_t*)u"", BS("\x80"));     // definite
        R_TEST((char16_t*)u"", BS("\x9F\xFF")); // indefinite
        R_TEST((char16_t*)u"\x00AE\x2708\xD83D\xDE80", BS("\x84\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80"));     // definite
        R_TEST((char16_t*)u"\x00AE\x2708\xD83D\xDE80", BS("\x9F\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80\xFF")); // indefinite
        W_TEST(BS("\x85\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80\00"), (char16_t*)u"\x00AE\x2708\xD83D\xDE80");
        R_TEST((char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A", // definite
                        BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A"));
        R_TEST((char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A", // indefinite
                        BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A\xFF"));
        W_TEST(         BS("\x98\x1B\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A\x00"),
               (char16_t*)u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A");
    // char32_t*
        R_TEST((char32_t*)U"", BS("\x80"));     // definite
        R_TEST((char32_t*)U"", BS("\x9F\xFF")); // indefinite
        R_TEST((char32_t*)U"\x000000AE\x00002708\x0001F680", BS("\x83\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80"));       // definite
        R_TEST((char32_t*)U"\x000000AE\x00002708\x0001F680", BS("\x9F\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80\xFF"));   // indefinite
        W_TEST(BS("\x84\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80\x00"), (char32_t*)U"\x000000AE\x00002708\x0001F680");
        R_TEST((char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A", // definite
                        BS("\x98\x19\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A"));
        W_TEST(         BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A\x00"),
               (char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A");
        R_TEST((char32_t*)U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A", // indefinite
                        BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A\xFF"));
TEST_END()


namespace {
    enum Enum1 { one = 1, two, three, four };
}

CXON_CBOR_ENM(Enum1,
    CXON_CBOR_ENM_VALUE_ASIS(one),
    CXON_CBOR_ENM_VALUE_NAME(42ULL, two),
    CXON_CBOR_ENM_VALUE_ASIS(three)
)

TEST_BEG(cxon::CBOR<>)
    R_TEST(Enum1::one, BS("\x01"));
    W_TEST(BS("\x01"), Enum1::one);
    R_TEST(Enum1::two, BS("\x18\x2A"));
    W_TEST(BS("\x18\x2A"), Enum1::two);
    R_TEST(Enum1::one, BS("\x02"), cbor::read_error::unexpected, 0);
    W_TEST(BS(""), Enum1::four, cbor::write_error::argument_invalid);
TEST_END()


namespace {

    struct Struct1 {
        int x;
        Enum1 y;
        int z;
        Struct1(int x = 0, Enum1 y = Enum1::one, int z = 0) : x(x), y(y), z(z) {}
        bool operator ==(const Struct1& t) const { return x == t.x && y == t.y && z == t.z; }
    };

}

CXON_CBOR_CLS_READ(Struct1,
    CXON_CBOR_CLS_FIELD_NAME("X", x),
    CXON_CBOR_CLS_FIELD_ASIS(y)
)
CXON_CBOR_CLS_WRITE(Struct1,
    CXON_CBOR_CLS_FIELD_NAME("X", x),
    CXON_CBOR_CLS_FIELD_ASIS(y)
)

TEST_BEG(cxon::CBOR<>)
    R_TEST(Struct1(0, Enum1::one), BS("\xA2\x62X\0\x00\x62y\0\x01")); // {X: 0, y: 'one'}
    R_TEST(Struct1(0, Enum1::two), BS("\xA2\x62X\0\x00\x62y\0\x18\x2A")); // {y: 'Two (2)', X: 0}
    R_TEST(Struct1(0, Enum1::three), BS("\xA1\x62y\0\x03")); // {y: 'three'}
    W_TEST(BS("\xA2\x62X\0\x00\x62y\0\x01"), Struct1(0, Enum1::one)); // {X: 0, y: 'one'}
    // errors
    R_TEST(Struct1(), BS("\xA1\x62z\0\x03"), cbor::read_error::unexpected, 1);
TEST_END()


namespace {

    struct Struct2 {
        int x;
        int y;
        Struct2(int x = 0, int y = 0) : x(x), y(y) {}
        bool operator ==(const Struct2& t) const { return x == t.x && y == t.y; }
    };

}

CXON_CBOR_CLS(Struct2,
    CXON_CBOR_CLS_FIELD_ASIS(x),
    CXON_CBOR_CLS_FIELD_NAME("Y", y)
)

TEST_BEG(cxon::CBOR<>)
    R_TEST(Struct2{1, 2}, BS("\xA2\x62x\0\x01\x62Y\0\x02")); // {x: 1, Y: 2}
    W_TEST(BS("\xA2\x62x\0\x01\x62Y\0\x02"), Struct2{1, 2}); // {x: 1, Y: 2}
TEST_END()


namespace {

    struct Struct3 {
        int x;
        Struct3* y;
        Struct3(int x = 0, Struct3* y = nullptr) : x(x), y(y) {}
        bool operator ==(const Struct3& t) const { return x == t.x && ((!y && !t.y) || (y && t.y && *y == *t.y)); }
    };

}

CXON_CBOR_CLS(Struct3,
    CXON_CBOR_CLS_FIELD_ASIS(x),
    CXON_CBOR_CLS_FIELD_ASIS(y)
)

TEST_BEG(cxon::CBOR<>)
    R_TEST(Struct3(1, new Struct3(2, nullptr)), BS("\xA2\x62x\0\x01\x62y\0\xA1\x62x\0\x02")); // {x: 1, y: {x: 2}}
    W_TEST(BS("\xA2\x62x\0\x01\x62y\0\xA2\x62x\0\x02\x62y\0\xF6"), Struct3(1, new Struct3(2, nullptr))); // {x: 1, y: {x: 2, y: nil}}
    R_TEST(Struct3(1, nullptr), BS("\xA2\x62x\0\x01\x62y\0\xF6")); // {x: 1, y: nil}
    W_TEST(BS("\xA2\x62x\0\x01\x62y\0\xF6"), Struct3(1, nullptr)); // {x: 1, y: nil}
TEST_END()


namespace {

    struct Struct4 {
        Struct4(int x = 0) : x(x) {}
        bool operator ==(const Struct4& t) const { return x == t.x; }

        template <typename X, typename II, typename C>
            static auto read_value(Struct4& t, II& b, II e, C& ctx) -> cxon::enable_for_t<X, cxon::CBOR> {
                return cxon::read_value<X>(t.x, b, e, ctx);
            }
        template <typename X, typename OI, typename C>
            static bool write_value(OI& o, const Struct4& t, C& ctx) {
                return cxon::write_value<X>(o, t.x, ctx);
            }

    private:
        int x;
    };

}

TEST_BEG(cxon::CBOR<>)
    R_TEST(Struct4(1), BS("\x01"));
    W_TEST(BS("\x03"), Struct4(3));
TEST_END()


namespace {

    struct Struct5 {
        Struct5(int x = 0) : x(x) {}
        bool operator ==(const Struct5& t) const { return x == t.x; }

        template <typename X, typename II, typename C>
            bool read_value(II& b, II e, C& ctx) {
                return cxon::read_value<X>(x, b, e, ctx);
            }
        template <typename X, typename OI, typename C>
            auto write_value(OI& o, C& ctx) const -> cxon::enable_for_t<X, cxon::CBOR> {
                return cxon::write_value<X>(o, x, ctx);
            }

    private:
        int x;
    };

}

TEST_BEG(cxon::CBOR<>)
    R_TEST(Struct5(1), BS("\x01"));
    W_TEST(BS("\x03"), Struct5(3));
TEST_END()


namespace {

    struct Struct6 {
        int x;
        Struct6(int x = 0) : x(x) {}
        bool operator ==(const Struct6& t) const { return x == t.x; }
    };

}

namespace cxon {
    template <typename X, typename II, typename C>
        inline enable_for_t<X, CBOR> read_value(Struct6& t, II& b, II e, C& ctx) {
            return read_value<X>(t.x, b, e, ctx);
        }
    template <typename X, typename OI, typename C>
        inline bool write_value(OI& o, const Struct6& t, C& ctx) {
            return write_value<X>(o, t.x, ctx);
        }
}

TEST_BEG(cxon::CBOR<>)
    R_TEST(Struct6(1), BS("\x1"));
    W_TEST(BS("\x03"), Struct6(3));
TEST_END()


namespace {

    struct Struct7 {
        Struct7(int x = 0, int y = 0) : x(x), y(y) {}
        bool operator ==(const Struct7& t) const { return x == t.x && y == t.y; }

        CXON_CBOR_CLS_MEMBER(Struct7,
            CXON_CBOR_CLS_FIELD_ASIS(x),
            CXON_CBOR_CLS_FIELD_ASIS(y)
        )

    private:
        int x;
        int y;
    };

}

TEST_BEG(cxon::CBOR<>)
    R_TEST(Struct7(1, 2), BS("\xA2\x62x\0\x01\x62y\0\x02")); // {x: 1, y: 2}
    R_TEST(Struct7(3, 0), BS("\xA1\x62x\0\x03")); // {x: 3}
    R_TEST(Struct7(0, 6), BS("\xA1\x62y\0\x06")); // {y: 6}
    W_TEST(BS("\xA2\x62x\0\x09\x62y\0\x0A"), Struct7(9, 10)); // {x: 9, y: 10}
TEST_END()


namespace {

    struct Struct8 {
        Struct8(int x = 0, int y = 0) : x(x), y(y) {}
        bool operator ==(const Struct8& t) const { return x == t.x && y == t.y; }

        template <typename X, typename II, typename C>
            static bool read_value(Struct8& t, II& i, II e, C& ctx) {
                using namespace cxon::cbor::cls;
                static CXON_CBOR_CLS_CONSTEXPR auto f = make_fields(
                    make_field("x", &Struct8::x),
                    make_field("y", &Struct8::y)
                );
                return read_fields<X>(t, f, i, e, ctx);
            }
        template <typename X, typename OI, typename C>
            static bool write_value(OI& o, const Struct8& t, C& ctx) {
                using namespace cxon::cbor::cls;
                static CXON_CBOR_CLS_CONSTEXPR auto f = make_fields(
                    make_field("x", &Struct8::x),
                    make_field("y", &Struct8::y)
                );
                return write_fields<X>(o, t, f, ctx);
            }

    private:
        int x;
        int y;
    };

}

TEST_BEG(cxon::CBOR<>)
    R_TEST(Struct8(1, 2), BS("\xA2\x62x\0\x01\x62y\0\x02")); // {x: 1, y: 2}
    W_TEST(BS("\xA2\x62x\0\x03\x62y\0\x04"), Struct8(3, 4)); // {x: 3, y: 4}
TEST_END()


namespace {

    struct Struct9 {
        static int x;
        static int const y;

        bool operator ==(const Struct9&) const { return true; }

        CXON_CBOR_CLS_READ_MEMBER(Struct9,
            CXON_CBOR_CLS_FIELD_ASIS(x)
        )
        CXON_CBOR_CLS_WRITE_MEMBER(Struct9,
            CXON_CBOR_CLS_FIELD_ASIS(x),
            CXON_CBOR_CLS_FIELD_ASIS(y)
        )
    };
    int Struct9::x = 0;
    int const Struct9::y = 3;

}

TEST_BEG(cxon::CBOR<>) // static field
    R_TEST(Struct9(), BS("\xA1\x62x\0\x00")); // {x: 0}
    W_TEST(BS("\xA2\x62x\0\x00\x62y\0\x03"), Struct9()); // {x: 0, y: 3}
    R_TEST(Struct9(), BS("\xA1\x62x\0\x01")); // {x: 1}
    W_TEST(BS("\xA2\x62x\0\x01\x62y\0\x03"), Struct9()); // {x: 1, y: 3}
    R_TEST(Struct9(), BS("\xA0")); // {}
    W_TEST(BS("\xA2\x62x\0\x01\x62y\0\x03"), Struct9()); // {x: 1, y: 3}
    R_TEST(Struct9(), BS("\xA1\x62x\0\x03")); // {x: 3}
    W_TEST(BS("\xA2\x62x\0\x03\x62y\0\x03"), Struct9()); // {x: 3, y: 3}
TEST_END()


namespace {

    struct Struct10 {
        int x;
        int y;
        Struct10(int x = 0, int y = 0) : x(x), y(y) {}
        bool operator ==(const Struct10& t) const { return x == t.x && y == t.y; }
    };

}

CXON_CBOR_CLS_READ(Struct10,
    CXON_CBOR_CLS_FIELD_ASIS(x),
    CXON_CBOR_CLS_FIELD_SKIP("y"),
    CXON_CBOR_CLS_FIELD_SKIP("z")
)
CXON_CBOR_CLS_WRITE(Struct10,
    CXON_CBOR_CLS_FIELD_ASIS(x),
    CXON_CBOR_CLS_FIELD_NAME("z", y)
)

TEST_BEG(cxon::CBOR<>)
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x17\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x18\x18\x62x\0\x01")); // {y: '2, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x19\x01\x01\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x1A\x00\x01\x01\x01\x62x\0\x01")); // {y: '5, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x1B\x00\x00\x00\x01\x01\x01\x01\x01\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x3B\x00\x00\x00\x01\x01\x01\x01\x01\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x3A\x00\x01\x01\x01\x62x\0\x01")); // {y: '5, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x39\x01\x01\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x38\x18\x62x\0\x01")); // {y: '2, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x37\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xD7\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xD8\x18\x62x\0\x01")); // {y: '2, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xD9\x01\x01\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xDA\x00\x01\x01\x01\x62x\0\x01")); // {y: '5, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xDB\x00\x00\x00\x01\x01\x01\x01\x01\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xFB\x00\x00\x00\x01\x01\x01\x01\x01\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xFA\x00\x01\x01\x01\x62x\0\x01")); // {y: '5, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xF9\x01\x01\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xF8\x18\x62x\0\x01")); // {y: '2, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xF4\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xF5\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xF6\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x42xx\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x58\x02xx\x62x\0\x01")); // {y: '4, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x5Fxxx\xFF\x62x\0\x01")); // {y: '5, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x82\x01\x02\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x82\x82\x01\x02\x82\x01\x02\x62x\0\x01")); // {y: '7, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x98\x05\x01\x02\x03\x04\x05\x62x\0\x01")); // {y: '4, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\x9F\x01\x02\x03\x04\x05\xFF\x62x\0\x01")); // {y: '4, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xA2\x01\x82\x02\x03\x02\x82\x03\x04\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xB8\x02\x01\x81\x02\x02\x82\x03\x04\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct10(1), BS("\xA2\x62y\0\xBF\x01\x81\x02\x02\x82\x03\x04\xFF\x62x\0\x01")); // {y: '9, x: 1}
    W_TEST(BS("\xA2\x62x\0\x01\x62z\0\x02"), Struct10(1, 2));
TEST_END()


namespace {

    struct Struct11 {
        int x;
        using sink = cxon::cbor::sink<cxon::test::bytes>;
        sink y;
        Struct11(int x = 0, sink y = sink{}) : x(x), y(y) {}
        bool operator ==(const Struct11& t) const { return x == t.x && y.value == t.y.value; }
    };

}

CXON_CBOR_CLS_READ(Struct11,
    CXON_CBOR_CLS_FIELD_ASIS(x),
    CXON_CBOR_CLS_FIELD_ASIS(y)
)
CXON_CBOR_CLS_WRITE(Struct11,
    CXON_CBOR_CLS_FIELD_ASIS(x),
    CXON_CBOR_CLS_FIELD_ASIS(y)
)

TEST_BEG(cxon::CBOR<>)
    R_TEST(Struct11(1, {BS("\x17")}), BS("\xA2\x62y\0\x17\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct11(1, {BS("\x18\x18")}), BS("\xA2\x62y\0\x18\x18\x62x\0\x01")); // {y: '2, x: 1}
    R_TEST(Struct11(1, {BS("\x19\x01\x01")}), BS("\xA2\x62y\0\x19\x01\x01\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct11(1, {BS("\x1A\x00\x01\x01\x01")}), BS("\xA2\x62y\0\x1A\x00\x01\x01\x01\x62x\0\x01")); // {y: '5, x: 1}
    R_TEST(Struct11(1, {BS("\x1B\x00\x00\x00\x01\x01\x01\x01\x01")}), BS("\xA2\x62y\0\x1B\x00\x00\x00\x01\x01\x01\x01\x01\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct11(1, {BS("\x3B\x00\x00\x00\x01\x01\x01\x01\x01")}), BS("\xA2\x62y\0\x3B\x00\x00\x00\x01\x01\x01\x01\x01\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct11(1, {BS("\x3A\x00\x01\x01\x01")}), BS("\xA2\x62y\0\x3A\x00\x01\x01\x01\x62x\0\x01")); // {y: '5, x: 1}
    R_TEST(Struct11(1, {BS("\x39\x01\x01")}), BS("\xA2\x62y\0\x39\x01\x01\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct11(1, {BS("\x38\x18")}), BS("\xA2\x62y\0\x38\x18\x62x\0\x01")); // {y: '2, x: 1}
    R_TEST(Struct11(1, {BS("\x37")}), BS("\xA2\x62y\0\x37\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct11(1, {BS("\xD7")}), BS("\xA2\x62y\0\xD7\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct11(1, {BS("\xD8\x18")}), BS("\xA2\x62y\0\xD8\x18\x62x\0\x01")); // {y: '2, x: 1}
    R_TEST(Struct11(1, {BS("\xD9\x01\x01")}), BS("\xA2\x62y\0\xD9\x01\x01\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct11(1, {BS("\xDA\x00\x01\x01\x01")}), BS("\xA2\x62y\0\xDA\x00\x01\x01\x01\x62x\0\x01")); // {y: '5, x: 1}
    R_TEST(Struct11(1, {BS("\xDB\x00\x00\x00\x01\x01\x01\x01\x01")}), BS("\xA2\x62y\0\xDB\x00\x00\x00\x01\x01\x01\x01\x01\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct11(1, {BS("\xFB\x00\x00\x00\x01\x01\x01\x01\x01")}), BS("\xA2\x62y\0\xFB\x00\x00\x00\x01\x01\x01\x01\x01\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct11(1, {BS("\xFA\x00\x01\x01\x01")}), BS("\xA2\x62y\0\xFA\x00\x01\x01\x01\x62x\0\x01")); // {y: '5, x: 1}
    R_TEST(Struct11(1, {BS("\xF9\x01\x01")}), BS("\xA2\x62y\0\xF9\x01\x01\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct11(1, {BS("\xF8\x18")}), BS("\xA2\x62y\0\xF8\x18\x62x\0\x01")); // {y: '2, x: 1}
    R_TEST(Struct11(1, {BS("\xF4")}), BS("\xA2\x62y\0\xF4\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct11(1, {BS("\xF5")}), BS("\xA2\x62y\0\xF5\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct11(1, {BS("\xF6")}), BS("\xA2\x62y\0\xF6\x62x\0\x01")); // {y: '1, x: 1}
    R_TEST(Struct11(1, {BS("\x42xx")}), BS("\xA2\x62y\0\x42xx\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct11(1, {BS("\x5Fxxx\xFF")}), BS("\xA2\x62y\0\x5Fxxx\xFF\x62x\0\x01")); // {y: '5, x: 1}
    R_TEST(Struct11(1, {BS("\x82\x01\x02")}), BS("\xA2\x62y\0\x82\x01\x02\x62x\0\x01")); // {y: '3, x: 1}
    R_TEST(Struct11(1, {BS("\x82\x82\x01\x02\x82\x01\x02")}), BS("\xA2\x62y\0\x82\x82\x01\x02\x82\x01\x02\x62x\0\x01")); // {y: '7, x: 1}
    R_TEST(Struct11(1, {BS("\x98\x05\x01\x02\x03\x04\x05")}), BS("\xA2\x62y\0\x98\x05\x01\x02\x03\x04\x05\x62x\0\x01")); // {y: '4, x: 1}
    R_TEST(Struct11(1, {BS("\x9F\x01\x02\x03\x04\x05\xFF")}), BS("\xA2\x62y\0\x9F\x01\x02\x03\x04\x05\xFF\x62x\0\x01")); // {y: '4, x: 1}
    R_TEST(Struct11(1, {BS("\xA2\x01\x82\x02\x03\x02\x82\x03\x04")}), BS("\xA2\x62y\0\xA2\x01\x82\x02\x03\x02\x82\x03\x04\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct11(1, {BS("\xB8\x02\x01\x81\x02\x02\x82\x03\x04")}), BS("\xA2\x62y\0\xB8\x02\x01\x81\x02\x02\x82\x03\x04\x62x\0\x01")); // {y: '9, x: 1}
    R_TEST(Struct11(1, {BS("\xBF\x01\x81\x02\x02\x82\x03\x04\xFF")}), BS("\xA2\x62y\0\xBF\x01\x81\x02\x02\x82\x03\x04\xFF\x62x\0\x01")); // {y: '9, x: 1}
    W_TEST(BS("\xA2\x62x\0\x01\x62y\0\xB8\x02\x01\x81\x02\x02\x82\x03\x04"), Struct11(1, {BS("\xB8\x02\x01\x81\x02\x02\x82\x03\x04")}));
    // errors
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x1C"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x1D"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x1E"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x1F"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x58"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x79"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x5A"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x7B"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x5C"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x7D"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x5E"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x5F"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x81"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\xA1"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\xA1\01"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x98"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x98\x01"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\xB8"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\xB8\x01"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\xB8\x01\01"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\xBF"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\xBF\x01\xFF"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x9C"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\xBC"), cbor::read_error::unexpected, 4);
    R_TEST(Struct11(0), BS("\xA2\x62y\0\x9E"), cbor::read_error::unexpected, 4);
TEST_END()


TEST_BEG(cxon::CBOR<>) // sink
    {   bio::byte o[2];
        auto const r = to_bytes(std::begin(o), std::end(o), cbor::sink<test::bytes>{BS("\01\02\03")});
        TEST_CHECK(!r && r.ec == cbor::write_error::output_failure);
    }
TEST_END()


TEST_BEG(cxon::CBOR<>) // tags
    using namespace test;
    // T[]
    {   unsigned a[] = { 0 };
        R_TEST(a, BS("\xC1\x40"));
        R_TEST(a, BS("\xD8\x18\x40"));
        R_TEST(a, BS("\xD9\x01\x01\x40"));
        R_TEST(a, BS("\xDA\x01\x01\x01\x01\x40"));
        R_TEST(a, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x40"));
        R_TEST(a, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x5F\xFF"));
        R_TEST(a, BS("\xDA\x01\x01\x01\x01\x5F\xFF"));
        R_TEST(a, BS("\xD9\x01\x01\x5F\xFF"));
        R_TEST(a, BS("\xD8\x18\x5F\xFF"));
        R_TEST(a, BS("\xC1\x5F\xFF"));
        R_TEST(a, BS("\xC1\x60"));
        R_TEST(a, BS("\xD8\x18\x60"));
        R_TEST(a, BS("\xD9\x01\x01\x60"));
        R_TEST(a, BS("\xDA\x01\x01\x01\x01\x60"));
        R_TEST(a, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x60"));
        R_TEST(a, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x7F\xFF"));
        R_TEST(a, BS("\xDA\x01\x01\x01\x01\x7F\xFF"));
        R_TEST(a, BS("\xD9\x01\x01\x7F\xFF"));
        R_TEST(a, BS("\xD8\x18\x7F\xFF"));
        R_TEST(a, BS("\xC1\x7F\xFF"));
        R_TEST(a, BS("\xC1\x80"));
        R_TEST(a, BS("\xD8\x18\x80"));
        R_TEST(a, BS("\xD9\x01\x01\x80"));
        R_TEST(a, BS("\xDA\x01\x01\x01\x01\x80"));
        R_TEST(a, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x80"));
        R_TEST(a, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x9F\xFF"));
        R_TEST(a, BS("\xDA\x01\x01\x01\x01\x9F\xFF"));
        R_TEST(a, BS("\xD9\x01\x01\x9F\xFF"));
        R_TEST(a, BS("\xD8\x18\x9F\xFF"));
        R_TEST(a, BS("\xC1\x9F\xFF"));
    }
    // T*
        R_TEST((int*)nullptr, BS("\xC1\xF6"));
        R_TEST((int*)nullptr, BS("\xD8\x18\xF6"));
        R_TEST((int*)nullptr, BS("\xD9\x01\x01\xF6"));
        R_TEST((int*)nullptr, BS("\xDA\x01\x01\x01\x01\xF6"));
        R_TEST((int*)nullptr, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\xF6"));
    // const char*
        R_TEST((const char*)"", BS("\xC1\x40"));
        R_TEST((const char*)"", BS("\xD8\x18\x40"));
        R_TEST((const char*)"", BS("\xD9\x01\x01\x40"));
        R_TEST((const char*)"", BS("\xDA\x01\x01\x01\x01\x40"));
        R_TEST((const char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x40"));
        R_TEST((const char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x5F\xFF"));
        R_TEST((const char*)"", BS("\xDA\x01\x01\x01\x01\x5F\xFF"));
        R_TEST((const char*)"", BS("\xD9\x01\x01\x5F\xFF"));
        R_TEST((const char*)"", BS("\xD8\x18\x5F\xFF"));
        R_TEST((const char*)"", BS("\xC1\x5F\xFF"));
        R_TEST((const char*)"", BS("\xC1\x60"));
        R_TEST((const char*)"", BS("\xD8\x18\x60"));
        R_TEST((const char*)"", BS("\xD9\x01\x01\x60"));
        R_TEST((const char*)"", BS("\xDA\x01\x01\x01\x01\x60"));
        R_TEST((const char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x60"));
        R_TEST((const char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x7F\xFF"));
        R_TEST((const char*)"", BS("\xDA\x01\x01\x01\x01\x7F\xFF"));
        R_TEST((const char*)"", BS("\xD9\x01\x01\x7F\xFF"));
        R_TEST((const char*)"", BS("\xD8\x18\x7F\xFF"));
        R_TEST((const char*)"", BS("\xC1\x7F\xFF"));
        R_TEST((const char*)"", BS("\xC1\x80"));
        R_TEST((const char*)"", BS("\xD8\x18\x80"));
        R_TEST((const char*)"", BS("\xD9\x01\x01\x80"));
        R_TEST((const char*)"", BS("\xDA\x01\x01\x01\x01\x80"));
        R_TEST((const char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x80"));
        R_TEST((const char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x9F\xFF"));
        R_TEST((const char*)"", BS("\xDA\x01\x01\x01\x01\x9F\xFF"));
        R_TEST((const char*)"", BS("\xD9\x01\x01\x9F\xFF"));
        R_TEST((const char*)"", BS("\xD8\x18\x9F\xFF"));
        R_TEST((const char*)"", BS("\xC1\x9F\xFF"));
    // char*
        R_TEST((char*)"", BS("\xC1\x40"));
        R_TEST((char*)"", BS("\xD8\x18\x40"));
        R_TEST((char*)"", BS("\xD9\x01\x01\x40"));
        R_TEST((char*)"", BS("\xDA\x01\x01\x01\x01\x40"));
        R_TEST((char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x40"));
        R_TEST((char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x5F\xFF"));
        R_TEST((char*)"", BS("\xDA\x01\x01\x01\x01\x5F\xFF"));
        R_TEST((char*)"", BS("\xD9\x01\x01\x5F\xFF"));
        R_TEST((char*)"", BS("\xD8\x18\x5F\xFF"));
        R_TEST((char*)"", BS("\xC1\x5F\xFF"));
        R_TEST((char*)"", BS("\xC1\x60"));
        R_TEST((char*)"", BS("\xD8\x18\x60"));
        R_TEST((char*)"", BS("\xD9\x01\x01\x60"));
        R_TEST((char*)"", BS("\xDA\x01\x01\x01\x01\x60"));
        R_TEST((char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x60"));
        R_TEST((char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x7F\xFF"));
        R_TEST((char*)"", BS("\xDA\x01\x01\x01\x01\x7F\xFF"));
        R_TEST((char*)"", BS("\xD9\x01\x01\x7F\xFF"));
        R_TEST((char*)"", BS("\xD8\x18\x7F\xFF"));
        R_TEST((char*)"", BS("\xC1\x7F\xFF"));
        R_TEST((char*)"", BS("\xC1\x80"));
        R_TEST((char*)"", BS("\xD8\x18\x80"));
        R_TEST((char*)"", BS("\xD9\x01\x01\x80"));
        R_TEST((char*)"", BS("\xDA\x01\x01\x01\x01\x80"));
        R_TEST((char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x80"));
        R_TEST((char*)"", BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x9F\xFF"));
        R_TEST((char*)"", BS("\xDA\x01\x01\x01\x01\x9F\xFF"));
        R_TEST((char*)"", BS("\xD9\x01\x01\x9F\xFF"));
        R_TEST((char*)"", BS("\xD8\x18\x9F\xFF"));
        R_TEST((char*)"", BS("\xC1\x9F\xFF"));
    // enum
        R_TEST(Enum1::one, BS("\xC1\x01"));
        R_TEST(Enum1::one, BS("\xD8\x18\x01"));
        R_TEST(Enum1::one, BS("\xD9\x01\x01\x01"));
        R_TEST(Enum1::one, BS("\xDA\x01\x01\x01\x01\x01"));
        R_TEST(Enum1::one, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x01"));
    // struct
        R_TEST(Struct2{1, 2}, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\xA2\x62x\0\x01\x62Y\0\x02"));
        R_TEST(Struct2{1, 2}, BS("\xDA\x01\x01\x01\x01\xA2\x62x\0\x01\x62Y\0\x02"));
        R_TEST(Struct2{1, 2}, BS("\xD9\x01\x01\xA2\x62x\0\x01\x62Y\0\x02"));
        R_TEST(Struct2{1, 2}, BS("\xD8\x18\xA2\x62x\0\x01\x62Y\0\x02"));
        R_TEST(Struct2{1, 2}, BS("\xC1\xA2\x62x\0\x01\x62Y\0\x02"));
        R_TEST(Struct9(), BS("\xC1\xA1\x62x\0\x00"));
        R_TEST(Struct9(), BS("\xD8\x18\xA1\x62x\0\x00"));
        R_TEST(Struct9(), BS("\xD9\x01\x01\xA1\x62x\0\x00"));
        R_TEST(Struct9(), BS("\xDA\x01\x01\x01\x01\xA1\x62x\0\x00"));
        R_TEST(Struct9(), BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\xA1\x62x\0\x00"));
    // errors
        R_TEST((int*)nullptr, BS("\xDC"), cbor::read_error::tag_invalid, 0);
        R_TEST((char*)"", BS("\xDD"), cbor::read_error::tag_invalid, 0);
        R_TEST(Enum1::one, BS("\xDE"), cbor::read_error::tag_invalid, 0);
        R_TEST(Struct9(), BS("\xDF"), cbor::read_error::tag_invalid, 0);
TEST_END()
