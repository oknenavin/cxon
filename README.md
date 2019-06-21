## `CXON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  
[![Language][img-lng]](https://isocpp.org/wiki/faq/cpp11)
[![Format][img-fmt]](http://json.org)  
[![License][img-lic]](LICENSE)

[![Build][img-lnx]![img-osx]![img-win]![img-bld]](https://travis-ci.org/oknenavin/cxon)
[![Coverage][img-cov]](https://codecov.io/gh/oknenavin/cxon)
[![Quality][img-lgtm-qual]](https://lgtm.com/projects/g/oknenavin/cxon/context:cpp)
[![Alerts][img-lgtm-alrt]](https://lgtm.com/projects/g/oknenavin/cxon/alerts/)

--------------------------------------------------------------------------------


> `CXON` is a simple, non-intrusive `C++` serialization library  
> `CXON`'s default serialization format is `UTF-8` encoded `JSON`  
> `CXON` is `C++11` compliant, self contained, header-only library  
> `CXON` core library consist of one header

`CXON/JSON` strictly complies with [`RFC7159`][RFC7159] / [`ECMA-404`][ECMA-404].

#### Introduction

`CXON` defines and implements an interface, which is a generalization of `C++17`'s
[`<charconv>`][cpp-charconv] interface.

Most libraries implement a polymorphic type to represent arbitrary `JSON` - many call it `DOM`,
`DOM`-like, etc.. In contrast, `CXON` binds `JSON` to any suitable `C++` type, though
[`CXJSON`](src/cxon/cxjson/README.md), which is part of `CXON`, is an implementation of such a polymorphic
type, and also an example of how `CXON` can be used.

`CXON` implements good part of `C++`'s fundamental and standard library types including:

- [fundamental types][cpp-fund-types]
    - `nullptr_t`
    - `bool`
    - character types - `char`, `wchar_t`, `char16_t` and `char32_t`
    - integral types - `signed` and `unsigned` `char`, `short`, `int`, `long`, `long long`
    - floating-point types - `float`, `double`, `long double`
- arrays and pointers of arbitrary types
- standard library types
    - [`std::basic_string`][cpp-bstr]
    - [`std::tuple`][cpp-tuple]
    - [`std::pair`][cpp-pair]
    - [containers library][cpp-container]

`CXON` can easily be extended for arbitrary types, using intrusive and non-intrusive methods
(see the [`MANUAL`](src/cxon/README.md#implementation-bridge) for details).

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

*Somewhat more meaningful example can be found here [`JSON-RPC`](src/cxon/README.md#example-json-rpc).*

#### Compilation

`CXON` requires [`C++11`][cpp-comp-support] compliant compiler, tested with `g++-5`,
`clang++-3.5` and `msvc++ 14.1` (see [builds](https://travis-ci.org/oknenavin/cxon)).

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

See the [MANUAL](src/cxon/README.md).

#### Contributing

Any kind of contribution (feedback, suggestions, code, tests, documentation, etc.) is welcome.  
Contact via [GitHub][GitHub] (create an issue even it's just a question or comment) or
via [mail](mailto:oknenavin@outlook.com).


-------------------------------------------------------------------------------
Distributed under the MIT license. See [`LICENSE`](LICENSE) for more information.  
[GitHub][GitHub]  


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060
[img-lng]: https://img.shields.io/badge/language-C++11-608060.svg?style=plastic&logo=C%2B%2B
[img-fmt]: https://img.shields.io/badge/language-JSON-608060.svg?style=plastic&logo=JSON
[img-lic]: https://img.shields.io/badge/license-MIT-608060.svg?style=plastic
[img-lnx]: .rc/linux.png
[img-osx]: .rc/osx.png
[img-win]: .rc/windows.png
[img-bld]: https://travis-ci.org/oknenavin/cxon.svg?branch=master
[img-cov]: https://codecov.io/gh/oknenavin/cxon/branch/master/graph/badge.svg
[img-lgtm-qual]: https://img.shields.io/lgtm/grade/cpp/g/oknenavin/cxon.svg?logo=lgtm&logoWidth=18
[img-lgtm-alrt]: https://img.shields.io/lgtm/alerts/g/oknenavin/cxon.svg?logo=lgtm&logoWidth=18
[RFC7159]: https://www.ietf.org/rfc/rfc7159.txt
[ECMA-404]: http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
[GitHub]: https://github.com/oknenavin/cxon
[cpp-charconv]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/charconv&oldid=105120
[cpp-comp-support]: https://en.cppreference.com/mwiki/index.php?title=cpp/compiler_support&oldid=108771
[cpp-fund-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[cpp-bstr]: https://en.cppreference.com/mwiki/index.php?title=cpp/string/basic_string&oldid=107637
[cpp-tuple]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/tuple&oldid=108562
[cpp-pair]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/pair&oldid=92191
[cpp-container]: https://en.cppreference.com/mwiki/index.php?title=cpp/container&oldid=105942
