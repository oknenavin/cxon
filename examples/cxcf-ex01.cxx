// CXCF configuration format

#include "cxon/cxcf.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"
#include <cassert>

// types representing a configuration file
struct type1 {
    std::map<std::string, std::vector<int>> key1;
    bool key2 = false;
};
struct config {
    int prop1 = 0;
    std::vector<std::string> prop_array;
    type1 prop_object;
};

// implemented for CXON
CXON_JSON_CLS(type1,
    CXON_JSON_CLS_FIELD_ASIS(key1),
    CXON_JSON_CLS_FIELD_NAME("key.2", key2)
)
CXON_JSON_CLS(config,
    CXON_JSON_CLS_FIELD_ASIS(prop1),
    CXON_JSON_CLS_FIELD_NAME("prop.array", prop_array),
    CXON_JSON_CLS_FIELD_NAME("prop-object", prop_object)
)
CXON_JSON_CLS_BARE(config) // bare root

// configuration data
static char const  cf_[] = R"(
    // the root is a bare type
    prop1 = 42                          // the values are of arbitrary JSON type
    prop.array = ["one" "two" "three"]  // the keys are unquoted
                                        // the list/map values are separated with white-spaces
    prop-object = {
        key1 = {
            key.1 = [1 3 5]
            key.2 = [2 4 6]
        }
        key.2 = true
    }
)";

int main() {
    // load
    config cf;
        auto const r = cxon::from_bytes(cf, cf_);
    // ready for use
    assert(
        r &&
        (cf.prop1 == 42) &&
        (cf.prop_array == decltype(cf.prop_array) {"one", "two", "three"}) &&
        (cf.prop_object.key1 == decltype(cf.prop_object.key1) {{"key.1", {1, 3, 5}}, {"key.2", {2, 4, 6}}}) &&
        (cf.prop_object.key2 == true)
    );
}
