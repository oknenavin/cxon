## `CXON/JSON/CXCF` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------

`CXCF` is a simple configuration format derived from `JSON`.  
It's defined as a [format-traits](../../README.md#format-traits) derived from the `JSON` format traits.

``` c++
namespace cxon { namespace cxcf { // format traits

    struct format_traits : json::format_traits {
        struct map : json::format_traits::map {
            static constexpr char div = '='; // { key1 = value1
            static constexpr char sep = ' '; //   key2 = value2 }
        };
        struct list : json::format_traits::list {
            static constexpr char sep = ' '; // [value1 value2]
        };
        // object keys for types serialized without quotes will be quoted
        // e.g. strings will still be quoted, but numbers will not
        // {1: 2} is valid
        static constexpr bool quote_unquoted_keys       = false;
        // object keys for types serialized with quotes will be unquoted (e.g. strings)
        // {key: 1} is valid
        // any white-space characters and object/key ('=') separator character
        // must be escaped - e.g. { a\ key: 1}
        static constexpr bool unquote_quoted_keys       = true;
        // allow c-style comments
        static constexpr bool allow_comments            = true;
        // allow NaN and Infinity as a floating-point numbers
        static constexpr bool allow_javascript_nans     = true;
    };

}}

namespace cxon { // format selector
    template <typename T = cxcf::format_traits>
        using CXCF = JSON<T>;
}
```

###### Example
```
// the root is a bare type, no braces needed
prop1 = 42                          // the values are of arbitrary JSON type
prop.array = ["one" "two" "three"]  // the keys are unquoted
                                    // the list/map values are separated with white-spaces
prop-object = {
    key1 = {
        key.1 = [1 3 5]
        key.2 = [2 4 6]
    }
    key.2 = false
}
```

###### Example

``` c++
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
CXON_JSON_CLS_BARE(config) // bare root, no braces

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
```


--------------------------------------------------------------------------------

Distributed under the MIT license. See [`LICENSE`](../../../../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  

<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060
