// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cbor.node.hxx"

#include "cxon/cbor.hxx"
#include "cxon/json.hxx"
#include "cxon/lang/json/tidy.hxx"
#include "cxon/lib/node.ordered.hxx"

#include <fstream>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////

namespace test {

    struct config {
        using index = std::map<std::string, fixture>;
        index tests;
        index sets;
    };

}


int main(int argc, char *argv[]) {
    if (argc == 1) {
#       ifndef CXON_TIME_ONLY
            return test::kind::self();
#       else
            return -1;
#       endif
    }

    test::config cf;
    std::map<std::string, test::fixture::vector> sets;

    if (argc > 1) {
        if (auto is = std::ifstream(argv[1])) {
            auto const str = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            auto const fbr = cxon::from_bytes<cxon::CXCF<>>(cf, str);
            if (!fbr) {
                std::fprintf(stderr, "%s\n", test::format_error(fbr, std::begin(str)).c_str());
                std::fprintf(stderr, "usage: cxon-cbor-node <tests>.cf <label>*\n");
                return -1;
            }
        }
        else {
            std::fprintf(stderr, "\"%s\": cannot be opened\n", argv[1]);
            return -1;
        }
        std::string const cfd = test::get_directory(argv[1]);
        for (auto& s : cf.sets)
            for (auto& f : s.second.group)
                if (!test::is_absolute(f.c_str())) f = cfd + f;
        static auto const load = [&](const test::fixture& ts) {
            auto& fs = sets[ts.label];
            for (auto& s : ts.group)
                fs.push_back(cf.sets[s]);
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
            err += test::kind::vector(sets["pass"]);
        if (!sets["round-trip"].empty())
            err += test::kind::roundtrip(sets["round-trip"]);
#   endif
        if (!sets["time"].empty())
            err += test::kind::time(sets["time"]);

    return err;
}

CXON_JSON_CLS_READ(test::fix,
    CXON_JSON_CLS_FIELD_ASIS(act),
    CXON_JSON_CLS_FIELD_ASIS(data),
    CXON_JSON_CLS_FIELD_ASIS(fail)
)
CXON_JSON_CLS_READ(test::fixture,
    CXON_JSON_CLS_FIELD_ASIS(label),
    CXON_JSON_CLS_FIELD_ASIS(group),
    CXON_JSON_CLS_FIELD_ASIS(fixup)
)
CXON_JSON_CLS_READ(test::config,
    CXON_JSON_CLS_FIELD_ASIS(tests),
    CXON_JSON_CLS_FIELD_ASIS(sets)
)
CXON_JSON_CLS_BARE(test::config)

namespace test { namespace kind { // test-vector

    struct unquoted_keys_traits : cxon::json::format_traits {
        static constexpr bool quote_unquoted_keys = false;
    };
    using UQK_JSON = cxon::JSON<unquoted_keys_traits>;

    struct test {
        std::string hex;
        bool roundtrip;
        cxon::json::node decoded;
        std::string diagnostic;

        using vector = std::vector<test>;

        CXON_JSON_CLS_READ_MEMBER(test,
            CXON_JSON_CLS_FIELD_SKIP("cbor"),
            CXON_JSON_CLS_FIELD_ASIS(hex),
            CXON_JSON_CLS_FIELD_ASIS(roundtrip),
            CXON_JSON_CLS_FIELD_ASIS(decoded),
            CXON_JSON_CLS_FIELD_ASIS(diagnostic)
        )

        std::string bin() const {
            return hex.length() % 2 == 0 ?
                hex2bin(hex) :
                std::string {}
            ;
        }

        static std::string hex2bin(const std::string& hex) {
            static constexpr unsigned char hex_[256] = {
                /*  0*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                /* 16*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                /* 32*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                /* 48*/ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
                /* 64*/ 0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                /* 80*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                /* 96*/ 0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };
            CXON_ASSERT(hex.length() % 2 == 0, "invalid input");
            std::string bin;
                for (std::size_t i = 0, is = hex.length(); i != is; i += 2) {
                    char const b = char((hex_[(unsigned)hex[i]] << 4) + hex_[(unsigned)hex[i + 1]]);
                    bin += b;
                }
            return bin;
        }
    };

    int vector(const fixture::vector& fs) {
        int all = 0, err = 0, skip = 0;

        for (auto& fixture : fs) for (auto& file: fixture.group) {
            std::ifstream is(file);
                if (!is) {
                    CXON_ASSERT(0, "unexpected");
                    ++err, std::fprintf(stderr, "error: cannot be opened: '%s'\n", file.c_str());
                    continue;
                }
            test::vector vector;
                auto const r = cxon::from_bytes<cxon::JSON<>>(vector, std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                if (!r) {
                    CXON_ASSERT(0, "unexpected");
                    ++err, std::fprintf(stderr, "error: test-vector invalid: '%s': %s\n", file.c_str(), r.ec.message().c_str());
                    continue;
                }

            for (auto& test : vector) {
                ++all;
                cxon::json::node decoded;
                std::string fail;
                {   auto const fix = fixture.fixup.find(test.hex);
                    if (fix != fixture.fixup.end()) {
                        if (fix->second.act == "json") {
#                           if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                                auto const r = cxon::from_bytes<UQK_JSON>(
                                    decoded, fix->second.data,
                                    cxon::node::json::extract_nans::set<true>()
                                );
#                           else
                                // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                                // seems to be fixed around 10
                                auto const r = cxon::from_bytes<UQK_JSON, cxon::json::node_traits<>>(
                                    decoded, fix->second.data,
                                    cxon::node::json::extract_nans::set<true>()
                                );
#                           endif
                            CXON_ASSERT(r, "invalid fixture");
                        }
                        else if (fix->second.act == "skip") {
                            ++skip/*, std::fprintf(stdout, "skip: '%s' (%s)\n", test.hex.c_str(), fix->second.data.c_str()), std::fflush(stdout)*/;
                            continue;
                        }
                        fail = fix->second.fail;
                    }
                    else {
                        decoded = test.decoded;
                    }
                }
                {   cxon::json::node json;
                        auto const r = cxon::from_bytes(json, test.bin());
                    if (!r) {
                        fail.empty() ?
                            (++err, std::fprintf(stderr, "fail: '%s'\n", test.hex.c_str())) :
                            (/*std::fprintf(stderr, "must fail: '%s' (%s)\n", test.hex.c_str(), fail.c_str()), */0)
                        ;
                    }
                    else if (json != decoded) {
                        fail.empty() ?
                            (++err, std::fprintf(stderr, "fail: '%s'\n", test.hex.c_str())) :
                            (/*std::fprintf(stderr, "must fail: '%s' (%s)\n", test.hex.c_str(), fail.c_str()), */0)
                        ;
                    }
                    else if (!fail.empty()) {
                        ++err, std::fprintf(stderr, "must fail but passed: '%s' (%s)\n", test.hex.c_str(), fail.c_str());
                    }
                }
            }
        }

        std::fprintf(stdout, "%-21s: %d of %4d failed (%d skipped)\n", "cxon/cbor/node/suite", err, all, skip); std::fflush(stdout);

        return err;
    }

}}

namespace test { namespace kind { // round-trip

    int roundtrip(const fixture::vector& fs) {
        using JSON = cxon::JSON<>;
        using CBOR = cxon::CBOR<>;

        static auto const name = [](const std::string& p) {
            auto f = p.find_last_of("/\\"); f = f != p.npos ? f + 1 : 0;
            auto l = p.rfind('.'); if (l == p.npos) l = p.size();
            return std::string(p, f, l - f);
        };

        int err = 0;
        std::vector<std::string> diff;

        for (auto& fixture : fs) for (auto& file: fixture.group) {
            std::ifstream is(file);
                if (!is) {
                    CXON_ASSERT(0, "unexpected");
                    ++err, std::fprintf(stderr, "error: cannot be opened: '%s'\n", file.c_str());
                    continue;
                }
            std::string const f0 = "cbor.round-trip." + name(file) + ".(0).json";
            std::string const f1 = "cbor.round-trip." + name(file) + ".(1).json";
            {   // 0
                std::ofstream os(f0, std::ofstream::binary);
                    if (!os) {
                        ++err, std::fprintf(stderr, "error: cannot be opened: %s\n", f0.c_str());
                        continue;
                    }
                cxon::json::tidy<JSON>(
                    std::ostreambuf_iterator<char>(os),
                    std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()
                );
            }
            {   // json => cbor => json
                is.seekg(0);
                cxon::cbor::ordered_node n0;
                    {   auto const r = cxon::from_bytes<JSON>(n0, std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                        if (!r) {
                            ++err, std::fprintf(stderr, "error: %s: %s\n", r.ec.category().name(), r.ec.message().c_str());
                            continue;
                        }
                    }
                std::string cbor;
                    {   auto const r = cxon::to_bytes<CBOR>(cbor, n0);
                        if (!r) {
                            ++err, std::fprintf(stderr, "error: %s: %s\n", r.ec.category().name(), r.ec.message().c_str());
                            continue;
                        }
                    }
                cxon::json::ordered_node n1;
                    {   auto const r = cxon::from_bytes<CBOR>(n1, cbor);
                        if (!r) {
                            ++err, std::fprintf(stderr, "error: %s: %s\n", r.ec.category().name(), r.ec.message().c_str());
                            continue;
                        }
                    }
                {   // 1
                    std::ofstream os(f1, std::ofstream::binary);
                    if (!os) {
                        ++err, std::fprintf(stderr, "error: cannot be opened: %s\n", f1.c_str());
                        continue;
                    }
                    auto const r = cxon::to_bytes<JSON>(cxon::json::make_indenter<JSON>(std::ostreambuf_iterator<char>(os)), n1);
                    if (!r) {
                        ++err, std::fprintf(stderr, "error: %s: %s\n", r.ec.category().name(), r.ec.message().c_str());
                        continue;
                    }
                }
                diff.push_back(f0), diff.push_back(f1);
            }
        }
        for (auto& d : diff)
            std::fprintf(stdout, "%s ", d.c_str());
        std::fputc('\n', stdout);

        return err;
    }

}}

namespace test { namespace kind { // time

    struct test_time {
        std::string source;
        std::string error;
        std::size_t size = 0;
        double read = 0;
        double write = 0;
    };

    int time(const fixture::vector& fs) {
        using JSON = cxon::JSON<>;
        using CBOR = cxon::CBOR<>;

        std::vector<test_time> time;

        for (auto& fixture : fs) for (auto& file: fixture.group) {
            test_time t;

            std::string cbor;
            {   // JSON => CBOR
                std::ifstream is(file);
                    if (!is) {
                        CXON_ASSERT(0, "unexpected");
                        t.error = "error: cannot be opened: ''" + file + "'";
                        continue;
                    }
                cxon::cbor::ordered_node n;
                {   auto const r = cxon::from_bytes<JSON>(n, std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                    if (!r) {
                        t.error = r.ec.category().name() + std::string(": ") + r.ec.message();
                        continue;
                    }
                }
                {   auto const r = cxon::to_bytes<CBOR>(cbor, n);
                    if (!r) {
                        t.error = r.ec.category().name() + std::string(": ") + r.ec.message();
                        continue;
                    }
                }
            }
            {   // time
                t.source = file;
                t.size = cbor.size();
                // read
                    std::vector<cxon::cbor::ordered_node> vo;
                    t.read = CXON_MEASURE(
                        vo.emplace_back();
                        auto const r = cxon::from_bytes(vo.back(), cbor);
                        if (!r) t.error = format_error(r, cbor.cbegin());
                    );
                // write
                    auto o = vo.back(); vo.clear();
                    std::vector<std::string> vs;
                    t.write = CXON_MEASURE(
                        vs.emplace_back();
                        cxon::to_bytes(vs.back(), o);
                    );
            }
            time.push_back(t);
        }

        {   // print
            std::vector<std::vector<std::string>> tab;
            {   // build the table
                static auto const fmt = [](double d) -> std::string {
                    char b[64];
                    int const r = std::snprintf(b, sizeof(b), "%.2f", d); CXON_ASSERT( r > 0 && r < (int)sizeof(b), "unexpected");
                    return b;
                };
                {   // header
                    tab.push_back({"# cbor/node", "Size", "Read", "Write"});
                }
                test_time total; double total_size = 0;
                {   // body
                    for (auto& t : time) {
                        if (!t.error.empty()) {
                            std::fprintf(stderr, "%s: %s\n", t.source.c_str(), t.error.c_str());
                            continue;
                        }
                        double const size = double(t.size) / (1024. * 1024);
                        tab.push_back({
                            t.source.substr(t.source.rfind('/') + 1),
                            fmt(size),
                            fmt(size / (t.read / 1000)),
                            fmt(size / (t.write/ 1000))
                        });
                        total.read += size / (t.read / 1000),
                        total.write += size / (t.write / 1000);
                        total_size += size;
                    }
                }
                {   // average
                    tab.push_back({
                        "<average>",
                        fmt(total_size / time.size()),
                        fmt(total.read / time.size()),
                        fmt(total.write/ time.size())
                    });
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
                        std::fprintf(stdout, " %*s", (unsigned)wid[i], tab.front()[i].c_str());
                    std::fputc('\n', stdout);
                }
                {   // body
                    for (std::size_t i = 1, is = tab.size(); i != is; ++i) {
                        std::fprintf(stdout, "%-*s", (unsigned)wid[0], tab[i][0].c_str());
                        for (std::size_t j = 1, js = tab[i].size(); j != js; ++j)
                            std::fprintf(stdout, " %*s", (unsigned)wid[j], tab[i][j].c_str());
                        std::fputc('\n', stdout);
                    }
                }
            }
            std::fflush(stdout);
        }

        int err = 0;
            for (auto& t : time)
                err += !t.error.empty();
        return err;
    }

}}
