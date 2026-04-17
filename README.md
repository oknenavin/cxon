## `CXON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  
[![Language][img-lng]](https://isocpp.org/wiki/faq/cpp14)
[![Format][img-fmt-json]](http://json.org)
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
  - `CXON` implements [`JSON`](http://json.org) (`UTF-8` encoded) as a serialization format
  - `CXON` is easy to extend for different formats and types with [zero-overhead][cpp-zeov]  
  - `CXON` is a `C++14` compliant, self contained and compact header-only library  

Unlike most of the "`JSON`" libraries (e.g. `Boost.JSON`, `RapidJSON`, etc.) that serialize `JSON` to/from a polymorphic type, 
`CXON` serializes directly to/from user provided types. Polymorphic types provide syntactic correctness, user types provide
semantic correctness in addition.

###### Example
``` c++
#include "cxon/json.hxx"
#include "cxon/lib/std/vector.hxx" // for <vector>
#include <cassert>

int main() {
    std::vector<int> cxx; // or std::array, std::list, std::set, etc.
        // the input is a JSON array, semantically a list of integers
        auto result = cxon::from_bytes(cxx, "[1, 2, 3]");
    assert(result);
    // the data is loaded successfully, no additional semantic validation is needed, so
    assert(cxx == (std::vector<int> {1, 2, 3}));
}
```
So, successful serialization guarantees syntactic and semantic correctness.  
In contrast, libraries using polymorphic types only validate the syntax:

###### Example
``` c++
// pseudo code
value_type json = json_lib::parse("[1, 2, 3]");
assert(json.has_parsing_errors()); // check for syntax errors
// check the semantics, expected is a list of integers
assert(json.is_array()); // check the type
auto& array = json.get_array();
assert( // check the values
    array.size() == 3 &&
    array[0].is_integer() &&
    array[1].is_integer() &&
    array[2].is_integer() &&
    // further value checks if needed
);
// the input is semantically correct, but
// the values still need special attention
int x0 = array[0].get_integer(); // it's an int, but not quite
```

Some libraries provide conversion utilities like `value_from()`/`value_to()` for their polymorphic types, but this adds overhead.  
`CXON` serializes directly into a user provided types, enabling optimizations that generic polymorphic types cannot.  
For completeness, `CXON` also provides polymorphic types (called `node`) for the supported formats.

The **performance** is important in many cases and `CXON` [demonstrates its premise](#performance).  
Floating-point serialization is inherently complex - `CXON` addresses this with [`<charconv>`][std-charconv] by default (available in `C++17`).  
For earlier standards, [`boost::charconv`][lib-boost-charconv] can be used by defining `CXON_USE_BOOST_CHARCONV`.  
Slower, pre-`C++17` standard library implementation is also provided.

The **memory management** is delegated to the types provided - `CXON` itself does not allocate. 
The library respects type's allocator - e.g. serializing into `std::vector` respects its allocator.  
CXON's polymorphic types are [`AllocatorAware`][cpp-alaw] containers, following standard library conventions.

`CXON` is non-throwing - exception safety depends on the types provided.

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

`CXON` defines and implements an interface similar to `C++17`'s [`<charconv>`][std-charconv].  
`CXON` extends [`<charconv>`][std-charconv]'s interface with:

  - traits template parameter (support for different serialization formats, 
    see [`Format traits`](src/cxon/README.md#format-traits))
  - trailing named parameters of arbitrary types (passing of parameters to specific 
    type serializers, see [Named parameters](src/cxon/README.md#named-parameters)
  - input and output iterators for I/O (allowing streams, containers and arrays, 
    see [`Interface`](src/cxon/README.md#interface))

The traits can be stateful or stateless allowing arbitrary complex formats.  
Named parameters can be compile-time or run-time giving flexibility for the implementations.  

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

##### [`CXCF`](src/cxon/lang/cxcf/README.md)

`CXCF` is a simple configuration format derived from `JSON`.


--------------------------------------------------------------------------------

#### Performance

- `CXON` deserialization using the default ([`<charconv>`][std-charconv]) number conversion.  
  ![read/native][img-time-read-gcc]  
  ![read/native][img-time-read-clang]

- `CXON` serialization using the default ([`<charconv>`][std-charconv]) number conversion.  
  ![write/native (default)][img-time-write-gcc]  
  ![write/native (default)][img-time-write-clang]

- `CXON` binary size and compilation times.  
  ![space (default)][img-space-gcc]  
  ![space (default)][img-space-clang]

Information about the benchmarks and additional benchmark results can be found [here][lnk-bench].

*Note: Boost.JSON and RapidJSON use faster but slightly imprecise number parsing algorithms by default.
In the benchmarks they are tested with full-precision enabled.*

*Given the benchmark results and assuming that the libraries `CXON` is compared to are reasonably well written,
it can be said that `CXON` satisfies the [zero-overhead][cpp-zeov] principle.*

--------------------------------------------------------------------------------

#### Compilation

`CXON` requires [`C++14`][cpp-comp-support] compliant compiler.
See the [builds](https://github.com/oknenavin/cxon/actions)) for the tested compilers.

*`CXON` is using [`<charconv>`][std-charconv] for numeric conversions if available.  
If not (ex. pre-`C++17`) a fallback implementation (based on `strto*` and `sprintf`) will be used.  
If `CXON_USE_BOOST_CHARCONV` is defined, [`boost::charconv`][lib-boost-charconv] will be used.
This will provide a good performance boost for `C++14` and some earlier implementations of
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

Distributed under the terms of the GNU Affero General Public License. See [`LICENSE`](LICENSE) for more information.  
[GitHub][GitHub]  


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060
[img-lng]: https://img.shields.io/badge/language-C++/14/17/20/23/26-608060.svg?style=plastic&logo=C%2B%2B
[img-fmt-json]: https://img.shields.io/badge/language-JSON-608060.svg?style=plastic&logo=JSON
[img-lic]: https://img.shields.io/badge/license-AGPLv3-608060.svg?style=plastic
[img-bld-lnx]: https://github.com/oknenavin/cxon/workflows/Linux/badge.svg
[img-bld-mac]: https://github.com/oknenavin/cxon/workflows/macOS/badge.svg
[img-bld-win]: https://github.com/oknenavin/cxon/workflows/Windows/badge.svg
[img-cvr]: https://scan.coverity.com/projects/18083/badge.svg

<!--{ branch-links -->
[img-cov]: https://codecov.io/gh/oknenavin/cxon/branch/master/graph/badge.svg
[img-time-read-gcc]: https://raw.githubusercontent.com/oknenavin/workflows-data/master/cxon/benchmarks/figures/g++.head.default.json.native-s1-read.svg
[img-time-read-clang]: https://raw.githubusercontent.com/oknenavin/workflows-data/master/cxon/benchmarks/figures/clang++.head.default.json.native-s1-read.svg
[img-time-write-gcc]: https://raw.githubusercontent.com/oknenavin/workflows-data/master/cxon/benchmarks/figures/g++.head.default.json.native-s1-write.svg
[img-time-write-clang]: https://raw.githubusercontent.com/oknenavin/workflows-data/master/cxon/benchmarks/figures/clang++.head.default.json.native-s1-write.svg
[img-space-gcc]: https://raw.githubusercontent.com/oknenavin/workflows-data/master/cxon/benchmarks/figures/g++.head.default.json-space.svg
[img-space-clang]: https://raw.githubusercontent.com/oknenavin/workflows-data/master/cxon/benchmarks/figures/clang++.head.default.json-space.svg
[lnk-bench]: https://github.com/oknenavin/workflows-data/tree/master/cxon
<!-- branch-links }-->

[RFC8259]: https://www.ietf.org/rfc/rfc8259.txt
[ECMA-404]: http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
[RFC7049]: https://tools.ietf.org/rfc/rfc7049.txt
<!--[RFC8746]: https://tools.ietf.org/rfc/rfc8746.txt-->
[GitHub]: https://github.com/oknenavin/cxon

[std-charconv]: https://en.cppreference.com/w/cpp/header/charconv.html
[lib-boost-charconv]: https://github.com/boostorg/charconv
[cpp-alaw]: https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer
[cpp-zeov]: https://en.cppreference.com/w/cpp/language/Zero-overhead_principle.html
[cpp-comp-support]: https://en.cppreference.com/w/cpp/compiler_support.html
