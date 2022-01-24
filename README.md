## `CXON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  
[![Language][img-lng]](https://isocpp.org/wiki/faq/cpp11)
[![Format][img-fmt-json]](http://json.org)
[![Format][img-fmt-cbor]](https://cbor.io)  
[![License][img-lic]](LICENSE)

[![Build][img-bld-lnx]](https://github.com/oknenavin/cxon/actions?query=workflow%3ALinux)
[![Build][img-bld-mac]](https://github.com/oknenavin/cxon/actions?query=workflow%3AmacOS)
[![Build][img-bld-win]](https://github.com/oknenavin/cxon/actions?query=workflow%3AWindows)  
[![Codecov][img-cov]](https://codecov.io/gh/oknenavin/cxon)
[![Coverity][img-cvr]](https://scan.coverity.com/projects/oknenavin-cxon)
[![Codacy][img-cod]](https://www.codacy.com/gh/oknenavin/cxon/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=oknenavin/cxon&amp;utm_campaign=Badge_Grade)
[![CodeQL][img-cql]](https://github.com/oknenavin/cxon/actions?query=workflow%3ACodeQL)
[![LGTM][img-lgtm-qual]](https://lgtm.com/projects/g/oknenavin/cxon/context:cpp)
[![LGTM/Alerts][img-lgtm-alrt]](https://lgtm.com/projects/g/oknenavin/cxon/alerts/)


--------------------------------------------------------------------------------

  - `CXON` is a C++ serialization interface  
  - `CXON` implements [`JSON`](http://json.org) (`UTF-8` encoded) as a serialization format (an example of text-based data format)  
  - `CXON` implements [`CBOR`](https://cbor.io) as a serialization format (an example of binary data format)  
  - `CXON` is easy to extend for different formats and types with [zero-overhead][cpp-zeov]  
  - `CXON` is a `C++11` compliant, self contained and compact header-only library  

Although `CXON` is a serialization library, its goal is to actually compete with `JSON`/`CBOR`/etc. libraries like
`Boost.JSON`/`RapidJSON`/etc. and its main **advantage** is, that no intermediate type is needed to represent the data - 
any `C++` type that matches it semantically can be used.

###### Example
``` c++
#include "cxon/json.hxx"
#include "cxon/lib/std/vector.hxx" // for <vector>
#include <cassert>

int main() {
    std::vector<int> cxx; // or std::array, std::list, std::set, etc.
        // the input is a JSON array, semantically a list of integers
        auto result = cxon::from_bytes(cxx,  "[1, 2, 3]");
    assert(result);
    // the data is loaded successfully, no additional semantic validation is needed, so
    assert(cxx == (std::vector<int> {1, 2, 3}));
}
```

Once the data is loaded successfully it means, that it is syntactically and semantically correct - ready for use.  
In contrast, most of the `JSON`/`CBOR`/etc. libraries represent arbitrary data
with a polymorphic type (called `DOM`, `value`, etc.) and parsing of the input only means,
that it is syntactically correct.

###### Example
``` c++
// pseudo code
value_type json = json_lib::parse("[1, 2, 3]");
assert(json.has_parsing_erros()); // check for syntax errors
// check the semantics, expected is a list of integers
assert(json.is_array()); // check the type
auto& array = json.get_array();
assert( // check the values
    array.size() > ... &&
    array[0].is_integer() &&
    array[1].is_integer() &&
    array[2].is_integer() &&
    ...
);
// the input is semantically correct, however
// the values still need special care
int x0 = array[0].get_integer(); // it's an int, but not quite
...
```

To help with this, some of the libraries provide utilities to convert the value type to a
`C++` type - e.g. `Boost.JSON` provides `value_from` / `value_to` for this.

For completeness, `CXON` also provides polymorphic types (called `node`) for the supported formats,
which match the functionality provided by these libraries.  
The **performance** is often important and is emphasized by many libraries like `Boost.JSON` and `RapidJSON` and
in this respect, `CXON` is [close to the best](#performance). An important note here, is that many of
the libraries emphasize the floating-point serialization and deserialization performance, utilizing very fast
(and complex) algorithms. In contrast, by default, `CXON` uses [`<charconv>`][std-charconv]
(with a fall back for `C++11`). [`<charconv>`][std-charconv] is fast, but especially the parsing can be
significantly slower than with these algorithms - at least with the implementations currently in the wild.
Another important note, is that the libraries based on polymorphic types have validation and use overhead
that should be taken into account.  
The **memory management** is often important, especially in the embedded space, and `CXON` is well suited - 
`CXON` does not allocate in general; it's up to the types provided. In the example above, the memory management
will be handled completely by `std::vector` and its allocator (whatever it is). In the same spirit, the
polymorphic types provided by `CXON` are [AllocatorAware][cpp-alaw] compliant.  
Like [`<charconv>`][std-charconv], `CXON` is **non-throwing**, provided that the serializers involved do not throw.

--------------------------------------------------------------------------------

#### Contents
  - [Overview](#overview)
  - [Formats](#formats)
  - [Performance](#performance)
  - [Compilation](#compilation)
  - [Installation](#installation)
  - [Documentation](#documentation)
  - [Contributing](#contributing)


--------------------------------------------------------------------------------

#### Overview

`CXON` defines and implements an interface similar to`C++17`'s [`<charconv>`][std-charconv] [interface](#interface).  
`CXON` extends `C++17`'s [`<charconv>`][std-charconv] interface with:

  - traits template parameter (support for different serialization formats, 
    see [`Format traits`](src/cxon/README.md#format-traits))
  - trailing named parameters of arbitrary types (passing of parameters to specific 
    type serializers, see [Named parameters](src/cxon/README.md#named-parameters)
  - input and output iterators for I/O (allowing streams, containers and arrays, 
    see [`Interface`](src/cxon/README.md#interface))

The traits can be stateful or stateless allowing arbitrary complex formats.  
Named parameters can be compile time or runtime giving flexibility for the implementations.

- `CXON` supports good part of `C++`'s fundamental, compound and standard library types out of the box, including:
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
      - [`std::basic_string_view`][std-strv] - _write only_
      - [`std::bitset`][std-bitset]
      - [`std::complex`][std-complex]
      - [`std::valarray`][std-valarr]
      - [`std::chrono::duration`][std-duration]
      - [`std::chrono::time_point`][std-time-pt]
- `CXON` support of [`Boost/Containers`](https://www.boost.org/doc/libs/1_78_0/?view=category_containers) include:
  - [`Container`](https://www.boost.org/doc/libs/1_78_0/doc/html/container.html)
  - [`Dynamic Bitset`](https://www.boost.org/doc/libs/1_78_0/libs/dynamic_bitset/dynamic_bitset.html)
  - [`Variant`](https://www.boost.org/doc/libs/1_78_0/doc/html/variant.html)
  - [`Variant2`](https://www.boost.org/doc/libs/1_78_0/libs/variant2/doc/html/variant2.html)

`CXON` can be extended for arbitrary types, using intrusive and non-intrusive methods
(see the [`MANUAL`](src/cxon/README.md#implementation-bridge) for details).


###### Interface

```c++
namespace cxon {

    template <typename It>
        struct from_bytes_result {
            std::error_condition ec; // read error
            It end; // one-past-the-end iterator of the matched range
        };

    // from input iterator
    template <typename Traits, typename T, typename InIt, typename ...Parameters>
        auto from_bytes(T& t, InIt b, InIt e, Parameters&&... ps)
            -> from_bytes_result<InIt>;
    // from iterable (e.g. std::string)
    template <typename Traits, typename T, typename Iterable, typename ...Parameters>
        auto from_bytes(T& t, const Iterable& i, Parameters&&... ps)
            -> from_bytes_result<decltype(std::begin(i))>;


    template <typename It>
        struct to_bytes_result {
            std::error_condition ec; // write error
            It end; // one-past-the-end output iterator
        };

    // to output iterator
    template <typename Traits, typename T, typename OutIt, typename ...Parameters>
        auto to_bytes(OutIt o, const T& t, Parameters&&... ps)
            -> to_bytes_result<OutIt>;
    // to back insertable (e.g. std::string)
    template <typename Traits, typename T, typename Insertable, typename ...Parameters>
        auto to_bytes(Insertable& i, const T& t, Parameters&&... ps)
            -> to_bytes_result<decltype(std::begin(i))>;
    // to range
    template <typename Traits, typename T, typename FwIt, typename ...Parameters>
        auto to_bytes(FwIt b, FwIt e, const T& t, Parameters&&... ps)
            -> to_bytes_result<FwIt>;

}
```


###### Example

``` c++
// common interface

#include "cxon/json.hxx" // included first, JSON will be the default format
#include "cxon/cbor.hxx" // not the first, CBOR format must be explicitly specified

// use for all formats, the relevant code will be included depending of the formats included above
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"

#include <cassert>

int main() {
    using CBOR = cxon::CBOR<>;

    char const  json_in[] = "[1,5,7]";
    std::string json_out;

    std::vector<unsigned char> cbor;
    {
        std::vector<int> cxx;
        auto fbr = cxon::from_bytes(cxx, json_in);      // JSON to C++
            assert(fbr);
        auto tbr = cxon::to_bytes<CBOR>(cbor, cxx);     // C++ to CBOR
            assert(tbr);
    }
    {
        std::vector<int> cxx;
        auto fbr = cxon::from_bytes<CBOR>(cxx, cbor);   // CBOR to C++
            assert(fbr);
        auto tbr = cxon::to_bytes(json_out, cxx);       // C++ to JSON
            assert(tbr);
    }

    assert(json_out == json_in);
}
```

###### Example

``` c++
// using of standard library types

#include "cxon/json.hxx"

#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"

#include <cassert>

using my_type = std::map<std::string, std::vector<int>>;

int main() {
    my_type mv1 = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} },
            mv2;
    std::string json;
        // write it to output-iterator, container, buffer, etc. - in this case, std::string
        cxon::to_bytes(json, mv1);
        // read it from input-iterator, container, buffer, etc. - in this case, std::string
        cxon::from_bytes(mv2, json);
    // json == "{ \"even\": [2, 4, 6], \"odd\": [1, 3, 5] }"
    assert(mv1 == mv2);
}
```

###### Example

``` c++
// using of user types

#include "cxon/json.hxx"

#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/list.hxx"

#include <cassert>

struct my_type {
    std::vector<int> even;
    std::list<int> odd;
    bool operator ==(const my_type& v) const noexcept {
        return even == v.even && odd == v.odd;
    }
};

// in this simple case, some trivial macros can be used to implement the type for CXON
// see cxon/lang/json/class.hxx
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
    // json == "{ \"even\": [2, 4, 6], \"odd\": [1, 3, 5] }"
    assert(mv1 == mv2);
}
```

###### Example

``` c++
// traits and named parameters

#include "cxon/json.hxx"

#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"

#include <cassert>

// custom traits for given format
struct custom_traits : cxon::json::format_traits {
    // for example, UTF-8 validation can be disabled
    static constexpr bool read_validate_string_utf8 = false;
};
using TRJS = cxon::JSON<custom_traits>; // TRusted JSon

int main() {
    {   // custom traits use
        std::vector<std::string> cxx;
            cxon::from_bytes<TRJS>(cxx, R"(["trusted", "strings"])");
        assert(cxx == std::vector<std::string> ({"trusted", "strings"}));
    }
    {   // passing of a named parameter
        using namespace cxon::json;
        std::string json;
            cxon::to_bytes(json, 3.1415926, fp_precision::set<3>()); // floating-point precision
        assert(json == "3.14");
    }
}
```

###### Example

``` c++
// overriding of a default parser

#include "cxon/json.hxx"

#include <cassert>

namespace cxon {
    // override the default double parser for JSON<X>
    template <typename X>
        struct read<JSON<X>, double> {
            template <typename II, typename Cx>
                static bool value(double& t, II& i, II e, Cx& cx) {
                    // implement fast double parsing for example
                    return true;
                }
        };
}

struct custom_traits : cxon::json::format_traits {};
using CUSTOM_JSON = cxon::JSON<custom_traits>;

namespace cxon {
    // override the default float parser for CUSTOM_JSON, leave the default JSON<> as is
    template <>
        struct read<CUSTOM_JSON, float> {
            template <typename II, typename Cx>
                static bool value(float& t, II& i, II e, Cx& cx) {
                    // implement custom float parsing
                    return true;
                }
        };
}

int main() {
    double d = 0.0;
        cxon::from_bytes(d, "42");
    assert(d == 0.0);   // the override is used for double
    int i = 0;
        cxon::from_bytes(i, "42");
    assert(i == 42);    // the other types are intact
    float f = 0.0;
        cxon::from_bytes<CUSTOM_JSON>(f, "42");
    assert(f == 0.0);   // the override is used for CUSTOM_JSON
    unsigned u = 0;
        cxon::from_bytes<CUSTOM_JSON>(i, "42");
    assert(i == 42);    // the other types are as in JSON<>
}
```


*Somewhat more meaningful example can be found here [`JSON-RPC`](src/cxon/README.md#example-json-rpc).*


--------------------------------------------------------------------------------

#### Formats

##### [`JSON`](http://json.org)

The implementation strictly complies with [`RFC7159`][RFC7159] / [`ECMA-404`][ECMA-404].  
`CXON/JSON` also provides a polymorphic type [`cxon::json::node`](src/cxon/lang/json/node/README.md),
which can represent arbitrary `JSON` data.

##### [`CBOR`](https://cbor.io)

The implementation strictly complies with [`RFC7049`][RFC7049].  
`CXON/CBOR` also provides a polymorphic type [`cxon::cbor::node`](src/cxon/lang/cbor/node/README.md),
which can represent arbitrary `CBOR` data.


--------------------------------------------------------------------------------

#### Performance

- `CXON` deserialization using the default ([`<charconv>`][std-charconv]) floating-point conversion.  
  The difference is noticeable with number-heavy data (like `canada.json` or `numbers.json`).
  ![read/native](https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/g++.head.default.json.native-read.svg)

- `CXON` deserialization using [`fast_float`][ff] for floating-point conversion.  
  With the bottleneck removed, `CXON` is close *(see \<average\>)*.
  ![read/native (fast_float)](https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/g++.head.fast_float.json.native-read.svg)

- `CXON` serialization using the default ([`<charconv>`][std-charconv]) floating-point conversion.  
  `CXON` is somewhat slower, but not by much.
  ![write/native (default)](https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/g++.head.default.json.native-write.svg)

- `CXON` binary size and compilation times.  
  ![space (default)](https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/g++.head.default.json-space.svg)  
  ![space (default)](https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/clang++.head.default.json-space.svg)

Information about the benchmarks and additional benchmark results can be found [here](https://github.com/oknenavin/workflows-data/tree/develop/cxon).

*Given the benchmark results and assuming that the libraries `CXON` is compared to are reasonably well written,
it can be said that `CXON` satisfies the [zero-overhead][cpp-zeov] principle.  
It is important to note, that no specific attempts has been made to optimize `CXON` for time or space - 
there is hardly any compiler or `CPU` specific code, just pure `C++`.*

--------------------------------------------------------------------------------

#### Compilation

`CXON` requires [`C++11`][cpp-comp-support] compliant compiler, tested with `g++ >= 5`, 
`clang++ >= 3.5` and `msvc++ >= 14.1` (see the [builds](https://github.com/oknenavin/cxon/actions)).

*`CXON` uses [`<charconv>`][std-charconv] for floating-point conversions by default. By defining
`CXON_USE_FAST_FLOAT` and [`fast_float`][ff] present in the include path, the floating-point parsing
can be switched to it. This will give a good performance boost over current implementations of
[`<charconv>`][std-charconv] - see [performance](#performance) for details.*

--------------------------------------------------------------------------------

#### Installation

`CXON` is a header-only library - copy the headers you need, or use 
the provided makefile to install it on `POSIX` systems:

``` bash
$ sudo make install
```

or run the test suites with:

``` bash
$ make check
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
[img-lng]: https://img.shields.io/badge/language-C++11/14/17/20-608060.svg?style=plastic&logo=C%2B%2B
[img-fmt-json]: https://img.shields.io/badge/language-JSON-608060.svg?style=plastic&logo=JSON
[img-fmt-cbor]: https://img.shields.io/badge/language-CBOR-608060.svg?style=plastic
[img-lic]: https://img.shields.io/badge/license-MIT-608060.svg?style=plastic
[img-bld-lnx]: https://github.com/oknenavin/cxon/workflows/Linux/badge.svg
[img-bld-mac]: https://github.com/oknenavin/cxon/workflows/macOS/badge.svg
[img-bld-win]: https://github.com/oknenavin/cxon/workflows/Windows/badge.svg
[img-cov]: https://codecov.io/gh/oknenavin/cxon/branch/master/graph/badge.svg
[img-cod]: https://app.codacy.com/project/badge/Grade/a4b8981f7ce34dd5963f10723f8188bf
[img-cql]: https://github.com/oknenavin/cxon/workflows/CodeQL/badge.svg
[img-lgtm-qual]: https://img.shields.io/lgtm/grade/cpp/g/oknenavin/cxon.svg?logo=lgtm&logoWidth=18
[img-lgtm-alrt]: https://img.shields.io/lgtm/alerts/g/oknenavin/cxon.svg?logo=lgtm&logoWidth=18
[img-cvr]: https://scan.coverity.com/projects/18083/badge.svg
[RFC7159]: https://www.ietf.org/rfc/rfc7159.txt
[ECMA-404]: http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
[RFC7049]: https://tools.ietf.org/rfc/rfc7049.txt
[RFC8746]: https://tools.ietf.org/rfc/rfc8746.txt
[GitHub]: https://github.com/oknenavin/cxon
[cpp-comp-support]: https://en.cppreference.com/mwiki/index.php?title=cpp/compiler_support&oldid=108771
[cpp-alaw]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/AllocatorAwareContainer&oldid=128189
[cpp-fund-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[cpp-ptr]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/pointer&oldid=109738
[cpp-arr]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/array&oldid=111607
[cpp-enum]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/enum&oldid=111809
[cpp-class]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/class&oldid=101735
[cpp-zeov]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/Zero-overhead_principle&oldid=118760
[std-charconv]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/charconv&oldid=105120
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
[ff]: https://github.com/fastfloat/fast_float
