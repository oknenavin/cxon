## `CXON` library

[![Version][url-version-image]](https://github.com/libcxon/cxon)  
[![Language][url-language-image]](https://isocpp.org/wiki/faq/cpp11)
[![Format][url-format-image]](http://json.org)  
[![License][url-license-image]](LICENSE)

> `CXON` is a _simple_, non-intrusive `C++` serialization library  
> `CXON`'s default serialization format is `UTF-8` encoded `JSON`  
> `CXON` makes it trivial to bind `JSON` to arbitrary `C++` types  
> `CXON` is `C++11` compliant, self contained, header-only library  
> `CXON` core library consist of one header

`CXON/JSON` strictly complies with [`RFC7159`](https://www.ietf.org/rfc/rfc7159.txt) / [`ECMA-404`](http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf).

#### Introduction

Majority of the `JSON` libraries implement some kind of polymorphic type to represent arbitrary
`JSON` - many call it DOM, DOM-like, etc., mimicking popular `XML` libraries.  
In contrast, `CXON` binds `JSON` to any suitable `C++` type, though [`CXJSON`](cxjson/README.md),
which is part of `CXON`, is an implementation of such type.

###### Example

Bind to a library type:

``` c++
using my_type = std::map<std::string, std::vector<int>>;

my_type mv1 = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} },
        mv2;
std::string json;
    cxon::to_chars(json, mv1);
    cxon::from_chars(mv2, json);
assert(mv1 == mv2);
```

Bind to a custom type:

``` c++
struct my_type {
    std::vector<int> even;
    std::list<int> odd;
    bool operator ==(const my_type& v) const {
        return even == v.even && odd == v.odd;
    }
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

#### Compilation

`CXON` requires [`C++11`](https://en.cppreference.com/w/cpp/compiler_support) compliant compiler.

#### Installation

Copy the header(s) you need or use the provided makefile to install `CXON` on `POSIX` systems:

``` bash
$ make install
```

Run the test suites with:

``` bash
# g++
$ make test
# clang++
$ make CXX=clang++ test
```

#### Documentation

See the [MANUAL](MANUAL.md).

#### Contributing

Any contributions are welcome.  
Contact via [GitHub][url-github] or [mail](mailto:oknenavin@outlook.com).


--------------------------------------------------------------------------------
<!-- links -->
[url-github]: https://github.com/oknenavin/cxon
[url-version-image]: https://img.shields.io/badge/version-0.42.0-608060.svg?style=plastic
[url-language-image]: https://img.shields.io/badge/language-C++11-608060.svg?style=plastic&logo=C%2B%2B
[url-format-image]: https://img.shields.io/badge/language-JSON-608060.svg?style=plastic&logo=JSON
[url-license-image]: https://img.shields.io/badge/license-MIT-608060.svg?style=plastic
