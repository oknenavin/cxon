// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "../cxon.hxx"
#include "../pretty.hxx"
#include "../cxjson/cxjson.hxx"
#include "../cxjson/pretty.hxx"

#include <fstream>
#include <memory>
#include <chrono>
#include <cstdio>
#include <cassert>

///////////////////////////////////////////////////////////////////////////////

using node = cxjson::ordered_node;
using JSON = cxon::JSON<cxjson::format_traits>;

struct test_time {
    double base = 0;
    double read = 0;
    double write = 0;
    double pretty = 0;
    double pretty_native = 0;
};

struct test_case {
    std::string source;
    test_time time;
    std::string error;
};

template <typename B>
    double measure(B block) {
        using clock = std::chrono::steady_clock;
        using std::chrono::duration_cast;
        using std::chrono::nanoseconds;
        auto const s = clock::now();
            block();
        return duration_cast<nanoseconds>(clock::now() - s).count() / 1e6;
    }

template <typename R, typename I>
    static std::string format_error(const R& r, I b) {
        return std::string()
            .append(r.ec.category().name())
            .append(":")
            .append(std::to_string(std::distance(b, r.end)))
            .append(": ")
            .append(r.ec.message())
        ;
    }

static void cxjson_test_time(test_case& test) {
    std::ifstream is(test.source, std::ifstream::binary);
        if (!is) return test.error = "cannot be opened", void();
    std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
    {   // base
        std::unique_ptr<char> s;
        test.time.base = measure([&] {
            s = std::unique_ptr<char>(_strdup(json.c_str()));
        });
    }
    {   // cxon
        node j;
        test.time.read = measure([&] {
            auto const r = cxon::from_chars(j, json);
            if (!r) test.error = format_error(r, json.begin());
        });
        test.time.write = measure([&] {
            std::string s; cxon::to_chars(s, j);
        });
        {   std::string s;
            test.time.pretty = measure([&] {
                std::string s;
                //cxon::to_chars(cxon::make_indenter(std::back_inserter(s)), j);
                cxon::to_chars(cxon::make_indenter(s), j);
            });
            test.time.pretty_native = measure([&] {
                s = cxjson::pretty(j);
            });
        }
    }
}

using cases = std::vector<test_case>;

bool cl_parse(int argc, char *argv[], cases& pass, cases& fail, cases& time, cases& diff) {
    if (argc < 2) return false;
    static auto const key = [](const char* v) {
        return  std::strcmp("pass", v) == 0 || std::strcmp("fail", v) == 0 ||
                std::strcmp("time", v) == 0 || std::strcmp("diff", v) == 0
        ;
    };
    static auto const add = [](const char* v, cases& c) {
        if (*v == '@') {
            if (std::ifstream is = std::ifstream(v + 1)) {
                    std::string l;
                    while (std::getline(is, l)) {
                        auto const f = l.find_first_not_of(" \t");
                        if (f != std::string::npos && l[f] != '#') {
                            c.push_back({l, {}, {}});
                        }
                    }
            }
            else    fprintf(stderr, "%s: cannot be opened\n", v);
        }
        else {
            c.push_back({v, {}, {}});
        }
    };
    for (int i = 1; i < argc; ++i) {
        restart:
#       define CXJSON_PROC(k)\
            if (std::strcmp(#k, argv[i]) == 0) {\
                if (++i == argc || key(argv[i])) return false;\
                do {\
                    if (key(argv[i])) goto restart;\
                    add(argv[i], k);\
                }   while (++i < argc);\
                continue;\
            }
        CXJSON_PROC(pass)
        CXJSON_PROC(fail)
        CXJSON_PROC(diff)
        CXJSON_PROC(time)
#       undef CXJSON_PROC
    }
    return true;
}

int main(int argc, char *argv[]) {
    cases pass, fail, time, diff;
    if (!cl_parse(argc, argv, pass, fail, time, diff)) {
        return fprintf(stderr, "usage: cxjson ((pass|fail|time|diff) (file|@file)+)+\n"), 1;
    }
    int err = 0;
    if (!pass.empty()) {
        for (auto& c : pass) {
            std::ifstream is(c.source, std::ifstream::binary);
                if (!is) {
                    ++err, c.error = c.source + ": cannot be opened";
                    continue;
                }
            std::string const s = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            node result; auto const r = cxon::from_chars(result, s);
            if (!r) {
                ++err, c.error += "must pass: '" + c.source + "' (failed with '" + format_error(r, s.begin()) + "')";
            }
        }
        size_t fc = 0;
            for (auto& c : pass) {
                if (!c.error.empty()) {
                    ++fc, fprintf(stderr, "%s\n", c.error.c_str());
                }
            }
        fc ?
            fprintf(stdout, "cxjson/pass: %zu of %zu failed\n", fc, pass.size()) :
            fprintf(stdout, "cxjson/pass: %zu of %zu passed\n", pass.size(), pass.size())
        ;
    }
    if (!fail.empty()) {
        for (auto& c : fail) {
            std::ifstream is(c.source, std::ifstream::binary);
                if (!is) {
                    ++err, c.error = c.source + ": cannot be opened";
                    continue;
                }
            node result;
            auto const e = std::istreambuf_iterator<char>();
            auto r = cxon::from_chars(result, std::istreambuf_iterator<char>(is), e);
            if (r) {
                cxon::io::consume<cxon::JSON<>>(r.end, e);
                if (r.end != e) continue; // trailing chars
                std::string tluser; cxon::to_chars(tluser, result);
                ++err, c.error += "must fail: '" + c.source + "' (passed as '" + tluser + "')";
            }
        }
        size_t fc = 0;
            for (auto& c : fail) {
                if (!c.error.empty()) {
                    ++fc, fprintf(stderr, "%s\n", c.error.c_str());
                }
            }
        fc ?
            fprintf(stdout, "cxjson/fail: %zu of %zu failed\n", fc, fail.size()) :
            fprintf(stdout, "cxjson/fail: %zu of %zu passed\n", fail.size(), fail.size())
        ;
    }
    if (!diff.empty()) {
        for (auto& c : diff) {
            std::string json;
            {
                std::ifstream is(c.source, std::ifstream::binary);
                    if (!is) {
                        c.error = "cannot be opened";
                        continue;
                    }
                json.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            }
            static auto const name = [](const std::string& p) {
                auto f = p.find_last_of("/\\"); f = f != p.npos ? f + 1 : 0;
                auto l = p.rfind('.'); if (l == p.npos) l = p.size();
                return std::string(p, f, l - f);
            };
            if (std::ofstream os = std::ofstream(name(c.source) + ".0.json", std::ofstream::binary)) {
                auto o = cxon::make_indenter(std::ostreambuf_iterator<char>(os));
                for (auto c : json)  *o = c;
            }
            node result;
            if (auto const r = cxon::from_chars(result, json)) {
                if (std::ofstream os = std::ofstream(name(c.source) + ".1.json", std::ofstream::binary)) {
                    auto o = cxon::make_indenter(std::ostreambuf_iterator<char>(os));
                    auto const r = cxon::to_chars(o, result); CXON_ASSERT(r, "unexpected");
                }
                fprintf(stdout, "%s %s ", (name(c.source) + ".0.json").c_str(), (name(c.source) + ".1.json").c_str());
            }
            else {
                c.error = format_error(r, json.cbegin());
            }
        }
        for (auto& c : diff) {
            if (!c.error.empty()) {
                fprintf(stderr, "%s:\n\tfailed: %s\n", c.source.c_str(), c.error.c_str());
            }
        }
    }
    if (!time.empty()) {
        for (auto& c : time) {
            cxjson_test_time(c);
        }
        test_time total;
        for (auto& c : time) {
            fprintf(stdout, "%s:\n", c.source.c_str());
            if (c.error.empty()) {
                fprintf(stdout, "\tbase          :\t%8.2f\n", c.time.base);
                fprintf(stdout, "\tread          :\t%8.2f\tx %6.2f\n", c.time.read, c.time.read / c.time.base);
                fprintf(stdout, "\twrite         :\t%8.2f\tx %6.2f\n", c.time.write, c.time.write / c.time.base);
                fprintf(stdout, "\tcxon::pretty  :\t%8.2f\tx %6.2f\n", c.time.pretty, c.time.pretty / c.time.base);
                fprintf(stdout, "\tcxjson::pretty:\t%8.2f\tx %6.2f\n", c.time.pretty_native, c.time.pretty_native / c.time.base);
                total.base += c.time.base,
                total.read += c.time.read,
                total.write += c.time.write,
                total.pretty += c.time.pretty,
                total.pretty_native += c.time.pretty_native;
            }
            else {
                fprintf(stdout, "\tfailed: %s\n", c.error.c_str());
            }
        }
        fprintf(stdout, "------------------------------------------------\n");
        fprintf(stdout, "\tread          :                 x %6.2f\n", total.read / total.base);
        fprintf(stdout, "\twrite         :                 x %6.2f\n", total.write / total.base);
        fprintf(stdout, "\tcxon::pretty  :                 x %6.2f\n", total.pretty / total.base);
        fprintf(stdout, "\tcxjson::pretty:                 x %6.2f\n", total.pretty_native / total.base);
    }
    return err;
}
