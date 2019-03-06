## `CXON` library

[![cxon][url-cxon-image]](https://github.com/libcxon/cxon)
[![CXON][url-version-image]](https://github.com/libcxon/cxon)

--------------------------------------------------------------------------------


#### Introduction

`CXON` defines and implements an interface, which is a generalization of C++17's
[`<charconv>`][url-cpp-charconv] interface.

##### Read interface  
*TODO* desciption

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
  - (1) `InIt`
  - (2) `decltype(std::begin(i))`
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
the non-matching input, and `ec` contains the error condition. The value is in valid, but unspecified
state.

###### Exceptions
Does not throw by itself, however specializations may throw or not:
- of fundamental types - does not throw
- of pointers - allocator and constructors may throw
- of standard library types - constructing and adding of elements may throw with the same guarantees as
  of the standard library
- of user types - may or may not throw depending of the implementation


##### Write interface  
*TODO* desciption

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
  - (1) `OutIt`
  - (2) `decltype(std::begin(i))`
  - (3) `FwIt`
- `CtxPrm` - the types of the remaining parameters (see [TODO]())

###### Parameters
- `o` - an output iterator to write to
- `i` - a back insertable value to write to
- `b`, `e` - a (`char`) range to write to
- `t` - the value to convert to its representation 
- `p...` - arbitrary parameters passed to the [Implementation Bridge](#implementation-bridge)

###### Return value
On success, returns a value of type `to_chars_result`, such that `ec` is value-initialized, and `end` is:
- (1) one-past-the-end output iterator
- (2) `std::begin(i)`
- (3) one-past-the-end iterator of the output written. Note that the output is not terminated.

On failure, returns a value of type `to_chars_result`, such that `ec` contains the error condition, and
`end` has the same value as in case of success.

###### Exceptions
Does not throw by itself, however writing to the output may throw (e.g. adding to a container).


--------------------------------------------------------------------------------
#### Implementation Bridge

The interface communicate the implementation via the so-called _implementation bridge_:

``` c++
namespace cxon {

    template <typename X, typename T, typename InIt>
        bool read_value(T& t, InIt& i, InIt e, rctx<X>& ctx) {
            ...
        }
    template <typename X, typename T, typename OutIt>
        void write_value(OutIt& o, const T& t, wctx<X>& ctx) {
            ...
        }

}
```

- `ctx` is read/write context defined in the [_format-traits_](#format-traits)

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
                    static void value(OutIt&, const T& t, wctx<X>& ctx) {
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
            static void write_value(OutIt& o, const T& t, cxon::wctx<X>& ctx) {
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
            void write_value(OutIt& o, cxon::wctx<X>& ctx) {
                ...
            }
    };
    ```

By using of the non-intrusive methods, core `CXON` implements good part of `C++`'s 
fundamental and standard libraries types:

- [fundamental types][url-cpp-fund-types]
    - `nullptr_t`
    - `bool`
    - character types - `char`, `wchar_t`, `char16_t` and `char32_t`
    - integral types - signed and unsigned `char`, `short`, `int`, `long`, `long long`
    - floating-point types - `float`, `double`, `long double`
- arrays and pointers
- standard library types
    - [`std::basic_string`][url-cpp-bstr]
    - [`std::tuple`][url-cpp-tuple]
    - [`std::pair`][url-cpp-pair]
    - [containers library][url-cpp-container] - in its entirety

Core library also provides convenient way for binding of `enum`s and `struct`s via
set of simple non-intrusive and intrusive macros (only a thin and debug friendly wrappers).


--------------------------------------------------------------------------------
#### Format Traits

`FormatTraits` template parameter has two roles:
  - to hold defining information for given serialization format
  - to hold configuration options for given serialization format

The implementation requires _format traits_ to provide one mandatory trait to support the 
[*implementation bridge*](#implementation-bridge) - a context:

``` c++
struct format_traits {
    struct context {
        struct read;
        struct write;
    };
};
```

In essence, the [*interface*](#interface) instantiates the context and then calls 
the [*implementation bridge*](#implementation-bridge) with it.

Although `JSON` is the default format, `CXON` defines a fall-back format called the same, `CXON`.
This format could be seen as a relaxed `JSON` and its traits are defined like this:

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
            static constexpr char   beg             = '{';
            static constexpr char   end             = '}';
            static constexpr char   div             = ':';
            static constexpr char   sep             = ',';
            static constexpr bool   unquoted_keys   = true;
        };
        ...
    };

}
```

and `JSON` reuses it as this:

``` c++
namespace cxon {

    struct format_traits;

    struct json_format_traits : format_traits {
        ...
        struct map : format_traits::map {
            static constexpr bool   unquoted_keys   = false;
        };
        ...
    };

}
```

As both definition and configuration are in one place, specialization for given format, 
is not possible (because changing of an option requires new type). Because of this,
`CXON` uses so-called _format-selector_ and it's defined like this:

``` c++
namespace cxon {

    template <typename T = struct cxon_format_traits>
        struct CXON : T { using traits = T; };  

    template <typename T = struct json_format_traits>
        struct JSON : T { using traits = T; };

}
```

Then the interface should be called with _format-selector_ instead of bare format traits:

``` c++
struct json_unquoted_keys_traits : cxon::json_format_traits {
    ...
    struct map : format_traits::map {
        static constexpr bool   unquoted_keys   = true;
    };
    ...
};
using MyTraits = cxon::JSON<json_unquoted_keys_traits>;

...
auto const result = cxon::from_chars<MyTraits>(...);
...

```

This way, specialization for given type and format is ensured and code like this:

``` c++
namespace cxon {

    template <typename X, typename II>
        inline auto read_value(char& t, II& i, II e, rctx<X>& ctx)
            -> enable_for_t<X, CXON, bool>
        { ... }
    template <typename X, typename II>
        inline auto read_value(char& t, II& i, II e, rctx<X>& ctx)
            -> enable_for_t<X, JSON, bool>
        { ... }

}
```

*Here, the helper types `cxon::enable_for_t` is convenience typedef based on 
[`std::enable_if`][url-cpp-enab-if].*


--------------------------------------------------------------------------------
###### Example (JSON-RPC)

`struct` binding with a toy [`JSON-RPC`](https://www.jsonrpc.org/) implementation:

``` c++
namespace jsonrpc {

    // request

    template <typename ...P>
        struct request {
            static char const*const   jsonrpc;
            size_t const              id;
            char const*const          method;
            std::tuple<P...> const    params;

            constexpr request(size_t id, const char* method, P... params) noexcept
            :   id(id), method(method), params(params...) { }

            // implements write_value static member for the fields we care about
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

    struct error {
        int           code;
        std::string   message;
    };

    template <typename R>
        struct response {
            char      jsonrpc[8];
            size_t    id;
            R         result;
            error     error;

            constexpr response() noexcept
            :   jsonrpc{0}, id(), result(), error() { }

            // here's what's behind the macros
            template <typename X, typename II>
                static bool read_value(response& t, II& i, II e, cxon::rctx<X>& ctx) {
                    using namespace cxon::structs;
                    static constexpr auto f = make_fields(
                        make_field("jsonrpc", &response::jsonrpc),
                        make_field("id", &response::id),
                        make_field("result", &response::result),
                        make_field("error", &response::error)
                    );
                    return read_fields<X>(t, f, i, e, ctx);
                }
        };

}

// the non-intrusive way
CXON_STRUCT_READ(jsonrpc::error,
    CXON_STRUCT_FIELD_ASIS(code),
    CXON_STRUCT_FIELD_ASIS(message),
    CXON_STRUCT_FIELD_SKIP(data) // 'data' will be skipped if present
)

...
// success

auto const call = jsonrpc::make_request(1, "subtract", 42, 23);
std::string req;
    cxon::to_chars(req, call);
assert(req == "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"subtract\",\"params\":[42,23]}");
// round-trip req -> res
char const res[] = "{\"jsonrpc\": \"2.0\", \"result\": 19, \"id\": 1}";
jsonrpc::response<int> ret;
    auto const r = cxon::from_chars(ret, res);
assert(r && ret.id == 1 && ret.result == 19);

...
// error

auto const call = jsonrpc::make_request(1, "div", 42, 0);
std::string req;
    cxon::to_chars(req, call);
assert(req == "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"div\",\"params\":[42,0]}");
// round-trip req -> res
char const res[] =  "{\"jsonrpc\": \"2.0\", \"error\": {\"code\": 42, \"message\": \"divide by zero\","
                    "\"data\": \"a black hole has been created somewhere\"}, \"id\": 1}";
jsonrpc::response<int> ret;
    auto const r = cxon::from_chars(ret, res);
assert(r && ret.id == 1 && ret.error.code == 42 && ret.error.message == "divide by zero");

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
[url-cpp-bstr]: https://en.cppreference.com/mwiki/index.php?title=cpp/string/basic_string&oldid=107637
[url-cpp-tuple]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/tuple&oldid=108562
[url-cpp-pair]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/pair&oldid=92191
[url-cpp-container]: https://en.cppreference.com/mwiki/index.php?title=cpp/container&oldid=105942
[url-cpp-enab-if]: https://en.cppreference.com/mwiki/index.php?title=cpp/types/enable_if&oldid=109334
