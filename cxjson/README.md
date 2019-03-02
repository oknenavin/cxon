# `CXJSON` library

[![CXON](https://img.shields.io/badge/version-0.42.0-608060.svg?style=plastic)](https://github.com/libcxon/cxon)  
[![Language](https://img.shields.io/badge/language-C++11-608060.svg?style=plastic&logo=C%2B%2B)](https://isocpp.org/wiki/faq/cpp11)
[![Format](https://img.shields.io/badge/language-JSON-608060.svg?style=plastic&logo=JSON)](http://json.org)  
[![License](https://img.shields.io/badge/license-MIT-608060.svg?style=plastic)](../LICENSE)

> `CXJSON` is a polymorphic `C++` type for binding of an arbitrary [`JSON`](http://json.org/)  
> `CXJSON` is using [`CXON`](../README.md) for serialization

## Introduction

`CXJSON` is an implementation of a polymorphic type, which can describe an arbitrary `JSON`.  
`JSON` value type mapping is configurable with the following defaults:
  - object - [`std::map`](https://en.cppreference.com/w/cpp/container/map)
  - array - [`std::vector`](https://en.cppreference.com/w/cpp/container/vector)
  - string - [`std::string`](https://en.cppreference.com/w/cpp/string/basic_string)
  - number - [`double`](https://en.cppreference.com/w/cpp/language/types)
  - true|false - [`bool`](https://en.cppreference.com/w/cpp/language/types)
  - null - [`std::nullptr_t`](https://en.cppreference.com/w/cpp/language/types)

##### Example 1

Build, write and read of a simple `JSON`:

``` c++
    using node = cxjson::node;
        
    char const s1[] = "{\"even\":[2,4,6],\"odd\":[1,3,5]}";
    node const n2 = node::object {
        { "even", node::array { 2, 4, 6 } },
        { "odd", node::array { 1, 3, 5 } }
    };

    node n1; // read
        cxon::from_chars(n1, s1);
    assert(n1 == n2);

    std::string s2; // write
        cxon::to_chars(s2, n2);
    assert(s1 == s2);
```

##### Example 2

Build using node's methods:

``` c++
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
    node n2; // default node (of node_type::null)
        auto& o = n2.imbue<node::object>(); // change the type and return its value
            o["object"] = node::object {};  // default node::object type is std::map
            o["array"] = node::array {};
            o["string"] = "string";
            o["number"] = 3.14;
            o["boolean"] = false;
            o["null"] = nullptr;
        auto& o1 = o["object"].get<node::object>(); // get node::object
            o1["object"] = 0;
        auto& a = o["array"].get<node::array>();    // get node::array
            a.push_back(node::object {});           // default node::array type is std::vector
            a.push_back(node::array { 1, 2, 3 });
            a.push_back("4");
            a.push_back(5);
            a.push_back(true);
            a.push_back(nullptr);
        auto* o2 = a[0].get_if<node::object>(); // get value pointer if node's type match
            (*o2)["object"] = 0;
    assert(n1 == n2);

    std::string s1;
        cxon::to_chars(s1, n1);
    std::string s2;
        cxon::to_chars(s2, n2);
    assert(s1 == s2);
```

## Installation

See [`CXON`](../README.md#installation).

## Documentation

[MANUAL.md](MANUAL.md)

-------------------------------------------------------------------------------

Distributed under the MIT license. See [`LICENSE`](../LICENSE) for more information.

[GitHub](https://github.com/oknenavin/cxon)  
