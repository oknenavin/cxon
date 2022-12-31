// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

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
#include "cxon/lib/std/unordered_set.hxx"
#include "cxon/lib/std/map.hxx"
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
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<XXON>(c, string("s\n"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, string("s\n"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, string("s\n"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[3];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, string("s\n"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[4];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, string("s\n"));
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
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<XXON>(c, wstring(L"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, wstring(L"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, wstring(L"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
    // std::basic_string<char8_t> / std::u8string
#       if __cplusplus > 201703L /* C++20 */
            R_TEST(u8string(u8"test"), QS("test"));
            W_TEST(QS("test"), u8string(u8"test"));
            R_TEST(u8string(u8"te\nst"), QS("te\\nst"));
            W_TEST(QS("te\\nst"), u8string(u8"te\nst"));
            R_TEST(u8string(), "a", json::read_error::unexpected, 0);
            R_TEST(u8string(), "\"", json::read_error::unexpected, 1);
            R_TEST(u8string(), QS("\\u001"), json::read_error::escape_invalid, 1);
            // errors
                {   char b[1];
                    auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                        c.push_back('x');
                        auto r = cxon::to_bytes<XXON>(c, u8string(u8"s"));
                    TEST_CHECK(r.ec == json::write_error::output_failure);
                }
                {   char b[1];
                    auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                        auto r = cxon::to_bytes<XXON>(c, u8string(u8"s"));
                    TEST_CHECK(r.ec == json::write_error::output_failure);
                }
                {   char b[2];
                    auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                        auto r = cxon::to_bytes<XXON>(c, u8string(u8"s"));
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
        W_TEST(QS("\xED\xB0\x80"), u16string(u"\xDC00"));
        R_TEST(u16string(), "a", json::read_error::unexpected, 0);
        R_TEST(u16string(), "\"", json::read_error::unexpected, 1);
        R_TEST(u16string(), QS("\\u001"), json::read_error::escape_invalid, 1);
        // errors
            {   char b[1];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<XXON>(c, u16string(u"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, u16string(u"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, u16string(u"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[3];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, u16string(u"s"));
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
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<XXON>(c, u32string(U"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, u32string(U"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, u32string(U"s"));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
    // errors
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<XXON>(c, string("s\n"));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, string("s\n"));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, string("s\n"));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, string("s\n"));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, string("s\n"));
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
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
            auto r = cxon::to_bytes<XXON>(c, tuple<>{});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, tuple<>{});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
            auto r = cxon::to_bytes<XXON>(c, tuple<int>{0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, tuple<int>{0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, tuple<int>{0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, tuple<int, long>{0, 0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, tuple<int, long>{0, 0});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, tuple<int, long>{0, 0});
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
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<XXON>(c, std::make_pair(3, 1));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, std::make_pair(3, 1));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, std::make_pair(3, 1));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, std::make_pair(3, 1));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, std::make_pair(3, 1));
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
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<XXON>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[5];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[6];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
TEST_END()


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
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<XXON>(c, bitset<8>(255));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, bitset<8>(255));
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[9];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, bitset<8>(255));
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
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
            auto r = cxon::to_bytes<XXON>(c, complex<float>());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, complex<float>());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, complex<float>());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, complex<float>());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, complex<float>());
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
    TEST_END()

    TEST_BEG(allow_comments_optional, cxon::JSON<cxon::test::allow_comments_traits<>>, "/std")
        using namespace std;
            R_TEST(optional<int>(42), "/42", json::read_error::unexpected, 1);
    TEST_END()
#endif


#ifdef CXON_HAS_LIB_STD_VARIANT
    TEST_BEG(variant, cxon::JSON<>, "/std")
        using namespace std;
        // std::variant
            R_TEST(variant<int, double>(in_place_index_t<0>(), 1), R"({"0":1})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({"1":0})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({"2":0})", json::read_error::unexpected, 1);
            W_TEST(R"({"0":1})", variant<int, double>(1));
            W_TEST(R"({"1":0})", variant<int, double>(in_place_index_t<1>(), 0));
            R_TEST(variant<monostate, int>(), R"({"0":null})");
            R_TEST(variant<monostate, int>(), R"({"0":1})", json::read_error::unexpected, 5);
            W_TEST(R"({"0":null})", variant<monostate, int>());
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
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[3];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[4];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[5];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[6];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
    TEST_END()

    TEST_BEG(variant_unquoted_keys, cxon::JSON<cxon::test::unquoted_keys_traits<>>, "/std")
        using namespace std;
        // std::variant
            R_TEST(variant<int, double>(in_place_index_t<0>(), 1), R"({0:1})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({1:0})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({2:0})", json::read_error::unexpected, 1);
            W_TEST(R"({0:1})", variant<int, double>(1));
            W_TEST(R"({1:0})", variant<int, double>(in_place_index_t<1>(), 0));
            R_TEST(variant<monostate, int>(), R"({0:null})");
            R_TEST(variant<monostate, int>(), R"({0:1})", json::read_error::unexpected, 3);
            W_TEST(R"({0:null})", variant<monostate, int>());
            R_TEST(variant<int, double>(0), R"()", json::read_error::unexpected, 0);
            R_TEST(variant<int, double>(0), R"({)", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({x)", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({0)", json::read_error::unexpected, 1);
            R_TEST(variant<int, double>(0), R"({0:)", json::read_error::integral_invalid, 3);
            R_TEST(variant<int, double>(0), R"({0:x)", json::read_error::integral_invalid, 3);
            R_TEST(variant<int, double>(0), R"({0:0)", json::read_error::unexpected, 4);
            {   char b[1];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    c.push_back('x');
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[1];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[2];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[3];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
            {   char b[4];
                auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                    auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
                TEST_CHECK(r.ec == json::write_error::output_failure);
            }
    TEST_END()
#endif

////////////////////////////////////////////////////////////////////////////////

namespace std {
    template <size_t N>
        inline bool operator <(const std::bitset<N>& t1, const std::bitset<N>& t2) {
            return t1.to_string() < t2.to_string();
        };
}

enum class keys_enum { E1, E2 };
CXON_JSON_ENM(keys_enum,
    CXON_JSON_ENM_VALUE_ASIS(E1),
    CXON_JSON_ENM_VALUE_ASIS(E2)
)

struct key_struct {
    int x, y;
    friend bool operator ==(const key_struct& t1, const key_struct& t2) {
        return t1.x == t2.x && t1.y == t2.y;
    }
    friend bool operator <(const key_struct& t1, const key_struct& t2) {
        return t1.x < t2.x || (t1.x == t2.x && t1.y < t2.y);
    }
};
CXON_JSON_CLS(key_struct,
    CXON_JSON_CLS_FIELD_ASIS(x),
    CXON_JSON_CLS_FIELD_ASIS(y)
)

TEST_BEG(quoted_keys, cxon::JSON<>, "/std")
    using namespace std;

    R_TEST(map<string, int>{{"1", 2}, {"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<string, int>{{"1", 2}, {"3", 4}});
    R_TEST(map<wstring, int>{{L"1", 2}, {L"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<wstring, int>{{L"1", 2}, {L"3", 4}});
    R_TEST(map<u16string, int>{{u"1", 2}, {u"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u16string, int>{{u"1", 2}, {u"3", 4}});
    R_TEST(map<u32string, int>{{U"1", 2}, {U"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u32string, int>{{U"1", 2}, {U"3", 4}});
    W_TEST(R"({"1":2,"3":4})", map<char, int>{{'1', 2}, {'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<wchar_t, int>{{L'1', 2}, {L'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char16_t, int>{{u'1', 2}, {u'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char32_t, int>{{U'1', 2}, {U'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<signed char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<unsigned char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<unsigned short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<int, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<unsigned, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<unsigned long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<unsigned long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<float, int>{{1.f, 2}, {3.f, 4}});
    W_TEST(R"({"1":2,"3":4})", map<double, int>{{1., 2}, {3., 4}});
    W_TEST(R"({"1":2,"3":4})", map<long double, int>{{1., 2}, {3., 4}});
    {   struct less { constexpr bool operator ()(nullptr_t, nullptr_t) const noexcept { return true; } };
        W_TEST(R"({"null":0})", map<nullptr_t, int, less>{{nullptr, 0}});
    }
    // escape quotes
    {   using xmap = map<map<char, int>, int>;
        R_TEST(xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}}, R"({"{\"1\":2}":3,"{\"4\":5}":6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}});
    }
    {   using xmap = map<map<wchar_t, int>, int>;
        R_TEST(xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}}, R"({"{\"1\":2}":3,"{\"4\":5}":6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}});
    }
    {   using xmap = map<map<char16_t, int>, int>;
        R_TEST(xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}}, R"({"{\"1\":2}":3,"{\"4\":5}":6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}});
    }
    {   using xmap = map<map<char32_t, int>, int>;
        R_TEST(xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}}, R"({"{\"1\":2}":3,"{\"4\":5}":6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}});
    }
    {   using xmap = map<map<string, int>, int>;
        R_TEST(xmap{{{{"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\u0022z\":1}":3})");
        W_TEST(R"({"{\"x\ty\u0022z\":1}":3})", xmap{{{{"x\ty\"z", 1}}, 3}});
        R_TEST(xmap{{{{"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\"z\":1}":3})", json::read_error::unexpected, 11);
    }
    {   using xmap = map<map<wstring, int>, int>;
        R_TEST(xmap{{{{L"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\u0022z\":1}":3})");
        W_TEST(R"({"{\"x\ty\u0022z\":1}":3})", xmap{{{{L"x\ty\"z", 1}}, 3}});
        R_TEST(xmap{{{{L"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\"z\":1}":3})", json::read_error::unexpected, 11);
    }
    {   using xmap = map<map<u16string, int>, int>;
        R_TEST(xmap{{{{u"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\u0022z\":1}":3})");
        W_TEST(R"({"{\"x\ty\u0022z\":1}":3})", xmap{{{{u"x\ty\"z", 1}}, 3}});
        R_TEST(xmap{{{{u"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\"z\":1}":3})", json::read_error::unexpected, 11);
    }
    {   using xmap = map<map<u32string, int>, int>;
        R_TEST(xmap{{{{U"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\u0022z\":1}":3})");
        W_TEST(R"({"{\"x\ty\u0022z\":1}":3})", xmap{{{{U"x\ty\"z", 1}}, 3}});
        R_TEST(xmap{{{{U"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\"z\":1}":3})", json::read_error::unexpected, 11);
    }
    {   using xmap = map<map<map<string, int>, int>, int>;
        R_TEST(xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}}, R"({"{{\"x\ty\u0022z\":1}:3}":5})");
        W_TEST(R"({"{{\"x\ty\u0022z\":1}:3}":5})", xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<wstring, int>, int>, int>;
        R_TEST(xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}}, R"({"{{\"x\ty\u0022z\":1}:3}":5})");
        W_TEST(R"({"{{\"x\ty\u0022z\":1}:3}":5})", xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u16string, int>, int>, int>;
        R_TEST(xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}}, R"({"{{\"x\ty\u0022z\":1}:3}":5})");
        W_TEST(R"({"{{\"x\ty\u0022z\":1}:3}":5})", xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u32string, int>, int>, int>;
        R_TEST(xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}}, R"({"{{\"x\ty\u0022z\":1}:3}":5})");
        W_TEST(R"({"{{\"x\ty\u0022z\":1}:3}":5})", xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<string, int>, int>;
        R_TEST(xmap{}, R"({"{\"1\)", json::read_error::escape_invalid, 6);
        R_TEST(xmap{}, R"({"{\"1\1)", json::read_error::escape_invalid, 6);
    }
    {   using xmap = map<map<wstring, int>, int>;
        R_TEST(xmap{}, R"({"{\"1\)", json::read_error::escape_invalid, 6);
        R_TEST(xmap{}, R"({"{\"1\1)", json::read_error::escape_invalid, 6);
    }
    {   using xmap = map<map<u16string, int>, int>;
        R_TEST(xmap{}, R"({"{\"1\)", json::read_error::escape_invalid, 6);
        R_TEST(xmap{}, R"({"{\"1\1)", json::read_error::escape_invalid, 6);
    }
    {   using xmap = map<map<u32string, int>, int>;
        R_TEST(xmap{}, R"({"{\"1\)", json::read_error::escape_invalid, 6);
        R_TEST(xmap{}, R"({"{\"1\1)", json::read_error::escape_invalid, 6);
    }
    {   char b[1]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[2]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[3]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[4]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[5]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[6]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[7]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[8]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[9]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[10]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[11]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[12]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[13]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[14]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[15]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r);
    }
    // inf/nan
    {
        constexpr auto ninf = -std::numeric_limits<double>::infinity();
        constexpr auto pinf =  std::numeric_limits<double>::infinity();
        //constexpr auto qnan =  std::numeric_limits<double>::quiet_NaN();
        {   using xmap = map<double, int>;
            R_TEST(xmap{{ninf, 1}, {pinf, 2}}, R"({"-inf":1,"inf":2})");
            W_TEST(R"({"-inf":1,"inf":2})", xmap{{ninf, 1}, {pinf, 2}});
        }
        {   using xmap = map<map<double, int>, int>;
            R_TEST(xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}}, R"({"{-inf:1}":2,"{inf:3}":4})");
            W_TEST(R"({"{-inf:1}":2,"{inf:3}":4})", xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}});
        }
    }
    // std::bitset
    {   using xmap = map<bitset<8>, int>;
        R_TEST(xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}}, R"({"00000000":1,"11111111":2})");
        W_TEST(R"({"00000000":1,"11111111":2})", xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}});
    }
    {   using xmap = map<map<bitset<8>, int>, int>;
        R_TEST(xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}}, R"({"{\"00000000\":1}":2,"{\"11111111\":3}":4})");
        W_TEST(R"({"{\"00000000\":1}":2,"{\"11111111\":3}":4})", xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}});
    }
    // enum
    {   using xmap = map<keys_enum, int>;
        R_TEST(xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}}, R"({"E1":1,"E2":2})");
        W_TEST(R"({"E1":1,"E2":2})", xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}});
    }
    {   using xmap = map<map<keys_enum, int>, int>;
        R_TEST(xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}}, R"({"{\"E1\":1}":2,"{\"E2\":3}":4})");
        W_TEST(R"({"{\"E1\":1}":2,"{\"E2\":3}":4})", xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}});
    }
    // struct
    {   using xmap = map<key_struct, int>;
        R_TEST(xmap{{{1, 2}, 3}, {{4, 5}, 6}}, R"({"{\"x\":1,\"y\":2}":3,"{\"x\":4,\"y\":5}":6})");
        W_TEST(R"({"{\"x\":1,\"y\":2}":3,"{\"x\":4,\"y\":5}":6})", xmap{{{1, 2}, 3}, {{4, 5}, 6}});
    }
TEST_END()

TEST_BEG(unquoted_keys, cxon::JSON<cxon::test::unquoted_keys_traits<>>, "/std")
    using namespace std;

    R_TEST(map<string, int>{{"1", 2}, {"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<string, int>{{"1", 2}, {"3", 4}});
    R_TEST(map<wstring, int>{{L"1", 2}, {L"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<wstring, int>{{L"1", 2}, {L"3", 4}});
    R_TEST(map<u16string, int>{{u"1", 2}, {u"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u16string, int>{{u"1", 2}, {u"3", 4}});
    R_TEST(map<u32string, int>{{U"1", 2}, {U"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u32string, int>{{U"1", 2}, {U"3", 4}});
    W_TEST(R"({"1":2,"3":4})", map<char, int>{{'1', 2}, {'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<wchar_t, int>{{L'1', 2}, {L'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char16_t, int>{{u'1', 2}, {u'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char32_t, int>{{U'1', 2}, {U'3', 4}});
    W_TEST(R"({1:2,3:4})", map<signed char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<int, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<float, int>{{1.f, 2}, {3.f, 4}});
    W_TEST(R"({1:2,3:4})", map<double, int>{{1., 2}, {3., 4}});
    W_TEST(R"({1:2,3:4})", map<long double, int>{{1., 2}, {3., 4}});
    {   struct less { constexpr bool operator ()(nullptr_t, nullptr_t) const noexcept { return true; } };
        W_TEST(R"({null:0})", map<nullptr_t, int, less>{{nullptr, 0}});
    }
    // escape quotes
    {   using xmap = map<map<char, int>, int>;
        R_TEST(xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}});
    }
    {   using xmap = map<map<wchar_t, int>, int>;
        R_TEST(xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}});
    }
    {   using xmap = map<map<char16_t, int>, int>;
        R_TEST(xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}});
    }
    {   using xmap = map<map<char32_t, int>, int>;
        R_TEST(xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}});
    }
    {   using xmap = map<map<string, int>, int>;
        R_TEST(xmap{ {{{"x\ty\"z", 1}}, 3} }, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<wstring, int>, int>;
        R_TEST(xmap{{{{L"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{L"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<u16string, int>, int>;
        R_TEST(xmap{{{{u"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{u"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<u32string, int>, int>;
        R_TEST(xmap{{{{U"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{U"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<map<string, int>, int>, int>;
        R_TEST(xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<wstring, int>, int>, int>;
        R_TEST(xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u16string, int>, int>, int>;
        R_TEST(xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u32string, int>, int>, int>;
        R_TEST(xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}});
    }
    // inf/nan
    {
        constexpr auto ninf = -std::numeric_limits<double>::infinity();
        constexpr auto pinf =  std::numeric_limits<double>::infinity();
        //constexpr auto qnan =  std::numeric_limits<double>::quiet_NaN();
        {   using xmap = map<double, int>;
            R_TEST(xmap{{ninf, 1}, {pinf, 2}}, R"({"-inf":1,"inf":2})");
            W_TEST(R"({"-inf":1,"inf":2})", xmap{{ninf, 1}, {pinf, 2}});
        }
        {   using xmap = map<map<double, int>, int>;
            R_TEST(xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}}, R"({{"-inf":1}:2,{"inf":3}:4})");
            W_TEST(R"({{"-inf":1}:2,{"inf":3}:4})", xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}});
        }
    }
    // std::bitset
    {   using xmap = map<bitset<8>, int>;
        R_TEST(xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}}, R"({"00000000":1,"11111111":2})");
        W_TEST(R"({"00000000":1,"11111111":2})", xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}});
    }
    {   using xmap = map<map<bitset<8>, int>, int>;
        R_TEST(xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}}, R"({{"00000000":1}:2,{"11111111":3}:4})");
        W_TEST(R"({{"00000000":1}:2,{"11111111":3}:4})", xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}});
    }
    // enum
    {   using xmap = map<keys_enum, int>;
        R_TEST(xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}}, R"({"E1":1,"E2":2})");
        W_TEST(R"({"E1":1,"E2":2})", xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}});
    }
    {   using xmap = map<map<keys_enum, int>, int>;
        R_TEST(xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}}, R"({{"E1":1}:2,{"E2":3}:4})");
        W_TEST(R"({{"E1":1}:2,{"E2":3}:4})", xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}});
    }
    // struct
    {   using xmap = map<key_struct, int>;
        R_TEST(xmap{{{1, 2}, 3}, {{4, 5}, 6}}, R"({{"x":1,"y":2}:3,{"x":4,"y":5}:6})");
        W_TEST(R"({{"x":1,"y":2}:3,{"x":4,"y":5}:6})", xmap{{{1, 2}, 3}, {{4, 5}, 6}});
    }
TEST_END()

TEST_BEG(unquoted_keys_input_iterator_keys, cxon::JSON<cxon::test::unquoted_keys_traits<cxon::test::input_iterator_traits<>>>, "/std")
    using namespace std;

    R_TEST(map<string, int>{{"1", 2}, {"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<string, int>{{"1", 2}, {"3", 4}});
    R_TEST(map<wstring, int>{{L"1", 2}, {L"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<wstring, int>{{L"1", 2}, {L"3", 4}});
    R_TEST(map<u16string, int>{{u"1", 2}, {u"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u16string, int>{{u"1", 2}, {u"3", 4}});
    R_TEST(map<u32string, int>{{U"1", 2}, {U"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u32string, int>{{U"1", 2}, {U"3", 4}});
    W_TEST(R"({"1":2,"3":4})", map<char, int>{{'1', 2}, {'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<wchar_t, int>{{L'1', 2}, {L'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char16_t, int>{{u'1', 2}, {u'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char32_t, int>{{U'1', 2}, {U'3', 4}});
    W_TEST(R"({1:2,3:4})", map<signed char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<int, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<float, int>{{1.f, 2}, {3.f, 4}});
    W_TEST(R"({1:2,3:4})", map<double, int>{{1., 2}, {3., 4}});
    W_TEST(R"({1:2,3:4})", map<long double, int>{{1., 2}, {3., 4}});
    {   struct less { constexpr bool operator ()(nullptr_t, nullptr_t) const noexcept { return true; } };
        W_TEST(R"({null:0})", map<nullptr_t, int, less>{{nullptr, 0}});
    }
    // escape quotes
    {   using xmap = map<map<char, int>, int>;
        R_TEST(xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}});
    }
    {   using xmap = map<map<wchar_t, int>, int>;
        R_TEST(xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}});
    }
    {   using xmap = map<map<char16_t, int>, int>;
        R_TEST(xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}});
    }
    {   using xmap = map<map<char32_t, int>, int>;
        R_TEST(xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}});
    }
    {   using xmap = map<map<string, int>, int>;
        R_TEST(xmap{ {{{"x\ty\"z", 1}}, 3} }, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<wstring, int>, int>;
        R_TEST(xmap{{{{L"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{L"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<u16string, int>, int>;
        R_TEST(xmap{{{{u"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{u"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<u32string, int>, int>;
        R_TEST(xmap{{{{U"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{U"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<map<string, int>, int>, int>;
        R_TEST(xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<wstring, int>, int>, int>;
        R_TEST(xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u16string, int>, int>, int>;
        R_TEST(xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u32string, int>, int>, int>;
        R_TEST(xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}});
    }
    // inf/nan
    {
        constexpr auto ninf = -std::numeric_limits<double>::infinity();
        constexpr auto pinf =  std::numeric_limits<double>::infinity();
        //constexpr auto qnan =  std::numeric_limits<double>::quiet_NaN();
        {   using xmap = map<double, int>;
            R_TEST(xmap{{ninf, 1}, {pinf, 2}}, R"({"-inf":1,"inf":2})");
            W_TEST(R"({"-inf":1,"inf":2})", xmap{{ninf, 1}, {pinf, 2}});
        }
        {   using xmap = map<map<double, int>, int>;
            R_TEST(xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}}, R"({{"-inf":1}:2,{"inf":3}:4})");
            W_TEST(R"({{"-inf":1}:2,{"inf":3}:4})", xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}});
        }
    }
    // std::bitset
    {   using xmap = map<bitset<8>, int>;
        R_TEST(xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}}, R"({"00000000":1,"11111111":2})");
        W_TEST(R"({"00000000":1,"11111111":2})", xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}});
    }
    {   using xmap = map<map<bitset<8>, int>, int>;
        R_TEST(xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}}, R"({{"00000000":1}:2,{"11111111":3}:4})");
        W_TEST(R"({{"00000000":1}:2,{"11111111":3}:4})", xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}});
    }
    // enum
    {   using xmap = map<keys_enum, int>;
        R_TEST(xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}}, R"({"E1":1,"E2":2})");
        W_TEST(R"({"E1":1,"E2":2})", xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}});
    }
    {   using xmap = map<map<keys_enum, int>, int>;
        R_TEST(xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}}, R"({{"E1":1}:2,{"E2":3}:4})");
        W_TEST(R"({{"E1":1}:2,{"E2":3}:4})", xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}});
    }
    // struct
    {   using xmap = map<key_struct, int>;
        R_TEST(xmap{{{1, 2}, 3}, {{4, 5}, 6}}, R"({{"x":1,"y":2}:3,{"x":4,"y":5}:6})");
        W_TEST(R"({{"x":1,"y":2}:3,{"x":4,"y":5}:6})", xmap{{{1, 2}, 3}, {{4, 5}, 6}});
    }
TEST_END()


namespace {
    struct struct_1 {
        std::string a;
        bool operator ==(const struct_1& t) const { return a == t.a; }
        bool operator  <(const struct_1& t) const { return a  < t.a; }
    };
    enum class enum_1 { aaa, bbb, ccc, ddd };
}
CXON_JSON_CLS(struct_1,
    CXON_JSON_CLS_FIELD_ASIS(a)
)
CXON_JSON_ENM(enum_1,
    CXON_JSON_ENM_VALUE_ASIS(aaa),
    CXON_JSON_ENM_VALUE_NAME("b\"b", bbb),
    CXON_JSON_ENM_VALUE_NAME("c c", ccc),
    CXON_JSON_ENM_VALUE_NAME("d:d", ddd)
)

TEST_BEG(unquote_quoted_keys, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<>>, "/std")
    using namespace std;
    {   using xmap = map<string, string>;
        R_TEST(xmap {{"x", "y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{"x", "y"}});
        R_TEST(xmap {{"", "y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{"", "y"}});
        R_TEST(xmap {{"x\"y", "z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{"x\"y", "z"}});
        R_TEST(xmap {{"x:y", "z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{"x:y", "z"}});
        R_TEST(xmap {{"x:y", "z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{"x y", "z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{"x y", "z"}});
        R_TEST(xmap {{"x y", "z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<wstring, wstring>;
        R_TEST(xmap {{L"x", L"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{L"x", L"y"}});
        R_TEST(xmap {{L"", L"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{L"", L"y"}});
        R_TEST(xmap {{L"x\"y", L"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{L"x\"y", L"z"}});
        R_TEST(xmap {{L"x:y", L"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{L"x:y", L"z"}});
        R_TEST(xmap {{L"x:y", L"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{L"x y", L"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{L"x y", L"z"}});
        R_TEST(xmap {{L"x y", L"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<u16string, u16string>;
        R_TEST(xmap {{u"x", u"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{u"x", u"y"}});
        R_TEST(xmap {{u"", u"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{u"", u"y"}});
        R_TEST(xmap {{u"x\"y", u"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{u"x\"y", u"z"}});
        R_TEST(xmap {{u"x:y", u"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{u"x:y", u"z"}});
        R_TEST(xmap {{u"x:y", u"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{u"x y", u"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{u"x y", u"z"}});
        R_TEST(xmap {{u"x y", u"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<u32string, u32string>;
        R_TEST(xmap {{U"x", U"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{U"x", U"y"}});
        R_TEST(xmap {{U"", U"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{U"", U"y"}});
        R_TEST(xmap {{U"x\"y", U"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{U"x\"y", U"z"}});
        R_TEST(xmap {{U"x:y", U"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{U"x:y", U"z"}});
        R_TEST(xmap {{U"x:y", U"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{U"x y", U"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{U"x y", U"z"}});
        R_TEST(xmap {{U"x y", U"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<map<string, string>, string>;
        R_TEST(xmap {{{{"x", "y"}}, "z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{"x", "y"}}, "z"}});
        R_TEST(xmap {{{{"", "y"}}, "z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{"", "y"}}, "z"}});
        R_TEST(xmap {{{{"a\"b", "c"}}, "d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{"a\"b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a:b", "c"}}, "d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{"a:b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a:b", "c"}}, "d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{"a b", "c"}}, "d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{"a b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a b", "c"}}, "d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<wstring, wstring>, wstring>;
        R_TEST(xmap {{{{L"x", L"y"}}, L"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{L"x", L"y"}}, L"z"}});
        R_TEST(xmap {{{{L"", L"y"}}, L"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{L"", L"y"}}, L"z"}});
        R_TEST(xmap {{{{L"a\"b", L"c"}}, L"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{L"a\"b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a:b", L"c"}}, L"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{L"a:b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a:b", L"c"}}, L"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{L"a b", L"c"}}, L"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{L"a b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a b", L"c"}}, L"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<u16string, u16string>, u16string>;
        R_TEST(xmap {{{{u"x", u"y"}}, u"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{u"x", u"y"}}, u"z"}});
        R_TEST(xmap {{{{u"", u"y"}}, u"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{u"", u"y"}}, u"z"}});
        R_TEST(xmap {{{{u"a\"b", u"c"}}, u"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{u"a\"b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a:b", u"c"}}, u"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{u"a:b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a:b", u"c"}}, u"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{u"a b", u"c"}}, u"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{u"a b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a b", u"c"}}, u"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<u32string, u32string>, u32string>;
        R_TEST(xmap {{{{U"x", U"y"}}, U"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{U"x", U"y"}}, U"z"}});
        R_TEST(xmap {{{{U"", U"y"}}, U"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{U"", U"y"}}, U"z"}});
        R_TEST(xmap {{{{U"a\"b", U"c"}}, U"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{U"a\"b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a:b", U"c"}}, U"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{U"a:b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a:b", U"c"}}, U"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{U"a b", U"c"}}, U"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{U"a b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a b", U"c"}}, U"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<vector<string>, string>;
        R_TEST(xmap {{{"x", "y"}, "z"}}, R"({["x","y"]:"z"})");
        W_TEST(R"({["x","y"]:"z"})", xmap {{{"x", "y"}, "z"}});
    }
    // char
    {   using xmap = map<char, char>;
        R_TEST(xmap {{'x', 'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{'x', 'y'}});
        R_TEST(xmap {{'"', 'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{'"', 'x'}});
        R_TEST(xmap {{':', 'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{':', 'x'}});
        //R_TEST(xmap {{':', 'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{' ', 'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{' ', 'x'}});
        R_TEST(xmap {{' ', 'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<wchar_t, wchar_t>;
        R_TEST(xmap {{L'x', L'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{L'x', L'y'}});
        R_TEST(xmap {{L'"', L'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{L'"', L'x'}});
        R_TEST(xmap {{L':', L'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{L':', L'x'}});
        //R_TEST(xmap {{L':', L'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{L' ', L'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{L' ', L'x'}});
        R_TEST(xmap {{L' ', L'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<char16_t, char16_t>;
        R_TEST(xmap {{u'x', u'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{u'x', u'y'}});
        R_TEST(xmap {{u'"', u'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{u'"', u'x'}});
        R_TEST(xmap {{u':', u'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{u':', u'x'}});
        //R_TEST(xmap {{u':', u'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{u' ', u'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{u' ', u'x'}});
        R_TEST(xmap {{u' ', u'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<char32_t, char32_t>;
        R_TEST(xmap {{U'x', U'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{U'x', U'y'}});
        R_TEST(xmap {{U'"', U'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{U'"', U'x'}});
        R_TEST(xmap {{U':', U'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{U':', U'x'}});
        //R_TEST(xmap {{U':', U'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{U' ', U'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{U' ', U'x'}});
        R_TEST(xmap {{U' ', U'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    // enum
    {   using xmap = map<enum_1, int>;
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c\ c:3,d\:d:4})");
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b\"b:2,c\ c:3,d\:d:4})");
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c c:3,d\:d:4})", json::read_error::unexpected, 13);
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c\ c:3,d:d:4})", json::read_error::unexpected, 20);
        W_TEST(R"({aaa:1,b"b:2,c\ c:3,d\:d:4})", xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}});
    }
    {   using xmap = map<map<enum_1, int>, int>;
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})");
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b\"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})");
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c c:5}:6,{d\:d:7}:8})", json::read_error::unexpected, 22);
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d:d:7}:8})", json::read_error::unexpected, 33);
        W_TEST(R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})", xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}});
    }
    // struct
    {   using xmap = map<struct_1, string>;
        R_TEST(xmap {{{"b"}, "c"}}, R"({{a:"b"}:"c"})");
        W_TEST(R"({{a:"b"}:"c"})", xmap {{{"b"}, "c"}});
    }
    {   using xmap = map<map<struct_1, string>, string>;
        R_TEST(xmap {{{{{"b"}, "c"}}, "d"}}, R"({{{a:"b"}:"c"}:"d"})");
        W_TEST(R"({{{a:"b"}:"c"}:"d"})", xmap {{{{{"b"}, "c"}}, "d"}});
    }
    // std::variant
#   ifdef CXON_HAS_LIB_STD_VARIANT
    {   using xmap = map<variant<string, int>, string>;
        R_TEST(xmap {{{"1"}, "2"}}, R"({{0:"1"}:"2"})");
        W_TEST(R"({{0:"1"}:"2"})", xmap {{{"1"}, "2"}});
    }
#   endif
    // std::bitset
    {   using xmap = map<bitset<8>, bitset<8>>;
        R_TEST(xmap {{bitset<8>(85), bitset<8>(170)}}, R"({01010101:"10101010"})");
        W_TEST(R"({01010101:"10101010"})", xmap {{bitset<8>(85), bitset<8>(170)}});
    }
TEST_END()

TEST_BEG(unquote_quoted_keys_input_iterator, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<cxon::test::input_iterator_traits<>>>, "/std")
    using namespace std;
    {   using xmap = map<string, string>;
        R_TEST(xmap {{"x", "y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{"x", "y"}});
        R_TEST(xmap {{"", "y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{"", "y"}});
        R_TEST(xmap {{"x\"y", "z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{"x\"y", "z"}});
        R_TEST(xmap {{"x:y", "z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{"x:y", "z"}});
        R_TEST(xmap {{"x:y", "z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{"x y", "z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{"x y", "z"}});
        R_TEST(xmap {{"x y", "z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<wstring, wstring>;
        R_TEST(xmap {{L"x", L"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{L"x", L"y"}});
        R_TEST(xmap {{L"", L"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{L"", L"y"}});
        R_TEST(xmap {{L"x\"y", L"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{L"x\"y", L"z"}});
        R_TEST(xmap {{L"x:y", L"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{L"x:y", L"z"}});
        R_TEST(xmap {{L"x:y", L"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{L"x y", L"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{L"x y", L"z"}});
        R_TEST(xmap {{L"x y", L"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<u16string, u16string>;
        R_TEST(xmap {{u"x", u"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{u"x", u"y"}});
        R_TEST(xmap {{u"", u"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{u"", u"y"}});
        R_TEST(xmap {{u"x\"y", u"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{u"x\"y", u"z"}});
        R_TEST(xmap {{u"x:y", u"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{u"x:y", u"z"}});
        R_TEST(xmap {{u"x:y", u"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{u"x y", u"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{u"x y", u"z"}});
        R_TEST(xmap {{u"x y", u"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<u32string, u32string>;
        R_TEST(xmap {{U"x", U"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{U"x", U"y"}});
        R_TEST(xmap {{U"", U"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{U"", U"y"}});
        R_TEST(xmap {{U"x\"y", U"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{U"x\"y", U"z"}});
        R_TEST(xmap {{U"x:y", U"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{U"x:y", U"z"}});
        R_TEST(xmap {{U"x:y", U"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{U"x y", U"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{U"x y", U"z"}});
        R_TEST(xmap {{U"x y", U"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<map<string, string>, string>;
        R_TEST(xmap {{{{"x", "y"}}, "z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{"x", "y"}}, "z"}});
        R_TEST(xmap {{{{"", "y"}}, "z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{"", "y"}}, "z"}});
        R_TEST(xmap {{{{"a\"b", "c"}}, "d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{"a\"b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a:b", "c"}}, "d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{"a:b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a:b", "c"}}, "d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{"a b", "c"}}, "d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{"a b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a b", "c"}}, "d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<wstring, wstring>, wstring>;
        R_TEST(xmap {{{{L"x", L"y"}}, L"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{L"x", L"y"}}, L"z"}});
        R_TEST(xmap {{{{L"", L"y"}}, L"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{L"", L"y"}}, L"z"}});
        R_TEST(xmap {{{{L"a\"b", L"c"}}, L"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{L"a\"b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a:b", L"c"}}, L"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{L"a:b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a:b", L"c"}}, L"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{L"a b", L"c"}}, L"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{L"a b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a b", L"c"}}, L"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<u16string, u16string>, u16string>;
        R_TEST(xmap {{{{u"x", u"y"}}, u"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{u"x", u"y"}}, u"z"}});
        R_TEST(xmap {{{{u"", u"y"}}, u"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{u"", u"y"}}, u"z"}});
        R_TEST(xmap {{{{u"a\"b", u"c"}}, u"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{u"a\"b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a:b", u"c"}}, u"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{u"a:b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a:b", u"c"}}, u"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{u"a b", u"c"}}, u"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{u"a b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a b", u"c"}}, u"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<u32string, u32string>, u32string>;
        R_TEST(xmap {{{{U"x", U"y"}}, U"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{U"x", U"y"}}, U"z"}});
        R_TEST(xmap {{{{U"", U"y"}}, U"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{U"", U"y"}}, U"z"}});
        R_TEST(xmap {{{{U"a\"b", U"c"}}, U"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{U"a\"b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a:b", U"c"}}, U"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{U"a:b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a:b", U"c"}}, U"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{U"a b", U"c"}}, U"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{U"a b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a b", U"c"}}, U"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<vector<string>, string>;
        R_TEST(xmap {{{"x", "y"}, "z"}}, R"({["x","y"]:"z"})");
        W_TEST(R"({["x","y"]:"z"})", xmap {{{"x", "y"}, "z"}});
    }
    // char
    {   using xmap = map<char, char>;
        R_TEST(xmap {{'x', 'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{'x', 'y'}});
        R_TEST(xmap {{'"', 'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{'"', 'x'}});
        R_TEST(xmap {{':', 'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{':', 'x'}});
        //R_TEST(xmap {{':', 'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{' ', 'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{' ', 'x'}});
        R_TEST(xmap {{' ', 'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<wchar_t, wchar_t>;
        R_TEST(xmap {{L'x', L'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{L'x', L'y'}});
        R_TEST(xmap {{L'"', L'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{L'"', L'x'}});
        R_TEST(xmap {{L':', L'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{L':', L'x'}});
        //R_TEST(xmap {{L':', L'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{L' ', L'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{L' ', L'x'}});
        R_TEST(xmap {{L' ', L'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<char16_t, char16_t>;
        R_TEST(xmap {{u'x', u'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{u'x', u'y'}});
        R_TEST(xmap {{u'"', u'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{u'"', u'x'}});
        R_TEST(xmap {{u':', u'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{u':', u'x'}});
        //R_TEST(xmap {{u':', u'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{u' ', u'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{u' ', u'x'}});
        R_TEST(xmap {{u' ', u'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<char32_t, char32_t>;
        R_TEST(xmap {{U'x', U'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{U'x', U'y'}});
        R_TEST(xmap {{U'"', U'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{U'"', U'x'}});
        R_TEST(xmap {{U':', U'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{U':', U'x'}});
        //R_TEST(xmap {{U':', U'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{U' ', U'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{U' ', U'x'}});
        R_TEST(xmap {{U' ', U'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    // enum
    {   using xmap = map<enum_1, int>;
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c\ c:3,d\:d:4})");
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b\"b:2,c\ c:3,d\:d:4})");
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c c:3,d\:d:4})", json::read_error::unexpected, 14);
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c\ c:3,d:d:4})", json::read_error::unexpected, 21);
        W_TEST(R"({aaa:1,b"b:2,c\ c:3,d\:d:4})", xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}});
    }
    {   using xmap = map<map<enum_1, int>, int>;
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})");
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b\"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})");
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c c:5}:6,{d\:d:7}:8})", json::read_error::unexpected, 23);
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d:d:7}:8})", json::read_error::unexpected, 34);
        W_TEST(R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})", xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}});
    }
    // struct
    {   using xmap = map<struct_1, string>;
        R_TEST(xmap {{{"b"}, "c"}}, R"({{a:"b"}:"c"})");
        W_TEST(R"({{a:"b"}:"c"})", xmap {{{"b"}, "c"}});
    }
    {   using xmap = map<map<struct_1, string>, string>;
        R_TEST(xmap {{{{{"b"}, "c"}}, "d"}}, R"({{{a:"b"}:"c"}:"d"})");
        W_TEST(R"({{{a:"b"}:"c"}:"d"})", xmap {{{{{"b"}, "c"}}, "d"}});
    }
    // std::variant
#   ifdef CXON_HAS_LIB_STD_VARIANT
    {   using xmap = map<variant<string, int>, string>;
        R_TEST(xmap {{{"1"}, "2"}}, R"({{0:"1"}:"2"})");
        W_TEST(R"({{0:"1"}:"2"})", xmap {{{"1"}, "2"}});
    }
#   endif
    // std::bitset
    {   using xmap = map<bitset<8>, bitset<8>>;
        R_TEST(xmap {{bitset<8>(85), bitset<8>(170)}}, R"({01010101:"10101010"})");
        W_TEST(R"({01010101:"10101010"})", xmap {{bitset<8>(85), bitset<8>(170)}});
    }
TEST_END()
