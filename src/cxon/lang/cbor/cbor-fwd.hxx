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
