## `CXON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------

#### Contents
  - [Introduction](#introduction)
    - Formats
      - [`JSON`](lang/json/README.md)
      - [`CXCF`](lang/cxcf/README.md)
    - [Supported types](#supported-types)
  - [Interface](#interface)
  - [Implementation bridge](#implementation-bridge)
  - [Parametrization](#parametrization)
    - [Format traits](#format-traits)
    - [Named parameters](#named-parameters)
    - [Context](#context)


--------------------------------------------------------------------------------

#### Introduction

`CXON` defines and implements an interface similar to`C++17`'s [`<charconv>`][std-charconv] [interface](#interface).  
`CXON` extends [`<charconv>`][std-charconv]'s interface with:

  - traits template parameter (support for different serialization formats, 
    see [`Format traits`](#format-traits))
  - trailing named parameters of arbitrary types (passing of parameters to specific 
    type serializers, see [Named parameters](#named-parameters)
  - input and output iterators for I/O (allowing streams, containers and arrays, 
    see [`Interface`](#interface))

##### Supported types

- `CXON` `C++`'s fundamental, compound and standard library types:
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
- `CXON` [`Boost/Containers`][boost-containers]
  - [`Container`][boost-container]
  - [`Dynamic Bitset`][boost-dynbitset]
  - [`Variant`][boost-variant]
  - [`Variant2`][boost-variant2]

`CXON` can be extended for arbitrary types, using intrusive and non-intrusive methods
(see the [`implementation-bridge`](#implementation-bridge) for details).


--------------------------------------------------------------------------------

#### Interface

*Defined in header [cxon.hxx](cxon.hxx), include `cxon/<format>.hxx`*

##### Read interface

``` c++
namespace cxon {

    template <typename Traits, typename T, typename InIt, typename ...NaPa>
        from_bytes_result<It>  from_bytes(T& t, InIt b, InIt e, NaPa&&... p);       (1)
    template <typename Traits, typename T, typename Iterable, typename ...NaPa>
        from_bytes_result<It)> from_bytes(T& t, const Iterable& i, NaPa&&... p);    (2)

    template <typename It>
        struct from_bytes_result {
            std::error_condition ec;
            It end;
            explicit operator bool() const noexcept;
        };

}
```

###### Template parameters

  - [`Traits`](#format-traits) - trait type that defines a given serialization format
  - `T` - the type of the value to serialize
  - `InIt` - the type of the iterator, must meet [InputIterator][cpp-init] requirements
  - `Iterable` - a type, for which `std::begin(i)` and `std::end(i)` are defined
  - `NaPa` - named parameter types (see [Context](#context))
  - `It` - an iterator
    - `(1)` `InIt`
    - `(2)` `decltype(std::begin(i))`

###### Parameters

  - `t` - the output value
  - `b`, `e` - a valid (`char`) range to parse
  - `i` - an iterable value representing a valid range (`char`) to parse
  - `p...` - named parameters (see [Context](#context))

###### Return value

On success, returns a value of type `from_bytes_result`, such that `end` is one-past-the-end iterator of
the matched range, or has a value equal to `e`, if the whole range match. `ec` is value initialized.  
On failure, returns a value of type `from_bytes_result`, such that `end` is an iterator pointing to
the non-matching input, and `ec` contains the [error condition][std-err-cnd]. The value is in valid, but
unspecified state.

###### Exceptions

Does not throw by itself, however specializations may throw or not:
  - for fundamental types - does not throw
  - for pointers - allocators and constructors may throw
  - for standard library types - constructing and adding of elements may throw with 
    the same guarantees as given by the standard library
  - for user types - may throw or not depending of the implementation

###### Example

``` c++
#include "cxon/json.hxx"
#include "cxon/lib/std/vector.hxx"

#include <cstring>
#include <cassert>

int main() {
    using namespace cxon;
    {   double v;
            auto const r = from_bytes(v, "42");
        assert(r && v == 42);
    }
    {   int v;
            auto const r = from_bytes(v, "42");
        assert(r && v == 42);
    }
    {   std::vector<float> v;
            auto const r = from_bytes(v, "[42, true]");
        assert( !r && // fails with error r.ec and location r.end
                r.ec == json::read_error::floating_point_invalid &&
                std::strcmp(r.end, "true]") == 0
        );
    }
}
```


##### Write interface

``` c++
namespace cxon {

    template <typename Traits, typename T, typename OutIt, typename ...NaPa>
        to_bytes_result<It> to_bytes(OutIt o, const T& t, NaPa&&... p);             (1)
    template <typename Traits, typename T, typename Insertable, typename ...NaPa>
        to_bytes_result<It> to_bytes(Insertable& i, const T& t, NaPa&&... p);       (2)
    template <typename Traits, typename T, typename FwIt, typename ...NaPa>
        to_bytes_result<It> to_bytes(FwIt b, FwIt e, const T& t, NaPa&&... p);      (3)

    template <typename It>
        struct to_bytes_result {
            std::error_condition ec;
            It end;
            explicit operator bool() const noexcept;
        };

}
```

###### Template parameters

  - [`Traits`](#format-traits) - traits type that defines a given serialization format
  - `T` - the type of the value to serialize
  - `OutIt` - the type of the iterator, must meet [OutputIterator][cpp-outit] requirements
  - `Insertable` - a type, for which `std::back_inserter(i)` and `std::begin(i)` are defined
  - `FwIt` - the type of the iterator, must meet [ForwardIterator][cpp-fwit] requirements
  - `NaPa` - named parameter types (see [Context](#context))
  - `It` - an iterator
    - `(1)` `OutIt`
    - `(2)` `decltype(std::begin(i))`
    - `(3)` `FwIt`

###### Parameters

  - `o` - an output iterator to write to
  - `i` - a back-insertable to write to
  - `b`, `e` - a (`char`) range to write to
  - `t` - the input value
  - `p...` - named parameters (see [Context](#context))

###### Return value

On success, returns a value of type `to_bytes_result`, such that `ec` is value-initialized, and `end` is:
  - `(1)` one-past-the-end output iterator
  - `(2)` `std::begin(i)` + number of the bytes written
  - `(3)` one-past-the-end iterator of the output written. Note that the output is not terminated.

On failure, returns a value of type `to_bytes_result`, such that `ec` contains the error condition, and 
`end` has the same value as in case of success.

###### Exceptions

Does not throw by itself, however writing to the output may throw (e.g. adding to a container).

###### Example

``` c++
#include "cxon/json.hxx"
#include "cxon/lib/std/vector.hxx"
#include <cassert>

int main() {
    using namespace cxon;
    {   std::string v;
            auto const r = to_bytes(v, 42);
        assert(r && v == "42");
    }
    {   std::string v;
            auto const r = to_bytes(v, "42");
        assert(r && v == "\"42\"");
    }
    {   char v[4];
            auto const r = to_bytes(std::begin(v), std::end(v), std::vector<int>{ 4, 2 });
        assert(!r && r.ec == json::write_error::output_failure);
    }
}
```


--------------------------------------------------------------------------------

#### Implementation Bridge

The interface communicates the implementation via the so-called *implementation bridge*.  
Read/write interfaces instantiate the [*context*](#context) with the named parameters (if any) 
and then call the *implementation bridge* with it:

``` c++
namespace cxon {

    template <typename X, typename T, typename InIt, typename Cx>
        bool read_value(T& t, InIt& i, InIt e, Cx& cx) {
            ...
        }
    template <typename X, typename T, typename OutIt, typename Cx>
        bool write_value(OutIt& o, const T& t, Cx& cx) {
            ...
        }

}
```

and this is the first non-intrusive way to extend `CXON` for a type - to implement it 
in namespace `cxon`.

The _implementation bridge_ however, bridges three additional methods of extension:

  - specialization of read/write structures for the type (non-intrusive, allows partial specialization)
    ``` c++
    namespace cxon {

        template <typename X>
            struct read<X, T/*, optional enabler */> {
                template <typename InIt, typename Cx>
                    static bool value(T& t, InIt& i, InIt e, Cx& cx) {
                        ...
                    }
            };
        template <typename X>
            struct write<X, T/*, optional enabler */> {
                template <typename OutIt, typename Cx>
                    static bool value(OutIt&, const T& t, Cx& cx) {
                        ...
                    }
            };

    }
    ```
  - static methods provided by the type
    ``` c++
    struct T {
        template <typename X, typename InIt, typename Cx>
            static bool read_value(T& t, InIt& i, InIt e, Cx& cx) {
                ...
            }
        template <typename X, typename OutIt, typename Cx>
            static bool write_value(OutIt& o, const T& t, Cx& cx) {
                ...
            }
    };
    ```
  - methods provided by the type
    ``` c++
    struct T {
        template <typename X, typename InIt, typename Cx>
            bool read_value(InIt& i, InIt e, Cx& cx) {
                ...
            }
        template <typename X, typename OutIt, typename Cx>
            bool write_value(OutIt& o, Cx& cx) const {
                ...
            }
    };
    ```


--------------------------------------------------------------------------------

#### Parametrization

`CXON` provides two ways to parametrize a given implementation:
  - [Format traits](#format-traits)
    - for passing of static parameters (e.g. to configure the format)
    - for passing of state parameters (e.g. to keep serialization state)
  - [Named parameters](#named-parameters)
    - for passing of constant (including constexpr) or mutable parameters to arbitrary type 
      serializers (e.g. floating-point precision, allocator, etc.)


--------------------------------------------------------------------------------

##### Format Traits

The purpose of `Traits` template parameter is to keep static and/or state 
parameters for a given serialization format.
As an example, `JSON` uses the following format traits structure:

``` c++
namespace cxon { namespace json {

    struct format_traits {
        struct map;     // key/value types such as std::map, struct/class, etc.
        struct list;    // array/list/set like types
        struct string;  // string types
        struct number;  // number types
        struct id;      // identifiers (true/false/null)
    };

}}
```

and for example, `map` is defined like this:

``` c++
namespace cxon { namespace json {

    struct map {
        static constexpr char beg           = '{';    // object begin
        static constexpr char end           = '}';    // object end
        static constexpr char div           = ':';    // key/value separator
        static constexpr char sep           = ',';    // key/value list separator
    };

}}
```

Because changing of `Traits` parameters  can only be done by introducing a new type,
 specialization of a serializer for a given format is not easy possible and because of this, 
`CXON` uses so-called *format-selectors* 
defined like this:

``` c++
namespace cxon {

    template <typename Traits>
        struct format_selector : Traits {};

    ...

    template <typename Traits>
        struct JSON : format_selector<Traits> {};

}
```

and the interface is called with a *format-selector* instead of a bare traits:

``` c++
struct my_json_traits : cxon::json::format_traits {
    struct map : cxon::json::format_traits::map {
        static constexpr char div = '='; // key/value separator
    };
};
using my_json = cxon::JSON<my_json_traits>;
...
auto const result = cxon::from_bytes<my_json>(...);
...
```

With this mechanism, specialization for a given type and format can be done with 
code like this:

``` c++
namespace cxon {

    template <typename X>
        struct write<CBOR<X>, Type> {
            template <typename O, typename Cx>
                static bool value(O& o, const Type& t, Cx& cx) {
                    ...
                }
        };

    template <typename X, typename II, typename Cx>
        auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON> {
            ...
        }

}
```

*Here, the helper type `cxon::enable_for_t` is a convenience typedef similar to 
[`std::enable_if`][std-enab-if].*


The implementation defines the following parameters, which can be used to tune
the behavior of the library or to change the `JSON` language:

``` c++
namespace cxon { namespace json { // format traits

    struct format_traits : cio::format_traits {
        // read: check UTF-8 encoding
        static constexpr bool validate_string_encoding  = true;

        // read: check for unescaped control characters
        static constexpr bool validate_string_escapes   = true;

        // write: escape U+2028 LINE SEPARATOR and U+2029 PARAGRAPH SEPARATOR (ECMA-262, 12.3 Line Terminators)
        static constexpr bool produce_strict_javascript = false;

        // read/write: assume that strings to be read/written do not contain escape characters
        static constexpr bool assume_no_escapes = false;

        // object keys for types serialized without quotes will be quoted (e.g. strings will still be quoted, but numbers will not)
        // if false, this JSON {1: 2} will now be valid
        static constexpr bool quote_unquoted_keys = true;
        // object keys for types serialized with quotes will be unquoted (e.g. strings)
        // if true, this JSON {key: 1} will now be valid
        // any whitespace characters and object/key separator character
        // must be escaped - e.g. { a\ key: 1}
        static constexpr bool unquote_quoted_keys = false;

        // allow c++-style comments
        static constexpr bool allow_cxx_comments        = false;

        // allow bash-style comments
        static constexpr bool allow_bash_comments       = false;

        // allow trailing separators for objects and arrays
        static constexpr bool allow_trailing_separators = false;

        // controls whether class-types serializer should allow repeating of a key
        // e.g. {key: 1, key: 2} will be valid or not
        static constexpr bool assume_unique_object_keys = true;

        // allow NaN and Infinity as a floating-point numbers
        // by default these are serialized as strings "inf", "-inf", "nan", "-nan"
        static constexpr bool allow_javascript_nans     = false;
    };

}}
```

The correctness parameters can also be set with preprocessor macros:

Parameter                 | Macro
--------------------------|------------------------------------
validate_string_encoding  | CXON_JSON_VALIDATE_STRING_ENCODING
validate_string_escapes   | CXON_JSON_VALIDATE_STRING_ESCAPES
produce_strict_javascript | CXON_JSON_PRODUCE_STRICT_JAVASCRIPT
assume_no_escapes         | CXON_JSON_ASSUME_NO_ESCAPES


###### Example

``` c++
// support for unquoted keys
#include "cxon/json.hxx"
#include "cxon/lib/std/map.hxx" // <map>
#include <string>
#include <cassert>

struct unquoted_keys_traits : cxon::json::format_traits {
    static constexpr bool quote_unquoted_keys = false;
};
using UK = cxon::JSON<unquoted_keys_traits>;

int main() {
    std::map<int, int> map;
        cxon::from_bytes<UK>(map, R"({1: 2, 3: 4})");
    assert(map == (std::map<int, int> {{1, 2}, {3, 4}}));
    std::string str;
        cxon::to_bytes<UK>(str, map);
    assert(str == R"({1:2,3:4})");
}
```
``` c++
// support for unquoted string keys
#include "cxon/json.hxx"
#include "cxon/lib/std/map.hxx" // <map>
#include "cxon/lib/std/string.hxx" // <string>
#include <cassert>

struct unquoted_string_keys_traits : cxon::json::format_traits {
    static constexpr bool unquote_quoted_keys = true;
};
using UQK = cxon::JSON<unquoted_string_keys_traits>;

int main() {
    std::map<std::string, int> map;
        cxon::from_bytes<UQK>(map, R"({one: 1, two: 2})");
    assert(map == (std::map<std::string, int> { {"one", 1}, {"two", 2}}));
    std::string str;
        cxon::to_bytes<UQK>(str, map);
    assert(str == R"({one:1,two:2})");
}
```


--------------------------------------------------------------------------------

##### Named parameters

A named parameter is an instance of `napa::parameter` with a value of an arbitrary type.  
`napa::parameter` provides various methods to access the value from the [Context](#context).

``` c++
namespace cxon::napa {

    template <typename Tag, typename Type>
        struct parameter {
            using tag = Tag;
            using tag_type = Type;

            template <typename Pk>
                struct in {                                     (1)
                    static bool value;
                }

            template <Type c>
                static constexpr auto set();                    (2)
            static constexpr auto set(Type&& v);                (3)
            static constexpr auto set(const Type& v);           (4)

            template <typename Pk>
                static constexpr Type constant(Type dflt);      (5)

            template <typename Pk>
                static Type& reference(Pk& p);                  (6)
            template <typename Pk>
                static Type value(const Pk& p);                 (7)

            template <typename Pk>
                static Type value(const Pk&, Type dflt);        (8)
        };

}
```

- `(1)` - `value` member is `true` if parameter `Tag` is in `Pk` and `false` otherwise
- `(2)` - creates `constexpr` parameter `Tag` with type `Type`
- `(3)`, `(4)` - create parameter `Tag` with type `Type`
- `(5)` - returns the value of the `constexpr` parameter `Tag` if set, `dflt` otherwise
- `(6)` - returns a reference of the value of parameter `Tag`, compilation error if not set
- `(7)` - returns the value of parameter `Tag`, compilation error if not set
- `(8)` - returns the value of parameter `Tag` if set, `dflt` otherwise

For convenience, a simple macro is provided to implement a parameter:

``` c++
#define CXON_PARAMETER(P, T) struct P : cxon::napa::parameter<P, T> {}
```

###### Example

``` c++
#include "cxon/json.hxx"

CXON_PARAMETER(my_constant, unsigned);
CXON_PARAMETER(my_state, int);

struct my_type { ... };

namespace cxon {

    template <typename X, typename II, typename Cx>
        inline auto read_value(my_type& t, II& i, II e, Cx& cx)
            // specialize if my_state is set
            -> enable_if_t< my_state::in<napa_type<Cx>>::value, bool>
        {
            char buffer[my_constant::constant<napa_type<Cx>>(32)]; // constexpr, 32 if not set
            auto& state = my_state::reference(cx.px);
            ++state;
            ...
        }

    template <typename X, typename II, typename Cx>
        inline auto read_value(my_type& t, II& i, II e, Cx& cx)
            // specialize if my_state isn't set
            -> enable_if_t<!my_state::in<napa_type<Cx>>::value, bool>
        {
            auto par = my_state::value(cx.px, 0); // 0 if not set
            ...
        }

}

int main() {
    my_type mv;
        auto res = cxon::from_bytes(mv, "...",
            my_constant::set<42>(), // constexpr parameter
            my_state::set(42) // runtime parameter
        );
}
```


--------------------------------------------------------------------------------

##### Context

The context is created by the interface with the format traits and the named parameters
and then passed to the implementation bridge.

``` c++
namespace cxon {

    template <typename X, typename ...NaPa>
        struct context;

}
```

###### Template parameters

  - `X` - format traits type
  - `Napa` - named parameter types

###### Member types

Member type |Definition
--------------|------------------------------------------
`traits_type` | `X`
`napa_type`   | `napa::pack_type<NaPa...>`

###### Member objects

Member name |Type                                   | Description
------------|---------------------------------------|-----------------
`ec`        | [`std::error_condition`][std-err-cnd] | error condition
`tx`        | `traits_type`                         | format traits
`px`        | `napa_type`                           | named parameters

###### Member functions

- `operator /` - assign an error condition enum
    ``` c++
    template <typename E>
        auto operator /(E e) noexcept -> enable_if_t<std::is_error_condition_enum<E>::value, bool>;
    ```
- `operator bool` - check if the context is good (i.e. no error condition)
    ``` c++
    explicit operator bool() const noexcept;
    ```

napa_type (`px`'s type) is a tagged tuple with parameter types as tags. 
See [Named parameters](#named-parameters).


--------------------------------------------------------------------------------

Distributed under the terms of the GNU Affero General Public License. See [`LICENSE`](../../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060

[cpp-init]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/InputIterator&oldid=103892
[cpp-outit]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/OutputIterator&oldid=108758
[cpp-fwit]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/ForwardIterator&oldid=106013
[cpp-comp-support]: https://en.cppreference.com/mwiki/index.php?title=cpp/compiler_support&oldid=108771
[cpp-alaw]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/AllocatorAwareContainer&oldid=128189
[cpp-fund-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[cpp-ptr]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/pointer&oldid=109738
[cpp-arr]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/array&oldid=111607
[cpp-enum]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/enum&oldid=111809
[cpp-class]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/class&oldid=101735
[cpp-zeov]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/Zero-overhead_principle&oldid=118760

[std-enab-if]: https://en.cppreference.com/mwiki/index.php?title=cpp/types/enable_if&oldid=109334
[std-err-cnd]: https://en.cppreference.com/mwiki/index.php?title=cpp/error/error_condition&oldid=88237
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

[boost-containers]: https://www.boost.org/doc/libs/1_81_0/?view=category_containers
[boost-container]: https://www.boost.org/doc/libs/1_81_0/doc/html/container.html
[boost-dynbitset]: https://www.boost.org/doc/libs/1_81_0/libs/dynamic_bitset/dynamic_bitset.html
[boost-variant]: https://www.boost.org/doc/libs/1_81_0/doc/html/variant.html
[boost-variant2]: https://www.boost.org/doc/libs/1_81_0/libs/variant2/doc/html/variant2.html
