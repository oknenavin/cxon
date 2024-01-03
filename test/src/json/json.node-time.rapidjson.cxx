// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include "json.node.hxx"

#ifdef CXON_TIME_RAPIDJSON

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
            t.time.read.push_back(CXON_MEASURE(
                vo.emplace_back(std::make_unique<rapidjson::Document>());
                rapidjson::ParseResult const r = vo.back()->Parse(json.c_str());
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
