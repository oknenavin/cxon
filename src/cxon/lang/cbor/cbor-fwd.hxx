// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_CBOR_FWD_HXX_
#define CXON_CBOR_CBOR_FWD_HXX_

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // format traits

    struct format_traits;

}}

namespace cxon { // format selector

    template <typename T = cbor::format_traits>
        struct CBOR;

}

namespace cxon { namespace cbor { // errors

    struct read_error;
    struct write_error;

    struct read_error_category;
    struct write_error_category;

}}

namespace std { // cxon errors

    template <> struct is_error_condition_enum<cxon::cbor::read_error>;
    template <> struct is_error_condition_enum<cxon::cbor::write_error>;

}

#endif // CXON_CBOR_CBOR_FWD_HXX_
