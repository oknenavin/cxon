## `CXON` library

[![cxon][url-cxon-image]](https://github.com/libcxon/cxon)
[![Version][url-version-image]](https://github.com/libcxon/cxon)  
[![Language][url-language-image]](https://isocpp.org/wiki/faq/cpp11)
[![Format][url-format-image]](http://json.org)  
[![License][url-license-image]](LICENSE)

--------------------------------------------------------------------------------


> `CXON` is a _simple_, non-intrusive `C++` serialization library  
> `CXON`'s default serialization format is `UTF-8` encoded `JSON`  
> `CXON` makes it trivial to bind `JSON` to arbitrary `C++` types  
> `CXON` is `C++11` compliant, self contained, header-only library  
> `CXON` core library consist of one header

`CXON/JSON` strictly complies with [`RFC7159`](https://www.ietf.org/rfc/rfc7159.txt) / [`ECMA-404`](http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf).

#### Introduction

`CXON` defines and implements an interface, which is a generalization of C++17's
[`<charconv>`][url-cpp-charconv] interface.

Majority of the `JSON` libraries implement some kind of polymorphic type to represent arbitrary
`JSON` - many call it DOM, DOM-like, etc., refering to popular `XML` libraries.  
In contrast, `CXON` binds `JSON` to any suitable `C++` type (though [`CXJSON`](cxjson/README.md),
which is part of `CXON`, is an implementation of such a type, and also an example of how
`CXON` can be used).

`CXON` implements good part of `C++`'s fundamental and standard library types including:

- [fundamental types][url-cpp-fund-types]
    - `nullptr_t`
    - `bool`
    - character types - `char`, `wchar_t`, `char16_t` and `char32_t`
    - integral types - `signed` and `unsigned` `char`, `short`, `int`, `long`, `long long`
    - floating-point types - `float`, `double`, `long double`
- arrays and pointers of arbitrary types
- standard library types
    - [`std::basic_string`][url-cpp-bstr]
    - [`std::tuple`][url-cpp-tuple]
    - [`std::pair`][url-cpp-pair]
    - [containers library][url-cpp-container] - in its entirety

`CXON` can easily be extended for arbitrary types, using intrusive and non-intrusive methods
(see the [`MANUAL`](MANUAL.md#implementation-bridge) for details).

###### Example

Bind to a library type:

``` c++
#include "cxon/cxon.hxx"
#include <cassert>

// an arbitrary complex combination of fundamental and library types
using my_type = std::map<std::string, std::vector<int>>;

int main() {
    my_type mv1 = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} },
            mv2;
    std::string json;
        // write it to output-iterator, container, buffer, etc. - in this case, std::string
        cxon::to_chars(json, mv1);
        // read it from input-iterator, container, buffer, etc. - in this case, std::string
        cxon::from_chars(mv2, json);
    assert(mv1 == mv2);
}
```

Bind to a custom type:

``` c++
#include "cxon/cxon.hxx"
#include <cassert>

struct my_type {
    std::vector<int> even;
    std::list<int> odd;
    bool operator ==(const my_type& v) const {
        return even == v.even && odd == v.odd;
    }
};

// in this simple case, some trivial macros can be used to implement the type for CXON
CXON_STRUCT(my_type,
    CXON_STRUCT_FIELD_ASIS(even),
    CXON_STRUCT_FIELD_ASIS(odd)
)

int main() {
    my_type mv1 = { {2, 4, 6}, {1, 3, 5} },
            mv2;
    std::string json;
        cxon::to_chars(json, mv1);
        cxon::from_chars(mv2, json);
    assert(mv1 == mv2);
}
```

In both cases `my_type` is bound to the same `JSON`:

``` json
{
    "even": [2, 4, 6],
    "odd": [1, 3, 5]
}
```

#### Compilation

`CXON` requires [`C++11`][url-cpp-comp-support] compliant compiler.

#### Installation

`CXON` is a header-only library, copy the header(s) you need or use
the provided makefile to install it on `POSIX` systems:

``` bash
$ make install
```

or run the test suites with:

``` bash
# g++
$ make test
# clang++
$ make CXX=clang++ test
```

#### Documentation

See the [MANUAL](MANUAL.md).

#### Contributing

Any kind of contribution (feedback, suggestions, code, tests, documentation, etc.) is welcome.  
Contact via [GitHub][url-github] (create an issue even it's just a question) or
[mail](mailto:oknenavin@outlook.com).


-------------------------------------------------------------------------------
Distributed under the MIT license. See [`LICENSE`](LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  


<!-- links -->
[url-cxon-image]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[url-version-image]: https://img.shields.io/badge/version-0.42.0-608060.svg?style=plastic
[url-language-image]: https://img.shields.io/badge/language-C++11-608060.svg?style=plastic&logo=C%2B%2B
[url-format-image]: https://img.shields.io/badge/language-JSON-608060.svg?style=plastic&logo=JSON
[url-license-image]: https://img.shields.io/badge/license-MIT-608060.svg?style=plastic
[url-github]: https://github.com/oknenavin/cxon
[url-cpp-charconv]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/charconv&oldid=105120
[url-cpp-comp-support]: https://en.cppreference.com/mwiki/index.php?title=cpp/compiler_support&oldid=108771
[url-cpp-fund-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[url-cpp-bstr]: https://en.cppreference.com/mwiki/index.php?title=cpp/string/basic_string&oldid=107637
[url-cpp-tuple]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/tuple&oldid=108562
[url-cpp-pair]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/pair&oldid=92191
[url-cpp-container]: https://en.cppreference.com/mwiki/index.php?title=cpp/container&oldid=105942
