// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon.hxx"

#include "cxon/lib/std/utility.hxx" // std::pair
#include "cxon/lib/std/complex.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/string_view.hxx"
#include "cxon/lib/std/tuple.hxx"
#include "cxon/lib/std/array.hxx"
#include "cxon/lib/std/valarray.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/queue.hxx"
#include "cxon/lib/std/stack.hxx"
#include "cxon/lib/std/deque.hxx"
#include "cxon/lib/std/list.hxx"
#include "cxon/lib/std/forward_list.hxx"
#include "cxon/lib/std/set.hxx"
#include "cxon/lib/std/unordered_set.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/unordered_map.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/variant.hxx"
#include "cxon/lib/std/bitset.hxx"
#include "cxon/lib/std/chrono.hxx"

///////////////////////////////////////////////////////////////////////////////

TEST_BEG(cxon::CXON<>) // string
    using namespace std;
    // std::basic_string<char> / std::string
        R_TEST(string("test"), QS("test"));
        W_TEST(QS("test"), string("test"));
        R_TEST(string("te\nst"), QS("te\\nst"));
        W_TEST(QS("te\\nst"), string("te\nst"));
        R_TEST(string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<char16_t> / std::u16string
        R_TEST(u16string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), u16string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(u16string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), u16string({0x6D4B, 0x8BD5}));
        R_TEST(u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}));
        R_TEST(u16string(u"\xdbff\xdfff"), QS("\\udbff\\udfff")); // surrogate
        W_TEST(QS("\xf4\x8f\xbf\xbf"), u16string(u"\xdbff\xdfff")); // surrogate
        R_TEST(u16string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(u16string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(u16string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<char32_t> / std::u32string
        R_TEST(u32string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), u32string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(u32string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), u32string({0x6D4B, 0x8BD5}));
        R_TEST(u32string({0x0001241D, 0x0001241C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), u32string({0x0001241D, 0x0001241C}));
        R_TEST(u32string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(u32string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(u32string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<wchar_t> / std::wstring
        R_TEST(wstring({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), wstring({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(wstring({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), wstring({0x6D4B, 0x8BD5}));
        R_TEST(wstring(), "a", cxon::read_error::unexpected, 0);
        R_TEST(wstring(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(wstring(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
TEST_END()

TEST_BEG(cxon::JSON<>) // string
    using namespace std;
    // std::basic_string<char> / std::string
        R_TEST(string("test"), QS("test"));
        W_TEST(QS("test"), string("test"));
        R_TEST(string("te\nst"), QS("te\\nst"));
        W_TEST(QS("te\\nst"), string("te\nst"));
        R_TEST(string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<char16_t> / std::u16string
        R_TEST(u16string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), u16string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(u16string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), u16string({0x6D4B, 0x8BD5}));
        R_TEST(u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}));
        R_TEST(u16string(u"\xdbff\xdfff"), QS("\\udbff\\udfff")); // surrogate
        W_TEST(QS("\xf4\x8f\xbf\xbf"), u16string(u"\xdbff\xdfff")); // surrogate
        R_TEST(u16string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(u16string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(u16string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<char32_t> / std::u32string
        R_TEST(u32string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), u32string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(u32string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), u32string({0x6D4B, 0x8BD5}));
        R_TEST(u32string({0x0001241D, 0x0001241C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), u32string({0x0001241D, 0x0001241C}));
        R_TEST(u32string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(u32string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(u32string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<wchar_t> / std::wstring
        R_TEST(wstring({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), wstring({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(wstring({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), wstring({0x6D4B, 0x8BD5}));
        R_TEST(wstring(), "a", cxon::read_error::unexpected, 0);
        R_TEST(wstring(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(wstring(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
TEST_END()


namespace string_quote {
    struct single_traits : cxon::format_traits {
        struct string {
            static constexpr char beg = '\'';
            static constexpr char end = '\'';
        };
    };
}

TEST_BEG(cxon::CXON<string_quote::single_traits>) // cxon string quoting
    using namespace std;
    R_TEST(u32string(U"\x2Atest\052"), "'\\x2Atest\\052'");
    W_TEST("'test\u002A\\\\'", u32string(U"test\U0000002A\\"));
    R_TEST(u32string(U"'test'"), "'\\\'test\\\''");
    W_TEST("'\\\'test\\\''", u32string(U"'test'"));
    R_TEST(u32string(U"\"test\""), "'\"test\"'");
    W_TEST("'\"test\"'", u32string(U"\"test\""));
    R_TEST(u32string(U"\'"), "'\\''");
    R_TEST(u32string(U"\""), "'\\\"'");
    R_TEST(u32string(U"?"), "'\\?'");
    R_TEST(u32string(U"\\"), "'\\\\'");
    R_TEST(u32string(U"\a"), "'\\a'");
    R_TEST(u32string(U"\b"), "'\\b'");
    R_TEST(u32string(U"\f"), "'\\f'");
    R_TEST(u32string(U"\n"), "'\\n'");
    R_TEST(u32string(U"\r"), "'\\r'");
    R_TEST(u32string(U"\t"), "'\\t'");
    R_TEST(u32string(U"\v"), "'\\v'");
    R_TEST(u32string(U"\u002A"), "'\\u002A'");
    R_TEST(u32string(U"\U0000002A"), "'\\U0000002A'");
    R_TEST(u32string(U"\U0010ffff"), "'\\udbff\\udfff'"); // surrogate
    R_TEST(u32string(), "'\\udbff\\uffff'", cxon::read_error::surrogate_invalid, 1); // invalid surrogate
    R_TEST(u32string(), "'\\z'", cxon::read_error::escape_invalid, 1);
    R_TEST(u32string(), "'", cxon::read_error::unexpected, 1);
    R_TEST(u32string(), "x", cxon::read_error::unexpected, 0);
    R_TEST(u32string(), "'''", cxon::read_error::ok, 2); // trailing '
    R_TEST(u32string({0x0442, 0x0435, 0x0441, 0x0442}), "'\xD1\x82\xD0\xB5\xD1\x81\xD1\x82'");
    W_TEST("'\xD1\x82\xD0\xB5\xD1\x81\xD1\x82'", u32string({0x0442, 0x0435, 0x0441, 0x0442}));
    R_TEST(u32string({0x6D4B, 0x8BD5}), "'\xE6\xB5\x8B\xE8\xAF\x95'");
    W_TEST("'\xE6\xB5\x8B\xE8\xAF\x95'", u32string({0x6D4B, 0x8BD5}));
    R_TEST(u32string({0x0001241D, 0x0001241C}), "'\xF0\x92\x90\x9D\xF0\x92\x90\x9C'");
    W_TEST("'\xF0\x92\x90\x9D\xF0\x92\x90\x9C'", u32string({0x0001241D, 0x0001241C}));
    R_TEST(u32string(), "a", cxon::read_error::unexpected, 0);
    R_TEST(u32string(), "'\\u001'", cxon::read_error::escape_invalid, 1);
TEST_END()


namespace key {
    template <typename X, bool Q> struct unquoted : X::traits {
        struct map : X::traits::map {
            static constexpr bool unquoted_keys = Q;
        };
    };
    struct less_cstr {
        bool operator ()(const char* k0, const char* k1) const { return (!k0 && k1) || (k0 && k1 && std::strcmp(k0, k1) < 0); }
    };
}

TEST_BEG(cxon::CXON<>) // cxon
    using namespace std;
    // std::tuple<int, double, std::string>
        R_TEST((tuple<int, double, string>{0, 0, "0"}), "{0, 0, \"0\"}");
        W_TEST("{0,0,\"0\"}", (tuple<int, double, string>{0, 0, "0"}));
        R_TEST((tuple<int, double, string>{}), "{0, 0}", cxon::read_error::unexpected, 5);
        R_TEST((tuple<int, double, string>{}), "{0, 0, \"\", 0}", cxon::read_error::unexpected, 9);
        R_TEST((tuple<int, double, string>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((tuple<int, double, string>{}), "}", cxon::read_error::unexpected, 0);
        R_TEST((tuple<int, double, string>{}), "{", cxon::read_error::integral_invalid, 1);
        R_TEST((tuple<int, double, string>{}), "{0, x}", cxon::read_error::floating_point_invalid, 4);
    // std::tuple<int, double>
        R_TEST((tuple<int, double>{}), "{0, 0, \"0\"}", cxon::read_error::unexpected, 5);
        R_TEST((tuple<int, double>{}), "{x}", cxon::read_error::integral_invalid, 1);
    // std::pair<int, std::string>
        R_TEST((pair<int, string>{0, "0"}), "{0, \"0\"}");
        W_TEST("{0,\"0\"}", (pair<int, string>{0, "0"}));
        R_TEST((pair<int, string>{}), "{x}", cxon::read_error::integral_invalid, 1);
        R_TEST((pair<int, string>{}), "{0}", cxon::read_error::unexpected, 2);
        R_TEST((pair<int, string>{}), "{0, \"\", 0}", cxon::read_error::unexpected, 6);
        R_TEST((pair<int, string>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((pair<int, string>{}), "}", cxon::read_error::unexpected, 0);
        R_TEST((pair<int, string>{}), "{", cxon::read_error::integral_invalid, 1);
    // std::valarray<int>
        R_TEST((valarray<int>{1, 2, 3}), "{1, 2, 3}");
        W_TEST("{1,2,3}", (valarray<int>{1, 2, 3}));
        R_TEST((valarray<int>{1, 2, 3, 4}), "{1, 2, 3, 4}");
        R_TEST((valarray<int>{1, 2, 3, 4, 5}), "{1, 2, 3, 4, 5}");
        R_TEST((valarray<int>{1, 2, 3, 4, 5, 6, 7, 8}), "{1, 2, 3, 4, 5, 6, 7, 8}");
        R_TEST((valarray<int>{1, 2, 3, 4, 5, 6, 7, 8, 9}), "{1, 2, 3, 4, 5, 6, 7, 8, 9}");
        R_TEST((valarray<int>()), "", cxon::read_error::unexpected, 0);
        R_TEST((valarray<int>()), "}", cxon::read_error::unexpected, 0);
        R_TEST((valarray<int>()), "{", cxon::read_error::integral_invalid, 1);
        R_TEST((valarray<int>()), "{x", cxon::read_error::integral_invalid, 1);
    // std::array<int, 0>
        R_TEST((array<int, 0>{}), "{}");
        W_TEST("{}", (array<int, 0>{}));
        R_TEST((array<int, 0>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((array<int, 0>{}), "}", cxon::read_error::unexpected, 0);
        R_TEST((array<int, 0>{}), "{", cxon::read_error::unexpected, 1);
    // std::array<int, 3>
        R_TEST((array<int, 3>{{1, 2, 3}}), "{1, 2, 3}");
        W_TEST("{1,2,3}", (array<int, 3>{1, 2, 3}));
        R_TEST((array<int, 4>{{1, 2, 3, 0}}), "{1, 2, 3}");
        R_TEST((array<int, 2>{}), "{1, 2, 3}", cxon::read_error::overflow, 0);
        R_TEST((array<int, 2>{}), "{x}", cxon::read_error::integral_invalid, 1);
        R_TEST((array<int, 2>{}), "{1, x}", cxon::read_error::integral_invalid, 4);
    // std::queue<int>
        R_TEST((queue<int>{}), "{}");
        W_TEST("{}", (queue<int>{}));
        R_TEST((queue<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{1,2,3}", (queue<int>({1, 2, 3})));
        R_TEST(queue<int>{}, "{1, x}", cxon::read_error::integral_invalid, 4);
    // std::priority_queue<int>
        R_TEST((priority_queue<int>{}), "{}");
        W_TEST("{}", (priority_queue<int>{}));
        R_TEST(priority_queue<int>(less<int>(), {1, 2, 3}), "{1, 2, 3}");
        W_TEST("{3,2,1}", priority_queue<int>(less<int>(), {1, 2, 3}));
        R_TEST(priority_queue<int>{}, "{1, x}", cxon::read_error::integral_invalid, 4);
    // std::stack<int>
        R_TEST((stack<int>{}), "{}");
        W_TEST("{}", (stack<int>{}));
        R_TEST((stack<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{3,2,1}", (stack<int>({1, 2, 3})));
    // std::deque<int>
        R_TEST((deque<int>{}), "{}");
        W_TEST("{}", (deque<int>{}));
        R_TEST((deque<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{1,2,3}", (deque<int>({1, 2, 3})));
    // std::list<int>
        R_TEST((list<int>{}), "{}");
        W_TEST("{}", (list<int>{}));
        R_TEST((list<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{1,2,3}", (list<int>({1, 2, 3})));
    // std::forward_list<int>
        R_TEST((forward_list<int>{}), "{}");
        W_TEST("{}", (forward_list<int>{}));
        R_TEST((forward_list<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{1,2,3}", (forward_list<int>({1, 2, 3})));
    // std::vector<int>
        R_TEST((vector<int>{}), "{}");
        W_TEST("{}", (vector<int>{}));
        R_TEST((vector<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{1,2,3}", (vector<int>({1, 2, 3})));
        R_TEST((vector<int>()), "", cxon::read_error::unexpected, 0);
        R_TEST((vector<int>()), "}", cxon::read_error::unexpected, 0);
        R_TEST((vector<int>()), "{", cxon::read_error::integral_invalid, 1);
        R_TEST((vector<float>()), "", cxon::read_error::unexpected, 0);
        R_TEST((vector<float>()), "}", cxon::read_error::unexpected, 0);
        R_TEST((vector<float>()), "{", cxon::read_error::floating_point_invalid, 1);
    // std::set<int>;
        R_TEST((set<int>{}), "{}");
        W_TEST("{}", (set<int>{}));
        R_TEST((set<int>({1, 2, 3})), "{1, 1, 2, 3}");
        W_TEST("{1,2,3}", (set<int>({1, 2, 3})));
        R_TEST(set<int>{}, "{1, x}", cxon::read_error::integral_invalid, 4);
    // std::multiset<int>;
        R_TEST((multiset<int>{}), "{}");
        W_TEST("{}", (multiset<int>{}));
        R_TEST((multiset<int>({1, 1, 2, 3})), "{1, 1, 2, 3}");
        W_TEST("{1,1,2,3}", (multiset<int>({1, 1, 2, 3})));
    // std::unordered_set<int>;
        R_TEST((unordered_set<int>{}), "{}");
        W_TEST("{}", (unordered_set<int>{}));
        R_TEST((unordered_set<int>({1, 2, 3})), "{1, 1, 2, 3}");
        W_TEST("{1}", (unordered_set<int>({1})));
    // std::unordered_multiset<int>;
        R_TEST((unordered_multiset<int>{}), "{}");
        W_TEST("{}", (unordered_multiset<int>{}));
        R_TEST((unordered_multiset<int>({1, 1, 2, 3})), "{1, 1, 2, 3}");
        W_TEST("{1,1}", (unordered_multiset<int>({1, 1})));
    // std::vector<std::list<int>>
        R_TEST((vector<list<int>>{}), "{}");
        W_TEST("{}", (vector<list<int>>{}));
        R_TEST((vector<list<int>>{{}}), "{{}}");
        W_TEST("{{}}", (vector<list<int>>{{}}));
        R_TEST((vector<list<int>>{{1, 2, 3}, {3, 2, 1}}), "{{1, 2, 3}, {3, 2, 1}}");
        W_TEST("{{1,2,3},{3,2,1}}", (vector<list<int>>{{1, 2, 3}, {3, 2, 1}}));
    // std::list<std::vector<int>>
        R_TEST((list<vector<int>>{}), "{}");
        W_TEST("{}", (list<vector<int>>{}));
        R_TEST((list<vector<int>>{{}}), "{{}}");
        W_TEST("{{}}", (list<vector<int>>{{}}));
        R_TEST((list<vector<int>>{{1, 2, 3}, {3, 2, 1}}), "{{1, 2, 3}, {3, 2, 1}}");
        W_TEST("{{1,2,3},{3,2,1}}", (list<vector<int>>{{1, 2, 3}, {3, 2, 1}}));
TEST_END()


TEST_BEG(cxon::CXON<key::unquoted<cxon::CXON<>, false>>)
    using namespace std;
    // std::map<int, std::string>
        R_TEST((map<int, string>{}), "{}");
        W_TEST("{}", (map<int, string>{}));
        R_TEST((map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}), "{1 : \"1\" , 1 : \"1\" , 2 : \"2\" , 3 : \"3\"}");
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}));
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, const char*>{{1, "1"}, {2, "2"}, {3, "3"}}));
        R_TEST((map<int, string>{}), "{x}", cxon::read_error::integral_invalid, 1);
        R_TEST((map<int, string>{}), "{1: x}", cxon::read_error::unexpected, 4);
    // std::map<std::string, int>
        R_TEST((map<string, int>{}), "{}");
        W_TEST("{}", (map<string, int>{}));
        R_TEST((map<string, int>{{"", 1}}), "{\"\":1}");
        W_TEST("{\"\":1}", (map<string, int>{{"", 1}}));
        R_TEST((map<string, int>{{"1", 1}, {"2", 2}, {" 3 ", 3}}), "{\"1\": 1, \"1\": 1, \"2\": 2, \" 3 \": 3}");
        W_TEST("{\"1\":1,\"2\":2,\"3\":3}", (map<string, int>{{"1", 1}, {"2", 2}, {"3", 3}}));
        W_TEST("{\"1\":1,\"2\":2,\"3\":3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<int, int>
        R_TEST((map<int, int>{}), "{}");
        W_TEST("{}", (map<int, int>{}));
        R_TEST((map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,2:2,3:3}", (map<int, int>{{1, 1}, {2, 2}, {3, 3}}));
    // std::multimap<int, int>
        R_TEST((multimap<int, int>{}), "{}");
        W_TEST("{}", (multimap<int, int>{}));
        R_TEST((multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,1:1,2:2,3:3}", (multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}));
    // std::unordered_map<int, int>
        R_TEST((unordered_map<int, int>{}), "{}");
        W_TEST("{}", (unordered_map<int, int>{}));
        R_TEST((unordered_map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1}", (unordered_map<int, int>{{1, 1}, {1, 1}}));
    // std::unordered_multimap<int, int>
        R_TEST((unordered_multimap<int, int>{}), "{}");
        W_TEST("{}", (unordered_multimap<int, int>{}));
        R_TEST((unordered_multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,1:1}", (unordered_multimap<int, int>{{1, 1}, {1, 1}}));
TEST_END()

TEST_BEG(cxon::CXON<key::unquoted<cxon::CXON<>, true>>)
    using namespace std;
    // std::map<int, std::string>
        R_TEST((map<int, string>{}), "{}");
        W_TEST("{}", (map<int, string>{}));
        R_TEST((map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}), "{1 : \"1\" , 1 : \"1\" , 2 : \"2\" , 3 : \"3\"}");
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}));
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, const char*>{{1, "1"}, {2, "2"}, {3, "3"}}));
        R_TEST((map<int, string>{}), "{x}", cxon::read_error::integral_invalid, 1);
        R_TEST((map<int, string>{}), "{1: x}", cxon::read_error::unexpected, 4);
    // std::map<std::string, int>
        R_TEST((map<string, int>{}), "{}");
        W_TEST("{}", (map<string, int>{}));
        R_TEST((map<string, int>{{"", 1}}), "{:1}");
        R_TEST((map<string, int>{{"", 1}}), "{\"\":1}");
        W_TEST("{:1}", (map<string, int>{{"", 1}}));
        R_TEST((map<string, int>{{"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<string, int>{{"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<string, int>{{"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<string, int>{{"1", 1}, {"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<string, int>{{"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<string, int>{{"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<string, int>{{"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<string, int>{{"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<string, int>{{"1", 1}, {"2", 2}, {"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
        W_TEST("{null:0,1:1,2:2}", (map<const char*, int, key::less_cstr>{{nullptr, 0}, {"1", 1}, {"2", 2}}));
    // std::map<std::u16string, int>
        R_TEST((map<u16string, int>{}), "{}");
        W_TEST("{}", (map<u16string, int>{}));
        R_TEST((map<u16string, int>{{u"", 1}}), "{:1}");
        R_TEST((map<u16string, int>{{u"", 1}}), "{\"\":1}");
        W_TEST("{:1}", (map<u16string, int>{{u"", 1}}));
        R_TEST((map<u16string, int>{{u"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<u16string, int>{{u"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<u16string, int>{{u"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<u16string, int>{{u"1", 1}, {u"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<u16string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<u16string, int>{{u"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<u16string, int>{{u"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<u16string, int>{{u"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<u16string, int>{{u"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<u16string, int>{{u"1", 1}, {u"2", 2}, {u"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<std::u32string, int>
        R_TEST((map<u32string, int>{}), "{}");
        W_TEST("{}", (map<u32string, int>{}));
        R_TEST((map<u32string, int>{{U"", 1}}), "{:1}");
        R_TEST((map<u32string, int>{{U"", 1}}), "{\"\":1}");
        W_TEST("{:1}", (map<u32string, int>{{U"", 1}}));
        R_TEST((map<u32string, int>{{U"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<u32string, int>{{U"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<u32string, int>{{U"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<u32string, int>{{U"1", 1}, {U"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<u32string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<u32string, int>{{U"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<u32string, int>{{U"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<u32string, int>{{U"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<u32string, int>{{U"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<u32string, int>{{U"1", 1}, {U"2", 2}, {U"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<std::wstring, int>
        R_TEST((map<wstring, int>{}), "{}");
        W_TEST("{}", (map<wstring, int>{}));
        R_TEST((map<wstring, int>{{L"", 1}}), "{:1}");
        R_TEST((map<wstring, int>{{L"", 1}}), "{\"\":1}");
        W_TEST("{:1}", (map<wstring, int>{{L"", 1}}));
        R_TEST((map<wstring, int>{{L"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<wstring, int>{{L"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<wstring, int>{{L"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<wstring, int>{{L"1", 1}, {L"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<wstring, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<wstring, int>{{L"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<wstring, int>{{L"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<wstring, int>{{L"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<wstring, int>{{L"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<wstring, int>{{L"1", 1}, {L"2", 2}, {L"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<int, int>
        R_TEST((map<int, int>{}), "{}");
        W_TEST("{}", (map<int, int>{}));
        R_TEST((map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,2:2,3:3}", (map<int, int>{{1, 1}, {2, 2}, {3, 3}}));
TEST_END()


TEST_BEG(cxon::JSON<key::unquoted<cxon::JSON<>, false>>)
    using namespace std;
    // std::map<int, std::string>
        R_TEST((map<int, string>{}), "{}");
        W_TEST("{}", (map<int, string>{}));
        R_TEST((map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}), "{\"1\" : \"1\" , \"1\" : \"1\" , \"2\" : \"2\" , \"3\" : \"3\"}");
        W_TEST("{\"1\":\"1\",\"2\":\"2\",\"3\":\"3\"}", (map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}));
        W_TEST("{\"1\":\"1\",\"2\":\"2\",\"3\":\"3\"}", (map<int, const char*>{{1, "1"}, {2, "2"}, {3, "3"}}));
        R_TEST((map<int, string>{}), "{\"x\"}", cxon::read_error::integral_invalid, 2);
        R_TEST((map<int, string>{}), "{\"1\": x}", cxon::read_error::unexpected, 6);
    // std::map<std::string, int>
        R_TEST((map<string, int>{}), "{}");
        W_TEST("{}", (map<string, int>{}));
        R_TEST((map<string, int>{{"", 1}}), "{\"\":1}");
        W_TEST("{\"\":1}", (map<string, int>{{"", 1}}));
        R_TEST((map<string, int>{{"1", 1}, {"2", 2}, {" 3 ", 3}}), "{\"1\": 1, \"1\": 1, \"2\": 2, \" 3 \": 3}");
        W_TEST("{\"1\":1,\"2\":2,\"3\":3}", (map<string, int>{{"1", 1}, {"2", 2}, {"3", 3}}));
        W_TEST("{\"1\":1,\"2\":2,\"3\":3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<int, int>
        R_TEST((map<int, int>{}), "{}");
        W_TEST("{}", (map<int, int>{}));
        R_TEST((map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{\"1\" : 1 , \"1\" : 1 , \"2\" : 2 , \"3\" : 3}");
        W_TEST("{\"1\":1,\"2\":2,\"3\":3}", (map<int, int>{{1, 1}, {2, 2}, {3, 3}}));
    // std::multimap<int, int>
        R_TEST((multimap<int, int>{}), "{}");
        W_TEST("{}", (multimap<int, int>{}));
        R_TEST((multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{\"1\" : 1 , \"1\" : 1 , \"2\" : 2 , \"3\" : 3}");
        W_TEST("{\"1\":1,\"1\":1,\"2\":2,\"3\":3}", (multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}));
    // std::unordered_map<int, int>
        R_TEST((unordered_map<int, int>{}), "{}");
        W_TEST("{}", (unordered_map<int, int>{}));
        R_TEST((unordered_map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{\"1\" : 1 , \"1\" : 1 , \"2\" : 2 , \"3\" : 3}");
        W_TEST("{\"1\":1}", (unordered_map<int, int>{{1, 1}, {1, 1}}));
    // std::unordered_multimap<int, int>
        R_TEST((unordered_multimap<int, int>{}), "{}");
        W_TEST("{}", (unordered_multimap<int, int>{}));
        R_TEST((unordered_multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{\"1\" : 1 , \"1\" : 1 , \"2\" : 2 , \"3\" : 3}");
        W_TEST("{\"1\":1,\"1\":1}", (unordered_multimap<int, int>{{1, 1}, {1, 1}}));
TEST_END()

TEST_BEG(cxon::JSON<key::unquoted<cxon::JSON<>, true>>)
    using namespace std;
    // std::map<int, std::string>
        R_TEST((map<int, string>{}), "{}");
        W_TEST("{}", (map<int, string>{}));
        R_TEST((map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}), "{1 : \"1\" , 1 : \"1\" , 2 : \"2\" , 3 : \"3\"}");
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}));
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, const char*>{{1, "1"}, {2, "2"}, {3, "3"}}));
        R_TEST((map<int, string>{}), "{x}", cxon::read_error::integral_invalid, 1);
        R_TEST((map<int, string>{}), "{1: x}", cxon::read_error::unexpected, 4);
    // std::map<std::string, int>
        R_TEST((map<string, int>{}), "{}");
        W_TEST("{}", (map<string, int>{}));
        R_TEST((map<string, int>{{"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<string, int>{{"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<string, int>{{"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<string, int>{{"1", 1}, {"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<string, int>{{"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<string, int>{{"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<string, int>{{"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<string, int>{{"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<string, int>{{"1", 1}, {"2", 2}, {"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<std::u16string, int>
        R_TEST((map<u16string, int>{}), "{}");
        W_TEST("{}", (map<u16string, int>{}));
        R_TEST((map<u16string, int>{{u"", 1}}), "{:1}");
        R_TEST((map<u16string, int>{{u"", 1}}), "{\"\":1}");
        W_TEST("{:1}", (map<u16string, int>{{u"", 1}}));
        R_TEST((map<u16string, int>{{u"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<u16string, int>{{u"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<u16string, int>{{u"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<u16string, int>{{u"1", 1}, {u"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<u16string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<u16string, int>{{u"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<u16string, int>{{u"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<u16string, int>{{u"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<u16string, int>{{u"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<u16string, int>{{u"1", 1}, {u"2", 2}, {u"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<std::u32string, int>
        R_TEST((map<u32string, int>{}), "{}");
        W_TEST("{}", (map<u32string, int>{}));
        R_TEST((map<u32string, int>{{U"", 1}}), "{:1}");
        R_TEST((map<u32string, int>{{U"", 1}}), "{\"\":1}");
        W_TEST("{:1}", (map<u32string, int>{{U"", 1}}));
        R_TEST((map<u32string, int>{{U"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<u32string, int>{{U"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<u32string, int>{{U"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<u32string, int>{{U"1", 1}, {U"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<u32string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<u32string, int>{{U"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<u32string, int>{{U"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<u32string, int>{{U"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<u32string, int>{{U"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<u32string, int>{{U"1", 1}, {U"2", 2}, {U"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<std::wstring, int>
        R_TEST((map<wstring, int>{}), "{}");
        W_TEST("{}", (map<wstring, int>{}));
        R_TEST((map<wstring, int>{{L"", 1}}), "{:1}");
        R_TEST((map<wstring, int>{{L"", 1}}), "{\"\":1}");
        W_TEST("{:1}", (map<wstring, int>{{L"", 1}}));
        R_TEST((map<wstring, int>{{L"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<wstring, int>{{L"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<wstring, int>{{L"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<wstring, int>{{L"1", 1}, {L"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<wstring, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<wstring, int>{{L"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<wstring, int>{{L"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<wstring, int>{{L"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<wstring, int>{{L"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<wstring, int>{{L"1", 1}, {L"2", 2}, {L"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<int, int>
        R_TEST((map<int, int>{}), "{}");
        W_TEST("{}", (map<int, int>{}));
        R_TEST((map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,2:2,3:3}", (map<int, int>{{1, 1}, {2, 2}, {3, 3}}));
    // std::multimap<int, int>
        R_TEST((multimap<int, int>{}), "{}");
        W_TEST("{}", (multimap<int, int>{}));
        R_TEST((multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,1:1,2:2,3:3}", (multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}));
    // std::unordered_map<int, int>
        R_TEST((unordered_map<int, int>{}), "{}");
        W_TEST("{}", (unordered_map<int, int>{}));
        R_TEST((unordered_map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1}", (unordered_map<int, int>{{1, 1}}));
    // std::unordered_multimap<int, int>
        R_TEST((unordered_multimap<int, int>{}), "{}");
        W_TEST("{}", (unordered_multimap<int, int>{}));
        R_TEST((unordered_multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,1:1}", (unordered_multimap<int, int>{{1, 1}, {1, 1}}));
TEST_END()

TEST_BEG(cxon::JSON<>)
    using namespace std;
    // std::tuple<int, double, std::string>
        R_TEST((tuple<int, double, string>{0, 0, "0"}), "[0, 0, \"0\"]");
        W_TEST("[0,0,\"0\"]", (tuple<int, double, string>{0, 0, "0"}));
        R_TEST((tuple<int, double, string>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((tuple<int, double, string>{}), "]", cxon::read_error::unexpected, 0);
        R_TEST((tuple<int, double, string>{}), "[", cxon::read_error::integral_invalid, 1);
        R_TEST((tuple<int, double, string>{}), "[0, 0]", cxon::read_error::unexpected, 5);
        R_TEST((tuple<int, double, string>{}), "[0, 0, \"\", 0]", cxon::read_error::unexpected, 9);
    // std::tuple<int, double>
        R_TEST((tuple<int, double>{}), "[0, 0, \"0\"]", cxon::read_error::unexpected, 5);
        W_TEST("[0,0]", (tuple<int, double>{0, 0}));
    // std::pair<int, std::string>
        R_TEST((pair<int, string>{0, "0"}), "[0, \"0\"]");
        W_TEST("[0,\"0\"]", (pair<int, string>{0, "0"}));
        R_TEST((pair<int, string>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((pair<int, string>{}), "]", cxon::read_error::unexpected, 0);
        R_TEST((pair<int, string>{}), "[", cxon::read_error::integral_invalid, 1);
        R_TEST((pair<int, string>{}), "[x]", cxon::read_error::integral_invalid, 1);
        R_TEST((pair<int, string>{}), "[0]", cxon::read_error::unexpected, 2);
        R_TEST((pair<int, string>{}), "[0, \"\", 0]", cxon::read_error::unexpected, 6);
    // std::valarray<int>
        R_TEST((valarray<int>{1, 2, 3}), "[1, 2, 3]");
        W_TEST("[1,2,3]", (valarray<int>{1, 2, 3}));
        R_TEST((valarray<int>{1, 2, 3, 4}), "[1, 2, 3, 4]");
        R_TEST((valarray<int>{1, 2, 3, 4, 5}), "[1, 2, 3, 4, 5]");
        R_TEST((valarray<int>{1, 2, 3, 4, 5, 6, 7, 8}), "[1, 2, 3, 4, 5, 6, 7, 8]");
        R_TEST((valarray<int>{1, 2, 3, 4, 5, 6, 7, 8, 9}), "[1, 2, 3, 4, 5, 6, 7, 8, 9]");
        R_TEST((valarray<int>()), "", cxon::read_error::unexpected, 0);
        R_TEST((valarray<int>()), "]", cxon::read_error::unexpected, 0);
        R_TEST((valarray<int>()), "[", cxon::read_error::integral_invalid, 1);
        R_TEST((valarray<int>()), "[x", cxon::read_error::integral_invalid, 1);
    // std::array<int, 0>
        R_TEST((array<int, 0>{}), "[]");
        W_TEST("[]", (array<int, 0>{}));
        R_TEST((array<int, 0>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((array<int, 0>{}), "]", cxon::read_error::unexpected, 0);
        R_TEST((array<int, 0>{}), "[", cxon::read_error::unexpected, 1);
    // std::array<int, 3>
        R_TEST((array<int, 3>{{1, 2, 3}}), "[1, 2, 3]");
        W_TEST("[1,2,3]", (array<int, 3>{{1, 2, 3}}));
        R_TEST((array<int, 4>{{1, 2, 3, 0}}), "[1, 2, 3]");
        W_TEST("[1,2,3,4]", (array<int, 4>{{1, 2, 3, 4}}));
        R_TEST((array<int, 2>{}), "[1, 2, 3]", cxon::read_error::overflow, 0);
        W_TEST("[1,2]", (array<int, 2>{{1, 2}}));
    // std::queue<int>
        R_TEST((queue<int>{}), "[]");
        W_TEST("[]", (queue<int>{}));
        R_TEST((queue<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (queue<int>({1, 2, 3})));
    // std::priority_queue<int>
        R_TEST((priority_queue<int>{}), "[]");
        W_TEST("[]", (priority_queue<int>{}));
        R_TEST(priority_queue<int>(less<int>(), {1, 2, 3}), "[1, 2, 3]");
        W_TEST("[3,2,1]", priority_queue<int>(less<int>(), {1, 2, 3}));
    // std::stack<int>
        R_TEST((stack<int>{}), "[]");
        W_TEST("[]", (stack<int>{}));
        R_TEST((stack<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[3,2,1]", (stack<int>({1, 2, 3})));
    // std::deque<int>
        R_TEST((deque<int>{}), "[]");
        W_TEST("[]", (deque<int>{}));
        R_TEST((deque<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (deque<int>({1, 2, 3})));
    // std::list<int>
        R_TEST((list<int>{}), "[]");
        W_TEST("[]", (list<int>{}));
        R_TEST((list<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (list<int>({1, 2, 3})));
        R_TEST((list<int>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((list<int>{}), "]", cxon::read_error::unexpected, 0);
        R_TEST((list<int>{}), "{", cxon::read_error::unexpected, 0);
        R_TEST((list<int>{}), "[", cxon::read_error::integral_invalid, 1);
        W_TEST("[1,2,3]", (list<float>({1, 2, 3})));
        R_TEST((list<float>{}), "[", cxon::read_error::floating_point_invalid, 1);
    // std::list<int>
        R_TEST((forward_list<int>{}), "[]");
        W_TEST("[]", (forward_list<int>{}));
        R_TEST((forward_list<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (forward_list<int>({1, 2, 3})));
        R_TEST((forward_list<int>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((forward_list<int>{}), "]", cxon::read_error::unexpected, 0);
        R_TEST((forward_list<int>{}), "{", cxon::read_error::unexpected, 0);
        R_TEST((forward_list<int>{}), "[", cxon::read_error::integral_invalid, 1);
        W_TEST("[1,2,3]", (forward_list<float>({1, 2, 3})));
        R_TEST((forward_list<float>{}), "[", cxon::read_error::floating_point_invalid, 1);
    // std::vector<int>
        R_TEST((vector<int>{}), "[]");
        W_TEST("[]", (vector<int>{}));
        R_TEST((vector<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (vector<int>({1, 2, 3})));
    // std::set<int>
        R_TEST((set<int>{}), "[]");
        W_TEST("[]", (set<int>{}));
        R_TEST((set<int>({1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1,2,3]", (set<int>({1, 2, 3})));
    // std::multiset<int>;
        R_TEST((multiset<int>{}), "[]");
        W_TEST("[]", (multiset<int>{}));
        R_TEST((multiset<int>({1, 1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1,1,2,3]", (multiset<int>({1, 1, 2, 3})));
    // std::unordered_set<int>
        R_TEST((unordered_set<int>{}), "[]");
        W_TEST("[]", (unordered_set<int>{}));
        R_TEST((unordered_set<int>({1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1]", (unordered_set<int>({1})));
    // std::unordered_multiset<int>;
        R_TEST((unordered_multiset<int>{}), "[]");
        W_TEST("[]", (unordered_multiset<int>{}));
        R_TEST((unordered_multiset<int>({1, 1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1,1]", (unordered_multiset<int>({1, 1})));
    // std::vector<std::list<int>>
        R_TEST((vector<list<int>>{}), "[]");
        W_TEST("[]", (vector<list<int>>{}));
        R_TEST((vector<list<int>>{{}}), "[[]]");
        W_TEST("[[]]", (vector<list<int>>{{}}));
        R_TEST((vector<list<int>>{{1, 2, 3}, {3, 2, 1}}), "[[1, 2, 3], [3, 2, 1]]");
        W_TEST("[[1,2,3],[3,2,1]]", (vector<list<int>>{{1, 2, 3}, {3, 2, 1}}));
    // std::list<std::vector<int>>
        R_TEST((list<vector<int>>{}), "[]");
        W_TEST("[]", (list<vector<int>>{}));
        R_TEST((list<vector<int>>{{}}), "[[]]");
        W_TEST("[[]]", (list<vector<int>>{{}}));
        R_TEST((list<vector<int>>{{1, 2, 3}, {3, 2, 1}}), "[[1, 2, 3], [3, 2, 1]]");
        W_TEST("[[1,2,3],[3,2,1]]", (list<vector<int>>{{1, 2, 3}, {3, 2, 1}}));
TEST_END()


template <size_t N>
    struct less {
        constexpr bool operator()(const std::bitset<N>& l, const std::bitset<N>& r) const {
            return l.to_string() < r.to_string();
        }
    };

TEST_BEG(cxon::CXON<>) // std::bitset
    using namespace std;
    R_TEST(bitset<8>(85), "01010101");
    R_TEST(bitset<8>(255), "11111111");
    R_TEST(bitset<8>(255), "x1111111", cxon::read_error::unexpected, 0);
    W_TEST("01010101", bitset<8>(85));
    W_TEST("11111111", bitset<8>(255));
    R_TEST(map<bitset<8>, int, ::less<8>>{{bitset<8>(85), 1}}, "{01010101:1}");
    W_TEST("{01010101:1}", map<bitset<8>, int, ::less<8>>{{bitset<8>(85), 1}});
TEST_END()

TEST_BEG(cxon::JSON<>) // std::bitset
    using namespace std;
    R_TEST(bitset<8>(85), QS("01010101"));
    R_TEST(bitset<8>(255), QS("11111111"));
    R_TEST(bitset<8>(255), QS("x1111111"), cxon::read_error::unexpected, 1);
    W_TEST(QS("01010101"), bitset<8>(85));
    W_TEST(QS("11111111"), bitset<8>(255));
    R_TEST(map<bitset<8>, int, ::less<8>>{{bitset<8>(85), 1}}, "{\"01010101\":1}");
    W_TEST("{\"01010101\":1}", map<bitset<8>, int, ::less<8>>{{bitset<8>(85), 1}});
TEST_END()

TEST_BEG(cxon::JSON<key::unquoted<cxon::JSON<>, true>>)
    using namespace std;
    R_TEST(map<bitset<8>, int, ::less<8>>{{bitset<8>(85), 1}}, "{01010101:1}");
    W_TEST("{01010101:1}", map<bitset<8>, int, ::less<8>>{{bitset<8>(85), 1}});
TEST_END()


TEST_BEG(cxon::CXON<>) // std::complex
    using namespace std;
    R_TEST(complex<float>(), "{0, 0}");
    R_TEST(complex<float>(), "{}", cxon::read_error::floating_point_invalid, 1);
    R_TEST(complex<float>(), "{0}", cxon::read_error::unexpected, 2);
    W_TEST("{0,0}", complex<float>());
    R_TEST(complex<double>(), "{0, 0}");
    R_TEST(complex<double>(), "{}", cxon::read_error::floating_point_invalid, 1);
    R_TEST(complex<double>(), "{0}", cxon::read_error::unexpected, 2);
    W_TEST("{0,0}", complex<double>());
    R_TEST(complex<long double>(), "{0, 0}");
    R_TEST(complex<long double>(), "{}", cxon::read_error::floating_point_invalid, 1);
    R_TEST(complex<long double>(), "{0}", cxon::read_error::unexpected, 2);
    W_TEST("{0,0}", complex<long double>());
TEST_END()


TEST_BEG(cxon::CXON<>) // std::chrono
    using namespace std::chrono;
    R_TEST(duration<unsigned>(42), "42");
    R_TEST(duration<unsigned>(42), "x", cxon::read_error::integral_invalid, 0);
    W_TEST("42", duration<unsigned>(42));
    R_TEST(time_point<system_clock>(system_clock::duration(42)), "42");
    R_TEST(time_point<system_clock>(system_clock::duration(42)), "x", cxon::read_error::integral_invalid, 0);
    W_TEST("42", time_point<system_clock>(system_clock::duration(42)));
TEST_END()


#ifdef CXON_HAS_STRING_VIEW
    TEST_BEG(cxon::CXON<>) // std::string_view
        using namespace std;
        char const s[] = "1";
        //R_TEST(string_view(s), QS("1")); // std::string_view cannot be modified
        W_TEST(QS("1"), string_view(s));
        W_TEST("{1:1}", map<string_view, int>{{string_view(s), 1}});
    TEST_END()
#endif

#ifdef CXON_HAS_STRING_VIEW
    TEST_BEG(cxon::JSON<key::unquoted<cxon::JSON<>, true>>) // std::string_view
        using namespace std;
        char const s[] = "1";
        W_TEST(QS("1"), string_view(s));
        W_TEST("{1:1}", map<string_view, int>{{string_view(s), 1}});
    TEST_END()
#endif


#ifdef CXON_HAS_OPTIONAL
    TEST_BEG(cxon::CXON<>) // std::optional
        using namespace std;
        R_TEST(optional<int>(42), "42");
        W_TEST("42", optional<int>(42));
        R_TEST(optional<int>(), "null");
        W_TEST("null", optional<int>());
    TEST_END()
#endif


#ifdef CXON_HAS_VARIANT
    TEST_BEG(cxon::CXON<>) // std::variant
        using namespace std;
        R_TEST(variant<int, double>(in_place_index_t<0>(), 1), "{0:1}");
        R_TEST(variant<int, double>(in_place_index_t<1>(), 0), "{1:0}");
        R_TEST(variant<int, double>(in_place_index_t<1>(), 0), "{2:0}", cxon::read_error::unexpected, 1);
        W_TEST("{0:1}", variant<int, double>(1));
        W_TEST("{1:0}", variant<int, double>(in_place_index_t<1>(), 0));
        R_TEST(variant<monostate, int>(), "{0:null}");
        R_TEST(variant<monostate, int>(), "{0:1}", cxon::read_error::unexpected, 3);
        W_TEST("{0:null}", variant<monostate, int>());
    TEST_END()
#endif
