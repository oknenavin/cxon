## `CXON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  

--------------------------------------------------------------------------------

#### Contents
  - [Common interface](#common-interface)
  - [Standard library types](#standard-library-types)
    - [Arrays and Objects](#example-1-1)
    - [Heterogeneous arrays as a tuples](#example-1-2)
    - [`null` as optional](#example-1-3)
    - [Variants](#example-1-4)
  - [User types](#user-types)
    - [Classes and structures](#example-2-1)
    - [Inheritance](#example-2-2)
  - [Format traits](#format-traits)
    - [Unquoted keys](#example-3-1)
    - [Unquoted string keys and single quotes](#example-3-2)
  - [Named parameters](#named-parameters)
    - [Floating-point precision](#example-4-1)
  - [Overriding default serializers](#overriding-default-serializers)
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

##### Example 1-1

*[Source](std-types-ex01.cxx)*

``` c++
// string, arrays and objects

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

##### Example 1-2

*[Source](std-types-ex02.cxx)*

``` c++
// JSON heterogeneous arrays as a tuples

#include "cxon/json.hxx"
#include "cxon/lib/std/tuple.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include <cassert>

static char in[] = // some heterogeneous JSON arrays
    R"([["Fibonacci", 5, [0, 1, 1, 2, 3]],)"
    R"( ["Pascal", 5, [1, 8, 24, 32, 16]]])"
;
// mapped to std::tuple
using sequence = std::tuple<std::string, unsigned, std::vector<unsigned>>;

int main() {
    std::vector<sequence> sqs;
        cxon::from_bytes(sqs, std::begin(in), std::end(in));
    assert(sqs == (std::vector<sequence>{
        sequence {"Fibonacci", 5, std::vector<unsigned> {0, 1, 1, 2, 3}},
        sequence {"Pascal", 5, std::vector<unsigned> {1, 8, 24, 32, 16}}
    }));
}
```

##### Example 1-3

*[Source](std-types-ex03.cxx)*

``` c++
// JSON null mapped to std::optional

#include "cxon/json.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include <cassert>

int main() {
#   ifdef CXON_HAS_LIB_STD_OPTIONAL
        std::vector<std::optional<std::string>> opt;
            cxon::from_bytes(opt, R"(["string", "", null])"); // expecting string or null
        assert(opt == (std::vector<std::optional<std::string>>{
            std::make_optional("string"),
            std::make_optional(""),
            std::nullopt,
        }));
#   endif
}
```

##### Example 1-4

*[Source](std-types-ex04.cxx)*

``` c++
// JSON values mapped to std::variant

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/variant.hxx"
#include <cassert>

int main() {
#   ifdef CXON_HAS_LIB_STD_VARIANT
    {   // JSON value types are unambiguous and so, if std::variant types are mapped
        // to distinct JSON value types, they'll be serialized correspondingly
        using uvar = std::variant<std::monostate, std::vector<int>, std::map<std::string, int>>; // std::variant<null, array, object>
        uvar var;
            cxon::from_bytes(var, R"([1, 3])");
        assert(var == (uvar(std::vector<int>{1, 3})));
            cxon::from_bytes(var, R"({"one": 1, "three": 3})");
        assert(var == (uvar(std::map<std::string, int>{{"one", 1}, {"three", 3}})));
            cxon::from_bytes(var, R"(null)");
        assert(var == (uvar()));
    }
    {   // if not, they'll be serialized as object {"<index>": <value>}
        using avar = std::variant<std::map<std::string, int>, std::map<int, std::string>>; // std::variant<object, object>
        avar var;
            cxon::from_bytes(var, R"({"0": {"one": 1, "three": 3}})");
        assert(var == (avar(std::map<std::string, int>{{"one", 1}, {"three", 3}})));
            cxon::from_bytes(var, R"({"1": {"1": "one", "3": "three"}})");
        assert(var == (avar(std::map<int, std::string>{{1, "one"}, {3, "three"}})));
    }
#   endif
}
```

*TODO: more examples needed here*


--------------------------------------------------------------------------------

#### User types

##### Example 2-1

*[Source](user-types-ex01.cxx)*

``` c++
// user types

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

##### Example 2-2

*[Source](user-types-ex02.cxx)*

``` c++
// inheritance

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/memory.hxx"
#include <cassert>

// class hierarchy with a base B and derived D1 and D2

struct B {
    enum type { D1, D2 } type;

    B(enum type t) : type(t) {}
    virtual ~B() {}
};
// the base class needs not be serializable

struct D1 : B {
    int d1;

    D1(int n) : B(B::D1), d1(n) {}
};
// derived classes should be writable
CXON_JSON_CLS_WRITE(D1, CXON_JSON_CLS_FIELD_ASIS(d1))

struct D2 : B {
    std::string d2;

    D2(std::string&& s) : B(B::D2), d2(std::move(s)) {}
};
CXON_JSON_CLS_WRITE(D2, CXON_JSON_CLS_FIELD_ASIS(d2))


// class holding a vector of B pointers
struct U {
    std::vector<std::unique_ptr<B>> polymorphic;
};
CXON_JSON_CLS(U, CXON_JSON_CLS_FIELD_ASIS(polymorphic))


namespace cxon {

    // this class will be used to parse B pointers
    // and should be a union of all derived classes
    struct B_union {
        int d1;                     // from D1
        std::string d2;             // from D2
        B_union() : d1(), d2() {}   // define proper defaults as from all the members
                                    // the real type should be decidable
        // tis class is only used for reading
        CXON_JSON_CLS_READ_MEMBER(B_union, CXON_JSON_CLS_FIELD_ASIS(d1), CXON_JSON_CLS_FIELD_ASIS(d2))
    };

    // specialize the read/write structs for B*

    template <typename X>
        struct read<JSON<X>, B*> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(B*& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<Y>(i, e, cx)) // consume any white-spaces
                        return false;
                    if (cio::peek(i, e) == *Y::id::nil) // check/read null
                        return read_value<Y>(t, i, e, cx);
                    B_union p; // the union type that represents a B*
                        if (read_value<Y>(p, i, e, cx)) { // try reading it
                            if (p.d1) // let d1 != 0 indicates D1
                                return (t = alc::make_in_context<D1>(cx, p.d1)); // create and assign D1
                            // create and assign D2
                            return (t = alc::make_in_context<D2>(cx, std::move(p.d2)));
                        }
                        return false;
                }
        };

    template <typename X>
        struct write<JSON<X>, B*> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const B* t, Cx& cx) {
                    if (!t) // check/write null
                            return write_value<Y>(o, nullptr, cx);
                    // cast and write the runtime type
                    switch (t->type) {
                        case B::D1:
                            return write_value<Y>(o, static_cast<const D1*>(t), cx);
                        case B::D2:
                            return write_value<Y>(o, static_cast<const D2*>(t), cx);
                    }
                    return false;
                }
        };

}


int main() {
    U u;
        cxon::from_bytes(u,
            R"({"polymorphic": [{"d2": "42"}, {"d1": 42}]})"
        );
    std::string o;
        cxon::to_bytes(o, u);
    assert(o == R"({"polymorphic":[{"d2":"42"},{"d1":42}]})");
}
```

*TODO: more examples needed here*


--------------------------------------------------------------------------------

#### Format traits

##### Example 3-1

*[Source](format-traits-ex01.cxx)*

``` c++
// format traits, unquoted keys

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

##### Example 3-2

*[Source](format-traits-ex02.cxx)*

``` c++
// format traits, unquoted string keys and single quotes

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

##### Example 4-1

*[Source](named-parameters-ex01.cxx)*

``` c++
// named parameters, floating-point precision

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

#### `CXCF`

##### Example 6-1

*[Source](cxcf-ex01.cxx)*

``` c++
// CXCF configuration format

#include "cxon/cxcf.hxx"
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

int main() {
    // load
    config cf;
        auto const r = cxon::from_bytes(cf, cf_);
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
