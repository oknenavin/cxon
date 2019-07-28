## `CXON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------
#### Contents
- [Introduction](#introduction)
- [Interface](#interface)
- [Implementation bridge](#implementation-bridge)
- [Parametrization](#parametrization)
  - [Format traits](#format-traits)
  - [Context](#context)
- [Example (`JSON-RPC`)](#example-json-rpc)


--------------------------------------------------------------------------------
#### Introduction

`CXON` defines and implements an interface, which is a generalization of `C++17`'s
[`<charconv>`][cpp-charconv] interface.

The default serialization format is `UTF-8` encoded `JSON`. The mapping between `C++` and `JSON`
types is as follow:
- [fundamental types][cpp-fund-types]
    - `nullptr_t` -> `null`
    - `bool` -> `true` or `false`
    - character types (`char`, `wchar_t`, `char16_t` and `char32_t`) -> `JSON` `string`
    - integral types (`signed` and `unsigned` `char`, `short`, `int`, `long`, `long long`) ->
      `JSON` `number`
    - floating-point types (`float`, `double`, `long double`) -> `JSON` `number`
- `enum` -> `JSON` `string`
- [compound types][cpp-struct] -> `JSON` `object`
- arrays -> `JSON` `array`
- pointers are represented as their value or `JSON` `null`
- standard library types
    - [`std::basic_string`][cpp-bstr] -> `JSON` `string`
    - [`std::tuple`][cpp-tuple] -> `JSON` `array`
    - [`std::pair`][cpp-pair] -> `JSON` `array`
    - [containers library][cpp-container]
      - maps ([std::map][cpp-map], [std:: unordered_map][cpp-umap],
              [std::multimap][cpp-mmap], [std:: unordered_multimap][cpp-ummap]) ->
        `JSON` `object` `(1)`
      - others -> `JSON` `array`
    - [`std::optional`][cpp-opt] -> underlying type
    - [`std::variant`][cpp-var] -> `JSON` `object`

*`(1)` [`ECMA-404(6)`](http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf)
specify this about object keys:*
  > *The JSON syntax does not impose any restrictions on the strings used as names,
  > __does not require that name strings be unique__*...

*and by choosing map or multi-map as a `C++` mapping type, value of an existing key
may be replaced or kept.*

Complete example with simple [`JSON-RPC`](https://www.jsonrpc.org/specification) implementation
may be found [at the end of the document](#example-json-rpc).


--------------------------------------------------------------------------------
#### Interface

*Defined in header [cxon.hxx](cxon.hxx)*

##### Read interface  

``` c++
namespace cxon {

    template <typename Traits, typename T, typename InIt, typename ...CxPs>
        from_chars_result<It> from_chars(T& t, InIt b, InIt e, CxPs... p);            (1)
    template <typename Traits, typename T, typename Iterable, typename ...CxPs>
        from_chars_result<It)> from_chars(T& t, const Iterable& i, CxPs... p);        (2)

    template <typename It>
        struct from_chars_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept;
        };
```

###### Template parameters
- [`Traits`](#format-traits) - traits class specifying given serialization format
- `T` - the type of the value to serialize
- `InIt` - the type of the iterator, must meet [InputIterator][cpp-init] requirements
- `Iterable` - a type, for which `std::begin(i)` and `std::end(i)` are defined
- `It` - an iterator
  - `(1)` `InIt`
  - `(2)` `decltype(std::begin(i))`
- `CxPs` - context parameter types (see [Context](#context))

###### Parameters
- `t` - the out-parameter where the parsed value is stored if successful
- `b`, `e` -  valid (`char`) range to parse
- `i` - a parameter representing a valid (`char`) range to parse
- `p...` - context parameters (see [Context](#context))

###### Context parameters

Parameter      | Context | Type                     | Default                             | Description
---------------|---------|--------------------------|-------------------------------------|-------------------------
`allocator`    | read    | [`Allocator`][cpp-alloc] | `std::allocator<T>`                 | `T*` allocator
`num_len_max`  | read    | `size_t`                 | 32 (integral) / 64 (floating-point) | number read buffer size
`ids_len_max`  | read    | `size_t`                 | 64                                  | token read buffer size

###### Return value
On success, returns a value of type `from_chars_result`, such that `end` is one-past-the-end iterator of
the matched range, or has the value equal to `e`, if the whole range match, and `ec` is value initialized.  
On failure, returns a value of type `from_chars_result`, such that `end` is an iterator pointing to
the non-matching input, and `ec` contains the [error condition][cpp-err-cnd]. The value is in valid, but
unspecified state.

Error code                         | Message
-----------------------------------|---------------------------------
read_error::ok                     | no error
read_error::unexpected             | unexpected input
read_error::character_invalid      | character invalid
read_error::integral_invalid       | integral invalid or out of range
read_error::floating_point_invalid | floating-point invalid
read_error::boolean_invalid        | boolean invalid
read_error::escape_invalid         | invalid escape sequence
read_error::surrogate_invalid      | invalid surrogate

###### Exceptions
Does not throw by itself, however specializations may throw or not:
- of fundamental types - does not throw
- of pointers - allocator and constructors may throw
- of standard library types - constructing and adding of elements may throw with the same guarantees as
  of the standard library
- of user types - may or may not throw depending of the implementation

###### Example

``` c++
#include "cxon/cxon.hxx"
#include <cassert>

int main() {
    using namespace cxon;
    {   double v;
            auto const r = from_chars(v, "42");
        assert(r && v == 42);
    }
    {   int v;
            auto const r = from_chars(v, "42");
        assert(r && v == 42);
    }
    {   std::vector<float> v;
            auto const r = from_chars(v, "[42, true]");
        assert( !r && // fails with error r.ec and location r.end
                r.ec == read_error::floating_point_invalid &&
                strcmp(r.end, "true]") == 0
        );
    }
}

```


##### Write interface  

``` c++
namespace cxon {

    template <typename Traits, typename T, typename OutIt, typename ...CxPs>
        to_chars_result<It> to_chars(OutIt o, const T& t, CxPs... p);                 (1)
    template <typename Traits, typename T, typename Insertable, typename ...CxPs>
        to_chars_result<It> to_chars(Insertable& i, const T& t, CxPs... p);           (2)
    template <typename Traits, typename T, typename FwIt, typename ...CxPs>
        to_chars_result<It> to_chars(FwIt b, FwIt e, const T& t, CxPs... p);          (3)

    template <typename It>
        struct to_chars_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept;
        };

}
```

###### Template parameters
- [`Traits`](#format-traits) - traits class specifying given serialization format
- `T` - the type of the value to serialize
- `OutIt` - the type of the iterator, must meet [OutputIterator][cpp-outit] requirements
- `Insertable` - a type, for which `std::back_inserter(i)` and `std::begin(i)` are defined
- `FwIt` - the type of the iterator, must meet [ForwardIterator][cpp-fwit] requirements
- `It` - an iterator
  - `(1)` `OutIt`
  - `(2)` `decltype(std::begin(i))`
  - `(3)` `FwIt`
- `CxPs` - context parameter types (see [Context](#context))

###### Parameters
- `o` - an output iterator to write to
- `i` - a back insertable value to write to
- `b`, `e` - a (`char`) range to write to
- `t` - the value to convert to its representation 
- `p...` - context parameter (see [Context](#context))

###### Context parameters

Parameter      | Context | Type  | Default                                | Description
---------------|---------|-------|----------------------------------------|-------------------------
`fp_precision` | write   | `int` | `std::numeric_limits<T>::max_digits10` | floating-point precision

###### Return value
On success, returns a value of type `to_chars_result`, such that `ec` is value-initialized, and `end` is:
- `(1)` one-past-the-end output iterator
- `(2)` `std::begin(i)`
- `(3)` one-past-the-end iterator of the output written. Note that the output is not terminated.

On failure, returns a value of type `to_chars_result`, such that `ec` contains the error condition, and
`end` has the same value as in case of success.

Error code                         | Message
-----------------------------------|---------------------------------
read_error::ok                     | no error
read_error::output_failure         | output cannot be written
read_error::argument_invalid       | invalid argument

###### Exceptions
Does not throw by itself, however writing to the output may throw (e.g. adding to a container).

###### Example

``` c++
#include "cxon/cxon.hxx"
#include <cassert>

int main() {
    using namespace cxon;
    {   std::string v;
            auto const r = to_chars(v, 42);
        assert(r && v == "42");
    }
    {   std::string v;
            auto const r = to_chars(v, "42");
        assert(r && v == "\"42\"");
    }
    {   char v[4];
            auto const r = to_chars(std::begin(v), std::end(v), std::vector<int>{ 4, 2 });
        assert(!r && r.ec == write_error::output_failure);
    }
}
```

##### Non-members

```c++
template <typename Traits, typename Out>
    struct indent_iterator;

template <typename Traits, typename OutIt>
    constexpr auto make_indenter(OutIt o, unsigned tab = 1, char pad = '\t');           (1)
template <typename Traits, typename Insertable>
    constexpr auto make_indenter(Insertable& i, unsigned tab = 1, char pad = '\t');     (2)
template <typename Traits, typename FwIt>
    constexpr auto make_indenter(FwIt b, FwIt e, unsigned tab = 1, char pad = '\t');    (3)
```

###### Template parameters
- [`Traits`](#format-traits) - traits class specifying given serialization format
- `OutIt` - the type of the iterator, must meet [OutputIterator][cpp-outit] requirements
- `Insertable` - a type, for which `std::back_inserter(i)` and `std::begin(i)` are defined
- `FwIt` - the type of the iterator, must meet [ForwardIterator][cpp-fwit] requirements

###### Parameters
- `o` - an output iterator to write to
- `i` - a back insertable value to write to
- `b`, `e` - a (`char`) range to write to
- `tab` - the number of `pad` characters to use for indentation 
- `pad` - character to use for indentation

###### Return value
`indent_iterator` instance.

###### Example

```c++
#include "cxon/cxon.hxx"
#include "cxon/pretty.hxx"
#include <map>
#include <vector>
#include <string>
#include <cassert>

int main() {
    using namespace cxon;
    std::map<std::string, std::vector<int>> const m = {
        {"even", {2, 4, 6}}, {"odd", {1, 3, 5}}
    };
    char const s0[] =
        "{\n"
        "  \"even\": [\n"
        "    2,\n"
        "    4,\n"
        "    6\n"
        "  ],\n"
        "  \"odd\": [\n"
        "    1,\n"
        "    3,\n"
        "    5\n"
        "  ]\n"
        "}"
    ;
    std::string s1;
        cxon::to_chars(cxon::make_indenter(s1, 4, ' '), m);
    assert(s1 == s0);
}
```

where `s1` will contain:

```json
{
    "even": [
        2,
        4,
        6
    ],
    "odd": [
        1,
        3,
        5
    ]
}
```


--------------------------------------------------------------------------------
#### Implementation Bridge

The interface communicates the implementation via the so-called *implementation bridge*.  
Read/write interfaces instantiate the [*context*](#context), with the `CxPs` parameters (if any)
and then calls the *implementation bridge* with it:

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

and it is the first non-intrusive way to extend `CXON` for a type - implementing it
in namespace `cxon`.

The _implementation bridge_ however, bridges three additional methods of extension:

- specialization of read/write structures for the type (non-intrusive)
    ``` c++
    namespace cxon {

        template <typename X>
            struct read<X, T> {
                template <typename InIt, typename Cx>
                    static bool value(T& t, InIt& i, InIt e, Cx& cx) {
                        ...
                    }
            };
        template <typename X>
            struct write<X, T> {
                template <typename OutIt, typename Cx>
                    static bool value(OutIt&, const T& t, Cx& cx) {
                        ...
                    }
            };

    }
    ```
- static members provided by the type
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
- members provided by the type
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

For convenience, core library also provides a set of simple, non-intrusive and intrusive macros
for binding of enumerations and compound types:

- [enumerations][cpp-enum]
    ``` c++
    // implements the read interface for enum Type
    #define CXON_ENUM_READ(Type, ...)
    // implements the write interface for enum Type
    #define CXON_ENUM_WRITE(Type, ...)
    // implements the read and write interfaces for enum Type
    #define CXON_ENUM(Type, ...)

    // defines enum value `Value`, which will be serialized as Name
    #define CXON_ENUM_VALUE_NAME(Name, Value)
    // defines enum value `Value`, which will be serialized as ##Value
    #define CXON_ENUM_VALUE_ASIS(Value)
    ```

    ###### Example

    ``` c++
    enum rgb { red, green, blue };

    CXON_ENUM(rgb,
        CXON_ENUM_VALUE_ASIS(red),
        CXON_ENUM_VALUE_NAME("green (1)", green),
        CXON_ENUM_VALUE_ASIS(blue)
    )

    ...

    std::vector<rgb> v0 = { rgb::red, rgb::green, rgb::blue };
    std::string s0;
        cxon::to_chars(s0, v0);
    assert(s0 == "[\"red\",\"green (1)\",\"blue\"]");
    std::vector<rgb> v1;
        cxon::from_chars(v1, s0);
    assert(v1 == v0);
    ```
- [compound types][cpp-struct]
    ``` c++
    // implements the read interface for type `Type`
    #define CXON_STRUCT_READ(Type, ...)
    // implements the write interface for type `Type`
    #define CXON_STRUCT_WRITE(Type, ...)
    // implements the read and write interfaces for type `Type`
    #define CXON_STRUCT(Type, ...)
    // and the same set for intrusive implementation
    #define CXON_STRUCT_READ_MEMBER(Type, ...)
    #define CXON_STRUCT_WRITE_MEMBER(Type, ...)
    #define CXON_STRUCT_MEMBER(Type, ...)
    
    // defines field `Field`, which will be serialized as Name
    #define CXON_STRUCT_FIELD_NAME(Name, Field)
    // defines field `Field`, which will be serialized as ##Field
    #define CXON_STRUCT_FIELD_ASIS(Field)
    // defines the key Name, which will be skipped during serialization (only meaningful for reading)
    #define CXON_STRUCT_FIELD_SKIP(Name)
    ```

    ###### Example

    ``` c++
    struct my_struct {
        int first;
        int second;
        int skip;
        bool operator ==(const my_struct& s) const { return first == s.first && second == s.second; }
    };
    CXON_STRUCT_READ(my_struct,
        CXON_STRUCT_FIELD_ASIS(first),
        CXON_STRUCT_FIELD_NAME("second field", second),
        CXON_STRUCT_FIELD_SKIP("skip")
    )
    CXON_STRUCT_WRITE(my_struct,
        CXON_STRUCT_FIELD_ASIS(first),
        CXON_STRUCT_FIELD_NAME("second field", second)
    )

    ...

    my_struct v0 = { 1, 2, 3 };
    std::string s0;
        cxon::to_chars(s0, v0);
    assert(s0 == "{\"first\":1,\"second field\":2}");
    my_struct v1;
        cxon::from_chars(v1, "{\"first\":1,\"second field\":2,\"skip\":42}");
    assert(v1 == v0);
    ```


--------------------------------------------------------------------------------
#### Parametrization

`CXON` provides two ways for parametrization, serving different purposes:
- [format-traits](#format-traits) - for parameterizing given serialization format,
  usually meaning that some property of the format changes. As an example,
  `unquoted_keys` parameter enables unquoted object keys.
- [context](#context) - for parameterizing the serialization of given type
  without changing of the format. As an example, `allocator` parameter allows
  using of a custom [allocator][cpp-alloc] while reading of a pointer types.


--------------------------------------------------------------------------------
##### Format Traits

The purpose of `Traits` template parameter is to keep parameters for given serialization format.
As an example, although `JSON` is the default serialization format, `CXON` defines a fall-back
format called the same - `CXON`, which is like somewhat relaxed `JSON` (e.g. object keys may not
be quoted) and its traits are defined like this:

``` c++
namespace cxon {

    struct format_traits {
        struct map;     // key/value types such as std::map, struct/class, etc.
        struct list;    // array/list/set like types
        struct string;  // string types
        struct number;  // number types
        struct id;      // identifiers such as JSON's true/false/null
        struct buffer;  // buffering, see further
    };

}
```

for example, `map` is defined as this:

``` c++
namespace cxon {

    struct format_traits {
        ...
        struct map {
            static constexpr char beg             = '{';
            static constexpr char end             = '}';
            static constexpr char div             = ':';
            static constexpr char sep             = ',';
            static constexpr bool unquoted_keys   = true;
        };
        ...
    };

}
```

and `JSON` reuses it like this:

``` c++
namespace cxon {

    struct format_traits;

    struct json_format_traits : format_traits {
        ...
        struct map : format_traits::map {
            static constexpr bool unquoted_keys   = false;
        };
        ...
    };

}
```

As changing of a `Traits` parameter requires new type, specialization for given format
is not directly possible and because of this, `CXON` uses so-called *format-selectors*
defined like this:

``` c++
namespace cxon {

    template <typename T = struct cxon_format_traits>
        struct CXON : T { using traits = T; };  

    template <typename T = struct json_format_traits>
        struct JSON : T { using traits = T; };

}
```

and the interface shall be called with *format-selector* instead of bare traits:

``` c++
struct json_unquoted_keys_traits : cxon::json_format_traits {
    struct map : format_traits::map {
        static constexpr bool unquoted_keys = true;
    };
};
using my_traits = cxon::JSON<json_unquoted_keys_traits>;

...
auto const result = cxon::from_chars<my_traits>(...);
...
```

By using of this mechanism, specialization for given type and format is ensured with
code like this:

``` c++
namespace cxon {

    template <typename X, typename II, typename Cx>
        auto read_value(char& t, II& i, II e, Cx& cx)
            -> enable_for_t<X, CXON, bool>
        { ... }
    template <typename X, typename II, typename Cx>
        auto read_value(char& t, II& i, II e, Cx& cx)
            -> enable_for_t<X, JSON, bool>
        { ... }

}
```

*Here, the helper type `cxon::enable_for_t` is a convenience typedef similar to 
[`std::enable_if`][cpp-enab-if].*


--------------------------------------------------------------------------------
##### Context

The context is created by the interface with the optional `CxPs` parameters and
passed to the implementation bridge.

``` c++
namespace cxon {
    template <typename ...Ps>
        struct context;
}
```

###### Template parameters

  - `Ps` - context parameter types

###### Member types

Member type |Definition
------------|------------------------------------------
`prms_type` | `prms::pack_type<Ps...>`

###### Member objects

Member name |Type
------------|------------------------------------------
`ec`        | [`std::error_condition`][cpp-err-cnd]
`ps`        | `prms_type`

###### Member functions

- `operator |` - assign error condition enum
    ``` c++
    template <typename E>
        auto operator |(E e) noexcept;
    ```


`ps`'s type is a tagged tuple with parameter types as tags. Concrete type is
an implementation detail and the code shall access it by the `tag` interface.

``` c++
namespace cxon::prms {
    template <typename Tag>
        struct tag {
            template <typename Pa>
                struct in {                                     (1)
                    static bool value;
                }

            template <typename Ty, Ty c>
                static constexpr auto set();                    (2)
            template <typename Ty>
                static constexpr auto set(Ty&& v);              (3)

            template <typename Pa, typename Ty>
                static constexpr Ty constant(Ty dflt);          (4)

            template <typename Pa>
                static auto reference(Pa& p);                   (5)
            template <typename Pa>
                static auto value(const Pa& p);                 (6)

            template <typename Pa, typename Ty>
                static Ty value(const Pa&, Ty dflt);            (7)
        };
}
```

- `(1)` - `value` member is `true` if parameter `Tag` is set and `false` otherwise.
- `(2)` - creates `constexpr` parameter `Tag` and type `Ty`.
- `(3)` - creates parameter `Tag` and type `Ty`.
- `(4)` - returns the value of the `constexpr` parameter `Tag` if set, `dflt` otherwise.
- `(5)` - returns reference to parameter `Tag` as the type is the one of the creation type.
  It's a compilation error if parameter `Tag` isn't set.
- `(6)` - returns the value of parameter `Tag` as the type is the one of the creation type.
  It's a compilation error if parameter `Tag` isn't set.
- `(7)` - returns the value of parameter `Tag` if set, `dflt` otherwise.

For convenience, parameter type could be inherited from `prms::tag` - `CXON` provides
simple macro for this.

``` c++
#define CXON_PARAMETER(P) struct P : cxon::prms::tag<P> {}
```

###### Example

``` c++
#include "cxon/cxon.hxx"

CXON_PARAMETER(my_constant);
CXON_PARAMETER(my_state);

struct my_type { ... };

namespace cxon {
    template <typename X, typename II, typename Cx>
        inline auto read_value(my_type& t, II& i, II e, Cx& cx)
            // specialize if my_state is set
            -> enable_if_t< my_state::in<prms_type<Cx>>::value, bool>
        {
            char buffer[my_constant::constant<prms_type<Cx>>(32U)]; // 32U if not set
            auto& state = my_state::reference(cx.ps);
            ...
        }
    template <typename X, typename II, typename Cx>
        inline auto read_value(my_type& t, II& i, II e, Cx& cx)
            // specialize if my_state isn't set
            -> enable_if_t<!my_state::in<prms_type<Cx>>::value, bool>
        {
            auto par = my_state::value(cx.ps, 0); // 0 if not set
            ...
        }
}

int main() {
    my_type mv;
        auto res = cxon::from_chars(mv, "...",
            my_constant::set<unsigned, 42U>(), // constexpr parameter
            my_state::set(42) // runtime parameter
        );
}
```


--------------------------------------------------------------------------------
###### Example (`JSON-RPC`)

A toy [`JSON-RPC`](https://www.jsonrpc.org/specification) implementation and example of its
usage with `CXON`.

``` c++
#include "cxon/cxon.hxx"
#include <cassert>

namespace jsonrpc {

    // request

    template <typename ...P>
        struct request {
            static char const*const jsonrpc;
            size_t const            id;
            char const*const        method;
            std::tuple<P...> const  params;

            constexpr request(size_t id, const char* method, P... params) noexcept
            :   id(id), method(method), params(params...) { }

            CXON_STRUCT_WRITE_MEMBER(request,
                CXON_STRUCT_FIELD_ASIS(jsonrpc),
                CXON_STRUCT_FIELD_ASIS(id),
                CXON_STRUCT_FIELD_ASIS(method),
                CXON_STRUCT_FIELD_ASIS(params)
            )
        };
    template <typename ...P>
        char const*const request<P...>::jsonrpc = "2.0";

    template <typename ...P>
        constexpr request<P...> make_request(size_t id, const char* method, P... params) {
            return request<P...>(id, method, params...);
        }

    // response

    template <typename D>
        struct error {
            int         code;
            std::string message;
            D           data;

            CXON_STRUCT_READ_MEMBER(error,
                CXON_STRUCT_FIELD_ASIS(code),
                CXON_STRUCT_FIELD_ASIS(message),
                CXON_STRUCT_FIELD_ASIS(data)
            )
        };

    template <typename R, typename D = cxon::structs::skip_type>
        struct response {
            char        jsonrpc[8];
            size_t      id;
            R           result;
            error<D>    error;

            constexpr response() noexcept
            :   jsonrpc{0}, id(), result(), error() { }

            CXON_STRUCT_READ_MEMBER(response,
                CXON_STRUCT_FIELD_ASIS(jsonrpc),
                CXON_STRUCT_FIELD_ASIS(id),
                CXON_STRUCT_FIELD_ASIS(result),
                CXON_STRUCT_FIELD_ASIS(error)
            )
        };

}

int main() {
    {   // success
        auto const call = jsonrpc::make_request(1, "sub", 42, 23);
        std::string req; // serialize call to req
            auto const w = cxon::to_chars(req, call);
        assert(w && req == "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"sub\",\"params\":[42,23]}");
        // round-trip req -> res
        char const res[] = "{\"jsonrpc\": \"2.0\", \"result\": 19, \"id\": 1}";
        jsonrpc::response<int> ret; // serialize res to ret
            auto const r = cxon::from_chars(ret, res);
        assert(r && ret.id == 1 && ret.result == 19);
    }
    {   // error
        auto const call = jsonrpc::make_request(1, "div", 42, 0);
        std::string req; // serialize call to req
            auto const w = cxon::to_chars(req, call);
        assert(w && req == "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"div\",\"params\":[42,0]}");
        // round-trip req -> res
        char const res[] =  "{\"jsonrpc\": \"2.0\", \"error\": {\"code\": 42, \"message\": \"divide by zero\","
                            "\"data\": \"a black hole has been created somewhere\"}, \"id\": 1}";
        {   // serialize res to ret, error's data will be skipped
            jsonrpc::response<int> ret;
                auto const r = cxon::from_chars(ret, res);
            assert( r &&
                    ret.id == 1 &&
                    ret.error.code == 42 &&
                    ret.error.message == "divide by zero"
            );
        }
        {   // serialize res to ret, error's data is bound to std::string
            jsonrpc::response<int, std::string> ret;
                auto const r = cxon::from_chars(ret, res);
            assert( r &&
                    ret.id == 1 &&
                    ret.error.code == 42 &&
                    ret.error.message == "divide by zero" &&
                    ret.error.data == "a black hole has been created somewhere"
            );
        }
    }
}
```


--------------------------------------------------------------------------------
Distributed under the MIT license. See [`LICENSE`](../../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060
[cpp-charconv]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/charconv&oldid=105120
[cpp-init]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/InputIterator&oldid=103892
[cpp-outit]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/OutputIterator&oldid=108758
[cpp-fwit]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/ForwardIterator&oldid=106013
[cpp-fund-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[cpp-enum]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/enum&oldid=106277
[cpp-struct]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/class&oldid=101735
[cpp-bstr]: https://en.cppreference.com/mwiki/index.php?title=cpp/string/basic_string&oldid=107637
[cpp-tuple]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/tuple&oldid=108562
[cpp-pair]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/pair&oldid=92191
[cpp-container]: https://en.cppreference.com/mwiki/index.php?title=cpp/container&oldid=105942
[cpp-alloc]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/Allocator&oldid=103869
[cpp-map]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/map&oldid=109218
[cpp-umap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_map&oldid=107669
[cpp-mmap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/multimap&oldid=107672
[cpp-ummap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_multimap&oldid=107675
[cpp-opt]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/optional&oldid=110327
[cpp-var]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/variant&oldid=109919
[cpp-enab-if]: https://en.cppreference.com/mwiki/index.php?title=cpp/types/enable_if&oldid=109334
[cpp-err-cnd]: https://en.cppreference.com/mwiki/index.php?title=cpp/error/error_condition&oldid=88237
