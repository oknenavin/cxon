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

#ifndef CXON_TEST_CBOR_NODE_HXX_
#define CXON_TEST_CBOR_NODE_HXX_

#include "../utility.hxx"

#include <string>
#include <vector>
#include <map>

namespace test {
    
    struct fix {
        std::string act;
        std::string data;
        std::string fail;
    };
    struct fixture {
        using vector = std::vector<fixture>;
        std::string label;
        std::vector<std::string> group;
        std::map<std::string, struct fix> fixup;
    };

}

namespace test { namespace kind {

    int self();

    int vector(const fixture::vector& fs);
    int roundtrip(const fixture::vector& fs);
    int time(const fixture::vector& fs);

}}

#endif // CXON_TEST_CBOR_NODE_HXX_
