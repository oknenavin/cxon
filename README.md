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
<!--
[![Codacy][img-cod]](https://www.codacy.com/gh/oknenavin/cxon/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=oknenavin/cxon&amp;utm_campaign=Badge_Grade)
[![CodeQL][img-cql]](https://github.com/oknenavin/cxon/actions?query=workflow%3ACodeQL)
[![LGTM][img-lgtm-qual]](https://lgtm.com/projects/g/oknenavin/cxon/context:cpp)
[![LGTM/Alerts][img-lgtm-alrt]](https://lgtm.com/projects/g/oknenavin/cxon/alerts/)
-->


--------------------------------------------------------------------------------

  - `CXON` is a C++ serialization interface  
  - `CXON` implements [`JSON`](http://json.org) (`UTF-8` encoded) as a serialization format (an example of a text-based data format)  
  - `CXON` implements [`CBOR`](https://cbor.io) as a serialization format (example of a binary data format)  
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

Successful deserialization means that the input is syntactically and semantically correct.

In contrast, most of the `JSON`/`CBOR`/etc. libraries represent arbitrary data
with a polymorphic type (called `DOM`, `value`, etc.), and successful parsing of the input only means,
that it is syntactically correct.

###### Example
``` c++
// pseudo code
value_type json = json_lib::parse("[1, 2, 3]");
assert(json.has_parsing_errors()); // check for syntax errors
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
// the input is semantically correct, bu
// the values still need special attention
int x0 = array[0].get_integer(); // it's an int, but not quite
...
```

To help with this, some of the libraries provide utilities to convert the value type to a
`C++` type - e.g. `Boost.JSON` provides `value_from` / `value_to`.
For completeness, `CXON` also provides polymorphic types (called `node`) for the supported formats
that match the functionality provided by these libraries.

The **performance** is often important and is emphasized by many libraries and in this respect,
`CXON` is [close to the alternatives](#performance).  
Many libraries emphasize the floating-point serialization and deserialization performance.
`CXON` uses [`<charconv>`][std-charconv] by default (with a fallback implementation for `C++11`),
but can be configured to use [`boost::charconv`][lib-boost-charconv] by defining `CXON_USE_BOOST_CHARCONV`.  
Note here, that libraries based on polymorphic types have validation and use overhead
that should be taken into account.

The **memory management** is often important. `CXON` does not allocate in general,
it's up to the types provided.  
In the example above, the memory management will be handled completely by `std::vector`
and its allocator (whatever it is).  
The polymorphic types provided by `CXON` are also [AllocatorAware][cpp-alaw] compliant.

`CXON` is **non-throwing**, provided that the serializers involved do not throw.

--------------------------------------------------------------------------------

#### Contents
  - [Overview](#overview)
  - [Formats](#formats)
  - [Performance](#performance)
  - [Compilation](#compilation)
  - [Installation](#installation)
  - [Documentation](doc/README.md)
  - [Examples](examples/README.md)
  - [Contributing](#contributing)


--------------------------------------------------------------------------------

#### Overview

`CXON` defines and implements an interface similar to`C++17`'s [`<charconv>`][std-charconv].  
`CXON` extends [`<charconv>`][std-charconv]'s interface with:

  - traits template parameter (support for different serialization formats, 
    see [`Format traits`](src/cxon/README.md#format-traits))
  - trailing named parameters of arbitrary types (passing of parameters to specific 
    type serializers, see [Named parameters](src/cxon/README.md#named-parameters)
  - input and output iterators for I/O (allowing streams, containers and arrays, 
    see [`Interface`](src/cxon/README.md#interface))

The traits can be stateful or stateless allowing arbitrary complex formats.  
Named parameters can be compile time or runtime giving flexibility for the implementations.  

*More about the interface can be found in the [`MANUAL`](src/cxon/README.md#interface).*

`CXON` supports good part of `C++`'s fundamental, compound and standard library types out of the box.  
`CXON` can be extended for arbitrary types, using intrusive and non-intrusive methods.  

*More details can be found in the [`MANUAL`](src/cxon/README.md#supported-types).*


--------------------------------------------------------------------------------

#### Formats

##### [`JSON`](http://json.org)

The implementation strictly complies with [`RFC8259`][RFC8259] / [`ECMA-404`][ECMA-404].  
`CXON/JSON` also provides a polymorphic type [`cxon::json::node`](src/cxon/lang/json/node/README.md),
which can represent arbitrary `JSON` data.

##### [`CBOR`](https://cbor.io)

The implementation complies with [`RFC7049`][RFC7049].  
`CXON/CBOR` also provides a polymorphic type [`cxon::cbor::node`](src/cxon/lang/cbor/node/README.md),
which can represent arbitrary `CBOR` data.

##### [`CXCF`](src/cxon/lang/cxcf/README.md)

`CXCF` is a simple configuration format derived from `JSON`.


--------------------------------------------------------------------------------

#### Performance

- `CXON` deserialization using the default ([`<charconv>`][std-charconv]) floating-point conversion.  
  ![read/native][img-time-read-gcc]  
  ![read/native][img-time-read-clang]

- `CXON` serialization using the default ([`<charconv>`][std-charconv]) floating-point conversion.  
  ![write/native (default)][img-time-write-gcc]  
  ![write/native (default)][img-time-write-clang]

- `CXON` binary size and compilation times.  
  ![space (default)][img-space-gcc]  
  ![space (default)][img-space-clang]

Information about the benchmarks and additional benchmark results can be found [here][img-bench].

*Given the benchmark results and assuming that the libraries `CXON` is compared to are reasonably well written,
it can be said that `CXON` satisfies the [zero-overhead][cpp-zeov] principle.*

--------------------------------------------------------------------------------

#### Compilation

`CXON` requires [`C++11`][cpp-comp-support] compliant compiler, tested with `g++ >= 5`, 
`clang++ >= 4.0` and `msvc++ >= 19.16` (see the [builds](https://github.com/oknenavin/cxon/actions)).

*`CXON` is using [`<charconv>`][std-charconv] for numeric conversions if available.  
If not (ex. pre-`C++17`) a fallback implementation (based on `strto*` and `sprintf`) will be used.  
If `CXON_USE_BOOST_CHARCONV` is defined, [`boost::charconv`][lib-boost-charconv] will be used.
This will provide a good performance boost for `C++11/14` and some earlier implementations of
[`<charconv>`][std-charconv].*

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
[img-cvr]: https://scan.coverity.com/projects/18083/badge.svg

<!--{ branch-links -->
[img-cov]: https://codecov.io/gh/oknenavin/cxon/branch/develop/graph/badge.svg
[img-time-read-gcc]: https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/g++.head.default.json.native-s1-read.svg
[img-time-read-clang]: https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/clang++.head.default.json.native-s1-read.svg
[img-time-write-gcc]: https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/g++.head.default.json.native-s1-write.svg
[img-time-write-clang]: https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/clang++.head.default.json.native-s1-write.svg
[img-space-gcc]: https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/g++.head.default.json-space.svg
[img-space-clang]: https://raw.githubusercontent.com/oknenavin/workflows-data/develop/cxon/benchmarks/figures/clang++.head.default.json-space.svg
[img-bench]: https://github.com/oknenavin/workflows-data/tree/develop/cxon
<!-- branch-links }-->

[RFC8259]: https://www.ietf.org/rfc/rfc8259.txt
[ECMA-404]: http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
[RFC7049]: https://tools.ietf.org/rfc/rfc7049.txt
<!--[RFC8746]: https://tools.ietf.org/rfc/rfc8746.txt-->
[GitHub]: https://github.com/oknenavin/cxon

[std-charconv]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/charconv&oldid=105120
[lib-boost-charconv]: https://github.com/boostorg/charconv
[cpp-alaw]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/AllocatorAwareContainer&oldid=128189
[cpp-zeov]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/Zero-overhead_principle&oldid=118760
[cpp-comp-support]: https://en.cppreference.com/mwiki/index.php?title=cpp/compiler_support&oldid=108771
