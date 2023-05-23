// format traits, unquoted string keys and single quotes

#include "cxon/json.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/string.hxx"
#include <cassert>

// custom traits for given format
struct custom_traits : cxon::json::format_traits {
    struct string : cxon::json::format_traits {
        static constexpr char del = '\'';
    };
    // enable unquoted string keys
    static constexpr bool unquote_quoted_keys = true;
};
using UQK = cxon::JSON<custom_traits>;

int main() {
    std::map<std::string, std::string> cxx;
        cxon::from_bytes<UQK>(cxx, R"({ key: 'value' })");
    assert(cxx == (std::map<std::string, std::string> { {"key", "value"} }));
    std::string str;
        cxon::to_bytes<UQK>(str, cxx);
    assert(str == R"({key:'value'})");
}
