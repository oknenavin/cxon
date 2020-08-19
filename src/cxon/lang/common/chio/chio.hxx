// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHIO_HXX_
#define CXON_CHIO_HXX_

#include "io.hxx"

namespace cxon { namespace chio { // context parameters

    CXON_PARAMETER(allocator, std::allocator<char>);    // read: T*
    CXON_PARAMETER(num_len_max, unsigned);              // read: constexpr: numbers
    CXON_PARAMETER(ids_len_max, unsigned);              // read: constexpr: object key
    CXON_PARAMETER(fp_precision, int);                  // write: constexpr: floating-points

}}  // cxon::chio context parameters

#endif // CXON_CHIO_HXX_
