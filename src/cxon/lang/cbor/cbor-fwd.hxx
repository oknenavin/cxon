// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_CBOR_FWD_HXX_
#define CXON_CBOR_CBOR_FWD_HXX_

#include <system_error>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // format traits

    struct format_traits;

}}

namespace cxon { // format selector

    template <typename T = cbor::format_traits>
        struct CBOR;

}

namespace cxon { namespace cbor { // errors

    enum class read_error;
    enum class write_error;

    struct read_error_category;
    inline std::error_condition make_error_condition(read_error e) noexcept;

    struct write_error_category;
    inline std::error_condition make_error_condition(write_error e) noexcept;

}}

namespace std { // cxon errors

    template <> struct is_error_condition_enum<cxon::cbor::read_error>;
    template <> struct is_error_condition_enum<cxon::cbor::write_error>;

}

#endif // CXON_CBOR_CBOR_FWD_HXX_
