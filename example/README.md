## `CXON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  

--------------------------------------------------------------------------------

#### Contents
  - [Common interface](#common-interface)
  - [Standard library types](#standard-library-types)
  - [User types](#user-types)
  - [Format traits](#format-traits)
  - [Named parameters](#named-parameters)
  - [Overriding default serializers](#overriding-default-serializers)
  - [`JSON-RPC`](#json-rpc)
  - [`CXCF`](#cxcf)


--------------------------------------------------------------------------------

#### Common interface

*[Source](common-interface.cxx)*

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


--------------------------------------------------------------------------------

#### Standard library types

##### Example 1

*[Source](user-types-ex01.cxx)*

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

*TODO: more examples needed here*


--------------------------------------------------------------------------------

#### User types

##### Example 1

*[Source](std-types-ex01.cxx)*

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

*TODO: more examples needed here*


--------------------------------------------------------------------------------

#### Format traits

##### Example 1

*[Source](format-traits-ex01.cxx)*

``` c++
// format traits

#include "cxon/json.hxx"
#include "cxon/lib/std/map.hxx"
#include <string>
#include <cassert>

struct unquoted_keys_traits : cxon::json::format_traits {
    // allow arbitrary keys without quotes
    static constexpr bool quote_unquoted_keys = false;
};
using UQK = cxon::JSON<unquoted_keys_traits>;

int main() {
    std::map<int, int> map;
        cxon::from_bytes<UQK>(map, R"({1: 2, 3: 4})");
    assert(map == (std::map<int, int> {{1, 2}, {3, 4}}));
    std::string str;
        cxon::to_bytes<UQK>(str, map);
    assert(str == R"({1:2,3:4})");
}
```

##### Example 2

*[Source](format-traits-ex02.cxx)*

``` c++
// format traits

#include "cxon/json.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/string.hxx"
#include <cassert>

// custom traits for given format
struct custom_traits : cxon::json::format_traits {
    struct string : cxon::json::format_traits {
        static constexpr char del = '\'';
    };
    // enable unquoted string keys
    static constexpr bool unquote_quoted_keys = true;
};
using UQK = cxon::JSON<custom_traits>;

int main() {
    std::map<std::string, std::string> cxx;
        cxon::from_bytes<UQK>(cxx, R"({ key: 'value' })");
    assert(cxx == (std::map<std::string, std::string> { {"key", "value"} }));
    std::string str;
        cxon::to_bytes<UQK>(str, cxx);
    assert(str == R"({key:'value'})");
}
```

*TODO: more examples needed here*

--------------------------------------------------------------------------------

#### Named parameters

##### Example 1

*[Source](named-parameters-ex01.cxx)*

``` c++
// named parameters

#include "cxon/json.hxx"
#include <cassert>

int main() {
    using namespace cxon::json;
    std::string json;
        cxon::to_bytes(json, 3.1415926, fp_precision::set<3>()); // floating-point precision
    assert(json == "3.14");
}
```

*TODO: more examples needed here*


--------------------------------------------------------------------------------

#### Overriding default serializers

*[Source](overriding-default-serializers.cxx)*

``` c++
// overriding default serializers

#include "cxon/json.hxx"
#include <cassert>

namespace cxon {
    // override the default double parser for JSON<X>
    template <typename X>
        struct read<JSON<X>, double> {
            template <typename II, typename Cx>
                static bool value(double& t, II& i, II e, Cx& cx) {
                    // implement fast double parsing for example
                    return t = 42., true;
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
                    return t = 42., true;
                }
        };
}

int main() {
    double d = 0.0;
        cxon::from_bytes(d, "0");
    assert(d == 42.);   // the override is used for double
    int i = 0;
        cxon::from_bytes(i, "42");
    assert(i == 42);    // the other types are intact
    float f = 0.0;
        cxon::from_bytes<CUSTOM_JSON>(f, "0");
    assert(f == 42.);   // the override is used for CUSTOM_JSON
    unsigned u = 0;
        cxon::from_bytes<CUSTOM_JSON>(u, "42");
    assert(u == 42);    // the other types are as in JSON<>
}
```


--------------------------------------------------------------------------------

##### `JSON-RPC`

*[Source](json-rpc.cxx)*

A toy [`JSON-RPC`](https://www.jsonrpc.org/specification) implementation and 
an example of its usage with `CXON`.

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

            template <typename X, typename O, typename Cx, typename Y = X>
                auto write_value(O& o, Cx& cx) const -> cxon::enable_for_t<Y, cxon::JSON> {
                    return  cxon::cio::write_map_key<Y>(o, key, cx) &&
                            cxon::cio::write_map_val<Y>(o, value, cx)
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
            std::size_t const       id;
            char const*const        method;
            std::tuple<P...> const  params;

            constexpr request(std::size_t id, const char* method, P&&... params) noexcept
            :   id(id), method(method), params(std::forward<P>(params)...)
            {
            }

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
        constexpr request<P...> make_request(std::size_t id, const char* method, P&&... params) {
            return request<P...>(id, method, std::forward<P>(params)...);
        }
    template <typename ...P>
        constexpr request<napa<P>...> make_request(std::size_t id, const char* method, napa<P>&&... params) {
            return request<napa<P>...>(id, method, std::forward<napa<P>>(params)...);
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

    template <typename R, typename D = cxon::cio::val::sink<>>
        struct response {
            char            jsonrpc[8];
            std::size_t     id;
            R               result;
            struct error<D> error;

            constexpr response() noexcept
            :   jsonrpc{0}, id(), result(), error()
            {
            }

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
                    return  cio::poke<J>(o, J::map::beg, cx) &&
                                cio::cnt::write_tuple<J>(o, t, cx) &&
                            cio::poke<J>(o, J::map::end, cx)
                    ;
                }
        };

}

int main() {
    using namespace cxon;
    {   // params array
        auto const call = jsonrpc::make_request(1, "sub", 42, 23);
        std::string req; // serialize call to req
            auto const w = to_bytes(req, call);
        assert(w && req == R"({"jsonrpc":"2.0","id":1,"method":"sub","params":[42,23]})");
    }
    {   // params object
        auto const call = jsonrpc::make_request(1, "sub", jsonrpc::make_napa("x", 42), jsonrpc::make_napa("y", 23));
        std::string req; // serialize call to req
            auto const w = to_bytes(req, call);
        assert(w && req == R"({"jsonrpc":"2.0","id":1,"method":"sub","params":{"x":42,"y":23}})");
    }
    {   // round-trip: req -> res success
        char const res[] = R"({"jsonrpc": "2.0", "result": 19, "id": 1})";
        jsonrpc::response<int> ret; // serialize res to ret
            auto const r = from_bytes(ret, res);
        assert(r && ret.id == 1 && ret.result == 19);
    }
    {   // round-trip: req -> res failure
        char const res[] = R"({
            "jsonrpc": "2.0",
            "error": {"code": 42, "message": "divide by zero", "data": "a black hole has been created somewhere"},
            "id": 1
        })";
        {   // serialize res to ret, error's data will be skipped
            jsonrpc::response<int> ret;
                auto const r = from_bytes(ret, res);
            assert( r &&
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

#### `CXCF`

##### Example 1

*[Source](cxcf-ex01.cxx)*

``` c++
// CXCF configuration format

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"
#include <cassert>

// types representing a configuration file
struct type1 {
    std::map<std::string, std::vector<int>> key1;
    bool key2 = false;
};
struct config {
    int prop1 = 0;
    std::vector<std::string> prop_array;
    type1 prop_object;
};

// implemented for CXON
CXON_JSON_CLS(type1,
    CXON_JSON_CLS_FIELD_ASIS(key1),
    CXON_JSON_CLS_FIELD_NAME("key.2", key2)
)
CXON_JSON_CLS(config,
    CXON_JSON_CLS_FIELD_ASIS(prop1),
    CXON_JSON_CLS_FIELD_NAME("prop.array", prop_array),
    CXON_JSON_CLS_FIELD_NAME("prop-object", prop_object)
)
CXON_JSON_CLS_BARE(config) // bare root

// configuration data
static char const  cf_[] = R"(
    // the root is a bare type
    prop1 = 42                          // the values are of arbitrary JSON type
    prop.array = ["one" "two" "three"]  // the keys are unquoted
                                        // the list/map values are separated with white-spaces
    prop-object = {
        key1 = {
            key.1 = [1 3 5]
            key.2 = [2 4 6]
        }
        key.2 = true
    }
)";

using CXCF = cxon::CXCF<>;

int main() {
    // load
    config cf;
        auto const r = cxon::from_bytes<CXCF>(cf, cf_);
    // ready for use
    assert(
        r &&
        (cf.prop1 == 42) &&
        (cf.prop_array == decltype(cf.prop_array) {"one", "two", "three"}) &&
        (cf.prop_object.key1 == decltype(cf.prop_object.key1) {{"key.1", {1, 3, 5}}, {"key.2", {2, 4, 6}}}) &&
        (cf.prop_object.key2 == true)
    );
}
```


--------------------------------------------------------------------------------

Distributed under the MIT license. See [`LICENSE`](../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060