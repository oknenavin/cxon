# `CXON/CXJSON` library

[![CXON](https://img.shields.io/badge/version-0.42.0-608060.svg?style=plastic)](https://github.com/libcxon/cxon)

-------------------------------------------------------------------------------

#### cxjson::basic_node

_Defined in header [cxjson.hxx](cxjson.hxx)_

``` c++    
    template <typename Traits>
        struct basic_node;
```

##### Aliases
-------------------------------------------------------------------------------

Type            | Definition
----------------|----------------------------------------------------
`cxjson::node`  | `using node = basic_node<node_traits>;`

##### Template parameters
-------------------------------------------------------------------------------

  - `Traits` - traits class specifying the actual types of each `JSON` value type

##### Member types
-------------------------------------------------------------------------------

Member type |Definition
------------|-----------
`object`    | `Traits::object_type`
`array`     | `Traits::array_type`
`string`    | `Traits::string_type`
`number`    | `Traits::number_type`
`boolean`   | `Traits::boolean_type`
`null`      | `Traits::null_type`

##### Member functions
-------------------------------------------------------------------------------

[`(constructor)`](#constructor) - construct a basic_node  
`(destructor)` - destroys the node, deallocating internal storage if used  
[`operator =`](#assignment-operator) - assigns values to the node  
[`reset`](#reset) - resets the node  
[`type`](#type) - returns node's value type  
[`is`](#is) - returns `true` if node's value is of given type  
[`imbue`](#imbue) - resets node's value type; returns value reference  
[`get`](#get) - returns value reference  
[`get_if`](#get_if) - returns value pointer if node's value type matches  
[`operator ==`](#comparison-operators) - assigns values to the node  
[`operator !=`](#comparison-operators) - assigns values to the node  

-------------------------------------------------------------------------------

##### Constructor

``` c++
    basic_node();

    basic_node(basic_node&& n);
    basic_node(const basic_node& n);

    basic_node(object&& v);
    basic_node(const object& v)
    basic_node(array&& v);
    basic_node(const array& v)
    basic_node(string&& v);
    basic_node(const string& v)
    basic_node(number&& v);
    basic_node(const number& v)
    basic_node(boolean&& v);
    basic_node(const boolean& v)
    basic_node(null&& v);
    basic_node(const null& v)

    basic_node(int v);
    basic_node(const char* v)
```

##### Destructor

``` c++
    ~basic_node();
```

##### assignment operator

``` c++
    basic_node& operator =(basic_node&& n);
    basic_node& operator =(const basic_node& n);

    basic_node& operator =(object&& v);
    basic_node& operator =(const object& v)
    basic_node& operator =(array&& v);
    basic_node& operator =(const array& v)
    basic_node& operator =(string&& v);
    basic_node& operator =(const string& v)
    basic_node& operator =(number&& v);
    basic_node& operator =(const number& v)
    basic_node& operator =(boolean&& v);
    basic_node& operator =(const boolean& v)
    basic_node& operator =(null&& v);
    basic_node& operator =(const null& v)

    basic_node& operator =(int v);
    basic_node& operator =(const char* v)
```

##### reset

``` c++
    void reset()
```

##### type

``` c++
    node_type type() const noexcept;
```

##### is

``` c++
    template <typename T>
        bool  is() const;
```

##### imbue

``` c++
    template <typename T>
        T& imbue()
```

##### get

``` c++
    template <typename T>
        T& get();
    template <typename T>
        const T& get() const;
```

##### get_if

``` c++
    template <typename T>
        T* get_if() noexcept;
    template <typename T>
        const T* get_if() const noexcept
```

##### comparison operators

``` c++
    bool operator == (const basic_node& n) const;
    bool operator != (const basic_node& n) const;
```
