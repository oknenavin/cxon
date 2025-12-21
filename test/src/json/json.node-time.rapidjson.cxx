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

#ifdef CXON_TIME_RAPIDJSON

#include "json.node.hxx"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#include <memory>
#include <fstream>

////////////////////////////////////////////////////////////////////////////////

namespace test { namespace kind {

    void time_rapidjson(test& t) {
        std::ifstream is(t.source, std::ifstream::binary);
            if (!is) return t.error = "cannot be opened", void();
        std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
        // size
            t.size = json.size() - 1;
        // read
            std::vector<std::unique_ptr<rapidjson::Document>> vo;
                auto constexpr opt = rapidjson::kParseValidateEncodingFlag|rapidjson::kParseFullPrecisionFlag;
            t.time.read.push_back(CXON_MEASURE(
                vo.emplace_back(std::unique_ptr<rapidjson::Document>(new rapidjson::Document));
                    rapidjson::ParseResult const r = vo.back()->Parse<opt>(json.c_str());
                if (!r) t.error = std::string("RapidJSON error: ") + rapidjson::GetParseError_En(r.Code());
            ));
        // write
            auto o = std::unique_ptr<rapidjson::Document>(vo.back().release()); vo.clear();
            {   const char *s;
                t.time.write.push_back(CXON_MEASURE(
                    rapidjson::StringBuffer buffer;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                    o->Accept(writer);
                    s = buffer.GetString();
                ));
            }
    }

}}

#endif // CXON_TIME_RAPIDJSON
