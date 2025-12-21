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

#ifndef CXON_LIB_STD_VECTOR_HXX_
#   define CXON_LIB_STD_VECTOR_HXX_

#   include <vector>

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/std/vector.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, typename ...R>
        struct traits<std::vector<T, R...>> {
            template <typename II>
                static bool append(std::vector<T, R...>& c, II f, II l) {
                    return c.insert(c.end(), f, l), true;
                }
        };

    template <typename X, typename ...A>
        struct element_reader<X, std::vector<bool, A...>> {
            template <typename II, typename Cx>
                static bool read(std::vector<bool, A...>& t, II& i, II e, Cx& cx) {
                    bool b; return read_value<X>(b, i, e, cx) && (t.push_back(b), true);
                }
        };
    template <typename X, typename ...A>
        struct element_writer<X, std::vector<bool, A...>> {
            template <typename O, typename Cx>
                static bool write(O& o, bool e, Cx& cx) {
                    return write_value<X>(o, e, cx);
                }
        };

}}

#endif // CXON_LIB_STD_VECTOR_HXX_
