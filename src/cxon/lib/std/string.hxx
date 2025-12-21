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

#ifndef CXON_LIB_STD_STRING_HXX_
#   define CXON_LIB_STD_STRING_HXX_

#   include <string>

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/std/string.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/std/string.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, typename ...R>
        struct traits<std::basic_string<T, R...>> {
            template <typename II>
                static bool append(std::basic_string<T, R...>& c, II f, II l) {
                    return c.append(f, l), true;
                }
        };

}}

#endif // CXON_LIB_STD_STRING_HXX_
