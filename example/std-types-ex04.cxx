// JSON values mapped to std::variant

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/variant.hxx"
#include <cassert>

int main() {
#   ifdef CXON_HAS_LIB_STD_VARIANT
    {   // JSON value types are unambiguous and so, if std::variant types are mapped
        // to distinct JSON value types, they'll be serialized correspondingly
        using uvar = std::variant<std::monostate, std::vector<int>, std::map<std::string, int>>; // std::variant<null, array, object>
        uvar var;
            cxon::from_bytes(var, R"([1, 3])");
        assert(var == (uvar(std::vector<int>{1, 3})));
            cxon::from_bytes(var, R"({"one": 1, "three": 3})");
        assert(var == (uvar(std::map<std::string, int>{{"one", 1}, {"three", 3}})));
            cxon::from_bytes(var, R"(null)");
        assert(var == (uvar()));
    }
    {   // if not, they'll be serialized as object {"<index>": <value>}
        using avar = std::variant<std::map<std::string, int>, std::map<int, std::string>>; // std::variant<object, object>
        avar var;
            cxon::from_bytes(var, R"({"0": {"one": 1, "three": 3}})");
        assert(var == (avar(std::map<std::string, int>{{"one", 1}, {"three", 3}})));
            cxon::from_bytes(var, R"({"1": {"1": "one", "3": "three"}})");
        assert(var == (avar(std::map<int, std::string>{{1, "one"}, {3, "three"}})));
    }
#   endif
}
