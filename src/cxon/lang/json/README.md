## `CXON/JSON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------

#### Contents
  - [Introduction](#introduction)
  - [Errors](#errors)
  - [Parameters](#parameters)
  - [Helpers](#helpers)
  - [`CXON/JSON/NODE`](node/README.md)


--------------------------------------------------------------------------------

#### Introduction

The implementation strictly complies with [`RFC7159`][RFC7159] / [`ECMA-404`][ECMA-404].  
The mapping between `C++` and `JSON`
types is as follow:

  - [fundamental types][cpp-fund-types]

    type                                                            | `JSON` type    | `#include cxon/<format>.hxx`<br>header
    ----------------------------------------------------------------|----------------|---------------------------------------
    `nullptr_t`                                                     | `null`         | `cxon/lang/<format>/fundamental.hxx`
    `bool`                                                          | `true`/`false` | `cxon/lang/<format>/fundamental.hxx`
    `char`, `wchar_t`, `char16_t` and `char32_t`                    | `string`       | `cxon/lang/<format>/fundamental.hxx`
    `signed`&`unsigned` `char`, `short`, `int`, `long`, `long long` | `number`       | `cxon/lang/<format>/fundamental.hxx`
    `float`, `double`, `long double`                                | `number`       | `cxon/lang/<format>/fundamental.hxx`
 
  - compound types

    type                            | `JSON` type            | `#include cxon/<format>.hxx`<br> header
    --------------------------------|------------------------|----------------------------------------
    [`reference types`][cpp-ref]    | `value type`           | `cxon/lang/<format>/compound.hxx`
    [`pointer types`][cpp-ptr]      | `value type` or `null` | `cxon/lang/<format>/compound.hxx`
    [`array types`][cpp-arr]        | `array`                | `cxon/lang/<format>/compound.hxx`
    [`enumeration types`][cpp-enum] | `string`               | `cxon/lang/<format>/compound.hxx`
    [`class types`][cpp-class]      | `object`               | `cxon/lang/<format>/compound.hxx`

  - standard library types

    type                                            | `JSON` type          | `cxon/lang/<format>/lib/std/<header>.hxx`<br> include
    ------------------------------------------------|----------------------|------------------------------------------------------
    [`std::pair`][std-pair]                         | `array`              | [`cxon/lib/std/utility.hxx`](lib/std/utility.hxx)
    [`std::tuple`][std-tuple]                       | `array`              | [`cxon/lib/std/tuple.hxx`](lib/std/tuple.hxx)
    [`std::optional`][std-optional]                 | `value_type`         | [`cxon/lib/std/optional.hxx`](lib/std/optional.hxx)
    [`std::variant`][std-variant]                   | value type for index | [`cxon/lib/std/variant.hxx`](lib/std/variant.hxx)
    [`std::basic_string`][std-bstr]                 | `string`             | [`cxon/lib/std/string.hxx`](lib/std/string.hxx)
    [`std::basic_string_view`][std-strv]            | `string`             | [`cxon/lib/std/string_view.hxx`](lib/std/string_view.hxx)
    [`std::array`][std-array]                       | `array`              | [`cxon/lib/std/array.hxx`](lib/std/array.hxx)
    [`std::deque`][std-deque]                       | `array`              | [`cxon/lib/std/deque.hxx`](lib/std/deque.hxx)
    [`std::forward_list`][std-forward_list]         | `array`              | [`cxon/lib/std/forward_list.hxx`](lib/std/forward_list.hxx)
    [`std::list`][std-list]                         | `array`              | [`cxon/lib/std/list.hxx`](lib/std/list.hxx)
    [`std::map`][std-map]`(1)`                      | `object`             | [`cxon/lib/std/map.hxx`](lib/std/map.hxx)
    [`std::multimap`][std-multimap]`(1)`            | `object`             | [`cxon/lib/std/map.hxx`](lib/std/map.hxx)
    [`std::multiset`][std-multiset]                 | `array`              | [`cxon/lib/std/set.hxx`](lib/std/set.hxx)
    [`std::priority_queue`][std-priority_queue]     | `array`              | [`cxon/lib/std/queue.hxx`](lib/std/queue.hxx)
    [`std::queue`][std-queue]                       | `array`              | [`cxon/lib/std/queue.hxx`](lib/std/queue.hxx)
    [`std::set`][std-set]                           | `array`              | [`cxon/lib/std/set.hxx`](lib/std/set.hxx)
    [`std::stack`][std-stack]                       | `array`              | [`cxon/lib/std/stack.hxx`](lib/std/stack.hxx)
    [`std::unordered_map`][std-umap]`(1)`           | `object`             | [`cxon/lib/std/unordered_map.hxx`](lib/std/unordered_map.hxx)
    [`std::unordered_set`][std-uset]                | `array`              | [`cxon/lib/std/unordered_set.hxx`](lib/std/unordered_set.hxx)
    [`std::unordered_multimap`][std-umultimap]`(1)` | `object`             | [`cxon/lib/std/unordered_map.hxx`](lib/std/unordered_map.hxx)
    [`std::unordered_multiset`][std-umultiset]      | `array`              | [`cxon/lib/std/unordered_set.hxx`](lib/std/unordered_set.hxx)
    [`std::vector`][std-vector]                     | `array`              | [`cxon/lib/std/vector.hxx`](lib/std/vector.hxx)
    [`std::bitset`][std-bitset]                     | `string`             | [`cxon/lib/std/bitset.hxx`](lib/std/bitset.hxx)
    [`std::complex`][std-complex]                   | `array`              | [`cxon/lib/std/complex.hxx`](lib/std/complex.hxx)
    [`std::valarray`][std-valarr]                   | `array`              | [`cxon/lib/std/valarray.hxx`](lib/std/valarray.hxx)
    [`std::chrono::duration`][std-duration]         | `number`             | [`cxon/lib/std/chrono.hxx`](lib/std/chrono.hxx)
    [`std::chrono::time_point`][std-time-pt]        | `number`             | [`cxon/lib/std/chrono.hxx`](lib/std/chrono.hxx)

*`(1)` [`ECMA-404(6)`](http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf)
specify this about object keys:*
  > *The JSON syntax does not impose any restrictions on the strings used as names,
  > __does not require that name strings be unique__*...

*and by picking `std::map` or `std::multi-map` as a `C++` mapping type, the value of an existing key 
may be replaced or kept.*

Complete example for a simple [`JSON-RPC`](https://www.jsonrpc.org/specification) implementation 
can be found here [`JSON-RPC`](../../README.md#example-json-rpc).


--------------------------------------------------------------------------------

#### Errors

  - _read_

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

  - _write_

    Error code                         | Message
    -----------------------------------|---------------------------------
    read_error::ok                     | no error
    read_error::output_failure         | output cannot be written
    read_error::argument_invalid       | invalid argument


--------------------------------------------------------------------------------

#### Parameters

  - _read_

    Parameter      | Type                     | Default                             | Description
    ---------------|--------------------------|-------------------------------------|-------------------------
    `allocator`    | [`Allocator`][std-alloc] | `std::allocator<T>`                 | `T*` allocator
    `num_len_max`  | `unsigned`               | 32 (integral) / 64 (floating-point) | number read buffer size
    `ids_len_max`  | `unsigned`               | 64                                  | token read buffer size

  - _write_

    Parameter      | Type  | Default                                | Description
    ---------------|-------|----------------------------------------|-------------------------
    `fp_precision` | `int` | `std::numeric_limits<T>::max_digits10` | floating-point precision


--------------------------------------------------------------------------------

#### Helpers

For convenience, the implementation also provides a set of simple, non-intrusive and intrusive
macros for binding of enumeration and class types:

  - [`enumeration types`][cpp-enum]
    ``` c++
    // implements the read interface for an enum Type
    #define CXON_JSON_ENM_READ(Type, ...)
    // implements the write interface for an enum Type
    #define CXON_JSON_ENM_WRITE(Type, ...)
    // implements the read and write interfaces for an enum Type
    #define CXON_JSON_ENM(Type, ...)

    // defines an enum value Value, which will be serialized as Name
    #define CXON_JSON_ENM_VALUE_NAME(Name, Value)
    // defines an enum value Value, which will be serialized as ##Value
    #define CXON_JSON_ENM_VALUE_ASIS(Value)
    ```

    ###### Example

    ``` c++
    enum rgb { red, green, blue };

    CXON_JSON_ENM(rgb,
        CXON_JSON_ENM_VALUE_ASIS(red),
        CXON_JSON_ENM_VALUE_NAME("green (1)", green),
        CXON_JSON_ENM_VALUE_ASIS(blue)
    )

    ...

    std::vector<rgb> v0 = { rgb::red, rgb::green, rgb::blue };
    std::string s0;
        cxon::to_bytes(s0, v0);
    assert(s0 == "[\"red\",\"green (1)\",\"blue\"]");
    std::vector<rgb> v1;
        cxon::from_bytes(v1, s0);
    assert(v1 == v0);
    ```
  - [`class types`][cpp-class]
    ``` c++
    // implements the read interface for type `Type`
    #define CXON_JSON_CLS_READ(Type, ...)
    // implements the write interface for type `Type`
    #define CXON_JSON_CLS_WRITE(Type, ...)
    // implements the read and write interfaces for type `Type`
    #define CXON_JSON_CLS(Type, ...)
    // and the same set for intrusive implementation
    #define CXON_JSON_CLS_READ_MEMBER(Type, ...)
    #define CXON_JSON_CLS_WRITE_MEMBER(Type, ...)
    #define CXON_JSON_CLS_MEMBER(Type, ...)

    // defines field `Field`, which will be serialized as Name
    #define CXON_JSON_CLS_FIELD_NAME(Name, Field)
    // defines field `Field`, which will be serialized as Name and a
    // boolean expression as a last parameter to evaluate if the field has a default
    // value and must not be written (only meaningful for writing)
    #define CXON_JSON_CLS_FIELD_NAME_DFLT(Name, Field, ...)
    // defines field `Field`, which will be serialized as ##Field
    #define CXON_JSON_CLS_FIELD_ASIS(Field)
    // defines field `Field`, which will be serialized as ##Field and a
    // boolean expression as a last parameter to evaluate if the field has a default
    // value and must not be written (only meaningful for writing)
    #define CXON_JSON_CLS_FIELD_ASIS_DFLT(Field, ...)
    // defines a key Name, which will be ignored when reading (only meaningful for reading)
    #define CXON_JSON_CLS_FIELD_SKIP(Name)
    ```

    ###### Example

    ``` c++
    #include "cxon/json.hxx"
    #include <cassert>

    struct my_struct {
        int x, y;
        int z = 0; // default
        bool operator ==(const my_struct& s) const { return x == s.x && y == s.y && z == s.z; }
    };
    CXON_JSON_CLS(my_struct,
        CXON_JSON_CLS_FIELD_ASIS(x),
        CXON_JSON_CLS_FIELD_NAME("field y", y),
        // 'z' will not be written if 0, 'self' is a reference to 'my_struct'
        CXON_JSON_CLS_FIELD_ASIS_DFLT(z, self.z == 0),
        // 'skip' will be ignored
        CXON_JSON_CLS_FIELD_SKIP("skip")
    )

    int main() {
        my_struct v0 = { 1, 2, 0 };
        std::string s0;
            cxon::to_bytes(s0, v0);
        assert(s0 == "{\"x\":1,\"field y\":2}");
        my_struct v1;
            cxon::from_bytes(v1, "{\"x\":1,\"field y\":2,\"skip\":42}");
        assert(v1 == v0);
    }
    ```


--------------------------------------------------------------------------------

Distributed under the MIT license. See [`LICENSE`](../../../../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  

<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060
[RFC7159]: https://www.ietf.org/rfc/rfc7159.txt
[ECMA-404]: http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
[cpp-fund-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[std-alloc]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/Allocator&oldid=103869
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
