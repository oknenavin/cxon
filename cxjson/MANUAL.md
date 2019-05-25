## `CXON/CXJSON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------
#### Contents
- [`cxjson::basic_node`](#cxjson-basic_node)
- [`CXON` Integration](#cxon-integration)


--------------------------------------------------------------------------------
#### `cxjson::basic_node`

*Defined in header [cxjson.hxx](cxjson.hxx)*

``` c++
namespace cxjson {
    template <typename Traits>
        struct basic_node;
}
```

###### Aliases

Type            | Definition
----------------|---------------------------------------
`cxjson::node`  | `using node = basic_node<node_traits>`

###### Template parameters

  - `Traits` - traits class specifying the actual types of each `JSON` value type

###### Member types

Member type |Definition
------------|------------------------------------------
`null`      | `Traits::null_type`
`boolean`   | `Traits::boolean_type`
`number`    | `Traits::number_type`
`string`    | `Traits::string_type`
`array`     | `Traits::array_type<basic_node>`
`object`    | `Traits::object_type<string, basic_node>`

###### Member functions

- [`(constructor)`](#constructors) - construct a basic_node  
- `(destructor)` - destroys the node, deallocating internal storage if used  
- [`operator =`](#assignment-operators) - assigns values to the node  
- [`reset`](#reset) - resets the node  
- [`type`](#type) - returns node's value type  
- [`is`](#is) - returns `true` if node's value is of given type  
- [`imbue`](#imbue) - resets node's value type; returns value reference  
- [`get`](#get) - returns value reference  
- [`get_if`](#get_if) - returns value pointer if node's value type matches  
- [`operator ==`](#comparison-operators) - compare for equality  
- [`operator !=`](#comparison-operators) - compare for inequality  


--------------------------------------------------------------------------------
##### Constructors

``` c++
basic_node();                       (1)

basic_node(basic_node&& o);         (2)
basic_node(const basic_node& o);

basic_node(object&& v);             (3)
basic_node(const object& v);
basic_node(array&& v);
basic_node(const array& v);
basic_node(string&& v);
basic_node(const string& v);
basic_node(number&& v);
basic_node(const number& v);
basic_node(boolean&& v);
basic_node(const boolean& v);
basic_node(null&& v);
basic_node(const null& v);

basic_node(int v);                  (4)
basic_node(const char* v);
```

Constructs new node from a variety of data sources.
- `(1)` Default constructor. Constructs node with `null` value type.
- `(2)` Move and copy constructors:
    - constructs the node with the contents of `o` using move semantics, `o` is left  
    in valid state, but its value is in unspecified state
    - constructs the node with the copy of the contents of `o`
- `(3)` Move and copy constructors for each value type
- `(4)` Constructors for `string` and `number` value types

###### Example

``` c++
using namespace cxjson;
{   // (1)
    node n; assert(n.is<node::null>());
}
{   // (2)
    node o = true; assert(o.is<node::boolean>());
    node n(o); assert(n.is<node::boolean>() && n.get<node::boolean>());
}
{   // (3)
    node n(42.0); assert(n.is<node::number>() && n.get<node::number>() == 42.0);
}
{   // (3)
    node::object const o = { {"key", "value"} };
    node n(o); assert(n.is<node::object>() && n.get<node::object>() == o);
}
{   // (3)
    node::array const a = { 1, "string" };
    node n(a); assert(n.is<node::array>() && n.get<node::array>() == a);
}
{   // (4)
    node n(42); assert(n.is<node::number>() && n.get<node::number>() == 42);
}
{   // (4)
    node n("string"); assert(n.is<node::string>() && n.get<node::string>() == "string");
}
```


--------------------------------------------------------------------------------
##### Assignment operators

``` c++
basic_node& operator =(basic_node&& o);         (1)
basic_node& operator =(const basic_node& o);

basic_node& operator =(object&& v);             (2)
basic_node& operator =(const object& v);
basic_node& operator =(array&& v);
basic_node& operator =(const array& v);
basic_node& operator =(string&& v);
basic_node& operator =(const string& v);
basic_node& operator =(number&& v);
basic_node& operator =(const number& v);
basic_node& operator =(boolean&& v);
basic_node& operator =(const boolean& v);
basic_node& operator =(null&& v);
basic_node& operator =(const null& v);

basic_node& operator =(int v);                  (3)
basic_node& operator =(const char* v);
```

Replaces the contents of the node: 
- `(1)` Replaces the content with those of `o`:
    - using move semantics, `o` is left in valid state, but its value is in unspecified state
    - constructs the node with copy of the contents of `o`
- `(2)` For each value type, replaces the content with those of `v`
- `(3)` For `string` and `number` value types, replaces the content with those of `v`

###### Return value
`*this`


--------------------------------------------------------------------------------
##### reset

``` c++
void reset();
```

Resets the content of node, the value type is `null`.


--------------------------------------------------------------------------------
##### type

``` c++
node_type type() const noexcept;
```

###### Return value
`node_type` of the value

###### Example

``` c++
using namespace ;
node const n; assert(n.type() == node_type::null);
```


--------------------------------------------------------------------------------
##### is

``` c++
template <typename T>
    bool is() const;
```

Checks if value type is `T`.

###### Return value
`true` if the type is `T`, `false` otherwise


--------------------------------------------------------------------------------
##### imbue

``` c++
template <typename T>
    T& imbue();
```

Changes the value type of the node. If `T` is different than nodes's value type,
the content is reset.

###### Return value
If `T` is same as the value type, a reference to it; otherwise, reference to the
new value

###### Example

``` c++
using namespace cxjson;
{   // T is the same
    node n = "string";
    node::string& v = n.imbue<node::string>(); assert(v == "string");
}
{   // T is not the same
    node n = "string";
    node::number& v = n.imbue<node::number>(); assert(v == 0);
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
using namespace cxjson;
node n = "one";
    n.get<node::string>() = "another";
assert(n.get<node::string>() == "another");
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
using namespace cxjson;
node n = "one";
    auto *v = n.get_if<node::string>(); assert(v != nullptr);
assert(n.get_if<node::array>() == nullptr);
```


--------------------------------------------------------------------------------
##### Comparison operators

``` c++
bool operator == (const basic_node& n) const; (1)
bool operator != (const basic_node& n) const; (2)
```

###### Return value
- `(1)` `true` if equal, `false` otherwise
- `(2)` `false` if equal, `true` otherwise


--------------------------------------------------------------------------------
#### `CXON` Integration

`cxjson::basic_node` can be serialized as any other type.  
`CXJSON` defines the following in addition:
- own error conditions

  Error code                      | Message
  --------------------------------|-------------------------------
  error::invalid                  | invalid `JSON`
  error::recursion_depth_exceeded | recursion depth limit exceeded

- own context parameters

  Parameter         | Context    | Type       | Default | Description
  ------------------|------------|------------|---------|-------------------------
  `recursion_guard` | read/write | `unsigned` | 0 (N/A) | recursion guard state
  `recursion_depth` | read/write | `unsigned` | 64      | max recursion depth
  
  *Note that the interface is overloaded for `cxjson::basic_node` and the overload
   passes `recursion_guard` parameter. If `cxjson::basic_node` is part of a type
   (e.g. `std::vector<basic_node>`) and guarding against recursion is needed, then
   `recursion_guard` parameter must be passed explicitly.*

###### Example

``` c++
#include "cxon/cxjson/cxjson.hxx"
#include <cassert>

int main() {
    using namespace cxjson;
    {   node n;
            auto const r = cxon::from_chars(n, "#[1]");
        assert(!r &&
                r.ec.category() == error_category::value &&
                r.ec == error::invalid
        );
    }
    {   node n;
            auto const r = cxon::from_chars(n, "[[[[1]]]]", recursion_depth::set<unsigned, 4U>());
        assert(!r &&
                r.ec.category() == error_category::value &&
                r.ec == error::recursion_depth_exceeded
        );
    }
}
```


--------------------------------------------------------------------------------
Distributed under the MIT license. See [`LICENSE`](../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060
