## `CXON` library

[![cxon][url-cxon-image]](https://github.com/libcxon/cxon)
[![cxon][url-version-image]](https://github.com/libcxon/cxon)


--------------------------------------------------------------------------------
#### Contents
- [Introduction](#introduction)
- [Interface](#interface)
- [Implementation bridge](#implementation-bridge)
- [Format traits](#format-traits)
- [Example (`JSON-RPC`)](#example-json-rpc)


--------------------------------------------------------------------------------
#### Introduction

`CXON` defines and implements an interface, which is a generalization of C++17's
[`<charconv>`][url-cpp-charconv] interface.

The default serialization format is `UTF-8` encoded `JSON`. The mapping between `C++` and `JSON`
types is as follow:
- [fundamental types][url-cpp-fund-types]
    - `nullptr_t` -> `null`
    - `bool` -> `true` or `false`
    - character types (`char`, `wchar_t`, `char16_t` and `char32_t`) -> `JSON` `string`
    - integral types (`signed` and `unsigned` `char`, `short`, `int`, `long`, `long long`) ->
      `JSON` `number`
    - floating-point types (`float`, `double`, `long double`) -> `JSON` `number`
- `enum` -> `JSON` `string`
- [compound types][url-cpp-struct] -> `JSON` `object`
- arrays -> `JSON` `array`
- pointers are represented as their value or `JSON` `null`
- standard library types
    - [`std::basic_string`][url-cpp-bstr] -> `JSON` `string`
    - [`std::tuple`][url-cpp-tuple] -> `JSON` `array`
    - [`std::pair`][url-cpp-pair] -> `JSON` `array`
    - [containers library][url-cpp-container]
      - maps ([std::map][url-cpp-map], [std:: unordered_map][url-cpp-umap],
              [std::multimap][url-cpp-mmap], [std:: unordered_multimap][url-cpp-ummap]) ->
        `JSON` `object` `(1)`
      - others -> `JSON` `array`

*`(1)` [`ECMA-404`](http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf)
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

    template <typename Traits, typename T, typename InIt, typename ...CtxPrm>
        from_chars_result<It> from_chars(T& t, InIt b, InIt e, CtxPrm... p);            (1)
    template <typename Traits, typename T, typename Iterable, typename ...CtxPrm>
        from_chars_result<It)> from_chars(T& t, const Iterable& i, CtxPrm... p);        (2)

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
- `InIt` - the type of the iterator, must meet [InputIterator][url-cpp-init] requirements
- `Iterable` - a type, for which `std::begin(i)` and `std::end(i)` are defined
- `It` - an iterator
  - `(1)` `InIt`
  - `(2)` `decltype(std::begin(i))`
- `CtxPrm` - the types of the remaining parameters (see [Implementation Bridge](#implementation-bridge))

###### Parameters
- `t` - the out-parameter where the parsed value is stored if successful
- `b`, `e` -  valid (`char`) range to parse
- `i` - a parameter representing a valid (`char`) range to parse
- `p...` - arbitrary parameters passed to the [Implementation Bridge](#implementation-bridge)

###### Return value
On success, returns a value of type `from_chars_result`, such that `end` is one-past-the-end iterator of
the matched range, or has the value equal to `e`, if the whole range match, and `ec` is value initialized.  
On failure, returns a value of type `from_chars_result`, such that `end` is an iterator pointing to
the non-matching input, and `ec` contains the [error condition][url-err-cnd]. The value is in valid, but unspecified
state.

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

    template <typename Traits, typename OutIt, typename T, typename ...CtxPrm>
        to_chars_result<It> to_chars(OutIt o, const T& t, CtxPrm... p);                 (1)
    template <typename Traits, typename Insertable, typename T, typename ...CtxPrm>
        to_chars_result<It> to_chars(Insertable& i, const T& t, CtxPrm... p);           (2)
    template <typename Traits, typename FwIt, typename T, typename ...CtxPrm>
        to_chars_result<It> to_chars(FwIt b, FwIt e, const T& t, CtxPrm... p);          (3)

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
- `OutIt` - the type of the iterator, must meet [OutputIterator][url-cpp-outit] requirements
- `Insertable` - a type, for which `std::back_inserter(i)` and `std::begin(i)` are defined
- `FwIt` - the type of the iterator, must meet [ForwardIterator][url-cpp-fwit] requirements
- `It` - an iterator
  - `(1)` `OutIt`
  - `(2)` `decltype(std::begin(i))`
  - `(3)` `FwIt`
- `CtxPrm` - the types of the remaining parameters (see [TODO]())

###### Parameters
- `o` - an output iterator to write to
- `i` - a back insertable value to write to
- `b`, `e` - a (`char`) range to write to
- `t` - the value to convert to its representation 
- `p...` - arbitrary parameters passed to the [Implementation Bridge](#implementation-bridge)

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


--------------------------------------------------------------------------------
#### Implementation Bridge

The interface communicate the implementation via the so-called *implementation bridge*.  
Read/write interface instantiates the context, defined in the [*format traits*](#format-traits),
with `CtxPrm` parameters (if any) and then calls the *implementation bridge* with it:

``` c++
namespace cxon {

    template <typename X, typename T, typename InIt>
        bool read_value(T& t, InIt& i, InIt e, rctx<X>& ctx) {
            ...
        }
    template <typename X, typename T, typename OutIt>
        bool write_value(OutIt& o, const T& t, wctx<X>& ctx) {
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
                template <typename InIt>
                    static bool value(T& t, InIt& i, InIt e, rctx<X>& ctx) {
                        ...
                    }
            };
        template <typename X>
            struct write<X, T> {
                template <typename OutIt>
                    static bool value(OutIt&, const T& t, wctx<X>& ctx) {
                        ...
                    }
            };

    }
    ```
- static members provided by the type
    ``` c++
    struct T {
        template <typename X, typename InIt>
            static bool read_value(T& t, InIt& i, InIt e, cxon::rctx<X>& ctx) {
                ...
            }
        template <typename X, typename OutIt>
            static bool write_value(OutIt& o, const T& t, cxon::wctx<X>& ctx) {
                ...
            }
    };
    ```
- members provided by the type
    ``` c++
    struct T {
        template <typename X, typename InIt>
            bool read_value(InIt& i, InIt e, cxon::rctx<X>& ctx) {
                ...
            }
        template <typename X, typename OutIt>
            bool write_value(OutIt& o, cxon::wctx<X>& ctx) {
                ...
            }
    };
    ```

For convenience, core library also provides a set of simple, non-intrusive and intrusive macros
for binding of enumerations and compound types:

- [enumerations][url-cpp-enum]
    ``` c++
    // implements the read interface for enum Type
    #define CXON_ENUM_READ(Type, ...)
    // implements the write interface for enum Type
    #define CXON_ENUM_WRITE(Type, ...)
    // implements the read and write interfaces for enum Type
    #define CXON_ENUM(Type, ...)

    // defines enum value Value, which will be serialized as Name
    #define CXON_ENUM_VALUE_NAME(Name, Value)
    // defines enum value Value, which will be serialized as ##Value
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
- [compound types][url-cpp-struct]
    ``` c++
    // implements the read interface for type Type
    #define CXON_STRUCT_READ(Type, ...)
    // implements the write interface for type Type
    #define CXON_STRUCT_WRITE(Type, ...)
    // implements the read and write interfaces for type Type
    #define CXON_STRUCT(Type, ...)
    // and the same set for intrusive implementation
    #define CXON_STRUCT_READ_MEMBER(Type, ...)
    #define CXON_STRUCT_WRITE_MEMBER(Type, ...)
    #define CXON_STRUCT_MEMBER(Type, ...)
    
    // defines field Field, which will be serialized as Name
    #define CXON_STRUCT_FIELD_NAME(Name, Field)
    // defines field Field, which will be serialized as ##Field
    #define CXON_STRUCT_FIELD_ASIS(Field)
    // defines the key Name, which will be skipped during serialization
    //  (only meaningful for reading)
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
#### Format Traits

`Traits` template parameter has two roles:
  - to keep configuration, related to the definition of given serialization format
  - to keep configuration parameters for given serialization format

The implementation requires *format traits* to provide one mandatory trait to support the 
[*implementation bridge*](#implementation-bridge) - a context:

``` c++
struct format_traits {
    struct context {
        struct read;
        struct write;
    };
};
```

The implementation requires these types to provide mandatory member `ec` of type
[`std::error_condition`][url-err-cnd].

Although `JSON` is the default format, `CXON` defines a fall-back format called the same - `CXON`.  
It is like somewhat relaxed `JSON` (e.g. object keys may not be quoted) and its traits are
defined like this:

``` c++
namespace cxon {

    struct format_traits {
        struct context; // read/write contexts
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

As both definition and configuration are in one place, specialization for given format 
is not directly possible (because changing of a parameter requires new type).
Because of this, `CXON` uses so-called *format-selector* and it is defined like this:

``` c++
namespace cxon {

    template <typename T = struct cxon_format_traits>
        struct CXON : T { using traits = T; };  

    template <typename T = struct json_format_traits>
        struct JSON : T { using traits = T; };

}
```

Then the interface should be called with *format-selector* instead of with bare format traits.

###### Example

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

This way, specialization for given type and format is ensured and code like this:

``` c++
namespace cxon {

    template <typename X, typename II>
        auto read_value(char& t, II& i, II e, rctx<X>& ctx)
            -> enable_for_t<X, CXON, bool>
        { ... }
    template <typename X, typename II>
        auto read_value(char& t, II& i, II e, rctx<X>& ctx)
            -> enable_for_t<X, JSON, bool>
        { ... }

}
```

will work with arbitrary *format traits*.

*Here, the helper type `cxon::enable_for_t` is a convenience typedef similar to 
[`std::enable_if`][url-cpp-enab-if].*


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
Distributed under the MIT license. See [`LICENSE`](LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  


<!-- links -->
[url-cxon-image]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[url-version-image]: https://img.shields.io/badge/version-0.42.0-608060.svg?style=plastic
[url-cpp-charconv]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/charconv&oldid=105120
[url-cpp-init]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/InputIterator&oldid=103892
[url-cpp-outit]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/OutputIterator&oldid=108758
[url-cpp-fwit]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/ForwardIterator&oldid=106013
[url-cpp-fund-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[url-cpp-enum]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/enum&oldid=106277
[url-cpp-struct]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/class&oldid=101735
[url-cpp-bstr]: https://en.cppreference.com/mwiki/index.php?title=cpp/string/basic_string&oldid=107637
[url-cpp-tuple]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/tuple&oldid=108562
[url-cpp-pair]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/pair&oldid=92191
[url-cpp-container]: https://en.cppreference.com/mwiki/index.php?title=cpp/container&oldid=105942
[url-cpp-map]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/map&oldid=109218
[url-cpp-umap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_map&oldid=107669
[url-cpp-mmap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/multimap&oldid=107672
[url-cpp-ummap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_multimap&oldid=107675
[url-cpp-enab-if]: https://en.cppreference.com/mwiki/index.php?title=cpp/types/enable_if&oldid=109334
[url-err-cnd]: https://en.cppreference.com/mwiki/index.php?title=cpp/error/error_condition&oldid=88237
