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

#include "test.hxx"

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
#include "cxon/lib/std/flat_set.hxx"
#include "cxon/lib/std/unordered_set.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/flat_map.hxx"
#include "cxon/lib/std/unordered_map.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/variant.hxx"
#include "cxon/lib/std/bitset.hxx"
#include "cxon/lib/std/chrono.hxx"


////////////////////////////////////////////////////////////////////////////////

TEST_BEG(basic_string, cxon::JSON<>, "/std")
    using namespace std;
    // std::basic_string<char> / std::string
        R_TEST(string("test"), QS("test"));
        W_TEST(QS("test"), string("test"));
        R_TEST(string("te\nst"), QS("te\\nst"));
        W_TEST(QS("te\\nst"), string("te\nst"));
        R_TEST(string(), "a", json::read_error::unexpected, 0);
        R_TEST(string(), "\"", json::read_error::unexpected, 1);
        R_TEST(string(), QS("\\u001"), json::read_error::escape_invalid, 1);
        // errors
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<X>(c, string("s\n"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, string("s\n"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, string("s\n"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[3];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, string("s\n"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[4];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, string("s\n"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
    // std::basic_string<wchar_t> / std::wstring
        R_TEST(wstring({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), wstring({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(wstring({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), wstring({0x6D4B, 0x8BD5}));
        R_TEST(wstring(), "a", json::read_error::unexpected, 0);
        R_TEST(wstring(), "\"", json::read_error::unexpected, 1);
        R_TEST(wstring(), QS("\\u001"), json::read_error::escape_invalid, 1);
        // errors
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<X>(c, wstring(L"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, wstring(L"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, wstring(L"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
    // std::basic_string<char8_t> / std::u8string
#       if defined(__cpp_char8_t)
            R_TEST(u8string(u8"test"), QS("test"));
            W_TEST(QS("test"), u8string(u8"test"));
            R_TEST(u8string(u8"te\nst"), QS("te\\nst"));
            W_TEST(QS("te\\nst"), u8string(u8"te\nst"));
            R_TEST(u8string(), "a", json::read_error::unexpected, 0);
            R_TEST(u8string(), "\"", json::read_error::unexpected, 1);
            R_TEST(u8string(), QS("\\u001"), json::read_error::escape_invalid, 1);
            // errors
                {   char b[1];
                    auto c = cxon::cnt::make_range_container(begin(b), end(b));
                        c.push_back('x');
                        auto r = cxon::to_bytes<X>(c, u8string(u8"s"));
                    TEST_CHECK(r.ec == json::write_error::output_failure);
                }
                {   char b[1];
                    auto c = cxon::cnt::make_range_container(begin(b), end(b));
                        auto r = cxon::to_bytes<X>(c, u8string(u8"s"));
                    TEST_CHECK(r.ec == json::write_error::output_failure);
                }
                {   char b[2];
                    auto c = cxon::cnt::make_range_container(begin(b), end(b));
                        auto r = cxon::to_bytes<X>(c, u8string(u8"s"));
                    TEST_CHECK(r.ec == json::write_error::output_failure);
                }
#       endif
    // std::basic_string<char16_t> / std::u16string
        R_TEST(u16string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), u16string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(u16string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), u16string({0x6D4B, 0x8BD5}));
        R_TEST(u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}));
        R_TEST(u16string(u"\xDBFF\xDFFF"), QS("\\uDBFF\\uDFFF")); // surrogate
        W_TEST(QS("\xF4\x8F\xBF\xBF"), u16string(u"\xDBFF\xDFFF")); // surrogate
        R_TEST(u16string(), "a", json::read_error::unexpected, 0);
        R_TEST(u16string(), "\"", json::read_error::unexpected, 1);
        R_TEST(u16string(), QS("\\u001"), json::read_error::escape_invalid, 1);
        // errors
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<X>(c, u16string(u"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, u16string(u"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, u16string(u"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[3];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, u16string(u"s"));
                TEST_CHECK(r);
            }
    // std::basic_string<char32_t> / std::u32string
        R_TEST(u32string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), u32string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(u32string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), u32string({0x6D4B, 0x8BD5}));
        R_TEST(u32string({0x0001241D, 0x0001241C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), u32string({0x0001241D, 0x0001241C}));
        R_TEST(u32string(), "a", json::read_error::unexpected, 0);
        R_TEST(u32string(), "\"", json::read_error::unexpected, 1);
        R_TEST(u32string(), QS("\\u001"), json::read_error::escape_invalid, 1);
        // errors
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<X>(c, u32string(U"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, u32string(U"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, u32string(U"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
    // errors
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<X>(c, string("s\n"));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, string("s\n"));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, string("s\n"));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, string("s\n"));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, string("s\n"));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
TEST_END()


TEST_BEG(tuple, cxon::JSON<>, "/std")
    using namespace std;
    // std::tuple<>
        R_TEST((tuple<>{}), "[]");
        R_TEST((tuple<>{}), "[ ]");
        W_TEST("[]", (tuple<>{}));
        R_TEST((tuple<>{}), "", json::read_error::unexpected, 0);
        R_TEST((tuple<>{}), " ", json::read_error::unexpected, 1);
        R_TEST((tuple<>{}), "[", json::read_error::unexpected, 1);
    // std::tuple<int, double, std::string>
        R_TEST((tuple<int, double, string>{0, 0, "0"}), R"([0, 0, "0"])");
        W_TEST(R"([0,0,"0"])", (tuple<int, double, string>{0, 0, "0"}));
        R_TEST((tuple<int, double, string>{}), "", json::read_error::unexpected, 0);
        R_TEST((tuple<int, double, string>{}), "]", json::read_error::unexpected, 0);
        R_TEST((tuple<int, double, string>{}), "[", json::read_error::integral_invalid, 1);
        R_TEST((tuple<int, double, string>{}), "[0, 0]", json::read_error::unexpected, 5);
        R_TEST((tuple<int, double, string>{}), R"([0, 0, "", 0])", json::read_error::unexpected, 9);
    // std::tuple<int, double>
        R_TEST((tuple<int, double>{}), R"([0, 0, "0"])", json::read_error::unexpected, 5);
        W_TEST("[0,0]", (tuple<int, double>{0, 0}));
    // write errors
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                c.push_back('x');
            auto r = cxon::to_bytes<X>(c, tuple<>{});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, tuple<>{});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                c.push_back('x');
            auto r = cxon::to_bytes<X>(c, tuple<int>{0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, tuple<int>{0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, tuple<int>{0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, tuple<int, long>{0, 0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, tuple<int, long>{0, 0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, tuple<int, long>{0, 0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
TEST_END()


TEST_BEG(pair, cxon::JSON<>, "/std")
    using namespace std;
    // std::pair<int, std::string>
        R_TEST((pair<int, int>{3, 1}), "[3, 1]");
        W_TEST("[3,1]", (pair<int, int>{3, 1}));
        R_TEST((pair<int, int>{}), "", json::read_error::unexpected, 0);
        R_TEST((pair<int, int>{}), "]", json::read_error::unexpected, 0);
        R_TEST((pair<int, int>{}), "[", json::read_error::integral_invalid, 1);
        R_TEST((pair<int, int>{}), "[3]", json::read_error::unexpected, 2);
        R_TEST((pair<int, int>{}), "[3,]", json::read_error::integral_invalid, 3);
        R_TEST((pair<int, int>{}), "[3,1,", json::read_error::unexpected, 4);
        R_TEST((pair<int, int>{}), "[3,1", json::read_error::unexpected, 4);
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<X>(c, make_pair(3, 1));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, make_pair(3, 1));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, make_pair(3, 1));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, make_pair(3, 1));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, make_pair(3, 1));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
TEST_END()


TEST_BEG(valarray, cxon::JSON<>, "/std")
    using namespace std;
    // std::valarray<int>
        R_TEST((valarray<int>{}), "[]");
        W_TEST("[]", (valarray<int>{}));
        R_TEST((valarray<int>{1, 2, 3}), "[1, 2, 3]");
        W_TEST("[1,2,3]", (valarray<int>{1, 2, 3}));
        R_TEST((valarray<int>{1, 2, 3, 4}), "[1, 2, 3, 4]");
        R_TEST((valarray<int>{1, 2, 3, 4, 5}), "[1, 2, 3, 4, 5]");
        R_TEST((valarray<int>{1, 2, 3, 4, 5, 6, 7, 8}), "[1, 2, 3, 4, 5, 6, 7, 8]");
        R_TEST((valarray<int>{1, 2, 3, 4, 5, 6, 7, 8, 9}), "[1, 2, 3, 4, 5, 6, 7, 8, 9]");
        R_TEST((valarray<int>()), "", json::read_error::unexpected, 0);
        R_TEST((valarray<int>()), "]", json::read_error::unexpected, 0);
        R_TEST((valarray<int>()), "[", json::read_error::integral_invalid, 1);
        R_TEST((valarray<int>()), "[x", json::read_error::integral_invalid, 1);
TEST_END()


TEST_BEG(array, cxon::JSON<>, "/std")
    using namespace std;
    // std::array<int, 0>
        R_TEST((array<int, 0>{}), "[]");
        W_TEST("[]", (array<int, 0>{}));
        R_TEST((array<int, 0>{}), "", json::read_error::unexpected, 0);
        R_TEST((array<int, 0>{}), "]", json::read_error::unexpected, 0);
        R_TEST((array<int, 0>{}), "[", json::read_error::unexpected, 1);
    // std::array<int, 3>
        R_TEST((array<int, 3>{{1, 2, 3}}), "[1, 2, 3]");
        W_TEST("[1,2,3]", (array<int, 3>{{1, 2, 3}}));
        R_TEST((array<int, 4>{{1, 2, 3, 0}}), "[1, 2, 3]");
        W_TEST("[1,2,3,4]", (array<int, 4>{{1, 2, 3, 4}}));
        R_TEST((array<int, 2>{}), "[1, 2, 3]", json::read_error::overflow, 0);
        W_TEST("[1,2]", (array<int, 2>{{1, 2}}));
        R_TEST((array<int, 2>{}), "", json::read_error::unexpected, 0);
        R_TEST((array<int, 2>{}), "[", json::read_error::integral_invalid, 1);
        R_TEST((array<int, 2>{}), "[1x", json::read_error::unexpected, 2);
        R_TEST((array<int, 2>{}), "[1,x", json::read_error::integral_invalid, 3);
        R_TEST((array<int, 2>{}), "[1,1", json::read_error::unexpected, 4);
TEST_END()


TEST_BEG(queue, cxon::JSON<>, "/std")
    using namespace std;
    // std::queue<int>
        R_TEST((queue<int>{}), "[]");
        W_TEST("[]", (queue<int>{}));
        R_TEST((queue<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (queue<int>({1, 2, 3})));
        R_TEST((queue<int>({1, 2, 3})), "[x]", json::read_error::integral_invalid, 1);
    // std::priority_queue<int>
        R_TEST((priority_queue<int>{}), "[]");
        W_TEST("[]", (priority_queue<int>{}));
        R_TEST(priority_queue<int>(less<int>(), {1, 2, 3}), "[1, 2, 3]");
        W_TEST("[3,2,1]", priority_queue<int>(less<int>(), {1, 2, 3}));
        R_TEST((priority_queue<int>(less<int>(), {3, 2, 1})), "[x]", json::read_error::integral_invalid, 1);
TEST_END()


TEST_BEG(stack, cxon::JSON<>, "/std")
    using namespace std;
    // std::stack<int>
        R_TEST((stack<int>{}), "[]");
        W_TEST("[]", (stack<int>{}));
        R_TEST((stack<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[3,2,1]", (stack<int>({1, 2, 3})));
TEST_END()


TEST_BEG(deque, cxon::JSON<>, "/std")
    using namespace std;
    // std::deque<int>
        R_TEST((deque<int>{}), "[]");
        W_TEST("[]", (deque<int>{}));
        R_TEST((deque<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (deque<int>({1, 2, 3})));
TEST_END()


TEST_BEG(list, cxon::JSON<>, "/std")
    using namespace std;
    // std::list<int>
        R_TEST((list<int>{}), "[]");
        W_TEST("[]", (list<int>{}));
        R_TEST((list<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (list<int>({1, 2, 3})));
        R_TEST((list<int>{}), "", json::read_error::unexpected, 0);
        R_TEST((list<int>{}), "]", json::read_error::unexpected, 0);
        R_TEST((list<int>{}), "{", json::read_error::unexpected, 0);
        R_TEST((list<int>{}), "[", json::read_error::integral_invalid, 1);
        W_TEST("[1,2,3]", (list<float>({1, 2, 3})));
        R_TEST((list<float>{}), "[", json::read_error::floating_point_invalid, 1);
TEST_END()


TEST_BEG(forward_list, cxon::JSON<>, "/std")
    using namespace std;
    // std::forward_list<int>
        R_TEST((forward_list<int>{}), "[]");
        W_TEST("[]", (forward_list<int>{}));
        R_TEST((forward_list<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (forward_list<int>({1, 2, 3})));
        R_TEST((forward_list<int>{}), "", json::read_error::unexpected, 0);
        R_TEST((forward_list<int>{}), "]", json::read_error::unexpected, 0);
        R_TEST((forward_list<int>{}), "{", json::read_error::unexpected, 0);
        R_TEST((forward_list<int>{}), "[", json::read_error::integral_invalid, 1);
        W_TEST("[1,2,3]", (forward_list<float>({1, 2, 3})));
        R_TEST((forward_list<float>{}), "[", json::read_error::floating_point_invalid, 1);
TEST_END()


TEST_BEG(vector, cxon::JSON<>, "/std")
    using namespace std;
    // std::vector<int>
        R_TEST((vector<int>{}), "[]");
        W_TEST("[]", (vector<int>{}));
        R_TEST((vector<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (vector<int>({1, 2, 3})));
TEST_END()


TEST_BEG(vector_bool, cxon::JSON<>, "/std")
    using namespace std;
    // std::vector<bool>
        R_TEST((vector<bool>{}), "[]");
        W_TEST("[]", (vector<bool>{}));
        R_TEST((vector<bool>({true, false})), "[true,false]");
        W_TEST("[true,false]", (vector<bool>({true, false})));
        R_TEST((vector<bool>({true, false})), "[ok,false]", json::read_error::boolean_invalid, 1);
TEST_END()


TEST_BEG(set, cxon::JSON<>, "/std")
    using namespace std;
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
TEST_END()


#ifdef CXON_HAS_LIB_STD_FLAT_SET
    TEST_BEG(flat_set, cxon::JSON<>, "/std")
        using namespace std;
        // std::flat_set<int>
            R_TEST((flat_set<int>{}), "[]");
            W_TEST("[]", (flat_set<int>{}));
            R_TEST((flat_set<int>({1, 2, 3})), "[1, 1, 2, 3]");
            W_TEST("[1,2,3]", (flat_set<int>({1, 2, 3})));
        // std::flat_multiflat_set<int>;
            R_TEST((flat_multiset<int>{}), "[]");
            W_TEST("[]", (flat_multiset<int>{}));
            R_TEST((flat_multiset<int>({1, 1, 2, 3})), "[1, 1, 2, 3]");
            W_TEST("[1,1,2,3]", (flat_multiset<int>({1, 1, 2, 3})));
    TEST_END()
#endif


TEST_BEG(map, cxon::JSON<>, "/std")
    using namespace std;
    // std::map
        R_TEST(map<string, int>{{"1", 2}, {"3", 4}}, R"({"1": 2, "3": 4})");
        W_TEST(R"({"1":2,"3":4})", map<int, int>{{1, 2}, {3, 4}});
        R_TEST(map<string, int>{{R"({"1":2})", 3}, {R"({"4":5})", 6}}, R"({"{\"1\":2}": 3, "{\"4\":5}": 6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", map<map<string, int>, int>{{map<string, int>{{"1", 2}}, 3}, {map<string, int>{{"4", 5}}, 6}});
    // std::multimap
        R_TEST(multimap<string, int>{{"1", 2}, {"1", 3}}, R"({"1": 2, "1": 3})");
        W_TEST(R"({"1":2,"1":3})", multimap<int, int>{{1, 2}, {1, 3}});
    // errors
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<X>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[5];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[6];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
TEST_END()


#ifdef CXON_HAS_LIB_STD_FLAT_MAP
    TEST_BEG(flat_map, cxon::JSON<>, "/std")
        using namespace std;
        // std::flat_map
            R_TEST(flat_map<string, int>{{"1", 2}, {"3", 4}}, R"({"1": 2, "3": 4})");
            W_TEST(R"({"1":2,"3":4})", flat_map<int, int>{{1, 2}, {3, 4}});
            R_TEST(flat_map<string, int>{{R"({"1":2})", 3}, {R"({"4":5})", 6}}, R"({"{\"1\":2}": 3, "{\"4\":5}": 6})");
            W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", flat_map<flat_map<string, int>, int>{{flat_map<string, int>{{"1", 2}}, 3}, {flat_map<string, int>{{"4", 5}}, 6}});
        // std::flat_multimap
            R_TEST(flat_multimap<string, int>{{"1", 2}, {"1", 3}}, R"({"1": 2, "1": 3})");
            W_TEST(R"({"1":2,"1":3})", flat_multimap<int, int>{{1, 2}, {1, 3}});
        // errors
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<X>(c, flat_map<int, int>{{3, 1}});
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, flat_map<int, int>{{3, 1}});
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, flat_map<int, int>{{3, 1}});
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[3];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, flat_map<int, int>{{3, 1}});
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[4];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, flat_map<int, int>{{3, 1}});
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[5];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, flat_map<int, int>{{3, 1}});
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[6];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, flat_map<int, int>{{3, 1}});
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
    TEST_END()
#endif


TEST_BEG(unordered_map, cxon::JSON<>, "/std")
    using namespace std;
    // std::unordered_map
        R_TEST(unordered_map<string, int>{{"1", 2}}, R"({"1": 2})");
        W_TEST(R"({"1":2})", unordered_map<int, int>{{1, 2}});
    // std::unordered_multimap
        R_TEST(unordered_multimap<string, int>{{"1", 1}, {"1", 1}}, R"({"1": 1, "1": 1})");
        W_TEST(R"({"1":1,"1":1})", unordered_multimap<int, int>{{1, 1}, {1, 1}});
TEST_END()

TEST_BEG(unordered_map_unquoted_keys, cxon::JSON<cxon::test::unquoted_keys_traits<>>, "/std")
    using namespace std;
    // std::unordered_map
        R_TEST(unordered_map<int, string>{{1, "2"}}, R"({1: "2"})");
        W_TEST(R"({1:2})", unordered_map<int, int>{{1, 2}});
    // std::unordered_multimap
        R_TEST(unordered_multimap<int, string>{{1, "1"}, {1, "1"}}, R"({1: "1", 1: "1"})");
        W_TEST(R"({1:1,1:1})", unordered_multimap<int, int>{{1, 1}, {1, 1}});
TEST_END()


TEST_BEG(unordered_set, cxon::JSON<>, "/std")
    using namespace std;
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
TEST_END()


TEST_BEG(cross, cxon::JSON<>, "/std")
    using namespace std;
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


template <std::size_t N>
    struct less {
        constexpr bool operator()(const std::bitset<N>& l, const std::bitset<N>& r) const {
            return l.to_string() < r.to_string();
        }
    };

TEST_BEG(bitset, cxon::JSON<>, "/std")
    using namespace std;
    // std::bitset
        R_TEST(bitset<8>(85), QS("01010101"));
        W_TEST(QS("01010101"), bitset<8>(85));
        R_TEST(bitset<8>(255), QS("11111111"));
        W_TEST(QS("11111111"), bitset<8>(255));
        R_TEST(bitset<8>(255), "x", json::read_error::unexpected, 0);
        R_TEST(bitset<8>(255), QS("x"), json::read_error::unexpected, 1);
        R_TEST(bitset<8>(255), "\"11111111", json::read_error::unexpected, 9);
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<X>(c, bitset<8>(255));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, bitset<8>(255));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[9];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                auto r = cxon::to_bytes<X>(c, bitset<8>(255));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        R_TEST(map<bitset<8>, int, ::less<8>>{{bitset<8>(85), 1}}, R"({"01010101":1})");
        W_TEST(R"({"01010101":1})", map<bitset<8>, int, ::less<8>>{{bitset<8>(85), 1}});
TEST_END()


TEST_BEG(complex, cxon::JSON<>, "/std")
    using namespace std;
    // std::complex
        R_TEST(complex<float>(), "[0, 0]");
        R_TEST(complex<float>(), "[]", json::read_error::floating_point_invalid, 1);
        R_TEST(complex<float>(), "[0]", json::read_error::unexpected, 2);
        R_TEST(complex<float>(), "[0,]", json::read_error::floating_point_invalid, 3);
        R_TEST(complex<float>(), "[0,0", json::read_error::unexpected, 4);
        W_TEST("[0,0]", complex<float>());
        R_TEST(complex<double>(), "[0, 0]");
        R_TEST(complex<double>(), "[]", json::read_error::floating_point_invalid, 1);
        R_TEST(complex<double>(), "[0]", json::read_error::unexpected, 2);
        W_TEST("[0,0]", complex<double>());
        R_TEST(complex<long double>(), "[0, 0]");
        R_TEST(complex<long double>(), "[]", json::read_error::floating_point_invalid, 1);
        R_TEST(complex<long double>(), "[0]", json::read_error::unexpected, 2);
        W_TEST("[0,0]", complex<long double>());
    // write errors
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
                c.push_back('x');
            auto r = cxon::to_bytes<X>(c, complex<float>());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, complex<float>());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, complex<float>());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, complex<float>());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, complex<float>());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
TEST_END()


TEST_BEG(chrono, cxon::JSON<>, "/std")
    using namespace std::chrono;
    // std::chrono
        R_TEST(duration<unsigned>(42), "42");
        R_TEST(duration<unsigned>(42), "x", json::read_error::integral_invalid, 0);
        W_TEST("42", duration<unsigned>(42));
        R_TEST(time_point<system_clock>(system_clock::duration(42)), "42");
        R_TEST(time_point<system_clock>(system_clock::duration(42)), "x", json::read_error::integral_invalid, 0);
        W_TEST("42", time_point<system_clock>(system_clock::duration(42)));
TEST_END()


#ifdef CXON_HAS_LIB_STD_STRING_VIEW
    TEST_BEG(basic_string_view, cxon::JSON<>, "/std")
        using namespace std;
        // std::string_view
            char const s[] = "1";
            W_TEST(QS("1"), string_view(s));
            W_TEST(R"({"1":1})", map<string_view, int>{{string_view(s), 1}});
    TEST_END()
#endif


#ifdef CXON_HAS_LIB_STD_OPTIONAL
    TEST_BEG(optional, cxon::JSON<>, "/std")
        using namespace std;
        // std::optional
            R_TEST(optional<int>(42), "42");
            W_TEST("42", optional<int>(42));
            R_TEST(optional<int>(), "null");
            W_TEST("null", optional<int>());
            R_TEST(optional<int>(), "nil", json::read_error::unexpected, 0);
            {   optional<int> o(42);
                TEST_CHECK(cxon::from_bytes<X>(o, "null") && o == nullopt);
            }
            {   optional<int> o(0);
                TEST_CHECK(cxon::from_bytes<X>(o, "null") && o == nullopt);
            }
    TEST_END()

    TEST_BEG(optional_allow_cxx_comments, cxon::JSON<cxon::test::allow_cxx_comments_traits<>>, "/std")
        using namespace std;
            R_TEST(optional<int>(42), "/42", json::read_error::unexpected, 1);
    TEST_END()
#endif


#ifdef CXON_HAS_LIB_STD_VARIANT

    struct variant_struct_1 {
        int x;
        variant_struct_1(int x = 0) : x(x) {}
        bool operator ==(const variant_struct_1& s) const noexcept { return x == s.x; }
        CXON_JSON_CLS_MEMBER(variant_struct_1, CXON_JSON_CLS_FIELD_ASIS(x))
    };
    struct variant_struct_2 {
        int x;
        variant_struct_2(int x = 0) : x(x) {}
        bool operator ==(const variant_struct_2& s) const noexcept { return x == s.x; }
        CXON_JSON_CLS_MEMBER(variant_struct_2, CXON_JSON_CLS_FIELD_ASIS(x))
    };
    CXON_JSON_CLS_TRAITS(variant_struct_2)
    template <typename T>
        struct variant_struct_3 {
            T x;
            variant_struct_3(T x = 0) : x(x) {}
            bool operator ==(const variant_struct_3& s) const noexcept { return x == s.x; }
            CXON_JSON_CLS_MEMBER(variant_struct_3, CXON_JSON_CLS_FIELD_ASIS(x))
        };
    namespace cxon { namespace cio {
        template <typename T> struct is_map<variant_struct_3<T>> : std::true_type {};
    }}

    enum class variant_enum { a, b };
    CXON_JSON_ENM(variant_enum, CXON_JSON_ENM_VALUE_ASIS(a), CXON_JSON_ENM_VALUE_ASIS(b))

    TEST_BEG(variant, cxon::JSON<>, "/std")
        using namespace std;
        // std::variant
            R_TEST(variant<int, string>(42), R"(42)");
            W_TEST(R"(42)", variant<int, string>(42));
            R_TEST(variant<int, string>("42"), R"("42")");
            W_TEST(R"("42")", variant<int, string>("42"));
            R_TEST(variant<monostate, map<int, int>, vector<int>, string, int, bool>(map<int, int>{{1, 3}}), R"({"1":3})");
            W_TEST(R"({"1":3})", variant<monostate, map<int, int>, vector<int>, string, int, bool>(map<int, int>{{1, 3}}));
            R_TEST(variant<monostate, map<int, int>, vector<int>, string, int, bool>(vector<int>{1, 3}), R"([1,3])");
            W_TEST(R"([1,3])", variant<monostate, map<int, int>, vector<int>, string, int, bool>(vector<int>{1, 3}));
            R_TEST(variant<monostate, map<int, int>, vector<int>, string, int, bool>("13"), R"("13")"); // bug: msvc/toolset 143 C++17 selects bool, ok with C++20
            W_TEST(R"("13")", variant<monostate, map<int, int>, vector<int>, string, int, bool>("13")); // bug: msvc/toolset 143 C++17 selects bool, ok with C++20
            R_TEST(variant<monostate, map<int, int>, vector<int>, string, int, bool>(13), R"(13)");
            W_TEST(R"(13)", variant<monostate, map<int, int>, vector<int>, string, int, bool>(13));
            R_TEST(variant<monostate, map<int, int>, vector<int>, string, int, bool>(true), R"(true)");
            W_TEST(R"(true)", variant<monostate, map<int, int>, vector<int>, string, int, bool>(true));
            R_TEST(variant<monostate, map<int, int>, vector<int>, string, int, bool>(), R"(null)");
            W_TEST(R"(null)", variant<monostate, map<int, int>, vector<int>, string, int, bool>());
            R_TEST(variant<map<int, int>, vector<int>, string, int, bool, nullptr_t>(nullptr), R"(null)");
            W_TEST(R"(null)", variant<map<int, int>, vector<int>, string, int, bool, nullptr_t>(nullptr));
            R_TEST(variant<string, variant<int>>("13"), R"("13")");
            W_TEST(R"("13")", variant<string, variant<int>>("13"));
            R_TEST(variant<string, variant<int>>(13), R"(13)");
            W_TEST(R"(13)", variant<string, variant<int>>(13));
            R_TEST(variant<double, variant<int>>(13.), R"({"0":13})");
            W_TEST(R"({"0":13})", variant<double, variant<int>>(13.));
            R_TEST(variant<double, variant<int>>(13), R"({"1":13})"); // bug: msvc/toolset 143 C++17 selects double, ok with C++20
            W_TEST(R"({"1":13})", variant<double, variant<int>>(13)); // bug: msvc/toolset 143 C++17 selects double, ok with C++20
            R_TEST(variant<monostate>(), R"({"1":3})", json::read_error::unexpected, 0);
            R_TEST(variant<monostate>(), R"([1,3])", json::read_error::unexpected, 0);
            R_TEST(variant<monostate>(), R"([1,3])", json::read_error::unexpected, 0);
            R_TEST(variant<monostate>(), R"("13")", json::read_error::unexpected, 0);
            R_TEST(variant<monostate>(), R"(13)", json::read_error::unexpected, 0);
            R_TEST(variant<monostate>(), R"(true)", json::read_error::unexpected, 0);
            R_TEST(variant<int>(), R"(null)", json::read_error::unexpected, 0);
            R_TEST(variant<int>(), R"(TRUE)", json::read_error::unexpected, 0);
            R_TEST(variant<nullptr_t, variant_struct_1>(13), R"({"1":{"x":13}})");
            W_TEST(R"({"1":{"x":13}})", variant<nullptr_t, variant_struct_1>(13));
            R_TEST(variant<nullptr_t, variant_struct_1>(nullptr), R"({"0":null})");
            W_TEST(R"({"0":null})", variant<nullptr_t, variant_struct_1>(nullptr));
            R_TEST(variant<nullptr_t, variant_struct_2>(13), R"({"x":13})");
            W_TEST(R"({"x":13})", variant<nullptr_t, variant_struct_2>(13));
            R_TEST(variant<nullptr_t, variant_struct_2>(nullptr), R"(null)");
            W_TEST(R"(null)", variant<nullptr_t, variant_struct_2>(nullptr));
            R_TEST(variant<nullptr_t, variant_struct_3<int>>(13), R"({"x":13})");
            W_TEST(R"({"x":13})", variant<nullptr_t, variant_struct_3<int>>(13));
            R_TEST(variant<nullptr_t, variant_struct_3<int>>(nullptr), R"(null)");
            W_TEST(R"(null)", variant<nullptr_t, variant_struct_3<int>>(nullptr));
            R_TEST(variant<nullptr_t, variant_enum>(variant_enum::b), R"("b")");
            W_TEST(R"("b")", variant<nullptr_t, variant_enum>(variant_enum::b));
            R_TEST(variant<nullptr_t, variant_enum>(nullptr), R"(null)");
            W_TEST(R"(null)", variant<nullptr_t, variant_enum>(nullptr));
        // std::variant
            R_TEST(variant<int, double>(in_place_index_t<0>(), 1), R"({"0":1})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({"1":0})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({"2":0})", json::read_error::unexpected, 1);
            W_TEST(R"({"0":1})", variant<int, double>(1));
            W_TEST(R"({"1":0})", variant<int, double>(in_place_index_t<1>(), 0));
            R_TEST(variant<monostate, int>(), R"(null)");
            R_TEST(variant<monostate, int>(), R"("1")", json::read_error::unexpected, 0);
            W_TEST(R"(null)", variant<monostate, int>());
            R_TEST(variant<int, double>(0), R"()", json::read_error::unexpected, 0);
            R_TEST(variant<int, double>(0), R"({)", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({")", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({"x)", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({"0)", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({"0")", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({"0":)", json::read_error::integral_invalid, 5);
            R_TEST(variant<int, double>(0), R"({"0":x)", json::read_error::integral_invalid, 5);
            R_TEST(variant<int, double>(0), R"({"0":0)", json::read_error::unexpected, 6);
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[3];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[4];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[5];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[6];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
    TEST_END()

    TEST_BEG(variant_allow_cxx_comments, cxon::JSON<cxon::test::allow_cxx_comments_traits<>>, "/std")
        R_TEST(std::variant<std::monostate>(), R"(/**/null)");
        R_TEST(std::variant<std::monostate>(), R"(/**/nil)", json::read_error::unexpected, 4);
        R_TEST(std::variant<std::monostate>(), R"(/nil)", json::read_error::unexpected, 1);
    TEST_END()

    TEST_BEG(variant_unquoted_keys, cxon::JSON<cxon::test::unquoted_keys_traits<>>, "/std")
        using namespace std;
        // std::variant
            R_TEST(variant<int, double>(in_place_index_t<0>(), 1), R"({0:1})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({1:0})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({2:0})", json::read_error::unexpected, 1);
            W_TEST(R"({0:1})", variant<int, double>(1));
            W_TEST(R"({1:0})", variant<int, double>(in_place_index_t<1>(), 0));
            R_TEST(variant<monostate, int>(), R"(null)");
            R_TEST(variant<monostate, int>(), R"("1")", json::read_error::unexpected, 0);
            W_TEST(R"(null)", variant<monostate, int>());
            R_TEST(variant<int, double>(0), R"()", json::read_error::unexpected, 0);
            R_TEST(variant<int, double>(0), R"({)", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({x)", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({0)", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({0:)", json::read_error::integral_invalid, 3);
            R_TEST(variant<int, double>(0), R"({0:x)", json::read_error::integral_invalid, 3);
            R_TEST(variant<int, double>(0), R"({0:0)", json::read_error::unexpected, 4);
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[3];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[4];
                auto c = cxon::cnt::make_range_container(begin(b), end(b));
                    auto r = cxon::to_bytes<X>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
    TEST_END()
#endif


#ifdef CXON_HAS_LIB_STD_SPAN
    namespace cxon { namespace test {
        template <typename T, std::size_t N>
            struct match<std::span<T, N>> {
                static bool values(const std::span<T, N>& s1, const std::span<T, N>& s2) {
                    return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end());
                }
            };
    }}
    TEST_BEG(span, cxon::JSON<>, "/std")
        using namespace std;
        {   // static
            // std::span<int, 0>
                R_TEST((span<int, 0>{}), "[]");
                W_TEST("[]", (span<int, 0>{}));
                R_TEST((span<int, 0>{}), "", json::read_error::unexpected, 0);
                R_TEST((span<int, 0>{}), "]", json::read_error::unexpected, 0);
                R_TEST((span<int, 0>{}), "[", json::read_error::unexpected, 1);
            // std::span<int, 3>
            {   array<int, 3> a; span s{a};
                TEST_CHECK(cxon::from_bytes<X>(s, "[1, 2, 3]") && a == (array<int, 3>{{1, 2, 3}}));
                W_TEST("[1,2,3]", span {a});
                TEST_CHECK(cxon::from_bytes<X>(s, "[1, 2]") && a == (array<int, 3>{{1, 2, 3}}));
                {   auto fbr = cxon::from_bytes<X>(s, "[1, 2, 3, 4]");
                    TEST_CHECK(fbr.ec == json::read_error::overflow && *fbr.end == '[');
                }
                {   auto fbr = cxon::from_bytes<X>(s, "");
                    TEST_CHECK(fbr.ec == json::read_error::unexpected && *fbr.end == '\0');
                }
                {   auto fbr = cxon::from_bytes<X>(s, "[");
                    TEST_CHECK(fbr.ec == json::read_error::integral_invalid && *fbr.end == '\0');
                }
                {   auto fbr = cxon::from_bytes<X>(s, "[1x");
                    TEST_CHECK(fbr.ec == json::read_error::unexpected && *fbr.end == 'x');
                }
                {   auto fbr = cxon::from_bytes<X>(s, "[1,x");
                    TEST_CHECK(fbr.ec == json::read_error::integral_invalid && *fbr.end == 'x');
                }
                {   auto fbr = cxon::from_bytes<X>(s, "[1,1");
                    TEST_CHECK(fbr.ec == json::read_error::unexpected && *fbr.end == '\0');
                }
            }
        }
        {   // dynamic
            // std::span<int>/0
                R_TEST((span<int, dynamic_extent>{}), "[]");
                W_TEST("[]", (span<int, dynamic_extent>{}));
                R_TEST((span<int, dynamic_extent>{}), "", json::read_error::unexpected, 0);
                R_TEST((span<int, dynamic_extent>{}), "]", json::read_error::unexpected, 0);
                R_TEST((span<int, dynamic_extent>{}), "[", json::read_error::unexpected, 1);
            // std::span<int>/3
            {   vector<int> v(3); span s{v};
                TEST_CHECK(cxon::from_bytes<X>(s, "[1, 2, 3]") && v == (vector<int>{{1, 2, 3}}));
                W_TEST("[1,2,3]", span {v});
                TEST_CHECK(cxon::from_bytes<X>(s, "[1, 2]") && v == (vector<int>{{1, 2, 3}}));
                {   auto fbr = cxon::from_bytes<X>(s, "[1, 2, 3, 4]");
                    TEST_CHECK(fbr.ec == json::read_error::overflow && *fbr.end == '[');
                }
                {   auto fbr = cxon::from_bytes<X>(s, "");
                    TEST_CHECK(fbr.ec == json::read_error::unexpected && *fbr.end == '\0');
                }
                {   auto fbr = cxon::from_bytes<X>(s, "[");
                    TEST_CHECK(fbr.ec == json::read_error::integral_invalid && *fbr.end == '\0');
                }
                {   auto fbr = cxon::from_bytes<X>(s, "[1x");
                    TEST_CHECK(fbr.ec == json::read_error::unexpected && *fbr.end == 'x');
                }
                {   auto fbr = cxon::from_bytes<X>(s, "[1,x");
                    TEST_CHECK(fbr.ec == json::read_error::integral_invalid && *fbr.end == 'x');
                }
                {   auto fbr = cxon::from_bytes<X>(s, "[1,1");
                    TEST_CHECK(fbr.ec == json::read_error::unexpected && *fbr.end == '\0');
                }
            }
        }
    TEST_END()
#endif
