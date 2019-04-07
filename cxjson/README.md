## `CXON/CXJSON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  
[![Language][img-lng]](https://isocpp.org/wiki/faq/cpp11)
[![Format][img-fmt]](http://json.org)  
[![License][img-lic]](../LICENSE)

[![Build][img-lnx]![img-osx]![img-win]![img-bld]](https://travis-ci.org/oknenavin/cxon)
[![Coverage][img-cov]](https://codecov.io/gh/oknenavin/cxon)

--------------------------------------------------------------------------------


> `CXJSON` is a polymorphic `C++` type for binding of an arbitrary `JSON`  
> `CXJSON` is using [`CXON`](../README.md) for serialization

#### Introduction

`CXJSON` is an implementation of a polymorphic type, which can represent an arbitrary `JSON`.  
`JSON` value type mapping is configurable with the following defaults:
  - `object` - [`std::map`][cpp-map]
  - `array` - [`std::vector`][cpp-vect]
  - `string` - [`std::string`][cpp-bstr]
  - `number` - [`double`][cpp-types]
  - `true|false` - [`bool`][cpp-types]
  - `null` - [`std::nullptr_t`][cpp-types]

###### Example 1

Build, write and read:

``` c++
#include "cxon/cxjson/cxjson.hxx"
#include <cassert>

int main() {
    using node = cxjson::node;
        
    char const s0[] = "{\"even\":[2,4,6],\"odd\":[1,3,5]}";
    node const n0 = node::object {
        { "even", node::array { 2, 4, 6 } },
        { "odd", node::array { 1, 3, 5 } }
    };

    node n1; // read
        cxon::from_chars(n1, s0);
    assert(n1 == n0);

    std::string s1; // write
        cxon::to_chars(s1, n0);
    assert(s1 == s0);
}
```

###### Example 2

Build using node's methods:

``` c++
#include "cxon/cxjson/cxjson.hxx"
#include <cassert>

int main() {
    using node = cxjson::node;

    // build using initializer lists
    node n1 = node::object {
        { "object", node::object { {"object", 0} } },
        { "array", node::array {
                node::object { {"object", 0} }, // objects and
                node::array { 1, 2, 3 },        // arrays must be explicit
                "4",        // string
                5,          // number
                true,       // boolean
                nullptr     // null
            }
        },
        { "string", "string" }, // "key": value
        { "number", 3.14 },
        { "boolean", false },
        { "null", nullptr }
    };

    // build using node's methods
    node n2;
        assert(n2.is<node::null>()); // default node type is node::null
        auto& o = n2.imbue<node::object>(); // change the type and return reference of the value
            assert(n2.is<node::object>());
            o["object"] = node::object {};      assert(o["object"].is<node::object>());
            o["array"] = node::array {};        assert(o["array"].is<node::array>());
            o["string"] = "string";             assert(o["string"].is<node::string>());
            o["number"] = 3.14;                 assert(o["number"].is<node::number>());
            o["boolean"] = false;               assert(o["boolean"].is<node::boolean>());
            o["null"] = nullptr;                assert(o["null"].is<node::null>());
        auto& o1 = o["object"].get<node::object>(); // get value reference, the type is known
            o1["object"] = 0;
        auto& a = o["array"].get<node::array>();    // get value reference, the type is known
            a.push_back(node::object {});       assert(a.back().is<node::object>());
            a.push_back(node::array {1, 2, 3}); assert(a.back().is<node::array>());
            a.push_back("4");                   assert(a.back().is<node::string>());
            a.push_back(5);                     assert(a.back().is<node::number>());
            a.push_back(true);                  assert(a.back().is<node::boolean>());
            a.push_back(nullptr);               assert(a.back().is<node::null>());
        auto* o2 = a[0].get_if<node::object>(); // get value pointer if the type match
            (*o2)["object"] = 0;
    assert(n1 == n2);

    std::string s1;
        cxon::to_chars(s1, n1);
    std::string s2;
        cxon::to_chars(s2, n2);
    assert(s1 == s2);
}
```

The resulting `JSON` is (*note, that the default number type is `double`*):

``` json
{
    "object": {"object": 0},
    "array": [
        {"object": 0},
        [1, 2, 3],
        "4",
        5,
        true,
        null
    ],
    "string": "string",
    "number": 3.1400000000000001,
    "boolean": false,
    "null": null
}
```

#### Installation & Compilation

See [`CXON`](../README.md#compilation).

#### Documentation

See the [MANUAL](MANUAL.md).

#### Contributing

Any kind of contribution (feedback, suggestions, code, tests, documentation, etc.) is welcome.  
Contact via [GitHub][GitHub] (create an issue even it's just a question or comment) or
via [mail](mailto:oknenavin@outlook.com).


--------------------------------------------------------------------------------
Distributed under the MIT license. See [`LICENSE`](../LICENSE) for more information.  
[GitHub][GitHub]


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060
[img-lng]: https://img.shields.io/badge/language-C++11-608060.svg?style=plastic&logo=C%2B%2B
[img-fmt]: https://img.shields.io/badge/language-JSON-608060.svg?style=plastic&logo=JSON
[img-lic]: https://img.shields.io/badge/license-MIT-608060.svg?style=plastic
[img-lnx]: ../.rc/linux.png
[img-osx]: ../.rc/osx.png
[img-win]: ../.rc/windows.png
[img-bld]: https://travis-ci.org/oknenavin/cxon.svg?branch=master
[img-cov]: https://codecov.io/gh/oknenavin/cxon/branch/master/graph/badge.svg
[GitHub]: https://github.com/oknenavin/cxon
[cpp-map]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/map&oldid=109218
[cpp-vect]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/vector&oldid=107643
[cpp-bstr]: https://en.cppreference.com/mwiki/index.php?title=cpp/string/basic_string&oldid=107637
[cpp-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
