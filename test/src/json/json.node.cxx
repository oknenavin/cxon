// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "json.node.hxx"

#include "cxon/json.hxx"
#include "cxon/lib/node.ordered.hxx"

#include "cxon/lang/json/tidy.hxx"

#include <fstream>
//#include <memory>
//#include <cstring>
#include <cstdio>

////////////////////////////////////////////////////////////////////////////////

using node = cxon::json::ordered_node;

namespace test { namespace benchmark {

    void print(const std::string& caption, const cases& c) {
        std::vector<std::vector<std::string>> tab;
        {   // build the table
            static auto const fmt = [](double d) -> std::string {
                char b[64];
                int const r = std::snprintf(b, sizeof(b), "%.2f", d); CXON_ASSERT( r > 0 && r < (int)sizeof(b), "unexpected");
                return b;
            };
            {   // header
                tab.push_back({
                    caption,
                    "Size",
                    "Read",
#                   ifdef CXON_TIME_BOOST_JSON
                        "x", "Boost/JSON",
#                   endif
#                   ifdef CXON_TIME_RAPIDJSON
                        "x", "RapidJSON",
#                   endif
#                   ifdef CXON_TIME_NLOHMANN
                        "x", "nlohmann/json",
#                   endif
                    "Write",
#                   ifdef CXON_TIME_BOOST_JSON
                        "x", "Boost/JSON",
#                   endif
#                   ifdef CXON_TIME_RAPIDJSON
                        "x", "RapidJSON",
#                   endif
#                   ifdef CXON_TIME_NLOHMANN
                        "x", "nlohmann/JSON",
#                   endif
                });
            }
            time total;
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
                }
            }
            {   // average
                tab.push_back({
                    "Average",
                    ""
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
            auto const line = [&wid]() {
                static auto const s = std::string(128, '-');
                std::fputc('+', stdout);
                for (auto s : wid)
                    std::fprintf(stdout, "-%s-+", std::string(s, '-').c_str());
                std::fputc('\n', stdout);
            };

            line();
            {   // header
                std::fprintf(stdout, "| %-*s |", (unsigned)wid[0], tab.front()[0].c_str());
                for (std::size_t i = 1, is = tab.front().size(); i != is; ++i)
                    std::fprintf(stdout, " %*s |", (unsigned)wid[i], tab.front()[i].c_str());
                std::fputc('\n', stdout);
            }
            line();
            {   // body
                for (std::size_t i = 1, is = tab.size() - 1; i != is; ++i) {
                    std::fprintf(stdout, "| %-*s |", (unsigned)wid[0], tab[i][0].c_str());
                    for (std::size_t j = 1, js = tab[i].size(); j != js; ++j)
                        std::fprintf(stdout, " %*s |", (unsigned)wid[j], tab[i][j].c_str());
                    std::fputc('\n', stdout);
                }
            }
            line();
            {   // average
                std::fprintf(stdout, "| %-*s |", (unsigned)wid[0], tab.back()[0].c_str());
                for (std::size_t i = 1, is = tab.back().size(); i != is; ++i)
                    std::fprintf(stdout, " %*s |", (unsigned)wid[i], tab.back()[i].c_str());
                std::fputc('\n', stdout);
            }
            line();
        }
    }

}}

static bool cl_parse(int argc, char *argv[], test::cases& pass, test::cases& fail, test::cases& time, test::cases& diff) {
    if (argc < 2) return false;
    static auto const key = [](const char* v) {
        return  std::strcmp("pass", v) == 0 || std::strcmp("fail", v) == 0 ||
                std::strcmp("time", v) == 0 || std::strcmp("diff", v) == 0
        ;
    };
    static auto const add = [](const char* v, test::cases& c) {
        if (*v == '@') {
            std::ifstream is(v + 1);
            if (is) {
                    std::string l;
                    while (std::getline(is, l)) {
                        auto const f = l.find_first_not_of(" \t");
                        if (f != std::string::npos && l[f] != '#') {
                            c.push_back({l, 0, {}, {}, false});
                        }
                    }
            }
            else    std::fprintf(stderr, "%s: cannot be opened\n", v);
        }
        else {
            c.push_back({v, 0, {}, {}, false});
        }
    };
    for (int i = 1; i < argc; ++i) {
        restart:
#       define CXON_JSON_PROC(k)\
            if (std::strcmp(#k, argv[i]) == 0) {\
                if (++i == argc || key(argv[i])) return false;\
                do {\
                    if (key(argv[i])) goto restart;\
                    add(argv[i], k);\
                }   while (++i < argc);\
                continue;\
            }
            CXON_JSON_PROC(pass)
            CXON_JSON_PROC(fail)
            CXON_JSON_PROC(diff)
            CXON_JSON_PROC(time)
#       undef CXON_JSON_PROC
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
#       ifndef CXON_TIME_ONLY
            return self::test();
#       else
            return -1;
#       endif
    }

    test::cases pass, fail, time, diff;

    if (!cl_parse(argc, argv, pass, fail, time, diff)) {
        return std::fprintf(stderr, "usage: cxon/json/node ((pass|fail|diff|time) (file|@file)+)+\n"), std::fflush(stderr), 1;
    }

    int err = 0;

#   ifndef CXON_TIME_ONLY
        if (!pass.empty()) {
            for (auto& c : pass) {
                std::ifstream is(c.source, std::ifstream::binary);
                    if (!is) {
                        ++err, c.error = "cannot be opened";
                        continue;
                    }
                std::string const s = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                node result; auto const r = cxon::from_bytes(result, s);
                if (!r) {
                    ++err, c.error += "must pass: '" + c.source + "' (failed with '" + test::format_error(r, s.begin()) + "')";
                }
            }
            std::size_t fc = 0;
                for (auto& c : pass) {
                    if (!c.error.empty()) {
                        ++fc, std::fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                    }
                }
            std::fprintf(stdout, "cxon/json/node/pass:  %zu of %3zu failed\n", fc, pass.size()); std::fflush(stdout);
        }

        if (!fail.empty()) {
            for (auto& c : fail) {
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
                for (auto& c : fail) {
                    if (!c.error.empty()) {
                        ++fc, std::fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                    }
                }
            std::fprintf(stdout, "cxon/json/node/fail:  %zu of %3zu failed\n", fc, fail.size()); std::fflush(stdout);
        }

        if (!diff.empty()) {
            static auto const name = [](const std::string& p) {
                auto f = p.find_last_of("/\\"); f = f != p.npos ? f + 1 : 0;
                auto l = p.rfind('.'); if (l == p.npos) l = p.size();
                return std::string(p, f, l - f);
            };
            for (auto& c : diff) {
                std::string json;
                {   // read
                    std::ifstream is(c.source, std::ifstream::binary);
                        if (!is) {
                            ++err, c.error = "cannot be opened";
                            continue;
                        }
                    json.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                }
                {   // tidy
                    std::ofstream os(name(c.source) + ".jt(0).json", std::ofstream::binary);
                        if (!os) {
                            ++err, c.error = name(c.source) + ".jt(0).json" + ": cannot be opened";
                            continue;
                        }
                    cxon::json::tidy(std::ostreambuf_iterator<char>(os), json);
                }
                node result;
                {   // from
                    auto const r = cxon::from_bytes(result, json);
                        if (!r) {
                            ++err, c.error = test::format_error(r, json.cbegin());
                            continue;
                        }
                }
                {   // to
                    std::ofstream os(name(c.source) + ".jt(1).json", std::ofstream::binary);
                        if (!os) {
                            ++err, c.error = name(c.source) + ".jt(1).json" + "cannot be opened";
                            continue;
                        }
                    auto const w = cxon::to_bytes(cxon::json::make_indenter(std::ostreambuf_iterator<char>(os)), result);
                        if (!w) {
                            ++err, c.error += w.ec.category().name(),
                            c.error += ": " + w.ec.message();
                        }
                }
            }
            std::size_t fc = 0;
                for (auto& c : diff) {
                    if (!c.error.empty()) {
                        ++fc, std::fprintf(stderr, "%s:\n\tfailed: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                    }
                }
            if (!fc) {
                for (auto& c : diff) {
                    std::fprintf(stdout, "%s %s ", (name(c.source) + ".jt(0).json").c_str(), (name(c.source) + ".jt(1).json").c_str()), std::fflush(stdout);
                }
                std::fputc('\n', stdout);
            }
            else {
                std::fprintf(stdout, "cxon/json/node/diff:  %zu of %3zu failed\n", fc, diff.size()), std::fflush(stdout);
            }
        }
#   endif

    if (!time.empty()) {
        namespace benchmark = test::benchmark;

        auto time_node = time;
        for (auto& t: time_node) {
            benchmark::cxon_node_time_run(t);
            benchmark::boostjson_time_run(t);
            benchmark::rapidjson_time_run(t);
            benchmark::nlohmannjson_time_run(t);
        }
        benchmark::print("CXON/JSON/NODE", time_node);

        auto time_cxon = time;
        for (auto& t: time_cxon) {
            benchmark::cxon_type_time_run(t);
            benchmark::boostjson_time_run(t);
            benchmark::rapidjson_time_run(t);
            benchmark::nlohmannjson_time_run(t);
            if (t.flag) // cxon_type_time_run resorts to node
                t.source.push_back('*');
        }
        benchmark::print("CXON/JSON", time_cxon);
    }

    return err;
}
