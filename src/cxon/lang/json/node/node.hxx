// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_NODE_HXX_
#define CXON_JSON_NODE_HXX_

#include "cxon/lang/json/json.hxx"
#include "cxon/lang/common/hash.hxx"
#include "cxon/lang/common/node-value.hxx"

#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace json { // node

    template <typename Al = std::allocator<void>>
        struct node_traits;

    template <typename Tr>
        struct basic_node;

    using node = basic_node<node_traits<>>;

    template <typename T, typename N> inline        bool    is(const N& n) noexcept;
#   if !defined(__GNUC__) || (__GNUC__ > 7 || (__GNUC__ == 7 && __GNUC_MINOR__ >= 5)) || defined(__clang__)
        // error: declaration of ‘template<class T, class N> T& cxon::json::imbue(N&) noexcept (noexcept (declval<N&>().imbue<T>()))’ has a different exception specifier
        template <typename T, typename N> inline        T&      imbue(N& n) noexcept(noexcept(std::declval<N&>().template imbue<T>()));
#   endif
    template <typename T, typename N> inline        T&      get(N& n) noexcept;
    template <typename T, typename N> inline const  T&      get(const N& n) noexcept;
    template <typename T, typename N> inline        T*      get_if(N& n) noexcept;
    template <typename T, typename N> inline const  T*      get_if(const N& n) noexcept;

}}

namespace cxon { namespace json { // node traits

    enum class node_kind { object, array, string, real, sint, uint, boolean, null };

    template <typename Al>
        struct node_traits {
            using                                       allocator_type  = Al;
            template <typename K, typename V> using     object_type     = std::map<K, V, std::less<K>, alc::rebind_t<Al, std::pair<const K, V>>>;
            template <typename T> using                 array_type      = std::vector<T, alc::rebind_t<Al, T>>;
            using                                       string_type     = std::basic_string<char, std::char_traits<char>, alc::rebind_t<Al, char>>;
            using                                       real_type       = double;
            using                                       sint_type       = long long;
            using                                       uint_type       = unsigned long long;
            using                                       boolean_type    = bool;
            using                                       null_type       = std::nullptr_t;
            using                                       dynamic_types   = integer_sequence<node_kind, node_kind::object, node_kind::array>;
        };

}}

namespace std {
    template <typename Tr>
        struct hash<cxon::json::basic_node<Tr>>;
}

#define CXON_JSON_NODE_DEFINED

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace json { // node

    template <typename Tr>
        struct basic_node {
            using allocator_type    = alc::rebind_t<typename Tr::allocator_type, basic_node>;
            using object            = typename Tr::template object_type<basic_node, basic_node>;
            using array             = typename Tr::template array_type<basic_node>;
            using string            = typename Tr::string_type;
            using real              = typename Tr::real_type;
            using sint              = typename Tr::sint_type;
            using uint              = typename Tr::uint_type;
            using boolean           = typename Tr::boolean_type;
            using null              = typename Tr::null_type;

            private: // value
                template <typename N, typename T> struct node_kind_;
                template <typename N> struct node_kind_<N, typename N::object>  : std::integral_constant<node_kind, node_kind::object>  {};
                template <typename N> struct node_kind_<N, typename N::array>   : std::integral_constant<node_kind, node_kind::array>   {};
                template <typename N> struct node_kind_<N, typename N::string>  : std::integral_constant<node_kind, node_kind::string>  {};
                template <typename N> struct node_kind_<N, typename N::real>    : std::integral_constant<node_kind, node_kind::real>    {};
                template <typename N> struct node_kind_<N, typename N::sint>    : std::integral_constant<node_kind, node_kind::sint>    {};
                template <typename N> struct node_kind_<N, typename N::uint>    : std::integral_constant<node_kind, node_kind::uint>    {};
                template <typename N> struct node_kind_<N, typename N::boolean> : std::integral_constant<node_kind, node_kind::boolean> {};
                template <typename N> struct node_kind_<N, typename N::null>    : std::integral_constant<node_kind, node_kind::null>    {};

                template <typename T>
                    using is_dynamic_type_ = integer_sequence_contains<
                        typename Tr::dynamic_types, node_kind_<basic_node, typename std::remove_pointer<T>::type>::value
                    >;
                template <typename T>
                    using dt_   = typename std::conditional<is_dynamic_type_<T>::value, T*, T>::type;
                using object_   = dt_<object>;
                using array_    = dt_<array>;
                using string_   = dt_<string>;
                using real_     = dt_<real>;
                using sint_     = dt_<sint>;
                using uint_     = dt_<uint>;
                using boolean_  = dt_<boolean>;
                using null_     = dt_<null>;

                template <typename T>
                    using dt_dbg_   = typename std::conditional<is_dynamic_type_<T>::value, T*, char>::type;
                using object_dbg_   = dt_dbg_<object>;
                using array_dbg_    = dt_dbg_<array>;
                using string_dbg_   = dt_dbg_<string>;
                using real_dbg_     = dt_dbg_<real>;
                using sint_dbg_     = dt_dbg_<sint>;
                using uint_dbg_     = dt_dbg_<uint>;
                using boolean_dbg_  = dt_dbg_<boolean>;
                using null_dbg_     = dt_dbg_<null>;

                using value_type_ = typename aligned_union<object_, array_, string_, real_, sint_, uint_, boolean_, null_>::type;

                value_type_     value_;
                node_kind       kind_;
                allocator_type  alloc_;

                // value access
                    using dynamic_types_ = type_sequence<object_, array_, string_, real_, sint_, uint_, boolean_, null_>;
                    static constexpr node_kind kind_default_ = node_kind::null;
                    template <typename N> friend struct value::controller;
            public:

            constexpr basic_node() noexcept
            :   kind_(node_kind::null)
            {
            }
            constexpr basic_node(const allocator_type& al) noexcept
            :   kind_(node_kind::null), alloc_(al)
            {
            }
            ~basic_node() noexcept {
                switch (kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: value::destruct<T>(*this); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
            }

            basic_node(basic_node&& o)
                noexcept(value::is_nothrow_move_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(std::move(o.alloc_))
            {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: value::move_construct<T>(*this, std::forward<basic_node>(o)); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
            }
            basic_node(basic_node&& o, const allocator_type& al)
                noexcept(value::is_nothrow_move_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(al)
            {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: value::move_construct<T>(*this, std::forward<basic_node>(o)); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
            }
            basic_node& operator =(basic_node&& o)
                noexcept(value::is_nothrow_move_assignable<basic_node>::value)
            {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: return value::move_assign<T>(*this, std::forward<basic_node>(o))
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
                return *this; // LCOV_EXCL_LINE
            }

            basic_node(const basic_node& o)
                noexcept(value::is_nothrow_copy_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(std::allocator_traits<allocator_type>::select_on_container_copy_construction(o.alloc_))
            {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: value::copy_construct<T>(*this, o); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
            }
            basic_node(const basic_node& o, const allocator_type& al)
                noexcept(value::is_nothrow_copy_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(al)
            {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: value::copy_construct<T>(*this, o); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
            }
            basic_node& operator =(const basic_node& o)
                noexcept(value::is_nothrow_copy_assignable<basic_node>::value)
            {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: return value::copy_assign<T>(*this, o)
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
                return *this; // LCOV_EXCL_LINE
            }

#           define CXON_JSON_TYPE_DEF(T)\
                    basic_node(T&& v)                                   noexcept(value::is_nothrow_constructible<basic_node, T, T&&>::value) \
                                                                        : kind_(node_kind::T)               { value::construct<T>(*this, std::forward<T>(v)); } \
                    basic_node(T&& v, const allocator_type& al)         noexcept(value::is_nothrow_constructible<basic_node, T, T&&>::value) \
                                                                        : kind_(node_kind::T), alloc_(al)   { value::construct<T>(*this, std::forward<T>(v)); } \
                    basic_node& operator =(T&& v)                       noexcept(noexcept(std::declval<basic_node&>().template imbue<T>() = std::forward<T>(v))) \
                                                                        { return imbue<T>() = std::forward<T>(v), *this; } \
                    basic_node(const T& v)                              noexcept(value::is_nothrow_constructible<basic_node, T, T&>::value) \
                                                                        : kind_(node_kind::T)               { value::construct<T>(*this, v); } \
                    basic_node(const T& v, const allocator_type& al)    noexcept(value::is_nothrow_constructible<basic_node, T, T&>::value) \
                                                                        : kind_(node_kind::T), alloc_(al)   { value::construct<T>(*this, v); } \
                    basic_node& operator =(const T& v)                  noexcept(noexcept(std::declval<basic_node&>().template imbue<T>() = v)) \
                                                                        { return imbue<T>() = v, *this; }
                CXON_JSON_TYPE_DEF(object)
                CXON_JSON_TYPE_DEF(array)
                CXON_JSON_TYPE_DEF(string)
                CXON_JSON_TYPE_DEF(real)
                CXON_JSON_TYPE_DEF(sint)
                CXON_JSON_TYPE_DEF(uint)
                CXON_JSON_TYPE_DEF(boolean)
                CXON_JSON_TYPE_DEF(null)
#           undef CXON_JSON_TYPE_DEF

            // object if empty or array(s) of two elements, array otherwise
                basic_node(std::initializer_list<basic_node> l)
                :   kind_(node_kind::null)
                {
                    *this = l;
                }
                basic_node(std::initializer_list<basic_node> l, const allocator_type& al)
                :   kind_(node_kind::null), alloc_(al)
                {
                    *this = l;
                }
                basic_node& operator =(std::initializer_list<basic_node> l) {
                    bool const obj = l.size() == 0 || std::all_of(l.begin(), l.end(), [&](const basic_node& e) {
                        return e.template is<array>() && e.template get<array>().size() == 2;
                    });
                    if (obj) {
                        auto& o = imbue<object>();
                        for (auto& e : l) {
                            auto& a = e.template get<array>();
                            cxon::cnt::append(o, { a[0], a[1] });
                        }
                    }
                    else
                        imbue<array>() = l;
                    return *this;
                }

            // literals
                private:
                    template <typename T, bool E = std::is_signed<T>::value && !is_char<T>::value>
                        struct int_kind_            : std::integral_constant<node_kind, node_kind::sint> {};
                    template <typename T>
                        struct int_kind_<T, false>  : std::integral_constant<node_kind, node_kind::uint> {};

                    template <typename T, bool E = std::is_signed<T>::value && !is_char<T>::value>
                        struct int_type__           { using type = sint; };
                    template <typename T>
                        struct int_type__<T, false> { using type = uint; };
                    template <typename T>
                        using int_type_ = typename int_type__<T>::type;
                public:
            // integral
                template <typename T, typename = enable_if_t<std::is_integral<T>::value>>
                    basic_node(T t) noexcept
                    :   kind_(int_kind_<T>::value)
                    {
                        value::construct<int_type_<T>>(*this, t);
                    }
                template <typename T, typename = enable_if_t<std::is_integral<T>::value>>
                    basic_node(T t, const allocator_type& al) noexcept
                    :   kind_(int_kind_<T>::value), alloc_(al)
                    {
                        value::construct<int_type_<T>>(*this, t);
                    }
                template <typename T, typename = enable_if_t<std::is_integral<T>::value>>
                    basic_node& operator =(T t) noexcept {
                        return imbue<int_type_<T>>() = t, *this;
                    }
            // std::nullptr_t
                template <typename ...>
                    constexpr basic_node(std::nullptr_t) noexcept
                    :   kind_(node_kind::null)
                    {
                    }
                template <typename ...>
                    constexpr basic_node(std::nullptr_t, const allocator_type& al) noexcept
                    :   kind_(node_kind::null), alloc_(al)
                    {
                    }
                template <typename ...>
                    basic_node& operator =(std::nullptr_t) noexcept {
                        return imbue<null>(), *this;
                    }
            // string
                basic_node(const typename string::value_type* s)
                :   kind_(node_kind::string)
                {
                    value::construct<string>(*this, s);
                }
                basic_node(const typename string::value_type* s, const allocator_type& al)
                :   kind_(node_kind::string), alloc_(al)
                {
                    value::construct<string>(*this, s);
                }
                basic_node& operator =(const typename string::value_type* s) {
                    return imbue<string>() = s, *this;
                }

            void reset() noexcept {
                this->~basic_node(), kind_ = node_kind::null;
            }

            node_kind kind() const noexcept { return kind_; }

            allocator_type get_allocator() const noexcept {
                return alloc_;
            }

            template <typename T> bool  is() const noexcept {
                return kind_ == node_kind_<basic_node, T>::value;
            }

            template <typename T> T& imbue()
                noexcept(noexcept(value::construct<T>(std::declval<basic_node&>())))
            {
                if (!is<T>()) {
                    reset(), kind_ = node_kind_<basic_node, T>::value;
                    value::construct<T>(*this);
                }
                return value::get<T>(*this);
            }

            template <typename T> T& get() noexcept {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return value::get<T>(*this);
            }
            template <typename T> const T& get() const noexcept {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return value::get<T>(*this);
            }

            template <typename T> T* get_if() noexcept {
                return is<T>() ? &value::get<T>(*this) : nullptr;
            }
            template <typename T> const T* get_if() const noexcept {
                return is<T>() ? &value::get<T>(*this) : nullptr;
            }

            void swap(basic_node& o) {
                using st = void (*)(basic_node& , basic_node&);
                static constexpr st swap[8][8] = {
#                   define S(...) value::swap<basic_node, __VA_ARGS__>
                        {S(object, object), S(object, array), S(object, string), S(object, real), S(object, sint), S(object, uint), S(object,  boolean), S(object,  null)},
                        {          nullptr, S(array,  array), S(array,  string), S(array,  real), S(array,  sint), S(array,  uint), S(array,   boolean), S(array,   null)},
                        {          nullptr,          nullptr, S(string, string), S(string, real), S(string, sint), S(string, uint), S(string,  boolean), S(string,  null)},
                        {          nullptr,          nullptr,           nullptr, S(real,   real), S(real,   sint), S(real,   uint), S(real,    boolean), S(real,    null)},
                        {          nullptr,          nullptr,           nullptr,         nullptr, S(sint,   sint), S(sint,   uint), S(sint,    boolean), S(sint,    null)},
                        {          nullptr,          nullptr,           nullptr,         nullptr,         nullptr, S(uint,   uint), S(uint,    boolean), S(uint,    null)},
                        {          nullptr,          nullptr,           nullptr,         nullptr,         nullptr,         nullptr, S(boolean, boolean), S(boolean, null)},
                        {          nullptr,          nullptr,           nullptr,         nullptr,         nullptr,         nullptr,             nullptr, S(null,    null)}
#                   undef S
                };
                kind_ < o.kind_ ?
                    swap[(int)kind_][(int)o.kind_](*this, o) :
                    swap[(int)o.kind_][(int)kind_](o, *this)
                ;
            }

            private:
                friend bool operator ==(const basic_node& f, const basic_node& s) noexcept {
                    if (f.kind_ != s.kind_)
                        return false;
                    switch (f.kind_) {
#                       define CXON_JSON_TYPE_DEF(T)    case node_kind::T: return f.get<T>() == s.get<T>()
                            CXON_JSON_TYPE_DEF(object);
                            CXON_JSON_TYPE_DEF(array);
                            CXON_JSON_TYPE_DEF(string);
                            CXON_JSON_TYPE_DEF(real);
                            CXON_JSON_TYPE_DEF(sint);
                            CXON_JSON_TYPE_DEF(uint);
                            CXON_JSON_TYPE_DEF(boolean);
                            case node_kind::null: return true;
#                       undef CXON_JSON_TYPE_DEF
                    }
                    return false; // LCOV_EXCL_LINE
                }
                friend bool operator !=(const basic_node& f, const basic_node& s) noexcept {
                    return !(f == s);
                }

                friend bool operator <(const basic_node& f, const basic_node& s) noexcept {
                    if (f.kind_ != s.kind_)
                        return f.kind_ < s.kind_;
                    switch (f.kind_) {
#                       define CXON_JSON_TYPE_DEF(T)    case node_kind::T: return f.get<T>() < s.get<T>()
                            CXON_JSON_TYPE_DEF(object);
                            CXON_JSON_TYPE_DEF(array);
                            CXON_JSON_TYPE_DEF(string);
                            CXON_JSON_TYPE_DEF(real);
                            CXON_JSON_TYPE_DEF(sint);
                            CXON_JSON_TYPE_DEF(uint);
                            CXON_JSON_TYPE_DEF(boolean);
                            case node_kind::null: return false;
#                       undef CXON_JSON_TYPE_DEF
                    }
                    return false; // LCOV_EXCL_LINE
                }

#               define CXON_JSON_DEF(T)\
                    friend bool operator ==(const basic_node& n, const T& v) noexcept { return  n.is<T>() && n.get<T>() == v; } \
                    friend bool operator ==(const T& v, const basic_node& n) noexcept { return  n.is<T>() && n.get<T>() == v; } \
                    friend bool operator !=(const basic_node& n, const T& v) noexcept { return !n.is<T>() || n.get<T>() != v; } \
                    friend bool operator !=(const T& v, const basic_node& n) noexcept { return !n.is<T>() || n.get<T>() != v; }
                    CXON_JSON_DEF(object)
                    CXON_JSON_DEF(array)
                    CXON_JSON_DEF(string)
                    // const string::value_type*
                    friend bool operator ==(const basic_node& n, const typename string::value_type* v) noexcept {
                        return n.is<string>() && n.get<string>() == v;
                    }
                    friend bool operator ==(const typename string::value_type* v, const basic_node& n) noexcept {
                        return n.is<string>() && n.get<string>() == v;
                    }
                    friend bool operator !=(const basic_node& n, const typename string::value_type* v) noexcept {
                        return !n.is<string>() || n.get<string>() != v;
                    }
                    friend bool operator !=(const typename string::value_type* v, const basic_node& n) noexcept {
                        return !n.is<string>() || n.get<string>() != v;
                    }
                    //CXON_JSON_DEF(real)
                    template <typename U = real>
                        friend auto operator ==(const basic_node& n, U v) noexcept
                            -> enable_if_t<std::is_floating_point<U>::value, bool>
                        { return n.is<real>() && n.get<real>() == v; }
                    template <typename U = real>
                        friend auto operator ==(U v, const basic_node& n) noexcept
                            -> enable_if_t<std::is_floating_point<U>::value, bool>
                        { return n.is<real>() && n.get<real>() == v; }
                    template <typename U = real>
                        friend auto operator !=(const basic_node& n, U v) noexcept
                            -> enable_if_t<std::is_floating_point<U>::value, bool>
                        { return !n.is<real>() || n.get<real>() != v; }
                    template <typename U = real>
                        friend auto operator !=(U v, const basic_node& n) noexcept
                            -> enable_if_t<std::is_floating_point<U>::value, bool>
                        { return !n.is<real>() || n.get<real>() != v; }
                    //CXON_JSON_DEF(sint)
                    template <typename U = sint>
                        friend auto operator ==(const basic_node& n, U v) noexcept
                            -> enable_if_t<std::is_signed<U>::value && !std::is_floating_point<U>::value, bool>
                        { return n.is<sint>() && n.get<sint>() == v; }
                    template <typename U = sint>
                        friend auto operator ==(U v, const basic_node& n) noexcept
                            -> enable_if_t<std::is_signed<U>::value && !std::is_floating_point<U>::value, bool>
                        { return n.is<sint>() && n.get<sint>() == v; }
                    template <typename U = sint>
                        friend auto operator !=(const basic_node& n, U v) noexcept
                            -> enable_if_t<std::is_signed<U>::value && !std::is_floating_point<U>::value, bool>
                        { return !n.is<sint>() || n.get<sint>() != v; }
                    template <typename U = sint>
                        friend auto operator !=(U v, const basic_node& n) noexcept
                            -> enable_if_t<std::is_signed<U>::value && !std::is_floating_point<U>::value, bool>
                        { return !n.is<sint>() || n.get<sint>() != v; }
                    //CXON_JSON_DEF(uint)
                    template <typename U = uint>
                        friend auto operator ==(const basic_node& n, U v) noexcept
                            -> enable_if_t<std::is_unsigned<U>::value, bool>
                        { return n.is<uint>() && n.get<uint>() == v; }
                    template <typename U = uint>
                        friend auto operator ==(U v, const basic_node& n) noexcept
                            -> enable_if_t<std::is_unsigned<U>::value, bool>
                        { return n.is<uint>() && n.get<uint>() == v; }
                    template <typename U = uint>
                        friend auto operator !=(const basic_node& n, U v) noexcept
                            -> enable_if_t<std::is_unsigned<U>::value, bool>
                        { return !n.is<uint>() || n.get<uint>() != v; }
                    template <typename U = uint>
                        friend auto operator !=(U v, const basic_node& n) noexcept
                            -> enable_if_t<std::is_unsigned<U>::value, bool>
                        { return !n.is<uint>() || n.get<uint>() != v; }
                    CXON_JSON_DEF(boolean)
                    friend bool operator ==(const basic_node& n, null) noexcept { return  n.is<null>(); }
                    friend bool operator ==(null, const basic_node& n) noexcept { return  n.is<null>(); }
#               undef CXON_JSON_DEF
        };

    template <typename T, typename N>
        inline bool is(const N& n) noexcept         { return n.template is<T>(); }

    template <typename T, typename N>
        inline T& imbue(N& n) noexcept(noexcept(std::declval<N&>().template imbue<T>()))
                                                    { return n.template imbue<T>(); }

    template <typename T, typename N>
        inline T& get(N& n) noexcept                { return n.template get<T>(); }
    template <typename T, typename N>
        inline const T& get(const N& n) noexcept    { return n.template get<T>(); }

    template <typename T, typename N>
        inline T* get_if(N& n) noexcept             { return n.template get_if<T>(); }
    template <typename T, typename N>
        inline const T* get_if(const N& n) noexcept { return n.template get_if<T>(); }

}}

namespace cxon { // hash

    template <typename Tr>
        struct hash<json::basic_node<Tr>> {
            std::size_t operator ()(const json::basic_node<Tr>& n) const noexcept {
                switch (n.kind()) {
#                   define CXON_JSON_TYPE_DEF(T)        case json::node_kind::T: return make_hash(json::get<typename json::basic_node<Tr>::T>(n))
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
                return 0; // LCOV_EXCL_LINE
            }
        };

}

namespace std { // hash

    template <typename Tr>
        struct hash<cxon::json::basic_node<Tr>> {
            std::size_t operator ()(const cxon::json::basic_node<Tr>& n) const noexcept {
                return cxon::make_hash(n);
            }
        };

}

#endif // CXON_JSON_NODE_HXX_
