// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "json.node.hxx"

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/node.ordered.hxx"

#include <fstream>
#include <cstdio>
#include <cstring>
#include <cctype>

////////////////////////////////////////////////////////////////////////////////


namespace test {

    struct config {
        struct set {
            std::string label;
            std::vector<std::string> group;
        };
        using index = std::map<std::string, set>;
        index tests;
        index sets;
    };

}


int main(int argc, char *argv[]) {
    if (argc == 1)
#       ifndef CXON_TIME_ONLY
            return test::kind::self();
#       else
            return -1;
#       endif

    test::config cf;
    std::map<std::string, test::cases> sets;

    if (argc > 1) {
        if (auto is = std::ifstream(argv[1])) {
            auto const str = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            auto const fbr = cxon::from_bytes<cxon::CXCF<>>(cf, str);
            if (!fbr) {
                std::fprintf(stderr, "%s\n", test::format_error(fbr, std::begin(str)).c_str());
                std::fprintf(stderr, "usage: cxon-json-node <tests>.cf <label>*\n");
                return -1;
            }
        }
        else {
            std::fprintf(stderr, "\"%s\": cannot be opened\n", argv[1]);
            return -1;
        }
        std::string const cfd = test::get_directory(argv[1]);
        static auto const load = [&](const test::config::set& ts) {
            auto& cs = sets[ts.label];
            for (auto& t : ts.group) {
                auto& ss = cf.sets[t];
                for (auto& s : ss.group) {
                    auto p = !test::is_absolute(s.c_str()) ? cfd + s : s;
                    cs.push_back({std::move(p), 0, {}, {}, false});
                }
            }
        };
        if (argc > 2) {
            for (int i = 2; i != argc; ++i)
                if (cf.tests.find(argv[i]) == cf.tests.end()) {
                        std::fprintf(stderr, "%s: test not found in \"%s\"\n", argv[i], argv[1]);
                        return -1;
                }
                else    load(cf.tests[argv[i]]);
        }
        else
            for (auto& ts : cf.tests) load(ts.second);
    }

    int err = 0;

#   ifndef CXON_TIME_ONLY
        if (!sets["pass"].empty())
            err += test::kind::pass(sets["pass"]);
        if (!sets["fail"].empty())
            err += test::kind::fail(sets["fail"]);
        if (!sets["diff-node"].empty())
            err += test::kind::diff_node(sets["diff-node"]);
        if (!sets["diff-native"].empty())
            err += test::kind::diff_native(sets["diff-native"]);
#   endif
        if (!sets["time-node"].empty())
            test::kind::time(sets["time-node"], test::kind::time_cxon_node);
        if (!sets["time-native"].empty())
            test::kind::time(sets["time-native"], test::kind::time_cxon_native);

    return err;
}

CXON_JSON_CLS_READ(test::config::set,
    CXON_JSON_CLS_FIELD_ASIS(label),
    CXON_JSON_CLS_FIELD_ASIS(group)
)
CXON_JSON_CLS_READ(test::config,
    CXON_JSON_CLS_FIELD_ASIS(tests),
    CXON_JSON_CLS_FIELD_ASIS(sets)
)
CXON_JSON_CLS_BARE(test::config)

#ifndef CXON_TIME_ONLY

    namespace test { namespace kind {

        using node = cxon::json::ordered_node;

        int pass(cases& cases) {
            int err = 0;

            for (auto& c : cases) {
                std::ifstream is(c.source, std::ifstream::binary);
                    if (!is) {
                        ++err, c.error = "cannot be opened";
                        continue;
                    }
                std::string const s = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                node result; auto const r = cxon::from_bytes(result, s);
                if (!r) {
                    ++err, c.error += "must pass: '" + c.source + "' (failed with '" + format_error(r, s.begin()) + "')";
                }
            }
            std::size_t fc = 0;
                for (auto& c : cases) {
                    if (!c.error.empty()) {
                        ++fc, std::fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                    }
                }
            std::fprintf(stdout, "%-21s: %zu of %4zu failed\n", "cxon/json/node/pass", fc, cases.size()); std::fflush(stdout);

            return err;
        }

        int fail(cases& cases) {
            int err = 0;

            for (auto& c : cases) {
                std::ifstream is(c.source, std::ifstream::binary);
                    if (!is) {
                        ++err, c.error = "cannot be opened";
                        continue;
                    }
                node result;
                std::string const s = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                auto const e = s.end();
                if (auto r = cxon::from_bytes(result, s.begin(), e)) {
                    cxon::cio::consume<cxon::JSON<>>(r.end, e);
                        if (r.end != e) continue; // trailing chars
                    std::string pass; cxon::to_bytes(pass, result);
                    ++err, c.error += "must fail: '" + c.source + "' (passed as '" + pass + "')";
                }
            }
            std::size_t fc = 0;
                for (auto& c : cases) {
                    if (!c.error.empty()) {
                        ++fc, std::fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                    }
                }
            std::fprintf(stdout, "%-21s: %zu of %4zu failed\n", "cxon/json/node/fail", fc, cases.size()); std::fflush(stdout);

            return err;
        }

    }}

#endif // CXON_TIME_ONLY

namespace test { namespace kind {

    template <typename TIME_CXON>
        void time(cases& cases, TIME_CXON time_cxon) {
            struct {
                std::vector<std::string> name;
                std::vector<void(*)(test&)> func;
            }   cnts;
                cnts.name.push_back(time_cxon == &time_cxon_node ? "# cxon/node" : "# cxon/native");
                cnts.func.push_back(time_cxon);
#           ifdef CXON_TIME_BOOST_JSON
                cnts.name.push_back("Boost/JSON");
                cnts.func.push_back(&time_boostjson);
#           endif
#           ifdef CXON_TIME_RAPIDJSON
                cnts.name.push_back("RapidJSON");
                cnts.func.push_back(&time_rapidjson);
#           endif
#           ifdef CXON_TIME_NLOHMANN
                cnts.name.push_back("nlohmann/JSON");
                cnts.func.push_back(&time_nlohmannjson);
#           endif
            {   // time
                auto time_node = cases;
                for (auto& test: time_node)
                    for (auto& func: cnts.func)
                        func(test);
                time_print(cnts.name, time_node);
            }
        }

}}

namespace test { namespace kind {

    void time_print(const std::vector<std::string>& names, const cases& c) {
        CXON_ASSERT(!names.empty(), "unexpected");
        for (auto& t: c)
            CXON_ASSERT(names.size() == t.time.read.size() && names.size() == t.time.write.size(), "inconsistent");
        std::vector<std::vector<std::string>> tab;
        {   // build the table
            static auto const fmt = [](double d) -> std::string {
                char b[64];
                int const r = std::snprintf(b, sizeof(b), "%.2f", d); CXON_ASSERT( r > 0 && r < (int)sizeof(b), "unexpected");
                return b;
            };
            {   // header
                tab.push_back({names[0], "Size"});
                tab.back().push_back("Read");
                for (size_t i = 1, is = names.size(); i != is; ++i)
                    tab.back().insert(tab.back().end(), {"x", names[i]});
                tab.back().push_back("Write");
                for (size_t i = 1, is = names.size(); i != is; ++i)
                    tab.back().insert(tab.back().end(), {"x", names[i]});
            }
            ::test::time total; double total_size = 0;
                total.read.resize(c.front().time.read.size());
                total.write.resize(c.front().time.write.size());
            {   // body
                for (auto& t: c) {
                    if (!t.error.empty()) {
                        std::fprintf(stderr, "%s: %s\n", t.source.c_str(), t.error.c_str());
                        continue;
                    }
                    double const size = double(t.size) / (1024. * 1024);
                    tab.push_back({
                        t.source.substr(t.source.rfind('/') + 1),
                        fmt(size)
                    });
                    tab.back().push_back(fmt(size / (t.time.read[0] / 1000)));
                    total.read[0] += size / (t.time.read[0] / 1000);
                    for (std::size_t i = 1, is = t.time.read.size(); i != is; ++i) {
                        tab.back().push_back(fmt(t.time.read[i] / t.time.read[0])),
                        tab.back().push_back(fmt(size / (t.time.read[i] / 1000)));
                        total.read[i] += size / (t.time.read[i] / 1000);
                    }
                    tab.back().push_back(fmt(size / (t.time.write[0] / 1000)));
                    total.write[0] += size / (t.time.write[0] / 1000);
                    for (std::size_t i = 1, is = t.time.write.size(); i != is; ++i) {
                        tab.back().push_back(fmt(t.time.write[i] / t.time.write[0])),
                        tab.back().push_back(fmt(size / (t.time.write[i] / 1000)));
                        total.write[i] += size / (t.time.write[i] / 1000);
                    }
                    total_size += size;
                }
            }
            {   // average
                tab.push_back({
                    "<average>",
                    fmt(total_size / c.size())
                });
                tab.back().push_back(fmt(total.read[0] / c.size()));
                for (std::size_t i = 1, is  = total.read.size(); i != is; ++i) {
                    tab.back().push_back(fmt(total.read[0] / total.read[i]));
                    tab.back().push_back(fmt(total.read[i] / c.size()));
                }
                tab.back().push_back(fmt(total.write[0] / c.size()));
                for (std::size_t i = 1, is  = total.write.size(); i != is; ++i) {
                    tab.back().push_back(fmt(total.write[0] / total.write[i]));
                    tab.back().push_back(fmt(total.write[i] / c.size()));
                }
            }
        }

        std::vector<std::size_t> wid(tab[0].size(), 0);
        {   // column width
            for (auto& r : tab)
                for (std::size_t i = 0, is = r.size(); i != is; ++i)
                    wid[i] = std::max(wid[i], r[i].length());
        }

        {   // print
            {   // header
                std::fprintf(stdout, "%-*s", (unsigned)wid[0], tab.front()[0].c_str());
                for (std::size_t i = 1, is = tab.front().size(); i != is; ++i)
                    std::fprintf(stdout, "  %*s", (unsigned)wid[i], tab.front()[i].c_str());
                std::fputc('\n', stdout);
            }
            {   // body
                for (std::size_t i = 1, is = tab.size(); i != is; ++i) {
                    std::fprintf(stdout, "%-*s", (unsigned)wid[0], tab[i][0].c_str());
                    for (std::size_t j = 1, js = tab[i].size(); j != js; ++j)
                        std::fprintf(stdout, "  %*s", (unsigned)wid[j], tab[i][j].c_str());
                    std::fputc('\n', stdout);
                }
            }
        }
        std::fflush(stdout);
    }

}}
