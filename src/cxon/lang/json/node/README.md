## `CXON/JSON/NODE` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------

#### Contents
- [Introduction](#introduction)
- [`cxon::json::basic_node`](#basic_node)
- [`CXON` integration](#cxon-integration)


--------------------------------------------------------------------------------

#### Introduction

`CXON/JSON/NODE` is an implementation of a polymorphic type, which can represent an arbitrary `JSON`.  
`JSON` value type mapping is configurable with the following defaults:

Type         | Default binding
-------------|-------------------------------------------------------------------------------------
`object`     | [`std::map`][cpp-map]
`array`      | [`std::vector`][cpp-vect]
`string`     | [`std::string`][cpp-bstr]
`number`     | [`long long`][cpp-types], [`unsigned long long`][cpp-types] or [`double`][cpp-types]
`true`       | [`bool`][cpp-types]
`false`      | [`bool`][cpp-types]
`null`       | [`std::nullptr_t`][cpp-types]

###### Example 1

Build, write and read:

``` c++
#include "cxon/json.hxx"
#include "cxon/lib/node.hxx"
#include <cassert>

int main() {
    using node = cxon::json::node;
        
    char const s0[] = "{\"even\":[2,4,6],\"odd\":[1,3,5]}";
    node const n0 = {
        { "even", { 2U, 4U, 6U } }, // list of key-value pairs form an object
        { "odd", { 1U, 3U, 5U } }
    };

    node n1; // read
        cxon::from_bytes(n1, s0);
    assert(n1 == n0);

    std::string s1; // write
        cxon::to_bytes(s1, n0);
    assert(s1 == s0);
}
```

###### Example 2

Build using node's methods:

``` c++
#include "cxon/json.hxx"
#include "cxon/lib/node.hxx"
#include <cassert>

int main() {
    using node = cxon::json::node;

    // build using initializer lists
    node n1 = { // list of key-value pairs form an object [1]
        { "object", { {"object", 0} } },
        { "array", { // an array if not [1]
                { {"object", 0} },  // object if empty or array(s) of two elements
                { 1, 2, 3 },        // array otherwise
                "4",                // string
                3.14,               // real
                3,                  // signed
                14U,                // unsigned
                true,               // boolean
                nullptr             // null
            }
        },
        { "string", "string" },
        { "real", 3.14 },
        { "sint", 3 },
        { "uint", 14U },
        { "boolean", false },
        { "null", nullptr }
    };

    // build using node's methods
    node n2;
        assert(n2.is<node::null>()); // default node type is node_kind::null
        auto& o = n2.imbue<node::object>(); // change the type and return its value
            assert(n2.is<node::object>());
            o["object"] = node::object {};      assert(o["object"].is<node::object>());
            o["array"] = node::array {};        assert(o["array"].is<node::array>());
            o["string"] = "string";             assert(o["string"].is<node::string>());
            o["real"] = 3.14;                   assert(o["real"].is<node::real>());
            o["sint"] = 3;                      assert(o["sint"].is<node::sint>());
            o["uint"] = 14U;                    assert(o["uint"].is<node::uint>());
            o["boolean"] = false;               assert(o["boolean"].is<node::boolean>());
            o["null"] = nullptr;                assert(o["null"].is<node::null>());
        auto& o1 = o["object"].get<node::object>(); // get value reference, the type is known
            o1["object"] = 0;
        auto& a = o["array"].get<node::array>();    // get value reference, the type is known
            a.push_back(node::object {});       assert(a.back().is<node::object>());
            a.push_back(node::array {1, 2, 3}); assert(a.back().is<node::array>());
            a.push_back("4");                   assert(a.back().is<node::string>());
            a.push_back(3.14);                  assert(a.back().is<node::real>());
            a.push_back(3);                     assert(a.back().is<node::sint>());
            a.push_back(14U);                   assert(a.back().is<node::uint>());
            a.push_back(true);                  assert(a.back().is<node::boolean>());
            a.push_back(nullptr);               assert(a.back().is<node::null>());
        auto* o2 = a[0].get_if<node::object>(); // get value pointer if the type match
            (*o2)["object"] = 0;
    assert(n1 == n2);

    std::string s1;
        cxon::to_bytes(s1, n1);
    std::string s2;
        cxon::to_bytes(s2, n2);
    assert(s1 == s2);
}
```

The resulting `JSON` is (*note, that the default `real` type is `double`*):

``` json
{
    "array": [
        {"object": 0},
        [1, 2, 3],
        "4",
        3.1400000000000001,
        3,
        14,
        true,
        null
    ],
    "boolean": false,
    "null": null,
    "object": {"object": 0},
    "real": 3.1400000000000001,
    "sint": 3,
    "string": "string",
    "uint": 14
}
```


--------------------------------------------------------------------------------

#### `basic_node`

*Defined in header [`cxon/lang/json/node/node.hxx`](node.hxx)*  
*Include [`cxon/lib/node.hxx`](../../../lib/node.hxx)*

``` c++
namespace cxon::json {
    template <typename Traits>
        struct basic_node;
}
```

`basic_node` is compliant with [AllocatorAwareContainer][cpp-alaw] requirements.

###### Aliases

Type                | Definition
--------------------|---------------------------------------
`cxon::json::node`  | `using node = basic_node<node_traits<>>`

###### Template parameters

  - [`Traits`](#traits) - traits class specifying the allocator, the mapping between C++ and JSON and whether the storage for
given type should be allocated on the stack or in the heap.

###### Non-member types

```c++
enum class node_kind { object, array, string, real, sint, uint, boolean, null };
```

###### Member types

Member type      | Definition
-----------------|------------------------------------------
`object`         | `Traits::object_type<basic_node, basic_node>`
`array`          | `Traits::array_type<basic_node>`
`string`         | `Traits::string_type`
`real`           | `Traits::real_type`
`sint`           | `Traits::sint_type`
`uint`           | `Traits::uint_type`
`boolean`        | `Traits::boolean_type`
`null`           | `Traits::null_type`
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
  - [`operator <`](#comparison-operators) - less than
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
        template <typename K, typename V> using     object_type     = std::map<K, V, std::less<K>, rebind_t<Al, std::pair<const K, V>>>;
        template <typename T> using                 array_type      = std::vector<T, rebind_t<Al, T>>;
        using                                       string_type     = std::basic_string<char, std::char_traits<char>, rebind_t<Al, char>>;
        using                                       real_type       = double;
        using                                       sint_type       = long long;
        using                                       uint_type       = unsigned long long;
        using                                       boolean_type    = bool;
        using                                       null_type       = std::nullptr_t;
        // the types listed here will be allocated in the heap
        using                                       dynamic_types   = integer_sequence<node_kind, node_kind::object, node_kind::array>;
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

struct traits : cxon::json::node_traits<> {
    template <class K, class V> using   object_type = std::multimap<K, V>;
    template <class T> using            array_type = std::list<T>;
    using                               string_type = std::u16string;
};
...
using node = cxon::json::basic_node<traits>;
node n;
    cxon::from_bytes(n, "{\"key\": 42, \"key\": 43}");
assert(n.is<node::object>() && n.get<node::object>().count(u"key") == 2);
```

``` c++
// custom allocator

using node = cxon::json::basic_node<
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

basic_node(object&& v);                 (3)
basic_node(const object& v);
basic_node(array&& v);
basic_node(const array& v);
basic_node(string&& v);
basic_node(const string& v);
basic_node(sint&& v);
basic_node(const sint& v);
basic_node(uint&& v);
basic_node(const uint& v);
basic_node(real&& v);
basic_node(const real& v);
basic_node(boolean&& v);
basic_node(const boolean& v);
basic_node(null&& v);
basic_node(const null& v);

basic_node(<integral> v);               (4)
basic_node(const char* v);

basic_node(std::initializer_list l);    (5)
```

Construct new node from a variety of sources.
  - `(1)` Default constructor. Constructs node with `null` value type.
  - `(2)` Move and copy constructors
  - `(3)` Move and copy constructors for each value type
  - `(4)` Constructors for `string` and `integral` value types
  - `(5)` Constructors from initializer list. Create an object if empty
          or array(s) of two elements - array otherwise

*Note: Each constructor has its allocator argument companion.*

###### Example

``` c++
using namespace cxon::json;
{   // (1)
    node n; assert(n.is<node::null>());
}
{   // (2)
    node n1 = true; assert(n1 == true);
    node n2(n1); assert(n2 == true);
    node n3(std::move(n1)); assert(n3 == true);
}
{   // (3)
    node n1(42.0); assert(n1 == 42.0); // floating-point
}
{   // (3)
    node::object const o = {{"key", "value"}};
    node n(o); assert(n == o);
}
{   // (3)
    node::array const a = {1, "string"};
    node n(a); assert(n == a);
}
{   // (4)
    node n(42); assert(n == 42); // signed
}
{   // (4)
    node n(42U); assert(n == 42U); // unsigned
}
{   // (4)
    node n("string"); assert(n == "string");
}
{   // (5)
    node n({{1, 2}, {3, 4}}); assert(n == (node::object {{1, 2}, {3, 4}}));
}
{   // (5)
    node n({1, 2, 3, 4}); assert(n == (node::array {1, 2, 3, 4}));
}
```


--------------------------------------------------------------------------------

##### Assignment operators

``` c++
basic_node& operator =(basic_node&& o);             (1)
basic_node& operator =(const basic_node& o);

basic_node& operator =(object&& v);                 (2)
basic_node& operator =(const object& v);
basic_node& operator =(array&& v);
basic_node& operator =(const array& v);
basic_node& operator =(string&& v);
basic_node& operator =(const string& v);
basic_node& operator =(sint&& v);
basic_node& operator =(const sint& v);
basic_node& operator =(uint&& v);
basic_node& operator =(const uint& v);
basic_node& operator =(real&& v);
basic_node& operator =(const real& v);
basic_node& operator =(boolean&& v);
basic_node& operator =(const boolean& v);
basic_node& operator =(null&& v);
basic_node& operator =(const null& v);

basic_node& operator =(int v);                      (3)
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
using namespace cxon::json;
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
using namespace cxon::json;
{   // T is the same
    node n = "string";
    node::string& v = n.imbue<node::string>(); assert(v == "string");
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
using namespace cxon::json;
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
using namespace cxon::json;
node n = "one";
    auto *v = n.get_if<node::string>(); assert(v != nullptr);
assert(n.get_if<node::array>() == nullptr);
```


--------------------------------------------------------------------------------

##### Swap

``` c++
void swap(basic_node& n);
```

###### Example

``` c++
using namespace cxon::json;
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

friend bool operator ==(const basic_node& n, const typename basic_node::string::value_type& v) noexcept;    (3)
friend bool operator ==(const typename basic_node::string::value_type& v, const basic_node& n) noexcept;
friend bool operator !=(const basic_node& n, const typename basic_node::string::value_type& v) noexcept;
friend bool operator !=(const typename basic_node::string::value_type& v, const basic_node& n) noexcept;
```
  -  `(1)` for one of `object`, `array`, `string`, `real`, `sint`, `uint`, `boolean`
  -  `(2)` for `null`
  -  `(3)` for `string::value_type**_` - char pointers

###### Example

``` c++
using namespace cxon::json;
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
    template <typename Tr> struct hash<json::basic_node<Tr>>;
}

namespace std {
    template <typename Tr> struct hash<cxon::json::basic_node<Tr>>;
}
```


--------------------------------------------------------------------------------

#### `CXON` integration

`basic_node` can be serialized as any other type.  
`CXON/JSON` defines the following in addition:
  - own error conditions

      Error code                              | Message
      ----------------------------------------|-------------------------------
      `node::error::invalid`                  | invalid `JSON`
      `node::error::recursion_depth_exceeded` | recursion depth limit exceeded

  - own context parameters

      Parameter                    | Context    | Type       | Default | Description
      -----------------------------|------------|------------|---------|---------------------------------------------
      `node::recursion_guard`      | read/write | `unsigned` | 0 (N/A) | recursion guard state
      `node::recursion_depth`      | read/write | `unsigned` | 64      | max recursion depth
      `node::json::extract_nans`   | read       | `bool`     | false   | convert `inf`/`nan` strings to `node::real`

      *Note: The interface is overloaded for `cxon::json::basic_node` and the overload
       passes the `recursion_guard` parameter. If `cxon::json::basic_node` is part of a type
       (e.g. `std::vector<basic_node>`) and guarding against recursion is needed, then
       the `recursion_guard` parameter must be passed explicitly.*

      *Note: Currently calling the overloads with named-parameter(s) - e.g.
      `from_bytes(..., recursion_depth::set<unsigned, 4U>())`,
      fail to compile with g++ due to a bug in the compiler ([Bug 90642](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90642)).
      As a workaround, they may be called by passing the traits parameters explicitly - e.g.
      `from_bytes<FormatTraits, NodeTraits>(..., recursion_depth::set<unsigned, 4U>())`*

      *Note: The bug mentioned above, seems to be resolved somewhere after 9.1 (at least it's not reproducible
      with 10.2), but still, 90642 is not yet closed.*

      *Note: The bug mentioned above, is fixed in 11 and back-ported to 10.*

###### Example

``` c++
#include "cxon/json.hxx"
#include "cxon/lib/node.hxx"
#include <cassert>

struct unquoted_keys_traits : cxon::json::format_traits {
    static constexpr bool unquoted_keys = true;
};
using UQK = cxon::JSON<unquoted_keys_traits>;

int main() {
    using namespace cxon::json;
    {   // cxon::node::error::invalid
        node n;
            auto const r = cxon::from_bytes(n, "#[1]");
        assert(!r && r.ec.category() == cxon::node::error_category::value() && r.ec == cxon::node::error::invalid);
    }
    {   // cxon::node::error::recursion_depth_exceeded
        node n;
            auto const r = cxon::from_bytes(n, "[[[[1]]]]", cxon::node::recursion_depth::set<4>());
        assert(!r && r.ec.category() == cxon::node::error_category::value() && r.ec == cxon::node::error::recursion_depth_exceeded);
    }
    {   // json::format_traits::unquoted_keys
        node n;
            cxon::from_bytes<UQK>(n, "{42: \"is the answer\"}");
        assert(n == (node::object {{42U, "is the answer"}}));
    }
    {   // cxon::node::extract_nans
        node n;
            cxon::from_bytes(n, "[\"-inf\", \"inf\"]", cxon::node::json::extract_nans::set<true>());
        assert(n == (node::array {-std::numeric_limits<node::real>::infinity(), std::numeric_limits<node::real>::infinity()}));
    }
}
```


--------------------------------------------------------------------------------

Distributed under the terms of the GNU Affero General Public License. See [`LICENSE`](../../../../../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060

[cpp-map]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/map&oldid=109218
[cpp-vect]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/vector&oldid=107643
[cpp-bstr]: https://en.cppreference.com/mwiki/index.php?title=cpp/string/basic_string&oldid=107637
[cpp-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[cpp-alaw]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/AllocatorAwareContainer&oldid=128189
[cpp-hash]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/Hash&oldid=120791
