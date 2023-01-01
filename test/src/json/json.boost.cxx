// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "test.hxx"

#include "cxon/lib/boost/container/stable_vector.hxx"
#include "cxon/lib/boost/container/small_vector.hxx"
#include "cxon/lib/boost/container/static_vector.hxx"
#include "cxon/lib/boost/container/deque.hxx"
#include "cxon/lib/boost/container/list.hxx"
#include "cxon/lib/boost/container/slist.hxx"
#include "cxon/lib/boost/container/string.hxx"
#include "cxon/lib/boost/container/map.hxx"
#include "cxon/lib/boost/container/set.hxx"
#include "cxon/lib/boost/container/flat_map.hxx"
#include "cxon/lib/boost/container/flat_set.hxx"
#if defined(__GNUC__) && !defined(__clang__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#   include "cxon/lib/boost/dynamic_bitset.hxx"
#   pragma GCC diagnostic pop
#else
#   include "cxon/lib/boost/dynamic_bitset.hxx"
#endif
#include "cxon/lib/boost/variant2.hxx"
#include "cxon/lib/boost/variant.hxx"

#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/map.hxx"

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(stable_vector, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::stable_vector<int>
        R_TEST((stable_vector<int>{}), "[]");
        W_TEST("[]", (stable_vector<int>{}));
        R_TEST((stable_vector<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (stable_vector<int>({1, 2, 3})));
TEST_END()


TEST_BEG(small_vector, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::small_vector<int>
        R_TEST((small_vector<int, 8>{}), "[]");
        W_TEST("[]", (small_vector<int, 8>{}));
        R_TEST((small_vector<int, 2>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (small_vector<int, 2>({1, 2, 3})));
TEST_END()


TEST_BEG(static_vector, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::static_vector<int, 0>
        R_TEST((static_vector<int, 0>{}), "[]");
        W_TEST("[]", (static_vector<int, 0>{}));
        R_TEST((static_vector<int, 0>{}), "", json::read_error::unexpected, 0);
        R_TEST((static_vector<int, 0>{}), "]", json::read_error::unexpected, 0);
        R_TEST((static_vector<int, 0>{}), "[", json::read_error::unexpected, 1);
    // boost::container::static_vector<int, 3>
        R_TEST((static_vector<int, 3>{{1, 2, 3}}), "[1, 2, 3]");
        W_TEST("[1,2,3]", (static_vector<int, 3>{{1, 2, 3}}));
        R_TEST((static_vector<int, 4>{{1, 2, 3}}), "[1, 2, 3]");
        W_TEST("[1,2,3,4]", (static_vector<int, 4>{{1, 2, 3, 4}}));
        R_TEST((static_vector<int, 2>{}), "[1, 2, 3]", json::read_error::overflow, 0);
        R_TEST((static_vector<int, 2>{}), "", json::read_error::unexpected, 0);
        R_TEST((static_vector<int, 2>{}), "[", json::read_error::integral_invalid, 1);
        R_TEST((static_vector<int, 2>{}), "[1x", json::read_error::unexpected, 2);
        R_TEST((static_vector<int, 2>{}), "[1,x", json::read_error::integral_invalid, 3);
        R_TEST((static_vector<int, 2>{}), "[1,1", json::read_error::unexpected, 4);
TEST_END()


TEST_BEG(deque, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::deque<int>
        R_TEST((deque<int>{}), "[]");
        W_TEST("[]", (deque<int>{}));
        R_TEST((deque<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (deque<int>({1, 2, 3})));
TEST_END()


TEST_BEG(list, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::list<int>
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


TEST_BEG(slist, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::slist<int>
        R_TEST((slist<int>{}), "[]");
        W_TEST("[]", (slist<int>{}));
        R_TEST((slist<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (slist<int>({1, 2, 3})));
        R_TEST((slist<int>{}), "", json::read_error::unexpected, 0);
        R_TEST((slist<int>{}), "]", json::read_error::unexpected, 0);
        R_TEST((slist<int>{}), "{", json::read_error::unexpected, 0);
        R_TEST((slist<int>{}), "[", json::read_error::integral_invalid, 1);
        W_TEST("[1,2,3]", (slist<float>({1, 2, 3})));
        R_TEST((slist<float>{}), "[", json::read_error::floating_point_invalid, 1);
TEST_END()


TEST_BEG(basic_string, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::basic_string<char> / boost::container::string
        R_TEST(string("test"), QS("test"));
        W_TEST(QS("test"), string("test"));
        R_TEST(string("te\nst"), QS("te\\nst"));
        W_TEST(QS("te\\nst"), string("te\nst"));
        R_TEST(string(), "a", json::read_error::unexpected, 0);
        R_TEST(string(), "\"", json::read_error::unexpected, 1);
        R_TEST(string(), QS("\\u001"), json::read_error::escape_invalid, 1);
    // boost::container::basic_string<wchar_t> / boost::container::wstring
        R_TEST(wstring({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), wstring({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(wstring({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), wstring({0x6D4B, 0x8BD5}));
        R_TEST(wstring(), "a", json::read_error::unexpected, 0);
        R_TEST(wstring(), "\"", json::read_error::unexpected, 1);
        R_TEST(wstring(), QS("\\u001"), json::read_error::escape_invalid, 1);
    // boost::container::basic_string<char8_t>
#       if __cplusplus > 201703L /* C++20 */
            using u8string = basic_string<char8_t, std::char_traits<char8_t>, new_allocator<char8_t>>;
            R_TEST(u8string(u8"test"), QS("test"));
            W_TEST(QS("test"), u8string(u8"test"));
            R_TEST(u8string(u8"te\nst"), QS("te\\nst"));
            W_TEST(QS("te\\nst"), u8string(u8"te\nst"));
            R_TEST(u8string(), "a", json::read_error::unexpected, 0);
            R_TEST(u8string(), "\"", json::read_error::unexpected, 1);
            R_TEST(u8string(), QS("\\u001"), json::read_error::escape_invalid, 1);
#       endif
    // boost::container::basic_string<char16_t>
        using u16string = basic_string<char16_t, std::char_traits<char16_t>, new_allocator<char16_t>>;
        R_TEST(u16string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), u16string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(u16string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), u16string({0x6D4B, 0x8BD5}));
        R_TEST(u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}));
        R_TEST(u16string(u"\xdbff\xdfff"), QS("\\udbff\\udfff")); // surrogate
        W_TEST(QS("\xf4\x8f\xbf\xbf"), u16string(u"\xdbff\xdfff")); // surrogate
        R_TEST(u16string(), "a", json::read_error::unexpected, 0);
        R_TEST(u16string(), "\"", json::read_error::unexpected, 1);
        R_TEST(u16string(), QS("\\u001"), json::read_error::escape_invalid, 1);
    // boost::container::basic_string<char32_t>
        using u32string = basic_string<char32_t, std::char_traits<char32_t>, new_allocator<char32_t>>;
        R_TEST(u32string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), u32string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(u32string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), u32string({0x6D4B, 0x8BD5}));
        R_TEST(u32string({0x0001241D, 0x0001241C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), u32string({0x0001241D, 0x0001241C}));
        R_TEST(u32string(), "a", json::read_error::unexpected, 0);
        R_TEST(u32string(), "\"", json::read_error::unexpected, 1);
        R_TEST(u32string(), QS("\\u001"), json::read_error::escape_invalid, 1);
TEST_END()


TEST_BEG(map, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::map
        R_TEST(map<string, int>{{"1", 2}, {"3", 4}}, R"({"1": 2, "3": 4})");
        W_TEST(R"({"1":2,"3":4})", map<int, int>{{1, 2}, {3, 4}});
        R_TEST(map<string, int>{{R"({"1":2})", 3}, {R"({"4":5})", 6}}, R"({"{\"1\":2}": 3, "{\"4\":5}": 6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", map<map<string, int>, int>{{map<string, int>{{"1", 2}}, 3}, {map<string, int>{{"4", 5}}, 6}});
    // boost::container::multimap
        R_TEST(multimap<string, int>{{"1", 2}, {"1", 3}}, R"({"1": 2, "1": 3})");
        W_TEST(R"({"1":2,"1":3})", multimap<int, int>{{1, 2}, {1, 3}});
TEST_END()

TEST_BEG(map_unquoted_keys, cxon::JSON<cxon::test::unquoted_keys_traits<>>, "/boost")
    using namespace boost::container;
    // std::map
        R_TEST(map<int, string>{{1, "2"}, {3, "4"}}, R"({1: "2", 3: "4"})");
        W_TEST(R"({1:2,3:4})", map<int, int>{{1, 2}, {3, 4}});
    // std::multimap
        R_TEST(multimap<int, string>{{1, "2"}, {1, "3"}}, R"({1: "2", 1: "3"})");
        W_TEST(R"({1:2,1:3})", multimap<int, int>{{1, 2}, {1, 3}});
TEST_END()


TEST_BEG(set, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::set<int>
        R_TEST((set<int>{}), "[]");
        W_TEST("[]", (set<int>{}));
        R_TEST((set<int>({1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1,2,3]", (set<int>({1, 2, 3})));
    // boost::container::multiset<int>;
        R_TEST((multiset<int>{}), "[]");
        W_TEST("[]", (multiset<int>{}));
        R_TEST((multiset<int>({1, 1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1,1,2,3]", (multiset<int>({1, 1, 2, 3})));
TEST_END()


TEST_BEG(flat_map, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::flat_map
        R_TEST(flat_map<string, int>{{"1", 2}, {"3", 4}}, R"({"1": 2, "3": 4})");
        W_TEST(R"({"1":2,"3":4})", flat_map<int, int>{{1, 2}, {3, 4}});
        R_TEST(flat_map<string, int>{{R"({"1":2})", 3}, {R"({"4":5})", 6}}, R"({"{\"1\":2}": 3, "{\"4\":5}": 6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", flat_map<flat_map<string, int>, int>{{flat_map<string, int>{{"1", 2}}, 3}, {flat_map<string, int>{{"4", 5}}, 6}});
    // boost::container::flat_multimap
        R_TEST(flat_multimap<string, int>{{"1", 2}, {"1", 3}}, R"({"1": 2, "1": 3})");
        W_TEST(R"({"1":2,"1":3})", flat_multimap<int, int>{{1, 2}, {1, 3}});
TEST_END()

TEST_BEG(flat_map_unquoted_keys, cxon::JSON<cxon::test::unquoted_keys_traits<>>, "/boost")
    using namespace boost::container;
    // std::map
        R_TEST(flat_map<int, string>{{1, "2"}, {3, "4"}}, R"({1: "2", 3: "4"})");
        W_TEST(R"({1:2,3:4})", flat_map<int, int>{{1, 2}, {3, 4}});
    // std::multimap
        R_TEST(flat_multimap<int, string>{{1, "2"}, {1, "3"}}, R"({1: "2", 1: "3"})");
        W_TEST(R"({1:2,1:3})", flat_multimap<int, int>{{1, 2}, {1, 3}});
TEST_END()


TEST_BEG(flat_set, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::flat_set<int>
        R_TEST((flat_set<int>{}), "[]");
        W_TEST("[]", (flat_set<int>{}));
        R_TEST((flat_set<int>({1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1,2,3]", (flat_set<int>({1, 2, 3})));
    // boost::container::flat_multiset<int>;
        R_TEST((flat_multiset<int>{}), "[]");
        W_TEST("[]", (flat_multiset<int>{}));
        R_TEST((flat_multiset<int>({1, 1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1,1,2,3]", (flat_multiset<int>({1, 1, 2, 3})));
TEST_END()


TEST_BEG(dynamic_bitset, cxon::JSON<>, "/boost")
    using namespace boost;
    using bitset = dynamic_bitset<unsigned char>;
    // boost::dynamic_bitset
        R_TEST(bitset(1, 0x1), QS("1"));
        W_TEST(QS("1"), bitset(1, 0x1));
        R_TEST(bitset(2, 0x1), QS("01"));
        W_TEST(QS("01"), bitset(2, 0x1));
        R_TEST(bitset(3, 0x5), QS("101"));
        W_TEST(QS("101"), bitset(3, 0x5));
        R_TEST(bitset(4, 0x5), QS("0101"));
        W_TEST(QS("0101"), bitset(4, 0x5));
        R_TEST(bitset(5, 0x15), QS("10101"));
        W_TEST(QS("10101"), bitset(5, 0x15));
        R_TEST(bitset(6, 0x15), QS("010101"));
        W_TEST(QS("010101"), bitset(6, 0x15));
        R_TEST(bitset(7, 0x55), QS("1010101"));
        W_TEST(QS("1010101"), bitset(7, 0x55));
        R_TEST(bitset(8, 0x55), QS("01010101"));
        W_TEST(QS("01010101"), bitset(8, 0x55));
        R_TEST(bitset(9, 0x155), QS("101010101"));
        W_TEST(QS("101010101"), bitset(9, 0x155));
        R_TEST(bitset(10, 0x155), QS("0101010101"));
        W_TEST(QS("0101010101"), bitset(10, 0x155));
        R_TEST(bitset(11, 0x555), QS("10101010101"));
        W_TEST(QS("10101010101"), bitset(11, 0x555));
        R_TEST(bitset(12, 0x555), QS("010101010101"));
        W_TEST(QS("010101010101"), bitset(12, 0x555));
        R_TEST(bitset(13, 0x1555), QS("1010101010101"));
        W_TEST(QS("1010101010101"), bitset(13, 0x1555));
        R_TEST(bitset(14, 0x1555), QS("01010101010101"));
        W_TEST(QS("01010101010101"), bitset(14, 0x1555));
        R_TEST(bitset(15, 0x5555), QS("101010101010101"));
        W_TEST(QS("101010101010101"), bitset(15, 0x5555));
        R_TEST(bitset(16, 0x5555), QS("0101010101010101"));
        W_TEST(QS("0101010101010101"), bitset(16, 0x5555));
        // keys
        R_TEST(std::map<bitset, int>{{bitset(8, 0x55), 1}}, R"({"01010101":1})");
        W_TEST(R"({"01010101":1})", std::map<bitset, int>{{bitset(8, 0x55), 1}});
        // errors
        R_TEST(bitset(1, 0x1), "1", cxon::json::read_error::unexpected, 0);
        R_TEST(bitset(1, 0x1), "\"1", cxon::json::read_error::unexpected, 2);
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<XXON>(c, bitset(1, 0x1));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, bitset(1, 0x1));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, bitset(1, 0x1));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
TEST_END()


#ifdef CXON_HAS_BOOST_VARIANT2
    TEST_BEG(variant2, cxon::JSON<>, "/boost")
        using namespace boost::variant2;
        // boost::variant2::variant
            R_TEST(variant<int, double>(in_place_index_t<0>(), 1), R"({"0":1})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({"1":0})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({"2":0})", json::read_error::unexpected, 1);
            W_TEST(R"({"0":1})", variant<int, double>(1));
            W_TEST(R"({"1":0})", variant<int, double>(in_place_index_t<1>(), 0));
            R_TEST(variant<monostate, int>(), R"({"0":null})");
            R_TEST(variant<monostate, int>(), R"({"0":1})", json::read_error::unexpected, 5);
            W_TEST(R"({"0":null})", variant<monostate, int>());
        // errors
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

    TEST_BEG(variant2_unquoted_keys, cxon::JSON<cxon::test::unquoted_keys_traits<>>, "/boost")
        using namespace boost::variant2;
        // boost::variant2::variant
            R_TEST(variant<int, double>(in_place_index_t<0>(), 1), R"({0:1})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({1:0})");
            R_TEST(variant<int, double>(in_place_index_t<1>(), 0), R"({2:0})", json::read_error::unexpected, 1);
            W_TEST(R"({0:1})", variant<int, double>(1));
            W_TEST(R"({1:0})", variant<int, double>(in_place_index_t<1>(), 0));
            R_TEST(variant<monostate, int>(), R"({0:null})");
            R_TEST(variant<monostate, int>(), R"({0:1})", json::read_error::unexpected, 3);
            W_TEST(R"({0:null})", variant<monostate, int>());
    // errors
        R_TEST(variant<int, double>(0), R"()", json::read_error::unexpected, 0);
        R_TEST(variant<int, double>(0), R"({)", json::read_error::unexpected, 1);
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


namespace cxon { namespace test {
    template <typename T>
        struct match<boost::recursive_wrapper<T>> {
            static bool values(const boost::recursive_wrapper<T>& t0, const boost::recursive_wrapper<T>& t1) {
                return t0.get() == t1.get();
            }
        };
}}
TEST_BEG(variant, cxon::JSON<>, "/boost")
    using namespace boost;
    // boost::variant
        R_TEST(variant<int, double>(  1), R"({"0":1})");
        R_TEST(variant<int, double>(0.0), R"({"1":0})");
        R_TEST(variant<int, double>(  0), R"({"2":0})", cxon::json::read_error::unexpected, 1);
        W_TEST(R"({"0":1})", variant<int, double>(  1));
        W_TEST(R"({"1":0})", variant<int, double>(0.0));
        R_TEST(variant<int, double>(), R"({"0":0})");
        W_TEST(R"({"0":0})", variant<int, double>());
        R_TEST(recursive_wrapper<int>(42), R"(42)");
        W_TEST(R"(42)", recursive_wrapper<int>(42));
    // errors
        R_TEST(variant<int, double>(0), R"()", cxon::json::read_error::unexpected, 0);
        R_TEST(variant<int, double>(0), R"({)", cxon::json::read_error::unexpected, 1);
        R_TEST(variant<int, double>(0), R"({")", cxon::json::read_error::unexpected, 1);
        R_TEST(variant<int, double>(0), R"({"x)", cxon::json::read_error::unexpected, 1);
        R_TEST(variant<int, double>(0), R"({"0)", cxon::json::read_error::unexpected, 1);
        R_TEST(variant<int, double>(0), R"({"0")", cxon::json::read_error::unexpected, 1);
        R_TEST(variant<int, double>(0), R"({"0":)", cxon::json::read_error::integral_invalid, 5);
        R_TEST(variant<int, double>(0), R"({"0":x)", cxon::json::read_error::integral_invalid, 5);
        R_TEST(variant<int, double>(0), R"({"0":0)", cxon::json::read_error::unexpected, 6);
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[5];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[6];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
TEST_END()

TEST_BEG(variant_unquoted_keys, cxon::JSON<cxon::test::unquoted_keys_traits<>>, "/boost")
    using namespace boost;
    boost::variant<int, boost::variant<int, double>> x;
    // boost::variant
        R_TEST(variant<int, double>(  1), R"({0:1})");
        R_TEST(variant<int, double>(0.0), R"({1:0})");
        R_TEST(variant<int, double>(  0), R"({2:0})", cxon::json::read_error::unexpected, 1);
        W_TEST(R"({0:1})", variant<int, double>(  1));
        W_TEST(R"({1:0})", variant<int, double>(0.0));
        R_TEST(variant<int, double>(), R"({0:0})");
        W_TEST(R"({0:0})", variant<int, double>());
        R_TEST(recursive_wrapper<int>(42), R"(42)");
        W_TEST(R"(42)", recursive_wrapper<int>(42));
    // errors
        R_TEST(variant<int, double>(0), R"()", cxon::json::read_error::unexpected, 0);
        R_TEST(variant<int, double>(0), R"({)", cxon::json::read_error::unexpected, 1);
        R_TEST(variant<int, double>(0), R"({0)", cxon::json::read_error::unexpected, 1);
        R_TEST(variant<int, double>(0), R"({0:)", cxon::json::read_error::integral_invalid, 3);
        R_TEST(variant<int, double>(0), R"({0:x)", cxon::json::read_error::integral_invalid, 3);
        R_TEST(variant<int, double>(0), R"({0:0)", cxon::json::read_error::unexpected, 4);
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[3];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, variant<int, double>(0));
            TEST_CHECK(r.ec == cxon::json::write_error::output_failure);
        }
TEST_END()


TEST_BEG(unquote_quoted_keys, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<>>, "/std")
    using namespace std;
    // boost::variant2
#   ifdef CXON_HAS_BOOST_VARIANT2
    {   using variant = boost::variant2::variant<string, int>;
        using xmap = map<variant, string>;
        R_TEST(xmap {{variant("1"), "2"}}, R"({{0:"1"}:"2"})");
        W_TEST(R"({{0:"1"}:"2"})", xmap {{variant("1"), "2"}});
    }
#   endif
    // boost::variant
    // error: alc::create_using_allocator_of matches irrelevant constructor
    //{   using variant = boost::variant<string, int>;
    //    using xmap = map<variant, string>;
    //    R_TEST(xmap {{variant("1"), "2"}}, R"({{0:"1"}:"2"})");
    //    W_TEST(R"({{0:"1"}:"2"})", xmap {{variant("1"), "2"}});
    //}
    // boost::dynamic_bitset
    {   using bitset = boost::dynamic_bitset<unsigned char>;
        using xmap = map<bitset, bitset>;
        R_TEST(xmap {{bitset(8, 85), bitset(8, 170)}}, R"({01010101:"10101010"})");
        W_TEST(R"({01010101:"10101010"})", xmap {{bitset(8, 85), bitset(8, 170)}});
    }
TEST_END()
