## `CXON/CBOR/NODE` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------

#### Contents
- [Introduction](#introduction)
- [`cxon::cbor::basic_node`](#basic_node)
- [Helper classes](#helper-classes)
  - [`cxon::cbor::undefined`](#undefined)
  - [`cxon::cbor::simple`](#simple)
  - [`cxon::cbor::taggle`](#taggle)
- [`CXON` integration](#cxon-integration)


--------------------------------------------------------------------------------

#### Introduction

`CXON/CBOR/NODE` is an implementation of a polymorphic type, which can represent an arbitrary `CBOR`.  
`CBOR` value type mapping is configurable with the following defaults:

Major type (MT)             | Default binding
----------------------------|-----------------------------------------
`positive integer / MT0`    | [`long long`][cpp-typ]
`negative integer / MT1`    | [`unsigned long long`][cpp-typ]
`byte string / MT2`         | [`std::vector<unsigned char>`][cpp-vec]
`text string / MT3`         | [`std::string`][cpp-str]
`array / MT4`               | [`std::vector`][cpp-vec]
`map / MT5`                 | [`std::map`][cpp-map]
`tag / MT6 (1)`             | [`cbor::taggle`](#taggle)
`boolean / MT7 (2)`         | [`bool`][cpp-typ]
`null / MT7 (3)`            | [`std::nullptr_t`][cpp-typ]
`undefined / MT7 (4)`       | [`cbor::undefined`](#undefined)
`floating point / MT7 (5)`  | [`double`][cpp-typ]
`simple / MT7 (6)`          | [`cbor::simple`](#simple)

*`(1)` essentially a pair of a `positive integer` and a `node`*  
*`(2)` `0xF4` / `0xF5`*  
*`(3)` `0xF6`*  
*`(4)` `0xF7`*  
*`(5)` `0xFA` (single-precision), `0xFB` (double-precision) - `0xF9` (half-precision) is not fully supported*  
*`(6)` only unassigned values*

###### Example 1

Build, write and read:

``` c++
#include "cxon/cbor.hxx"
#include "cxon/lib/node.hxx"
#include <cassert>

int main() {
    using node = cxon::cbor::node;

    unsigned char const b0[] = "\xA2\144even\x82\x02\x04\x63odd\x82\x01\x03";
    node const n0 = {
        { "even", { 2U, 4U } },
        { "odd", { 1U, 3U } }
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
    node n1 = {
        -1,                         // sint
        1U,                         // uint
        node::bytes {0x01, 0x02},   // bytes
        "text",                     // text
        {1, 2, 3},                  // array
        {                           // map
            { 1, 1 }, // { key, value }
            { 1U, 1 },
            { node::bytes {0x01, 0x02}, 1 },
            { "text", 1 },
            { {1, 2, 3}, 1 },
            { {{1, 2}, {3, 4}}, 1 },
            { node::tag {1, 2}, 1 },
            { false, 1 },
            { nullptr, 1 },
            { node::undefined(), 1 },
            { 1.0, 1 },
            { node::simple(16), 1 }
        },
        node::tag {1, 2},           // tag
        true,                       // bool
        false,                      // bool
        nullptr,                    // null
        node::undefined(),          // undefined
        1.0,                        // real
        node::simple(24)            // simple
    };

    // build using node's methods
    node n2;
        assert(n2.is<node::undefined>()); // default node type is node_kind::undefined
        auto& a = n2.imbue<node::array>(); // change the type and return its value
            assert(n2.is<node::array>());
            a.push_back(-1);                        assert(a.back().is<node::sint>());
            a.push_back(1U);                        assert(a.back().is<node::uint>());
            a.push_back(node::bytes {0x01, 0x02});  assert(a.back().is<node::bytes>());
            a.push_back("text");                    assert(a.back().is<node::text>());
            a.push_back(node:: array {});           assert(a.back().is<node::array>());
            auto* a1 = a.back().get_if<node::array>();
                a1->push_back(1);                   assert(a1->back().is<node::sint>());
                a1->push_back(2);                   assert(a1->back().is<node::sint>());
                a1->push_back(3);                   assert(a1->back().is<node::sint>());
                //a.push_back(node::map {});
                a.push_back(node({}));              assert(a.back().is<node::map>());
            auto& m = a.back().get<node::map>();
                m[1] = 1;                           assert(m.find(node::sint(1)) != m.end());
                m[1U] = 1;                          assert(m.find(node::uint(1)) != m.end());
                m[node::bytes {0x01, 0x02}] = 1;    assert(m.find(node::bytes {0x01, 0x02}) != m.end());
                m["text"] = 1;                      assert(m.find(node::text("text")) != m.end());
                m[{1, 2, 3}] = 1;                   assert(m.find(node::array({1, 2, 3})) != m.end());
                m[{{1, 2}, {3, 4}}] = 1;            assert(m.find(node::map({{1, 2}, {3, 4}})) != m.end());
                m[node::tag {1, 2}] = 1;            assert(m.find(node::tag(1, 2)) != m.end());
                m[false] = 1;                       assert(m.find(node::boolean(false)) != m.end());
                m[nullptr] = 1;                     assert(m.find(node::null(nullptr)) != m.end());
                m[node::undefined {}] = 1;          assert(m.find(node::undefined {}) != m.end());
                m[1.0] = 1;                         assert(m.find(node::real(1.0)) != m.end());
                m[node::simple {16}] = 1;           assert(m.find(node::simple {16}) != m.end());
            a.push_back(node::tag(1, 2));           assert(a.back().is<node::tag>());
            a.push_back(true);                      assert(a.back().is<node::boolean>());
            a.push_back(false);                     assert(a.back().is<node::boolean>());
            a.push_back(nullptr);                   assert(a.back().is<node::null>());
            a.push_back(node::undefined {});        assert(a.back().is<node::undefined>());
            a.push_back(1.0);                       assert(a.back().is<node::real>());
            a.push_back(node::simple {24});         assert(a.back().is<node::simple>());

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

`basic_node` is compliant with [AllocatorAwareContainer][cpp-alaw] requirements.

###### Aliases

Type               | Definition
-------------------|---------------------------------------
`cxon::cbor::node` | `using node = basic_node<node_traits>`

###### Template parameters

  - [`Traits`](#traits) - traits class specifying the allocator, the mapping between C++ and JSON and whether the storage for
given type should be allocated on the stack or in the heap.

###### Non-member types

```c++
enum class node_kind { map, array, bytes, text, tag, real, sint, uint, simple, boolean, null, undefined };
```

###### Member types

Member type      | Definition
-----------------|------------------------------------------
`map`            | `Traits::map_type<basic_node, basic_node>`
`array`          | `Traits::array_type<basic_node>`
`tag`            | `Traits::tag_type<uint, basic_node>`
`bytes`          | `Traits::bytes_type`
`text`           | `Traits::text_type`
`real`           | `Traits::real_type`
`sint`           | `Traits::sint_type`
`uint`           | `Traits::uint_type`
`simple`         | `Traits::simple_type`
`boolean`        | `Traits::boolean_type`
`null`           | `Traits::null_type`
`undefined`      | `Traits::undefined_type`
`allocator_type` | `rebind_t<typename Traits::allocator_type, basic_node>`

where `rebind_t` is defined as:
``` c++
template <typename Al, typename T>
    using rebind_t = typename std::allocator_traits<Al>::template rebind_alloc<T>;
```

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
  - [`swap`](#swap) - swaps the contents of two nodes

###### Non-member functions

  - [`operator ==`](#comparison-operators) - compare for equality
  - [`operator !=`](#comparison-operators) - compare for inequality
  - [`operator <`](#comparison-operators) - compare for less than
  - [`hash`](#hash) - function-object implementing [`Hash`](cpp-hash) requirements
  - `is` - returns `true` if node's value is of given type `(1)`
  - `imbue` - resets node's value type; returns value reference `(1)`
  - `get` - returns value reference `(1)`
  - `get_if` - returns value pointer if node's value type matches `(1)`

*`(1)` Same as the member counterparts with single `basic_node&` argument.*


--------------------------------------------------------------------------------

##### Traits

The traits type specifies the allocator, the mapping between C++ and JSON and whether the storage for
given type should be allocated on the stack or in the heap.

``` c++
template <typename Al = std::allocator<...>>
    struct node_traits {
        // the allocator
        using                                       allocator_type  = Al;
        // JSON/C++ mapping
        template <typename K, typename V> using     map_type        = std::map<K, V, std::less<K>, rebind_t<Al, std::pair<const K, V>>>;
        template <typename T> using                 array_type      = std::vector<T, rebind_t<Al, T>>;
        template <typename N, typename T> using     tag_type        = taggle<N, T>;
        using                                       bytes_type      = std::vector<unsigned char, rebind_t<Al, unsigned char>>;
        using                                       text_type       = std::basic_string<char, std::char_traits<char>, rebind_t<Al, char>>;
        using                                       real_type       = double;
        using                                       sint_type       = long long;
        using                                       uint_type       = unsigned long long;
        using                                       simple_type     = simple<unsigned char>;
        using                                       boolean_type    = bool;
        using                                       null_type       = std::nullptr_t;
        using                                       undefined_type  = undefined;
        // the types listed here will be allocated in the heap
        using                                       dynamic_types   = integer_sequence<node_kind, node_kind::map, node_kind::array, node_kind::tag>;
    };
```

`dynamic_types` serves two purposes:
  - Support for containers that do not support *incomplete* types.
    For example, unordered maps and sets implementations in `libstdc++` do not support incomplete types and
    if for example, `basic_node::object` is mapped to `std::unordered_map`, it must be a dynamic type, otherwise
    the result will be a compilation error.
  - To reduce the size of `basic_node` as it's implemented as a union type.

###### Example

```c++
// custom mapping

struct traits : cxon::cbor::node_traits {
    template <class K, class V> using   map_type = std::multimap<K, V>;
    template <class T> using            array_type = std::list<T>;
    using                               bytes_type = std::basic_string<unsigned char>;
};
...
using node = cxon::cbor::basic_node<traits>;
node n;
    cxon::from_bytes(n, "\xA2\x63key\x2A\x63key\x2B");
assert(n.is<node::map>() && n.get<node::map>().count("key") == 2);
```

``` c++
// custom allocator

using node = cxon::cbor::basic_node<
    cxon::json::node_traits<std::pmr::polymorphic_allocator<void>>
>;
...
char bf[4096];
std::pmr::monotonic_buffer_resource rs(bf, sizeof(bf));
std::pmr::polymorphic_allocator<node> al(&rs);

node n(al);
    n = { 1, 2, 3};
assert(n == node::array { 1, 2, 3});
```


--------------------------------------------------------------------------------

##### Constructors

``` c++
basic_node();                           (1)

basic_node(basic_node&& o);             (2)
basic_node(const basic_node& o);


basic_node(sint&& v);                   (3)
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

basic_node(<integral> v);               (4)
basic_node(const char* v);

basic_node(std::initializer_list l);    (5)
```

Construct new node from a variety of data sources.
  - `(1)` Default constructor. Constructs node with `undefined` value type.
  - `(2)` Move and copy constructors
  - `(3)` Move and copy constructors for each value type
  - `(4)` Constructors for `integral` and `string` value types
  - `(5)` Constructors from initializer list. Will create an object if empty
          or array(s) of two elements, array otherwise

*Note: Each constructor has its allocator argument companion.*

###### Example

``` c++
using namespace cxon::cbor;
{   // (1)
    node n; assert(n.is<node::undefined>());
}
{   // (2)
    node n1 = true; assert(n1.is<node::boolean>());
    node n2(n1); assert(n2 == true);
    node n3(std::move(n1)); assert(n3 == true);
}
{   // (3)
    node n(42.0); assert(n == 42.0);
}
{   // (3)
    node::map const m = {{"key", "value"}};
    node n(m); assert(n == m);
}
{   // (3)
    node::array const a = {1, "string"};
    node n(a); assert(n == a);
}
{   // (4)
    node n(42); assert(n == 42);
}
{   // (4)
    node n(42U); assert(n == 42U);
}
{   // (4)
    node n("string"); assert(n == "string");
}
{   // (5)
    node n({{1, 2}, {3, 4}}); assert(n == (node::map {{1, 2}, {3, 4}}));
}
{   // (5)
    node n({1, 2, 3, 4}); CHECK(n == (node::array {1, 2, 3, 4}));
}
```


--------------------------------------------------------------------------------

##### Assignment operators

``` c++
basic_node& operator =(basic_node&& o);             (1)
basic_node& operator =(const basic_node& o);

basic_node& operator =(sint&& v);                   (2)
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

basic_node& operator =(<integral> v);               (3)
basic_node& operator =(const char* v);

basic_node& operator =(std::initializer_list l);    (4)
```

Replaces the content of the node: 
  - `(1)` Move or copy of `o`
  - `(2)` Move or copy of `v`
  - `(3)` Copy of `v`
  - `(4)` From initializer list. Object if empty or array(s) of two elements,
          array otherwise

###### Return value

`*this`


--------------------------------------------------------------------------------

##### reset

``` c++
void reset();
```

Resets the content of the node, the value type is `undefined`.


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
node const n; assert(n.kind() == node_kind::undefined);
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

##### Swap

``` c++
void swap(basic_node& n);
```

###### Example

``` c++
using namespace cxon::cbor;
node n1 = "text", n2 = {1, 2, 3};
n1.swap(n2); assert(n1 == node::array {1, 2, 3} && n2 == "text");
```


--------------------------------------------------------------------------------

##### Comparison operators

``` c++
friend bool operator ==(const basic_node& f, const basic_node& s) noexcept; (1)
friend bool operator !=(const basic_node& f, const basic_node& s) noexcept; (2)
friend bool operator  <(const basic_node& f, const basic_node& s) noexcept; (3)
```

###### Return value

  - `(1)` `true` if `f` equals `s`, `false` otherwise
  - `(2)` `false` if `f` equals `s`, `true` otherwise
  - `(2)` `true` if `f` is less than `s`, `false` otherwise

Also, `(1)` and `(2)` have overloads for each value type:

``` c++
friend bool operator ==(const basic_node& n, const basic_node::<value-type>& v) noexcept;                   (1)
friend bool operator ==(const basic_node::<value-type>& v, const basic_node& n) noexcept;
friend bool operator !=(const basic_node& n, const basic_node::<value-type>& v) noexcept;
friend bool operator !=(const basic_node::<value-type>& v, const basic_node& n) noexcept;

friend bool operator ==(const basic_node& n, const basic_node::null& v) noexcept;                           (2)
friend bool operator ==(const T& basic_node::null, const basic_node& n) noexcept;

friend bool operator ==(const basic_node& n, undefined) noexcept;                                           (2)
friend bool operator ==(undefined, const basic_node& n) noexcept;

friend bool operator ==(const basic_node& n, const typename basic_node::text::value_type& v) noexcept;      (3)
friend bool operator ==(const typename basic_node::text::value_type& v, const basic_node& n) noexcept;
friend bool operator !=(const basic_node& n, const typename basic_node::text::value_type& v) noexcept;
friend bool operator !=(const typename basic_node::text::value_type& v, const basic_node& n) noexcept;
```
  -  `(1)` for one of `map`, `array`, `tag`, `bytes`, `string`, `real`, `sint`, `uint`, `boolean`
  -  `(2)` for `null` and `undefined`
  -  `(3)` for `string::value_type**_` - char pointers

###### Example

``` c++
using namespace cxon::cbor;
node n = 42;
assert(n == 42 && 42 == n);
assert(n != 24 && 24 != n);
node m = 43;
assert(n < m);
```


--------------------------------------------------------------------------------

#### hash

``` c++
namespace cxon {
    template <typename Tr>              struct hash<cbor::basic_node<Tr>>;
    template <>                         struct hash<cbor::undefined>;
    template <typename T>               struct hash<cbor::simple<T>>;
    template <typename T, typename V>   struct hash<cbor::taggle<T, V>>;
}

namespace std {
    template <typename Tr> struct hash<cxon::cbor::basic_node<Tr>>;
}
```


--------------------------------------------------------------------------------

#### Helper classes

##### `undefined`

``` c++
struct undefined {
    bool operator ==(const undefined&) const { return true; }
};
```

##### `simple`

``` c++
template <typename T>
    struct simple {
        T value;

        simple(T t = {}) : value(t) {}
        operator T() const { return value; }
    };
```

##### `taggle`

``` c++
template <typename N, typename T>
    struct taggle {
        public:     N tag;
        public:     T value;

        constexpr taggle() noexcept(std::is_nothrow_constructible<T>::value);
        taggle(N n, T&& t) noexcept(std::is_nothrow_move_constructible<T>::value);
        taggle(N n, const T& t) noexcept(std::is_nothrow_copy_constructible<T>::value);

        taggle(taggle&& t) noexcept(std::is_nothrow_move_constructible<T>::value);
        taggle& operator =(taggle&& t) noexcept(std::is_nothrow_move_assignable<T>::value);

        taggle(const taggle& t) noexcept(std::is_nothrow_copy_constructible<T>::value);
        taggle& operator =(const taggle& t) noexcept(std::is_nothrow_copy_assignable<T>::value);

        bool operator ==(const taggle& t) const noexcept;
        bool operator !=(const taggle& t) const noexcept;
        bool operator  <(const taggle& t) const noexcept;
    };
```


--------------------------------------------------------------------------------

#### `CXON` integration

`basic_node` can be serialized as any other type.  
`CXON/CBOR` defines the following in addition:
  - own error conditions

      Error code                              | Message
      ----------------------------------------|-------------------------------
      `node::error::invalid`                  | invalid `CBOR`
      `node::error::recursion_depth_exceeded` | recursion depth limit exceeded

  - own context parameters

      Parameter               | Context    | Type       | Default | Description
      ------------------------|------------|------------|---------|----------------------
      `node::recursion_guard` | read/write | `unsigned` | 0 (N/A) | recursion guard state
      `node::recursion_depth` | read/write | `unsigned` | 64      | max recursion depth
  
      *Note: The interface is overloaded for `cxon::cbor::basic_node` and the overload
       passes `recursion_guard` parameter. If `cxon::cbor::basic_node` is part of a type
       (e.g. `std::vector<basic_node>`) and guarding against recursion is needed, then
       `recursion_guard` parameter must be passed explicitly.*

      *Note: Currently calling of the overloads with parameter(s), e.g.
      `from_bytes(..., recursion_depth::set<unsigned, 4U>())`,
      fail to compile with g++ due to a bug in the compiler ([Bug 90642](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90642)).
      As a workaround, they may be called by passing the traits parameters explicitly - e.g.
      `from_bytes<FormatTraits, NodeTraits>(..., recursion_depth::set<unsigned, 4U>())`*

      *Note: The bug mentioned above, seems to be resolved somewhere after 9.1,
       at least it's not reproducible with 10.2, but still, 90642 is not yet closed.*

      *Note: The bug mentioned above, is fixed in 11 and back-ported to 10.*

###### Example

``` c++
#include "cxon/cbor.hxx"
#include "cxon/lib/node.hxx"
#include <cassert>

int main() {
    using namespace cxon::cbor;
    {   // cxon::node::error::invalid
        node n;
            auto const r = cxon::from_bytes(n, "\xFF");
        assert(!r && r.ec.category() == cxon::node::error_category::value() && r.ec == cxon::node::error::invalid);
    }
    {   // cxon::node::error::recursion_depth_exceeded
        node n;
            auto const r = cxon::from_bytes(n, "\x81\x81\x81\x81\x01", cxon::node::recursion_depth::set<4>());
        assert(!r && r.ec.category() == cxon::node::error_category::value() && r.ec == cxon::node::error::recursion_depth_exceeded);
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
[cpp-alaw]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/AllocatorAwareContainer&oldid=128189
[cpp-hash]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/Hash&oldid=120791
