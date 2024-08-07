## `CXON/CBOR` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------

#### Contents
  - [Introduction](#introduction)
  - [Errors](#errors)
  - [Parameters](#parameters)
  - [Helpers](#helpers)
  - [`CXON/CBOR/NODE`](node/README.md)


--------------------------------------------------------------------------------

#### Introduction

The implementation strictly complies with [`RFC7049`][RFC7049].  
The mapping between `C++` and `CBOR`
types is as follows:

  - [fundamental types][cpp-fund-types]

    type                                                               | `CBOR`<br>Major type (`MT`) | `#include cxon/cbor.hxx`<br>header
    -------------------------------------------------------------------|-----------------------------|-----------------------------------
    `nullptr_t`                                                        | `MT7` / `Null`              | `cxon/lang/cbor/fundamental.hxx`
    `bool`                                                             | `MT7` / `True` or `False`   | `cxon/lang/cbor/fundamental.hxx`
    `char`, `char8_t`, `wchar_t`, `char16_t` and `char32_t`            | `MT0` or `MT1` / `integer`  | `cxon/lang/cbor/fundamental.hxx`
    `signed`and `unsigned` `char`, `short`, `int`, `long`, `long long` | `MT0` or `MT1` / `integer`  | `cxon/lang/cbor/fundamental.hxx`
    `float`, `double`, `long double`                                   | `MT7` / `floating-point`    | `cxon/lang/cbor/fundamental.hxx`
 
  - compound types

    type                            | `CBOR`<br>Major type (`MT`)    | `#include cxon/cbor.hxx`<br> header
    --------------------------------|--------------------------------|------------------------------------
    [`reference types`][cpp-ref]    | `value type`                   | `cxon/lang/cbor/compound.hxx`
    [`pointer types`][cpp-ptr]      | `value type` or `MT7` / `Null` | `cxon/lang/cbor/compound.hxx`
    [`array types`][cpp-arr]`(1)`   | `MT7` / `array`                | `cxon/lang/cbor/compound.hxx`
    [`enumeration types`][cpp-enum] | `MT0` or `MT1` / `integer`     | `cxon/lang/cbor/compound.hxx`
    [`class types`][cpp-class]      | `MT5` / `map`                  | `cxon/lang/cbor/compound.hxx`

    *`(1)` Including typed arrays ([`RFC8746`][RFC8746])*

  - standard library types

    type                                             | `CBOR`<br>Major type (`MT`)                  | `cxon/lang/cbor/lib/std/<header>.hxx`<br> include
    -------------------------------------------------|----------------------------------------------|--------------------------------------------------------------
    [`std::pair`][std-pair]                          | `MT7` / `array`                              | [`cxon/lib/std/utility.hxx`](lib/std/utility.hxx)
    [`std::tuple`][std-tuple]                        | `MT7` / `array`                              | [`cxon/lib/std/tuple.hxx`](lib/std/tuple.hxx)
    [`std::optional`][std-optional]                  | `value type`                                 | [`cxon/lib/std/optional.hxx`](lib/std/optional.hxx)
    [`std::variant`][std-variant]                    | `value type for index`                       | [`cxon/lib/std/variant.hxx`](lib/std/variant.hxx)
    [`std::basic_string`][std-bstr]`(1)`             | `MT3` or `MT4` / `string`<br>`MT7` / `array` | [`cxon/lib/std/string.hxx`](lib/std/string.hxx)
    [`std::basic_string_view`][std-strv]`(1)`        | `MT3` or `MT4` / `string`<br>`MT7` / `array` | [`cxon/lib/std/string_view.hxx`](lib/std/string_view.hxx)
    [`std::array`][std-array]`(2)`                   | `MT7` / `array`                              | [`cxon/lib/std/array.hxx`](lib/std/array.hxx)
    [`std::deque`][std-deque]`(2)`                   | `MT7` / `array`                              | [`cxon/lib/std/deque.hxx`](lib/std/deque.hxx)
    [`std::forward_list`][std-forward_list]`(2)`     | `MT7` / `array`                              | [`cxon/lib/std/forward_list.hxx`](lib/std/forward_list.hxx)
    [`std::list`][std-list]`(2)`                     | `MT7` / `array`                              | [`cxon/lib/std/list.hxx`](lib/std/list.hxx)
    [`std::map`][std-map]`(3)`                       | `MT5` / `map`                                | [`cxon/lib/std/map.hxx`](lib/std/map.hxx)
    [`std::flat_map`][std-fmap]`(3)`                 | `MT5` / `map`                                | [`cxon/lib/std/flat_map.hxx`](lib/std/flat_map.hxx)
    [`std::unordered_map`][std-umap]`(3)`            | `MT5` / `map`                                | [`cxon/lib/std/unordered_map.hxx`](lib/std/unordered_map.hxx)
    [`std::multimap`][std-multimap]`(3)`             | `MT5` / `map`                                | [`cxon/lib/std/map.hxx`](lib/std/map.hxx)
    [`std::flat_multimap`][std-fmultimap]`(3)`       | `MT5` / `map`                                | [`cxon/lib/std/flat_map.hxx`](lib/std/flat_map.hxx)
    [`std::unordered_multimap`][std-umultimap]`(3)`  | `MT5` / `map`                                | [`cxon/lib/std/unordered_map.hxx`](lib/std/unordered_map.hxx)
    [`std::set`][std-set]`(2)`                       | `MT7` / `array`                              | [`cxon/lib/std/set.hxx`](lib/std/set.hxx)
    [`std::flat_set`][std-fset]`(2)`                 | `MT7` / `array`                              | [`cxon/lib/std/flat_set.hxx`](lib/std/flat_set.hxx)
    [`std::unordered_set`][std-uset]`(2)`            | `MT7` / `array`                              | [`cxon/lib/std/unordered_set.hxx`](lib/std/unordered_set.hxx)
    [`std::multiset`][std-multiset]`(2)`             | `MT7` / `array`                              | [`cxon/lib/std/set.hxx`](lib/std/set.hxx)
    [`std::flat_multiset`][std-fmultiset]`(2)`       | `MT7` / `array`                              | [`cxon/lib/std/flat_set.hxx`](lib/std/flat_set.hxx)
    [`std::unordered_multiset`][std-umultiset]`(2)`  | `MT7` / `array`                              | [`cxon/lib/std/unordered_set.hxx`](lib/std/unordered_set.hxx)
    [`std::priority_queue`][std-priority_queue]`(2)` | `MT7` / `array`                              | [`cxon/lib/std/queue.hxx`](lib/std/queue.hxx)
    [`std::queue`][std-queue]`(2)`                   | `MT7` / `array`                              | [`cxon/lib/std/queue.hxx`](lib/std/queue.hxx)
    [`std::stack`][std-stack]`(2)`                   | `MT7` / `array`                              | [`cxon/lib/std/stack.hxx`](lib/std/stack.hxx)
    [`std::vector`][std-vector]`(2)`                 | `MT7` / `array`                              | [`cxon/lib/std/vector.hxx`](lib/std/vector.hxx)
    [`std::bitset`][std-bitset]                      | `MT2` / `byte string`                        | [`cxon/lib/std/bitset.hxx`](lib/std/bitset.hxx)
    [`std::complex`][std-complex]                    | `MT7` / `array`                              | [`cxon/lib/std/complex.hxx`](lib/std/complex.hxx)
    [`std::valarray`][std-valarr]`(2)`               | `MT7` / `array`                              | [`cxon/lib/std/valarray.hxx`](lib/std/valarray.hxx)
    [`std::chrono::duration`][std-duration]          | `MT0` or `MT1` / `integer`                   | [`cxon/lib/std/chrono.hxx`](lib/std/chrono.hxx)
    [`std::chrono::time_point`][std-time-pt]         | `MT0` or `MT1` / `integer`                   | [`cxon/lib/std/chrono.hxx`](lib/std/chrono.hxx)

    *`(1)` Depending of the value type*  
    *`(2)` Including typed arrays ([`RFC8746`][RFC8746])*  
    *`(3)` See 3.7 in [`RFC7049`][RFC7049]). For the moment, the multimaps will use duplicate keys*


--------------------------------------------------------------------------------

#### Errors

  - _read_

    Error code                         | Message
    -----------------------------------|---------------------------------
    read_error::ok                     | no error
    read_error::unexpected             | unexpected input
    read_error::boolean_invalid        | invalid boolean
    read_error::null_invalid           | invalid null
    read_error::integer_invalid        | invalid integer
    read_error::floating_point_invalid | invalid integer
    read_error::size_invalid           | invalid size
    read_error::array_invalid          | invalid array
    read_error::tag_invalid            | invalid tag

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
    `ids_len_max`  | `std::size_t`            | 64                                  | token read buffer size


--------------------------------------------------------------------------------

#### Helpers

For convenience, the implementation also provides a set of simple, non-intrusive and intrusive
macros for binding of enumeration and class types:

  - [`enumeration types`][cpp-enum]
    ``` c++
    // implements the read interface for an enum Type
    #define CXON_CBOR_ENM_READ(Type, ...)
    // implements the write interface for an enum Type
    #define CXON_CBOR_ENM_WRITE(Type, ...)
    // implements the read and write interfaces for an enum Type
    #define CXON_CBOR_ENM(Type, ...)

    // defines an enum value Value, which will be serialized as Name
    #define CXON_CBOR_ENM_VALUE_NAME(Alias, Value)
    // defines an enum value Value, which will be serialized as ##Alias
    #define CXON_CBOR_ENM_VALUE_ASIS(Value)
    ```

    ###### Example

    ``` c++
    #include "cxon/cbor.hxx"
    #include "cxon/lib/std/vector.hxx"
    #include <cassert>

    enum rgb { red, green, blue };

    CXON_CBOR_ENM(rgb,
        CXON_CBOR_ENM_VALUE_ASIS(red),
        CXON_CBOR_ENM_VALUE_NAME(4, green),
        CXON_CBOR_ENM_VALUE_ASIS(blue)
    )

    int main() {
        std::vector<rgb> v0 = { rgb::red, rgb::green, rgb::blue };
        std::vector<unsigned char> s0;
            cxon::to_bytes(s0, v0);
        assert(s0 == (std::vector<unsigned char> {0x83, 0x00, 0x04, 0x02}));
        std::vector<rgb> v1;
            cxon::from_bytes(v1, s0);
        assert(v1 == v0);
    }
    ```
  - [`class types`][cpp-class]
    ``` c++
    // implements the read interface for type `Type`
    #define CXON_CBOR_CLS_READ(Type, ...)
    // implements the write interface for type `Type`
    #define CXON_CBOR_CLS_WRITE(Type, ...)
    // implements the read and write interfaces for type `Type`
    #define CXON_CBOR_CLS(Type, ...)
    // and the same set for intrusive implementation
    #define CXON_CBOR_CLS_READ_MEMBER(Type, ...)
    #define CXON_CBOR_CLS_WRITE_MEMBER(Type, ...)
    #define CXON_CBOR_CLS_MEMBER(Type, ...)

    // defines field `Field`, which will be serialized as Name
    #define CXON_CBOR_CLS_FIELD_NAME(Name, Field)
    // defines field `Field`, which will be serialized as Name and a
    // boolean expression as a last parameter to evaluate if the field has a default
    // value and must not be written (only meaningful for writing)
    #define CXON_CBOR_CLS_FIELD_NAME_DFLT(Name, Field, ...)
    // defines field `Field`, which will be serialized as ##Field
    #define CXON_CBOR_CLS_FIELD_ASIS(Field)
    // defines field `Field`, which will be serialized as ##Field and a
    // boolean expression as a last parameter to evaluate if the field has a default
    // value and must not be written (only meaningful for writing)
    #define CXON_CBOR_CLS_FIELD_ASIS_DFLT(Field, ...)
    // defines the key Name, which will be skipped during serialization (only meaningful for reading)
    #define CXON_CBOR_CLS_FIELD_SKIP(Name)
    ```

    ###### Example

    ``` c++
    #include "cxon/cbor.hxx"
    #include "cxon/lib/std/vector.hxx"
    #include <cassert>

    struct my_struct {
        int x, y;
        int z = 0; // default
        bool operator ==(const my_struct& s) const { return x == s.x && y == s.y && z == s.z; }
    };
    CXON_CBOR_CLS(my_struct,
        CXON_CBOR_CLS_FIELD_ASIS(x),
        CXON_CBOR_CLS_FIELD_NAME("my y", y),
        // 'z' will not be written if 0, 'self' is a reference to 'my_struct'
        CXON_CBOR_CLS_FIELD_ASIS_DFLT(z, self.z == 0),
        // 'skip' will be ignored
        CXON_CBOR_CLS_FIELD_SKIP("skip")
    )

    int main() {
        my_struct v0 = { 1, 2, 0 };
        std::vector<unsigned char> s0;
            cxon::to_bytes(s0, v0);
        assert(s0 == (std::vector<unsigned char> {0xA2, 0x62, 0x78, 0x00, 0x01, 0x65, 0x6D, 0x79, 0x20, 0x79, 0x00, 0x02}));
        my_struct v1;
            cxon::from_bytes(v1, "\xA2\x62x\0\x01\x65my y\0\x02\x65skip\0\x03");
        assert(v1 == v0);
    }
    ```


--------------------------------------------------------------------------------

Distributed under the MIT license. See [`LICENSE`](../../../../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060

[RFC7049]: https://tools.ietf.org/rfc/rfc7049.txt
[RFC8746]: https://tools.ietf.org/rfc/rfc8746.txt

[cpp-fund-types]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/types&oldid=108124
[cpp-ptr]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/pointer&oldid=109738
[cpp-ref]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/reference&oldid=105941
[cpp-arr]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/array&oldid=111607
[cpp-enum]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/enum&oldid=111809
[cpp-class]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/class&oldid=101735
[cpp-struct]: https://en.cppreference.com/mwiki/index.php?title=cpp/language/class&oldid=101735

[std-alloc]: https://en.cppreference.com/mwiki/index.php?title=cpp/named_req/Allocator&oldid=103869
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
[std-map]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/map&oldid=109218
[std-fmap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/flat_map&oldid=171871
[std-umap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_map&oldid=107669
[std-multimap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/multimap&oldid=107672
[std-fmultimap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/flat_multimap&oldid=171872
[std-umultimap]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_multimap&oldid=107675
[std-set]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/set&oldid=107670
[std-fset]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/flat_set&oldid=171873
[std-uset]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_set&oldid=107673
[std-multiset]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/multiset&oldid=107671
[std-fmultiset]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/flat_multiset&oldid=171874
[std-umultiset]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/unordered_multiset&oldid=107674
[std-stack]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/stack&oldid=106350
[std-queue]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/queue&oldid=103354
[std-priority_queue]: https://en.cppreference.com/mwiki/index.php?title=cpp/container/priority_queue&oldid=103092
[std-optional]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/optional&oldid=110327
[std-variant]: https://en.cppreference.com/mwiki/index.php?title=cpp/utility/variant&oldid=109919
