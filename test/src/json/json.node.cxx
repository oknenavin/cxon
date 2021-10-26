// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "json.node.hxx"

#include "cxon/json.hxx"
#include "cxon/lang/json/tidy.hxx"
#include "cxon/lib/node.ordered.hxx"

#include <fstream>
#include <cstdio>

////////////////////////////////////////////////////////////////////////////////

using node = cxon::json::ordered_node;

int main(int argc, char *argv[]) {
    if (argc == 1)
#       ifndef CXON_TIME_ONLY
            return test::kind::self();
#       else
            return -1;
#       endif

    test::cases pass, fail, diff, time;

    if (!test::parse_cl(argc, argv, pass, fail, diff, time))
        return std::fprintf(stderr, "usage: cxon/json/node ((pass|fail|diff|time) (file|@file)+)+\n"), std::fflush(stderr), 1;

    int err = 0;

#   ifndef CXON_TIME_ONLY
        if (!pass.empty())
            err += test::kind::pass(pass);
        if (!fail.empty())
            err += test::kind::fail(fail);
        if (!diff.empty())
            err += test::kind::diff(diff);
#   endif // CXON_TIME_ONLY

    if (!time.empty())
        test::kind::time(time);

    return err;
}

namespace test {

    bool parse_cl(int argc, char *argv[], cases& pass, cases& fail, cases& diff, cases& time) {
        if (argc < 2) return false;
        static auto const key = [](const char* v) {
            return  std::strcmp("pass", v) == 0 || std::strcmp("fail", v) == 0 ||
                    std::strcmp("time", v) == 0 || std::strcmp("diff", v) == 0
            ;
        };
        static auto const add = [](const char* v, cases& c) {
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
#           define CXON_JSON_PROC(k)\
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
#           undef CXON_JSON_PROC
        }
        return true;
    }

}

#ifndef CXON_TIME_ONLY

    namespace test { namespace kind {

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
            std::fprintf(stdout, "cxon/json/node/pass:  %zu of %3zu failed\n", fc, cases.size()); std::fflush(stdout);

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
            std::fprintf(stdout, "cxon/json/node/fail:  %zu of %3zu failed\n", fc, cases.size()); std::fflush(stdout);

            return err;
        }

        int diff(cases& cases) {
            int err = 0;

            static auto const name = [](const std::string& p) {
                auto f = p.find_last_of("/\\"); f = f != p.npos ? f + 1 : 0;
                auto l = p.rfind('.'); if (l == p.npos) l = p.size();
                return std::string(p, f, l - f);
            };
            for (auto& c : cases) {
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
                            ++err, c.error = format_error(r, json.cbegin());
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
                for (auto& c : cases) {
                    if (!c.error.empty()) {
                        ++fc, std::fprintf(stderr, "%s:\n\tfailed: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                    }
                }
            if (!fc) {
                for (auto& c : cases) {
                    std::fprintf(stdout, "%s %s ", (name(c.source) + ".jt(0).json").c_str(), (name(c.source) + ".jt(1).json").c_str()), std::fflush(stdout);
                }
                std::fputc('\n', stdout);
            }
            else {
                std::fprintf(stdout, "cxon/json/node/diff:  %zu of %3zu failed\n", fc, cases.size()), std::fflush(stdout);
            }

            return err;
        }

    }}

#endif // CXON_TIME_ONLY

namespace test { namespace kind {

    void time(cases& cases) {
        struct {
            std::vector<std::string> name;
            std::vector<void(*)(test&)> func;
        }   cnts;
            cnts.name.push_back("CXON/JSON/NODE");
            cnts.func.push_back(&time_cxon_node);
#       ifdef CXON_TIME_BOOST_JSON
            cnts.name.push_back("Boost/JSON");
            cnts.func.push_back(&time_boostjson);
#       endif
#       ifdef CXON_TIME_RAPIDJSON
            cnts.name.push_back("RapidJSON");
            cnts.func.push_back(&time_rapidjson);
#       endif
#       ifdef CXON_TIME_NLOHMANN
            cnts.name.push_back("nlohmann/JSON");
            cnts.func.push_back(&time_nlohmannjson);
#       endif

        {   // node
            auto time_node = cases;
            for (auto& test: time_node)
                for (auto& func: cnts.func)
                    func(test);
            time_print(cnts.name, time_node);
        }
        {   // type
            auto time_type = cases;
            cnts.name[0] = "CXON/JSON";
            cnts.func[0] = &time_cxon_type;
            for (auto& test: time_type)
                for (auto& func : cnts.func)
                    func(test);
            time_print(cnts.name, time_type);
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
            ::test::time total;
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
