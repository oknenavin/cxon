## `CXON/CBOR/NODE` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------

#### Contents
- [Introduction](#introduction)
- [`cxon::cbor::basic_node`](#basic_node)
- [`CXON` integration](#cxon-integration)


--------------------------------------------------------------------------------

#### Introduction

`CXON/CBOR/NODE` is an implementation of a polymorphic type, which can represent an arbitrary `CBOR`.  
`CBOR` value type mapping is configurable with the following defaults:

Major type (MT)             | Default binding
----------------------------|-----------------------------------------
`unsigned integer / MT0`    | [`long long`][cpp-typ]
`negative integer / MT1`    | [`unsigned long long`][cpp-typ]
`byte string / MT2`         | [`std::vector<unsigned char>`][cpp-vect]
`text string / MT3`         | [`std::string`][cpp-str]
`array / MT4`               | [`std::vector`][cpp-vect]
`map / MT5`                 | [`std::map`][cpp-map]
`boolean / MT7 (1)`         | [`bool`][cpp-typ]
`null / MT7 (2)`            | [`std::nullptr_t`][cpp-typ]
`floating point / MT7 (3)`  | [`double`][cpp-typ]

*`(1)` `0xF4` / `0xF5`*  
*`(2)` `0xF6`*  
*`(3)` `0xFA` (single-precision), `0xFA` (double-precision) - `0xF9` (half-precision) is not yet supported*

###### Example 1

Build, write and read:

``` c++
#include "cxon/cbor.hxx"
#include "cxon/lib/node.hxx"
#include <cassert>

int main() {
    using node = cxon::cbor::node;

    unsigned char const b0[] = "\xA2\144even\x82\x02\x04\x63odd\x82\x01\x03";
    node const n0 = node::map {
        { "even", node::array { 2U, 4U } },
        { "odd", node::array { 1U, 3U } }
    };

    node n1; // read
        cxon::from_bytes(n1, b0);
    assert(n1 == n0);

    std::basic_string<unsigned char> b1; // write
        cxon::to_bytes(b1, n0);
    assert(b1 == b0);
}
```

###### Example 2

Build using node's methods:

``` c++
#include "cxon/cbor.hxx"
#include "cxon/lib/node.hxx"
#include <cassert>

int main() {
    using node = cxon::cbor::node;

    // build using initializer lists
    node n1 = node::array {
        -1,                         // sint
        node::uint(1),              // uint
        node::bytes {0x01, 0x02},   // bytes
        "text",                     // text
        node::array {1, 2, 3},      // array
        node::map {                 // map
            { 1, 1 }, // { key, value }
            { node::uint(1), 1 },
            { node::bytes {0x01, 0x02}, 1 },
            { "text", 1 },
            { node::array {1, 2, 3}, 1 },
            { node::map {{1, 2}, {3, 4}}, 1 },
            { false, 1 },
            { nullptr, 1 },
            { 1.0, 1 }
        },
        true,                       // bool
        false,                      // bool
        nullptr,                    // null
        1.0                         // real
    };

    // build using node's methods
    node n2;
        assert(n2.is<node::null>()); // default node type is node_kind::null
        auto& a = n2.imbue<node::array>(); // change the type and return its value
            assert(n2.is<node::array>());
            a.push_back(-1);                        assert(a.back().is<node::sint>());
            a.push_back(node::uint(1));             assert(a.back().is<node::uint>());
            a.push_back(node::bytes {0x01, 0x02});  assert(a.back().is<node::bytes>());
            a.push_back("text");                    assert(a.back().is<node::text>());
            a.push_back(node:: array {});           assert(a.back().is<node::array>());
            auto* a1 = a.back().get_if<node::array>();
                a1->push_back(1);                   assert(a1->back().is<node::sint>());
                a1->push_back(2);                   assert(a1->back().is<node::sint>());
                a1->push_back(3);                   assert(a1->back().is<node::sint>());
            a.push_back(node:: map {});             assert(a.back().is<node::map>());
            auto& m = a.back().get<node::map>();
                m[1] = 1;                           assert(m.find(node::sint(1)) != m.end());
                m[node::uint(1)] = 1;               assert(m.find(node::uint(1)) != m.end());
                m[node::bytes {0x01, 0x02}] = 1;    assert(m.find(node::bytes {0x01, 0x02}) != m.end());
                m["text"] = 1;                      assert(m.find(node::text("text")) != m.end());
                m[node::array {1, 2, 3}] = 1;       assert(m.find(node::array({1, 2, 3})) != m.end());
                m[node::map {{1, 2}, {3, 4}}] = 1;  assert(m.find(node::map({{1, 2}, {3, 4}})) != m.end());
                m[false] = 1;                       assert(m.find(node::boolean(false)) != m.end());
                m[nullptr] = 1;                     assert(m.find(node::null(nullptr)) != m.end());
                m[1.0] = 1;                         assert(m.find(node::real(1.0)) != m.end());
            a.push_back(true);                      assert(a.back().is<node::boolean>());
            a.push_back(false);                     assert(a.back().is<node::boolean>());
            a.push_back(nullptr);                   assert(a.back().is<node::null>());
            a.push_back(1.0);                       assert(a.back().is<node::real>());

    assert(n1 == n2);

    std::vector<unsigned char> b1;
        cxon::to_bytes(b1, n1);
    std::vector<unsigned char> b2;
        cxon::to_bytes(b2, n2);
    assert(b1 == b2);

    node n3;
        cxon::from_bytes(n3, b2);
    node n4;
        cxon::from_bytes(n4, b1);
    assert(n4 == n3);
}
```


--------------------------------------------------------------------------------

#### `basic_node`

*Defined in header [`cxon/lang/cbor/node/node.hxx`](node.hxx)*  
*Include [`cxon/lib/node.hxx`](../../../lib/node.hxx)*

``` c++
namespace cxon::cbor {
    template <typename Traits>
        struct basic_node;
}
```

###### Aliases

Type            | Definition
----------------|---------------------------------------
`cxon::cbor::node`  | `using node = basic_node<node_traits>`

###### Template parameters

  - [`Traits`](#traits) - traits class specifying the mapping type for each `CBOR` value type

###### Non-member types

```c++
enum class node_kind { sint, uint, bytes, text, array, map, boolean, null, real };
```

###### Member types

Member type | Definition
------------|------------------------------------------
`sint`      | `Traits::sint_type`
`uint`      | `Traits::uint_type`
`bytes`     | `Traits::bytes_type`
`text`      | `Traits::text_type`
`array`     | `Traits::array_type<basic_node>`
`map`       | `Traits::map_type<basic_node, basic_node>`
`boolean`   | `Traits::boolean_type`
`null`      | `Traits::null_type`
`real`      | `Traits::real_type`

###### Member functions

  - [`(constructor)`](#constructors) - construct a basic_node
  - `(destructor)` - destroys the node, deallocating any internal storage if used
  - [`operator =`](#assignment-operators) - assigns values to the node
  - [`reset`](#reset) - resets the node
  - [`kind`](#kind) - returns node's value type
  - [`is`](#is) - returns `true` if node's value is of given type
  - [`imbue`](#imbue) - resets node's value type; returns value reference
  - [`get`](#get) - returns value reference
  - [`get_if`](#get_if) - returns value pointer if node's value type matches
  - [`operator ==`](#comparison-operators) - compare for equality
  - [`operator !=`](#comparison-operators) - compare for inequality
  - [`operator <`](#comparison-operators) - compare for less than

###### Non-member functions

  - `is` - returns `true` if node's value is of given type
  - `imbue` - resets node's value type; returns value reference
  - `get` - returns value reference
  - `get_if` - returns value pointer if node's value type matches

Same as the member counterparts with single `basic_node&` argument.


--------------------------------------------------------------------------------

##### Traits

###### Example

```c++
struct traits : cxon::cbor::node_traits {
    using                               bytes_type = std::basic_string<unsigned char>;
    template <class T> using            array_type = std::list<T>;
    template <class K, class V> using   map_type = std::multimap<K, V>;
};
...
using node = cxon::cbor::basic_node<traits>;
node n;
cxon::from_bytes(n, "\xA2\x63key\x2A\x63key\x2B");
assert(n.is<node::map>() && n.get<node::map>().count("key") == 2);
```

--------------------------------------------------------------------------------

##### Constructors

``` c++
basic_node();                       (1)

basic_node(basic_node&& o);         (2)
basic_node(const basic_node& o);


basic_node(sint&& v);               (3)
basic_node(const sint& v);
basic_node(uint&& v);
basic_node(const uint& v);
basic_node(bytes&& v);
basic_node(const bytes& v);
basic_node(text&& v);
basic_node(const text& v);
basic_node(array&& v);
basic_node(const array& v);
basic_node(map&& v);
basic_node(const map& v);
basic_node(boolean&& v);
basic_node(const boolean& v);
basic_node(null&& v);
basic_node(const null& v);
basic_node(real&& v);
basic_node(const real& v);

basic_node(<integral> v);           (4)
basic_node(const char* v);
```

Construct new node from a variety of data sources.
  - `(1)` Default constructor. Constructs node with `null` value type.
  - `(2)` Move and copy constructors
  - `(3)` Move and copy constructors for each value type
  - `(4)` Constructors for `string` and `real` value types

###### Example

``` c++
using namespace cxon::cbor;
{   // (1)
    node n; assert(n.is<node::null>());
}
{   // (2)
    node o = true; assert(o.is<node::boolean>());
    node n(o); assert(n.is<node::boolean>() && n.get<node::boolean>());
}
{   // (3)
    node n(42.0); assert(n.is<node::real>() && n.get<node::real>() == 42.0);
}
{   // (3)
    node::map const o = { {"key", "value"} };
    node n(o); assert(n.is<node::map>() && n.get<node::map>() == o);
}
{   // (3)
    node::array const a = { 1, "string" };
    node n(a); assert(n.is<node::array>() && n.get<node::array>() == a);
}
{   // (4)
    node n(42); assert(n.is<node::sint>() && n.get<node::sint>() == 42);
}
{   // (4)
    node n(42U); assert(n.is<node::uint>() && n.get<node::uint>() == 42);
}
{   // (4)
    node n("string"); assert(n.is<node::text>() && n.get<node::text>() == "string");
}
```


--------------------------------------------------------------------------------

##### Assignment operators

``` c++
basic_node& operator =(basic_node&& o);         (1)
basic_node& operator =(const basic_node& o);

basic_node& operator =(sint&& v);               (2)
basic_node& operator =(const sint& v);
basic_node& operator =(uint&& v);
basic_node& operator =(const uint& v);
basic_node& operator =(map&& v);
basic_node& operator =(const map& v);
basic_node& operator =(array&& v);
basic_node& operator =(const array& v);
basic_node& operator =(bytes&& v);
basic_node& operator =(const bytes& v);
basic_node& operator =(text&& v);
basic_node& operator =(const text& v);
basic_node& operator =(boolean&& v);
basic_node& operator =(const boolean& v);
basic_node& operator =(null&& v);
basic_node& operator =(const null& v);
basic_node& operator =(real&& v);
basic_node& operator =(const real& v);

basic_node& operator =(<integral> v);           (3)
basic_node& operator =(const char* v);
```

Replaces the content of the node: 
  - `(1)` Move or copy of `o`
  - `(2)` Move or copy of `v`
  - `(3)` Copy of `v`

###### Return value

`*this`


--------------------------------------------------------------------------------

##### reset

``` c++
void reset();
```

Resets the content of the node, the value type is `null`.


--------------------------------------------------------------------------------

##### kind

``` c++
node_kind kind() const noexcept;
```

###### Return value

`node_kind` of the value

###### Example

``` c++
using namespace cxon::cbor;
node const n; assert(n.kind() == node_kind::null);
```


--------------------------------------------------------------------------------

##### is

``` c++
template <typename T>
    bool is() const;
```

Checks if the value type is `T`.

###### Return value

`true` if the type is `T`, `false` otherwise


--------------------------------------------------------------------------------

##### imbue

``` c++
template <typename T>
    T& imbue();
```

Changes the value type of the node. If `T` is different than nodes's value type,
the content will be reset.

###### Return value
If `T` is same as the value type, a reference to it; otherwise, reference to the
new value

###### Example

``` c++
using namespace cxon::cbor;
{   // T is the same
    node n = "string";
    node::text& v = n.imbue<node::text>(); assert(v == "string");
}
{   // T is not the same
    node n = "string";
    node::real& v = n.imbue<node::real>(); assert(v == 0);
}
```


--------------------------------------------------------------------------------

##### get

``` c++
template <typename T>
    T& get();
template <typename T>
    const T& get() const;
```

###### Return value

`T&`, the behavior is undefined if `T` is not same as the value type

###### Example

``` c++
using namespace cxon::cbor;
node n = "one";
    n.get<node::text>() = "another";
assert(n.get<node::text>() == "another");
```


--------------------------------------------------------------------------------

##### get_if

``` c++
template <typename T>
    T* get_if() noexcept;
template <typename T>
    const T* get_if() const noexcept;
```

###### Return value

`T*` if `T` is same as the value type, `nullptr` otherwise

###### Example

``` c++
using namespace cxon::cbor;
node n = "one";
    auto *v = n.get_if<node::text>(); assert(v != nullptr);
assert(n.get_if<node::array>() == nullptr);
```


--------------------------------------------------------------------------------

##### Comparison operators

``` c++
bool operator == (const basic_node& n) const; (1)
bool operator != (const basic_node& n) const; (2)
bool operator  < (const basic_node& n) const; (3)
```

###### Return value

  - `(1)` `true` if equal, `false` otherwise
  - `(2)` `false` if equal, `true` otherwise
  - `(3)` `true` if less than, `false` otherwise


--------------------------------------------------------------------------------

#### `CXON` integration

`basic_node` can be serialized as any other type.  
`CXON/CBOR` defines the following in addition:
  - own error conditions

      Error code                           | Message
      -------------------------------------|-------------------------------
      node_error::invalid                  | invalid `CBOR`
      node_error::recursion_depth_exceeded | recursion depth limit exceeded

  - own context parameters

      Parameter         | Context    | Type       | Default | Description
      ------------------|------------|------------|---------|-------------------------
      `recursion_guard` | read/write | `unsigned` | 0 (N/A) | recursion guard state
      `recursion_depth` | read/write | `unsigned` | 64      | max recursion depth
  
      *Note: the interface is overloaded for `cxon::cbor::basic_node` and the overload
       passes `recursion_guard` parameter. If `cxon::cbor::basic_node` is part of a type
       (e.g. `std::vector<basic_node>`) and guarding against recursion is needed, then
       `recursion_guard` parameter must be passed explicitly.*

      *Note: currently calling of the overloads with parameter(s), e.g.
      `from_bytes(..., recursion_depth::set<unsigned, 4U>())`,
      fail to compile with g++ due to a bug in the compiler ([Bug 90642](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90642)).
      As a workaround, they may be called by passing the traits parameters explicitly - e.g.
      `from_bytes<FormatTraits, NodeTraits>(..., recursion_depth::set<unsigned, 4U>())`*

###### Example

``` c++
#include "cxon/cbor.hxx"
#include "cxon/lib/node.hxx"
#include <cassert>

int main() {
    using namespace cxon::cbor;
    {   node n;
            auto const r = cxon::from_bytes(n, "\xFF");
        assert(!r &&
                r.ec.category() == node_error_category::value() &&
                r.ec == node_error::invalid
        );
    }
    {   node n;
            auto const r = cxon::from_bytes(n, "\x81\x81\x81\x81\x01", recursion_depth::set<4>());
        assert(!r &&
                r.ec.category() == node_error_category::value() &&
                r.ec == node_error::recursion_depth_exceeded
        );
    }
}
```


--------------------------------------------------------------------------------

Distributed under the MIT license. See [`LICENSE`](../../../../../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060
[cpp-map]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/map&oldid=109218
[cpp-vec]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/vector&oldid=107643
[cpp-str]: https://en.cppreference.com/mwiki/index.php?title=cpp/string/basic_string&oldid=107637
[cpp-typ]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
