## `CXON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  
[![Language][img-lng]](https://isocpp.org/wiki/faq/cpp11)
[![Format][img-fmt-json]](http://json.org)
[![Format][img-fmt-cbor]](https://cbor.io)  
[![License][img-lic]](LICENSE)

[![Linux][img-bld-lnx]](https://github.com/oknenavin/cxon/actions?query=workflow%3ALinux)
[![macOS][img-bld-mac]](https://github.com/oknenavin/cxon/actions?query=workflow%3AmacOS)
[![Windows][img-bld-win]](https://github.com/oknenavin/cxon/actions?query=workflow%3AWindows)  
[![Coverage][img-cov]](https://codecov.io/gh/oknenavin/cxon)
[![CodeQL][img-cql]](https://github.com/oknenavin/cxon/actions?query=workflow%3ACodeQL)
[![Quality][img-cod]](https://www.codacy.com/app/oknenavin/cxon?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=oknenavin/cxon&amp;utm_campaign=Badge_Grade)
[![Quality][img-lgtm-qual]](https://lgtm.com/projects/g/oknenavin/cxon/context:cpp)
[![Alerts][img-lgtm-alrt]](https://lgtm.com/projects/g/oknenavin/cxon/alerts/)


--------------------------------------------------------------------------------

`CXON` is a C++ serialization interface  
`CXON` is a `C++11` compliant, self contained, header-only library  
`CXON` implements [`JSON`](http://json.org) (`UTF-8` encoded) as a serialization format  
`CXON` implements [`CBOR`](https://cbor.io) as a serialization format  
`CXON` is easy to extend for different formats and types imposing minimal overhead  


--------------------------------------------------------------------------------

#### Contents
  - [Introduction](#introduction)
  - [Formats](#formats)
  - [Compilation](#compilation)
  - [Installation](#installation)
  - [Documentation](#documentation)
  - [Contributing](#contributing)


--------------------------------------------------------------------------------

#### Introduction

`CXON` defines and implements an interface similar to`C++17`'s [`<charconv>`][std-charconv] 
interface with these differences:

  - traits template parameter (support for different serialization formats, 
    see [`Format traits`](src/cxon/README.md#format-traits))
  - trailing named parameters of arbitrary type (passing of parameters to specific 
    type serializers, see [Named parameters](src/cxon/README.md#named-parameters)
  - input and output iterators for I/O (allowing streams, containers and arrays, 
    see [`Interface`](src/cxon/README.md#interface))

```c++
namespace cxon {

    template <typename It>
        struct from_bytes_result {
            std::error_condition ec; // read error
            It end; // one-past-the-end iterator of the matched range
        };

    // from input iterator
    template <typename Traits, typename T, typename InIt, typename ...Parameters>
        auto from_bytes(T& t, InIt b, InIt e, Parameters... ps)     -> from_bytes_result<InIt>;
    // from iterable (e.g. std::string)
    template <typename Traits, typename T, typename Iterable, typename ...Parameters>
        auto from_bytes(T& t, const Iterable& i, Parameters... ps)  -> from_bytes_result<decltype(std::begin(i))>;


    template <typename It>
        struct to_bytes_result {
            std::error_condition ec; // write error
            It end; // one-past-the-end output iterator
        };

    // to output iterator
    template <typename Traits, typename T, typename OutIt, typename ...Parameters>
        auto to_bytes(OutIt o, const T& t, Parameters... ps)        -> to_bytes_result<OutIt>;
    // to back insertable (e.g. std::string)
    template <typename Traits, typename T, typename Insertable, typename ...Parameters>
        auto to_bytes(Insertable& i, const T& t, Parameters... ps)  -> to_bytes_result<decltype(std::begin(i))>;
    // to range
    template <typename Traits, typename T, typename FwIt, typename ...Parameters>
        auto to_bytes(FwIt b, FwIt e, const T& t, Parameters... ps) -> to_bytes_result<FwIt>;

}
```

###### Example

``` c++
#include "cxon/json.hxx" // first include - JSON will be the default format
#include "cxon/cbor.hxx" // following format includes - require format specification
...

// in = "[1, 5, 7]"; // JSON in UTF-8
std::vector<int> value;
auto fbr = cxon::from_bytes(value, std::begin(in), std::end(in), ...); // read from iterator (default format `JSON`)
assert(fbr); // the result: error and serialization status
...
std::vector<unsigned char> cbor;
auto tbr = cxon::to_bytes<CBOR<>>(cbor, value, ...); // write to std::vector<unsigned char> (format `CBOR`)
assert(tbr); // the result: error and serialization status
```

`CXON` supports good part of `C++`'s fundamental, compound and standard library types out of the box, including:

  - [fundamental types][cpp-fund-types]
      - `nullptr_t`
      - `bool`
      - character types - `char`, `wchar_t`, `char8_t`, `char16_t` and `char32_t`
      - integral types - `signed` and `unsigned` `char`, `short int`, `int`, `long int`, `long long int`
      - floating-point types - `float`, `double`, `long double`
  - compound types
      - [`pointer types`][cpp-ptr]
      - [`array types`][cpp-arr]
      - [`enumeration types`][cpp-enum]
      - [`class types`][cpp-class]
  - standard library types
      - [`containers library`][std-container]
      - [`std::pair`][std-pair]
      - [`std::tuple`][std-tuple]
      - [`std::optional`][std-opt]
      - [`std::variant`][std-var]
      - [`std::basic_string`][std-bstr]
      - [`std::basic_string_view`][std-strv]
      - [`std::bitset`][std-bitset]
      - [`std::complex`][std-complex]
      - [`std::valarray`][std-valarr]
      - [`std::chrono::duration`][std-duration]
      - [`std::chrono::time_point`][std-time-pt]

`CXON` can be extended for arbitrary types, using intrusive and non-intrusive methods
(see the [`MANUAL`](src/cxon/README.md#implementation-bridge) for details).

###### Example

Bind to a library type:

``` c++
#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx" // include cxon/lib/std/<stdlib>.hxx
#include "cxon/lib/std/vector.hxx" // instead of the standard headers
#include "cxon/lib/std/map.hxx"    // for any of the supported formats
#include <cassert>

// an arbitrary combination of types
using my_type = std::map<std::string, std::vector<int>>;

int main() {
    my_type mv1 = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} },
            mv2;
    std::string json;
        // write it to output-iterator, container, buffer, etc. - in this case, std::string
        cxon::to_bytes(json, mv1);
        // read it from input-iterator, container, buffer, etc. - in this case, std::string
        cxon::from_bytes(mv2, json);
    assert(mv1 == mv2);
}
```

Binding to a user type:

``` c++
#include "cxon/json.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/list.hxx"
#include <cassert>

struct my_type {
    std::vector<int> even;
    std::list<int> odd;
    bool operator ==(const my_type& v) const {
        return even == v.even && odd == v.odd;
    }
};

// in this simple case, some trivial macros can be used to implement the type for CXON
CXON_JSON_CLS(my_type,
    CXON_JSON_CLS_FIELD_ASIS(even),
    CXON_JSON_CLS_FIELD_ASIS(odd)
)

int main() {
    my_type mv1 = { {2, 4, 6}, {1, 3, 5} },
            mv2;
    std::string json;
        cxon::to_bytes(json, mv1);
        cxon::from_bytes(mv2, json);
    assert(mv1 == mv2);
}
```

In both examples, `my_type` is bound to the same `JSON`:

``` json
{ "even": [2, 4, 6], "odd": [1, 3, 5] }
```

*Somewhat more meaningful example can be found here [`JSON-RPC`](src/cxon/README.md#example-json-rpc).*


--------------------------------------------------------------------------------

#### Formats

##### [`JSON`](http://json.org)

The implementation strictly complies with [`RFC7159`][RFC7159] / [`ECMA-404`][ECMA-404].

Most so-called `JSON` libraries, e.g. [`nlohmann/json`](https://github.com/nlohmann/json),
implement a kind of polymorphic type to represent arbitrary `JSON` - many call it `DOM`, `DOM`-like, etc. and 
[`cxon::json::node`](src/cxon/lang/json/node/README.md), which is part of `CXON/JSON`, is an implementation 
of such a polymorphic type (and also an example of how `CXON` can be used).

##### [`CBOR`](https://cbor.io)

The implementation strictly complies with [`RFC7049`][RFC7049].


--------------------------------------------------------------------------------

#### Compilation

`CXON` requires [`C++11`][cpp-comp-support] compliant compiler, tested with `g++-5`, 
`clang++-3.5` and `msvc++ 14.1` (see [builds](https://travis-ci.com/oknenavin/cxon)).


--------------------------------------------------------------------------------

#### Installation

`CXON` is a header-only library - copy the headers you need, or use 
the provided makefile to install it on `POSIX` systems:

``` bash
$ sudo make install
```

or run the test suites with:

``` bash
# g++
$ make check
# clang++
$ make CXX=clang++ check
```


--------------------------------------------------------------------------------

#### Documentation

See the [MANUAL](doc/README.md).


--------------------------------------------------------------------------------

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
[img-lng]: https://img.shields.io/badge/language-C++11/17-608060.svg?style=plastic&logo=C%2B%2B
[img-fmt-json]: https://img.shields.io/badge/language-JSON-608060.svg?style=plastic&logo=JSON
[img-fmt-cbor]: https://img.shields.io/badge/language-CBOR-608060.svg?style=plastic
[img-lic]: https://img.shields.io/badge/license-MIT-608060.svg?style=plastic
[img-bld-lnx]: https://github.com/oknenavin/cxon/workflows/Linux/badge.svg?branch=features%2Fcbor
[img-bld-mac]: https://github.com/oknenavin/cxon/workflows/macOS/badge.svg?branch=features%2Fcbor
[img-bld-win]: https://github.com/oknenavin/cxon/workflows/Windows/badge.svg?branch=features%2Fcbor
[img-cov]: https://codecov.io/gh/oknenavin/cxon/branch/master/graph/badge.svg
[img-cod]: https://api.codacy.com/project/badge/Grade/503ff2e908b745ba9dfeba2b50ec2201
[img-cql]: https://github.com/oknenavin/cxon/workflows/CodeQL/badge.svg
[img-lgtm-qual]: https://img.shields.io/lgtm/grade/cpp/g/oknenavin/cxon.svg?logo=lgtm&logoWidth=18
[img-lgtm-alrt]: https://img.shields.io/lgtm/alerts/g/oknenavin/cxon.svg?logo=lgtm&logoWidth=18
[RFC7159]: https://www.ietf.org/rfc/rfc7159.txt
[ECMA-404]: http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
[RFC7049]: https://tools.ietf.org/rfc/rfc7049.txt
[GitHub]: https://github.com/oknenavin/cxon
[std-charconv]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/charconv&oldid=105120
[cpp-comp-support]: https://en.cppreference.com/mwiki/index.php?title=cpp/compiler_support&oldid=108771
[cpp-fund-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[cpp-ptr]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/pointer&oldid=109738
[cpp-arr]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/array&oldid=111607
[cpp-enum]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/enum&oldid=111809
[cpp-class]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/class&oldid=101735
[std-complex]: https://en.cppreference.com/mwiki/index.php?title=cpp/numeric/complex&oldid=103532
[std-valarr]: https://en.cppreference.com/mwiki/index.php?title=cpp/numeric/valarray&oldid=109876
[std-bitset]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/bitset&oldid=103231
[std-duration]: https://en.cppreference.com/mwiki/index.php?title=cpp/chrono/duration&oldid=100475
[std-time-pt]: https://en.cppreference.com/mwiki/index.php?title=cpp/chrono/time_point&oldid=103361
[std-bstr]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/string&oldid=111300
[std-strv]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/string_view&oldid=107572
[std-tuple]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/tuple&oldid=108562
[std-pair]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/pair&oldid=92191
[std-container]: https://en.cppreference.com/mwiki/index.php?title=cpp/container&oldid=105942
[std-opt]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/optional&oldid=110327
[std-var]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/variant&oldid=109919
