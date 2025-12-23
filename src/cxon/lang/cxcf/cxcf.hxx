// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CXON_CXCF_CXCF_HXX_
#define CXON_CXCF_CXCF_HXX_

#include "cxon/lang/json/json.hxx"

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cxcf { // format traits

    struct format_traits : json::format_traits {
        struct map : json::format_traits::map {
            static constexpr char div = '=';
            static constexpr char sep = ' ';
        };
        struct list : json::format_traits::list {
            static constexpr char sep = ' ';
        };
        static constexpr bool quote_unquoted_keys       = false;    // object keys for types serialized without quotes will be quoted (e.g. strings will still be quoted, but numbers will not)
        static constexpr bool unquote_quoted_keys       = true;     // object keys for types serialized with quotes will be unquoted (e.g. strings)
        static constexpr bool allow_comments            = true;     // allow c-style comments
        static constexpr bool allow_javascript_nans     = true;     // allow NaN and Infinity
    };

}}

namespace cxon { // format selector
    template <typename T = cxcf::format_traits>
        using CXCF = JSON<T>;
}

#endif // CXON_CXCF_CXCF_HXX_
