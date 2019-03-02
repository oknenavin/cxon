# `CXON` library

[![CXON](https://img.shields.io/badge/version-0.42.0-608060.svg?style=plastic)](https://github.com/libcxon/cxon)  
[![Language](https://img.shields.io/badge/language-C++11-608060.svg?style=plastic&logo=C%2B%2B)](https://isocpp.org/wiki/faq/cpp11)
[![Format](https://img.shields.io/badge/language-JSON-608060.svg?style=plastic&logo=JSON)](http://json.org)  
[![License](https://img.shields.io/badge/license-MIT-608060.svg?style=plastic)](LICENSE)

> `CXON` is a _simple_, non-intrusive `C++` serialization library  
> `CXON`'s default serialization format is `UTF-8` encoded [`JSON`](http://json.org/) thus,
> `CXON` allows direct binding of `JSON` data to `C++` types - no other `JSON` library is needed  
> `CXON` is `C++11` compliant, header-only library with no dependencies besides the standard library  
> `CXON` core library consist of one header

## Introduction

Majority of the `JSON` libraries implement some kind of polymorphic type to represent arbitrary
`JSON` data, which in turn implies post processing. In contrast, `CXON` binds `JSON` to any
suitable `C++` type.

##### Example

Library type:

``` c++
    using my_type = std::map<std::string, std::vector<int>>;

    my_type mv1 = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} },
            mv2;
    std::string json;
        cxon::to_chars(json, mv1);
        cxon::from_chars(mv2, json);
    assert(mv1 == mv2);
```

Custom type:

``` c++
    struct my_type {
        std::vector<int> even;
        std::vector<int> odd;
        bool operator ==(const my_type& t) const { return even == t.even && odd == t.odd; }
    };
    CXON_STRUCT(my_type,
        CXON_STRUCT_FIELD_ASIS(even),
        CXON_STRUCT_FIELD_ASIS(odd)
    )
    ...
    my_type mv1 = { {2, 4, 6}, {1, 3, 5} },
            mv2;
    std::string json;
        cxon::to_chars(json, mv1);
        cxon::from_chars(mv2, json);
    assert(mv1 == mv2);
```

In both cases `my_type` is bound to the same `JSON`:

``` json
    {
        "even": [2, 4, 6],
        "odd": [1, 3, 5]
    }
```

## Installation

Copy the header(s) you need or use the provided makefile to install it on `POSIX` systems:

``` bash
$ make install
```

Run the test suites with:

``` bash
$ make test
```

## Documentation

[MANUAL.md](MANUAL.md)

-------------------------------------------------------------------------------

Distributed under the MIT license. See [`LICENSE`](LICENSE) for more information.

[GitHub](https://github.com/oknenavin/cxon)  
