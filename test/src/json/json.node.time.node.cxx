// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "json.node.hxx"

#include "cxon/json.hxx"
#include "cxon/lib/node.ordered.hxx"

#include <fstream>

////////////////////////////////////////////////////////////////////////////////

using node = cxon::json::ordered_node;

namespace test { namespace benchmark {

    void cxon_node_time_run(test& t) {
        std::ifstream is(t.source, std::ifstream::binary);
            if (!is) return t.error = "cannot be opened", void();
        std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
        // size
            t.size = json.size() - 1;
        // read
            std::vector<node> vo;
            t.time.read.push_back(CXON_MEASURE(
                vo.emplace_back();
                auto const r = cxon::from_bytes(vo.back(), json);
                if (!r) t.error = format_error(r, json.begin());
            ));
        // write
            auto o = vo.back(); vo.clear();
            std::vector<std::string> vs;
            t.time.write.push_back(CXON_MEASURE(
                vs.emplace_back();
                cxon::to_bytes(vs.back(), o);
            ));
    }

}}