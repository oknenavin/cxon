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

    using cio::read_error;
    using cio::write_error;

    using cio::read_error_category;
    using cio::write_error_category;

    // node

    enum class node_error;
    struct node_error_category;

}}

namespace std { // cxon errors

    template <> struct is_error_condition_enum<cxon::json::read_error>;
    template <> struct is_error_condition_enum<cxon::json::write_error>;

}

#endif // CXON_JSON_JSON_FWD_HXX_
