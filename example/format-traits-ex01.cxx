// format traits

#include "cxon/json.hxx"
#include "cxon/lib/std/map.hxx"
#include <string>
#include <cassert>

struct unquoted_keys_traits : cxon::json::format_traits {
    // allow arbitrary keys without quotes
    static constexpr bool quote_unquoted_keys = false;
};
using UQK = cxon::JSON<unquoted_keys_traits>;

int main() {
    std::map<int, int> map;
        cxon::from_bytes<UQK>(map, R"({1: 2, 3: 4})");
    assert(map == (std::map<int, int> {{1, 2}, {3, 4}}));
    std::string str;
        cxon::to_bytes<UQK>(str, map);
    assert(str == R"({1:2,3:4})");
}
