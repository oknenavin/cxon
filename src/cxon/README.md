## `CXON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------

#### Contents
  - [Introduction](#introduction)
    - Formats
      - [`JSON`](lang/json/README.md)
  - [Interface](#interface)
  - [Implementation bridge](#implementation-bridge)
  - [Parametrization](#parametrization)
    - [Format traits](#format-traits)
    - [Context](#context)
  - [Example (`JSON-RPC`)](#example-json-rpc)


--------------------------------------------------------------------------------

#### Introduction

`CXON` defines and implements an interface similar to`C++17`'s [`<charconv>`][std-charconv]
interface with these differences:

  - traits template parameter (to allow arbitrary serialization formats, see
    [`Format traits`](#format-traits))
  - trailing named parameters of an arbitrary type (to allow passing of parameters  
    to given type serializer, see [`Context`](#context))
  - input and output iterators for I/O (allowing streams, containers and arrays,
    see [`Interface`](README.md#interface))


--------------------------------------------------------------------------------

#### Interface

*Defined in header [cxon.hxx](cxon.hxx), include `cxon/<format>.hxx`*

##### Read interface  

``` c++
namespace cxon {

    template <typename Traits, typename T, typename InIt, typename ...NaPa>
        from_bytes_result<It> from_bytes(T& t, InIt b, InIt e, NaPa... p);            (1)
    template <typename Traits, typename T, typename Iterable, typename ...NaPa>
        from_bytes_result<It)> from_bytes(T& t, const Iterable& i, NaPa... p);        (2)

    template <typename It>
        struct from_bytes_result {
            std::error_condition ec;
            It end;
            operator bool() const noexcept;
        };

}
```

###### Template parameters

  - [`Traits`](#format-traits) - traits class specifying given serialization format
  - `T` - the type of the value to serialize
  - `InIt` - the type of the iterator, must meet [InputIterator][cpp-init] requirements
  - `Iterable` - a type, for which `std::begin(i)` and `std::end(i)` are defined
  - `It` - an iterator
    - `(1)` `InIt`
    - `(2)` `decltype(std::begin(i))`
  - `NaPa` - named parameter types (see [Context](#context))

###### Parameters

  - `t` - the out-parameter where the parsed value is stored in case of success
  - `b`, `e` -  valid (`char`) range to parse
  - `i` - an iterable type representing a valid (`char`) range to parse
  - `p...` - named parameters (see [Context](#context))

###### Return value

On success, returns a value of type `from_bytes_result`, such that `end` is one-past-the-end iterator of
the matched range, or has the value equal to `e`, if the whole range match, and `ec` is value initialized.  
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
                r.ec == read_error::floating_point_invalid &&
                strcmp(r.end, "true]") == 0
        );
    }
}
```


##### Write interface  

``` c++
namespace cxon {

    template <typename Traits, typename T, typename OutIt, typename ...NaPa>
        to_bytes_result<It> to_bytes(OutIt o, const T& t, NaPa... p);                 (1)
    template <typename Traits, typename T, typename Insertable, typename ...NaPa>
        to_bytes_result<It> to_bytes(Insertable& i, const T& t, NaPa... p);           (2)
    template <typename Traits, typename T, typename FwIt, typename ...NaPa>
        to_bytes_result<It> to_bytes(FwIt b, FwIt e, const T& t, NaPa... p);          (3)

    template <typename It>
        struct to_bytes_result {
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
  - `NaPa` - named parameter types (see [Context](#context))

###### Parameters

  - `o` - an output iterator to write to
  - `i` - a back insertable value to write to
  - `b`, `e` - a (`char`) range to write to
  - `t` - serialization value
  - `p...` - named parameters (see [Context](#context))

###### Return value

On success, returns a value of type `to_bytes_result`, such that `ec` is value-initialized, and `end` is:
  - `(1)` one-past-the-end output iterator
  - `(2)` `std::begin(i)`
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
        assert(!r && r.ec == write_error::output_failure);
    }
}
```


--------------------------------------------------------------------------------

#### Implementation Bridge

The interface communicates the implementation via the so-called *implementation bridge*.  
Read/write interfaces instantiate the [*context*](#context), with the named parameters (if any)
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

  - specialization of read/write structures for the type (non-intrusive, allows partial specialization)
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


--------------------------------------------------------------------------------

#### Parametrization

`CXON` provides two ways for parametrization, serving different purposes:
- [format-traits](#format-traits) - for parameterizing given serialization format,
  usually meaning that some properties of the format change. As an example,
  `unquoted_keys` parameter enables unquoted object keys.
- [context](#context) - for parameterizing the serialization of given type
  without changing the format. As an example, `allocator` parameter allows
  using of a custom [allocator][std-alloc] when reading pointer types.


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

for example, `map` is defined like this:

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

As changing of a `Traits` parameter introduces new type, specialization for given format
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
auto const result = cxon::from_bytes<my_traits>(...);
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
[`std::enable_if`][std-enab-if].*


--------------------------------------------------------------------------------

##### Context

The context is created by the interface with the optional `NaPa` parameters and
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
`napa_type` | `napa::pack_type<Ps...>`

###### Member objects

Member name |Type
------------|------------------------------------------
`ec`        | [`std::error_condition`][std-err-cnd]
`ps`        | `napa_type`

###### Member functions

- `operator |` - assign an error condition enum
    ``` c++
    template <typename E>
        auto operator |(E e) noexcept;
    ```
- `operator bool` - check if the context is good (i.e., no error condition)
    ``` c++
    operator bool() const noexcept;
    ```


`ps`'s type is a tagged tuple with parameter types as tags. Concrete type is
an implementation detail and the code shall access it by the `parameter` interface.

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

- `(1)` - `value` member is `true` if parameter `Tag` is set and `false` otherwise
- `(2)` - creates `constexpr` parameter `Tag` and type `Type`
- `(3)`, `(4)` - creates parameter `Tag` and type `Type`
- `(5)` - returns the value of the `constexpr` parameter `Tag` if set, `dflt` otherwise
- `(6)` - returns a reference to parameter `Tag`, compilation error if not set
- `(7)` - returns the value of parameter `Tag`, compilation error if not set
- `(8)` - returns the value of parameter `Tag` if set, `dflt` otherwise

For convenience, parameter type could be inherited from `napa::tag` - `CXON` provides
simple macro for this.

``` c++
#define CXON_PARAMETER(P, T) struct P : cxon::napa::parameter<P, T> {}
```

###### Example

``` c++
#include "cxon/cxon.hxx"

CXON_PARAMETER(my_constant, unsigned);
CXON_PARAMETER(my_state, int);

struct my_type { ... };

namespace cxon {
    template <typename X, typename II, typename Cx>
        inline auto read_value(my_type& t, II& i, II e, Cx& cx)
            // specialize if my_state is set
            -> enable_if_t< my_state::in<napa_type<Cx>>::value, bool>
        {
            char buffer[my_constant::constant<napa_type<Cx>>(32)]; // 32 if not set
            auto& state = my_state::reference(cx.ps);
            ...
        }
    template <typename X, typename II, typename Cx>
        inline auto read_value(my_type& t, II& i, II e, Cx& cx)
            // specialize if my_state isn't set
            -> enable_if_t<!my_state::in<napa_type<Cx>>::value, bool>
        {
            auto par = my_state::value(cx.ps, 0); // 0 if not set
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

###### Example (`JSON-RPC`)

A toy [`JSON-RPC`](https://www.jsonrpc.org/specification) implementation and an
example of its usage with `CXON`.

``` c++
#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/tuple.hxx"
#include <cassert>

namespace jsonrpc {

    // request

    template <typename T>
        struct napa { // named parameter
            char const*const key;
            T const value;

            template <typename X, typename O, typename Cx, typename J = X>
                auto write_value(O& o, Cx& cx) const -> cxon::enable_for_t<J, cxon::JSON, bool> {
                    return  cxon::chio::write_key<J>(o, key, cx) &&
                            cxon::write_value<J>(o, value, cx)
                    ;
                }
        };

    template <typename T>
        constexpr napa<T> make_napa(const char* k, T&& v) {
            return {k, v};
        }

    template <typename ...P>
        struct request {
            static char const*const jsonrpc;
            size_t const            id;
            char const*const        method;
            std::tuple<P...> const  params;

            constexpr request(size_t id, const char* method, P... params) noexcept
            :   id(id), method(method), params(params...) { }

            CXON_JSON_CLS_WRITE_MEMBER(request,
                CXON_JSON_CLS_FIELD_ASIS(jsonrpc),
                CXON_JSON_CLS_FIELD_ASIS(id),
                CXON_JSON_CLS_FIELD_ASIS(method),
                CXON_JSON_CLS_FIELD_ASIS(params)
            )
        };
    template <typename ...P>
        char const*const request<P...>::jsonrpc = "2.0";

    template <typename ...P>
        constexpr request<P...> make_request(size_t id, const char* method, P... params) {
            return request<P...>(id, method, params...);
        }
    template <typename ...P>
        constexpr request<napa<P>...> make_request(size_t id, const char* method, napa<P>... params) {
            return request<napa<P>...>(id, method, params...);
        }

    // response

    template <typename D>
        struct error {
            int         code;
            std::string message;
            D           data;

            CXON_JSON_CLS_READ_MEMBER(error,
                CXON_JSON_CLS_FIELD_ASIS(code),
                CXON_JSON_CLS_FIELD_ASIS(message),
                CXON_JSON_CLS_FIELD_ASIS(data)
            )
        };

    template <typename R, typename D = cxon::chio::val::skip_t<>>
        struct response {
            char            jsonrpc[8];
            size_t          id;
            R               result;
            struct error<D> error;

            constexpr response() noexcept
            :   jsonrpc{0}, id(), result(), error() { }

            CXON_JSON_CLS_READ_MEMBER(response,
                CXON_JSON_CLS_FIELD_ASIS(jsonrpc),
                CXON_JSON_CLS_FIELD_ASIS(id),
                CXON_JSON_CLS_FIELD_ASIS(result),
                CXON_JSON_CLS_FIELD_ASIS(error)
            )
        };

}

namespace cxon { // json-rpc - serialize tuple of named parameters as a JSON object instead of an array

    template <typename X, typename ...T>
        struct write<JSON<X>, std::tuple<jsonrpc::napa<T>...>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::tuple<jsonrpc::napa<T>...>& t, Cx& cx) {
                    return  chio::poke<J>(o, J::map::beg, cx) &&
                                chio::con::write_tuple<J>(o, t, cx) &&
                            chio::poke<J>(o, J::map::end, cx)
                    ;
                }
        };

}

int main() {
    {   // params array
        auto const call = jsonrpc::make_request(1, "sub", 42, 23);
        std::string req; // serialize call to req
            auto const w = to_bytes(req, call);
        assert(w && req == "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"sub\",\"params\":[42,23]}");
    }
    {   // params object
        auto const call = jsonrpc::make_request(1, "sub", jsonrpc::make_napa("x", 42), jsonrpc::make_napa("y", 23));
        std::string req; // serialize call to req
            auto const w = to_bytes(req, call);
        assert(w && req == "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"sub\",\"params\":{\"x\":42,\"y\":23}}");
    }
    {   // round-trip: req -> res ok
        char const res[] = "{\"jsonrpc\": \"2.0\", \"result\": 19, \"id\": 1}";
        jsonrpc::response<int> ret; // serialize res to ret
            auto const r = from_bytes(ret, res);
        assert(r && ret.id == 1 && ret.result == 19);
    }
    {   // round-trip: req -> res ko
        char const res[] =  "{\"jsonrpc\": \"2.0\", \"error\": {\"code\": 42, \"message\": \"divide by zero\","
                            "\"data\": \"a black hole has been created somewhere\"}, \"id\": 1}";
        {   // serialize res to ret, error's data will be skipped
            jsonrpc::response<int> ret;
                auto const r = from_bytes(ret, res);
            TEST_CHECK( r &&
                        ret.id == 1 &&
                        ret.error.code == 42 &&
                        ret.error.message == "divide by zero"
            );
        }
        {   // serialize res to ret, error's data is bound to std::string
            jsonrpc::response<int, std::string> ret;
                auto const r = from_bytes(ret, res);
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
[std-charconv]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/charconv&oldid=105120
[cpp-init]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/InputIterator&oldid=103892
[cpp-outit]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/OutputIterator&oldid=108758
[cpp-fwit]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/ForwardIterator&oldid=106013
[cpp-fund-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[cpp-ptr]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/pointer&oldid=109738
[cpp-ref]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/reference&oldid=105941
[cpp-arr]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/array&oldid=111607
[cpp-enum]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/enum&oldid=111809
[cpp-class]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/class&oldid=101735
[cpp-struct]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/class&oldid=101735
[std-complex]: https://en.cppreference.com/mwiki/index.php?title=cpp/numeric/complex&oldid=103532
[std-valarr]: https://en.cppreference.com/mwiki/index.php?title=cpp/numeric/valarray&oldid=109876
[std-bitset]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/bitset&oldid=103231
[std-bstr]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/string&oldid=111300
[std-strv]: https://en.cppreference.com/mwiki/index.php?title=cpp/header/string_view&oldid=107572
[std-duration]: https://en.cppreference.com/mwiki/index.php?title=cpp/chrono/duration&oldid=100475
[std-time-pt]: https://en.cppreference.com/mwiki/index.php?title=cpp/chrono/time_point&oldid=103361
[std-tuple]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/tuple&oldid=108562
[std-pair]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/pair&oldid=92191
[cpp-container]: https://en.cppreference.com/mwiki/index.php?title=cpp/container&oldid=105942
[std-array]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/array&oldid=111731
[std-vector]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/vector&oldid=107643
[std-deque]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/deque&oldid=107644
[std-forward_list]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/forward_list&oldid=107645
[std-list]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/list&oldid=107646
[std-set]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/set&oldid=107670
[std-map]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/map&oldid=109218
[std-multiset]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/multiset&oldid=107671
[std-multimap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/multimap&oldid=107672
[std-uset]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_set&oldid=107673
[std-umap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_map&oldid=107669
[std-umultiset]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_multiset&oldid=107674
[std-umultimap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_multimap&oldid=107675
[std-stack]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/stack&oldid=106350
[std-queue]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/queue&oldid=103354
[std-priority_queue]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/priority_queue&oldid=103092
[std-optional]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/optional&oldid=110327
[std-variant]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/variant&oldid=109919
[std-enab-if]: https://en.cppreference.com/mwiki/index.php?title=cpp/types/enable_if&oldid=109334
[std-err-cnd]: https://en.cppreference.com/mwiki/index.php?title=cpp/error/error_condition&oldid=88237
[std-alloc]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/Allocator&oldid=103869
