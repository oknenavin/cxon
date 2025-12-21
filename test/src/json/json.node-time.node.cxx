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

#include "json.node-time.hxx"
#include "cxon/lib/node.ordered.hxx"

#include <array>
#include <fstream>

////////////////////////////////////////////////////////////////////////////////

using node = cxon::json::ordered_node;

namespace test { namespace kind {

    void time_cxon_node(test& t) {
        std::ifstream is(t.source, std::ifstream::binary);
            if (!is) return t.error = "cannot be opened", void();
        std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
        // size
            t.size = json.size() - 1;
        // read
            std::vector<node> vo;
            {   // check
                vo.emplace_back();
                auto const r = cxon::from_bytes<TIME>(vo.back(), json);
                    if (!r) return t.error = format_error(r, json.begin()), void();
            }
            t.time.read.push_back(CXON_MEASURE(
                vo.emplace_back();
                auto const r = cxon::from_bytes<TIME>(vo.back(), json);
                if (!r) t.error = format_error(r, json.begin());
            ));
        // write
            auto o = vo.back(); vo.clear();
            std::vector<std::string> vs;
            {   // check
                vs.emplace_back();
                auto r = cxon::to_bytes<TIME>(vs.back(), o);
                    if (!r) return t.error = format_error(r, vs.back().begin()), void();
            }
            std::array<char, 4 * 1024> bf;
            t.time.write.push_back(CXON_MEASURE(
                vs.emplace_back();
                cxon::to_bytes<TIME>(cxon::cio::buffered_back_inserter(vs.back(), bf), o);
            ));
    }

}}
