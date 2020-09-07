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
        
        static constexpr bio::byte  neg     = 0xF4;
        static constexpr bio::byte  pos     = 0xF5;
        static constexpr bio::byte  nil     = 0xF6;

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
