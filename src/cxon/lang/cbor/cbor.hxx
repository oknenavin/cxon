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

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // format traits

    struct format_traits {
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
