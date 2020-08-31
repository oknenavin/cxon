// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_JSON_FWD_HXX_
#define CXON_JSON_JSON_FWD_HXX_

#include "cxon/lang/common/chio/error-fwd.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace json { // format traits

    struct format_traits;

}}

namespace cxon { // format selector

    template <typename T = json::format_traits>
        struct JSON;

}

namespace cxon { namespace json { // errors

    using chio::read_error;
    using chio::write_error;

    using chio::read_error_category;
    using chio::make_error_condition;

    using chio::write_error_category;
    using chio::make_error_condition;

}}

namespace std { // cxon errors

    template <> struct is_error_condition_enum<cxon::json::read_error>;
    template <> struct is_error_condition_enum<cxon::json::write_error>;

}

#endif // CXON_JSON_JSON_FWD_HXX_
