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

#ifndef CXON_TIME_ONLY

#include "json.node.hxx"
#include "json.node-time.native.hxx"

#include "cxon/lib/node.ordered.hxx"
#include "cxon/lang/json/tidy.hxx"

#include <fstream>


////////////////////////////////////////////////////////////////////////////////

namespace test { namespace kind {

    using node = cxon::json::ordered_node;

    static std::string name(const std::string& p) {
        auto f = p.find_last_of("/\\"); f = f != p.npos ? f + 1 : 0;
        auto l = p.rfind('.'); if (l == p.npos) l = p.size();
        return std::string(p, f, l - f);
    };

    template <typename T>
        int diff(test& test, const char* prefix) {
            std::string json;
            {   // read
                std::ifstream is(test.source, std::ifstream::binary);
                    if (!is) {
                        test.error = "cannot be opened";
                        return 1;
                    }
                json.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            }
            {   // tidy
                std::ofstream os(prefix + name(test.source) + ".(0).json", std::ofstream::binary);
                    if (!os) {
                        test.error = prefix + name(test.source) + ".(0).json" + ": cannot be opened";
                        return 1;
                    }
                cxon::json::tidy(std::ostreambuf_iterator<char>(os), json);
            }
            T result;
            {   // from
                auto const r = cxon::from_bytes(result, json);
                    if (!r) {
                        test.error = format_error(r, json.cbegin());
                        return 1;
                    }
            }
            {   // to
                std::ofstream os(prefix + name(test.source) + ".(1).json", std::ofstream::binary);
                    if (!os) {
                        test.error = prefix + name(test.source) + ".(1).json" + "cannot be opened";
                        return 1;
                    }
                auto const w = cxon::to_bytes(cxon::json::make_indenter(std::ostreambuf_iterator<char>(os)), result);
                    if (!w) {
                        test.error += w.ec.category().name(),
                        test.error += ": " + w.ec.message();
                    }
            }
            return 0;
        }

    int diff_node(cases& cases) {
        int err = 0;

            char const*const prefix = "json.node.";

            for (auto& c : cases) {
                err += diff<node>(c, prefix);
            }
            std::size_t fc = 0;
                for (auto& c : cases) {
                    if (!c.error.empty()) {
                        ++fc, std::fprintf(stderr, "%s:\n\tfailed: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                    }
                }
            if (!fc) {
                for (auto& c : cases) {
                    std::fprintf(stdout, "%s %s ", (prefix + name(c.source) + ".(0).json").c_str(), (prefix + name(c.source) + ".(1).json").c_str()), std::fflush(stdout);
                }
                std::fputc('\n', stdout);
            }
            else {
                std::fprintf(stdout, "%-21s: %zu of %4zu failed\n", "cxon/json/node/diff", fc, cases.size()), std::fflush(stdout);
            }

        return err;
    }

    namespace native {

        static std::map<std::string, int (*)(test&, const char*)> executors_ = {
            { "blns.json",              &diff<blns::object> },
            { "emoji.json",             &diff<emoji::object> },
            { "apache_builds.json",     &diff<apache_builds::object> },
            { "canada.json",            &diff<canada::object> },
#           ifdef CXON_HAS_LIB_STD_OPTIONAL
            { "citm_catalog.json",      &diff<citm_catalog::object> },
#           endif
#           ifdef CXON_HAS_LIB_STD_OPTIONAL
            { "github_events.json",     &diff<github_events::object> },
#           endif
            { "gsoc-2018.json",         &diff<gsoc_2018::object> },
#           ifdef CXON_HAS_LIB_STD_OPTIONAL
            { "instruments.json",       &diff<instruments::object> },
#           endif
            { "marine_ik.json",         &diff<marine_ik::object> },
            { "mesh.json",              &diff<mesh::object> },
            { "mesh.pretty.json",       &diff<mesh::object> },
            { "numbers.json",           &diff<numbers::object> },
            { "random.json",            &diff<random::object> },
#           ifdef CXON_HAS_LIB_STD_OPTIONAL
            { "twitter.json",           &diff<twitter::object> },
            { "twitterescaped.json",    &diff<twitter::object> },
#           endif
            { "update-center.json",     &diff<update_center::object> }
        };

    }

    int diff_native(cases& cases) {
        int err = 0;

            char const*const prefix = "json.native.";

            for (auto& c : cases) {
                auto i = native::executors_.find(c.source.substr(c.source.rfind('/') + 1));
                if (i == native::executors_.end()) {
                    c.flag = true, c.error = "unknown format or unsupported with this C++ standard version";
                    continue;
                }
                else
                    err += i->second(c, prefix);
            }
            std::size_t fc = 0;
                for (auto& c : cases) {
                    if (!c.error.empty()) {
                        std::fprintf(stderr, "%s:\n\tfailed: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                        if (!c.flag) ++fc;
                    }
                }
            if (!fc) {
                for (auto& c : cases) {
                    if (c.flag) continue;
                    std::fprintf(stdout, "%s %s ", (prefix + name(c.source) + ".(0).json").c_str(), (prefix + name(c.source) + ".(1).json").c_str()), std::fflush(stdout);
                }
                std::fputc('\n', stdout);
            }
            else {
                std::fprintf(stdout, "%-21s: %zu of %4zu failed\n", "cxon/json/node/diff", fc, cases.size()), std::fflush(stdout);
            }

        return err;
    }

}}

#endif // CXON_TIME_ONLY
