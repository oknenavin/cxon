// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_NODE_HXX_
#define CXON_JSON_NODE_HXX_

#include "cxon/lang/json/json.hxx"

#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"

#include <functional>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace json { // node

    template <typename Tr>
        struct basic_node;

    using node = basic_node<struct node_traits>;

    template <typename T, typename N> inline        bool    is(const N& n) noexcept;
    template <typename T, typename N> inline        T&      imbue(N& n)/* noexcept(std::is_nothrow_default_constructible<T>::value)*/;
    template <typename T, typename N> inline        T&      get(N& n) noexcept;
    template <typename T, typename N> inline const  T&      get(const N& n) noexcept;
    template <typename T, typename N> inline        T*      get_if(N& n) noexcept;
    template <typename T, typename N> inline const  T*      get_if(const N& n) noexcept;

}}

namespace cxon { namespace json { // node traits

    enum class node_kind { object, array, string, sint, uint, real, boolean, null };

    struct node_traits {
        using                                       null_type       = std::nullptr_t;
        using                                       boolean_type    = bool;
        using                                       sint_type       = long long;
        using                                       uint_type       = unsigned long long;
        using                                       real_type       = double;
        using                                       string_type     = std::basic_string<char>;
        template <typename T> using                 array_type      = std::vector<T>;
        template <typename K, typename V> using     object_type     = std::map<K, V>;
    };

}}

namespace std {
    template <typename Tr>
        struct hash<cxon::json::basic_node<Tr>>;
}

#define CXON_JSON_NODE_DEFINED

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace json { // node

    namespace imp {

        template <typename N, typename T>
            struct node_kind_;
        template <typename N> struct node_kind_<N, typename N::null>        { static constexpr node_kind value = node_kind::null; };
        template <typename N> struct node_kind_<N, typename N::boolean>     { static constexpr node_kind value = node_kind::boolean; };
        template <typename N> struct node_kind_<N, typename N::sint>        { static constexpr node_kind value = node_kind::sint; };
        template <typename N> struct node_kind_<N, typename N::uint>        { static constexpr node_kind value = node_kind::uint; };
        template <typename N> struct node_kind_<N, typename N::real>        { static constexpr node_kind value = node_kind::real; };
        template <typename N> struct node_kind_<N, typename N::string>      { static constexpr node_kind value = node_kind::string; };
        template <typename N> struct node_kind_<N, typename N::array>       { static constexpr node_kind value = node_kind::array; };
        template <typename N> struct node_kind_<N, typename N::object>      { static constexpr node_kind value = node_kind::object; };

        template <typename N, typename T>
            constexpr node_kind node_kind_from_() noexcept { return node_kind_<N, T>::value; }

        template <template <typename C> class X, typename ...>
            struct is_nothrow_x_                { static constexpr bool value = true; };
        template <template <typename C> class X, typename T>
            struct is_nothrow_x_<X, T>          { static constexpr bool value = X<T>::value; };
        template <template <typename C> class X, typename H, typename ...T>
            struct is_nothrow_x_<X, H, T...>    { static constexpr bool value = is_nothrow_x_<X, H>::value && is_nothrow_x_<X, T...>::value; };

    }

    template <typename Tr>
        struct basic_node {
            using null      = typename Tr::null_type;
            using boolean   = typename Tr::boolean_type;
            using sint      = typename Tr::sint_type;
            using uint      = typename Tr::uint_type;
            using real      = typename Tr::real_type;
            using string    = typename Tr::string_type;
            using array     = typename Tr::template array_type<basic_node>;
            using object    = typename Tr::template object_type<basic_node, basic_node>;

            private:
                using value_type = typename std::aligned_union<0, object, array, string, sint, uint, real, boolean, null>::type;
                value_type  value_;
                node_kind   kind_;

            private:
#               ifdef _MSC_VER // std::map move copy/assign are not noexcept, force
                    template <template <typename C> class X, bool = false>
                        struct msvc_map_override            : imp::is_nothrow_x_<X, object, array, string, sint, uint, real, boolean, null> {};
                    template <template <typename C> class X>
                        struct msvc_map_override<X, true>   : imp::is_nothrow_x_<X, /*object, */array, string, sint, uint, real, boolean, null> {};
                    using is_nothrow_move_constructible = msvc_map_override<std::is_nothrow_move_constructible, std::is_same<object, std::map<basic_node, basic_node>>::value>;
                    using is_nothrow_move_assignable    = msvc_map_override<std::is_nothrow_move_assignable,    std::is_same<object, std::map<basic_node, basic_node>>::value>;
#               else
                    using is_nothrow_move_constructible = imp::is_nothrow_x_<std::is_nothrow_move_constructible, object, array, string, sint, uint, real, boolean, null>;
                    using is_nothrow_move_assignable    = imp::is_nothrow_x_<std::is_nothrow_move_assignable, object, array, string, sint, uint, real, boolean, null>;
#               endif
                    using is_nothrow_copy_constructible = imp::is_nothrow_x_<std::is_nothrow_copy_constructible, object, array, string, sint, uint, real, boolean, null>;
                    using is_nothrow_copy_assignable    = imp::is_nothrow_x_<std::is_nothrow_copy_assignable, object, array, string, sint, uint, real, boolean, null>;
            public:

            basic_node() noexcept : kind_(node_kind::null)  { get<null>() = nullptr; }
            ~basic_node()                                   { reset(); }

            basic_node(basic_node&& o) noexcept(is_nothrow_move_constructible::value) : kind_(o.kind_) {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: new (&reinterpret_cast<T&>(value_)) T(std::move(o.get<T>())); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
            }
            basic_node& operator =(basic_node&& o) noexcept(is_nothrow_move_assignable::value) {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: imbue<T>() = std::move(o.get<T>()); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
                return *this;
            }

            basic_node(const basic_node& o) noexcept(is_nothrow_copy_constructible::value) : kind_(o.kind_) {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: new (&reinterpret_cast<T&>(value_)) T(o.get<T>()); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
            }
            basic_node& operator =(const basic_node& o) noexcept(is_nothrow_copy_assignable::value) {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: imbue<T>() = o.get<T>(); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
                return *this;
            }

#           define CXON_JSON_TYPE_DEF(T)\
                    basic_node(T&& v) : kind_(node_kind::null)      { imbue<T>() = std::forward<T>(v); }\
                    basic_node& operator =(T&& v)                   { imbue<T>() = std::forward<T>(v); return *this; }\
                    basic_node(const T& v) : kind_(node_kind::null) { imbue<T>() = v; }\
                    basic_node& operator =(const T& v)              { imbue<T>() = v; return *this; }
                CXON_JSON_TYPE_DEF(object)
                CXON_JSON_TYPE_DEF(array)
                CXON_JSON_TYPE_DEF(string)
                CXON_JSON_TYPE_DEF(sint)
                CXON_JSON_TYPE_DEF(uint)
                CXON_JSON_TYPE_DEF(real)
                CXON_JSON_TYPE_DEF(boolean)
                CXON_JSON_TYPE_DEF(null)
#           undef CXON_JSON_TYPE_DEF

            // object if empty or array(s) of two elements, array otherwise
            basic_node(std::initializer_list<basic_node> l) : kind_(node_kind::null) {
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
                        cxon::cnt::append(o, {a[0], a[1]});
                    }
                }
                else
                    imbue<array>() = l;
                return *this;
            }

            // literals
            private:
                template <typename T, bool E = std::is_signed<T>::value && !is_char<T>::value>
                    struct int_type             { using type = sint; };
                template <typename T>
                    struct int_type<T, false>   { using type = uint; };

                template <typename T>
                    struct is_int_unique {
                        static constexpr bool value = std::is_integral<T>::value && !std::is_same<T, typename int_type<T>::type>::value;
                    };
            public:

            // integral
            template <typename T, typename = enable_if_t<is_int_unique<T>::value>>
                basic_node(T t) : kind_(node_kind::null) {
                    imbue<typename int_type<T>::type>() = t;
                }
            template <typename T, typename = enable_if_t<is_int_unique<T>::value>>
                basic_node& operator =(T t) {
                    imbue<typename int_type<T>::type>() = t; return *this;
                }
            // string
            basic_node(const typename string::value_type* s) : kind_(node_kind::null)   { imbue<string>() = s; }
            basic_node& operator =(const typename string::value_type* s)                { imbue<string>() = s; return *this; }

            void reset() noexcept {
                switch (kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: reinterpret_cast<T&>(value_).~T(); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
                kind_ = node_kind::null;
            }

            node_kind kind() const noexcept { return kind_; }

            template <typename T> bool  is() const noexcept {
                return kind_ == imp::node_kind_from_<basic_node, T>();
            }

            template <typename T> T& imbue()/* noexcept(std::is_nothrow_default_constructible<T>::value)*/ {
                if (!is<T>()) {
                    reset(), kind_ = imp::node_kind_from_<basic_node, T>();
                    new (&value_) T();
                }
                return reinterpret_cast<T&>(value_);
            }

            template <typename T> T& get() noexcept {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return reinterpret_cast<T&>(value_);
            }
            template <typename T> const T& get() const noexcept {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return reinterpret_cast<const T&>(value_);
            }

            template <typename T> T* get_if() noexcept {
                return is<T>() ? &reinterpret_cast<T&>(value_) : nullptr;
            }
            template <typename T> const T* get_if() const noexcept {
                return is<T>() ? &reinterpret_cast<const T&>(value_) : nullptr;
            }

            bool operator == (const basic_node& n) const noexcept {
                if (kind() != n.kind()) return false;
                switch (kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: return get<T>() == n.get<T>()
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
                return false; // LCOV_EXCL_LINE
            }
            bool operator != (const basic_node& n) const noexcept {
                return !operator ==(n);
            }

            bool operator < (const basic_node& n) const noexcept {
                if (kind() != n.kind())
                    return kind() < n.kind();
                switch (kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: return get<T>() < n.get<T>()
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(boolean);
                        case node_kind::null: return false;
#                   undef CXON_JSON_TYPE_DEF
                }
                return false; // LCOV_EXCL_LINE
            }
        };

    template <typename T, typename N>
        inline bool is(const N& n) noexcept         { return n.template is<T>(); }

    template <typename T, typename N>
        inline T& imbue(N& n)/* noexcept(std::is_nothrow_default_constructible<T>::value)*/
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

namespace std {

    template <typename Tr>
        struct hash<cxon::json::basic_node<Tr>> {

            using node = cxon::json::basic_node<Tr>;
            using node_kind = cxon::json::node_kind;

            size_t operator()(const node& n) const {
                switch (n.kind()) {
                    case node_kind::object: {
                        size_t s = 0;
                        hash<typename node::object::key_type> h;
                        for (auto& v: get<typename node::object>(n))
                            s = combine(combine(s, h(v.first)), h(v.second));
                        return s;
                    }
                    case node_kind::array: {
                        size_t s = 0;
                        hash<typename node::array::value_type> h;
                        for (auto& v: get<typename node::array>(n))
                            s = combine(s, h(v));
                        return s;
                    }
                    case node_kind::string:     return make_hash(get<typename node::string>(n));
                    case node_kind::real:       return make_hash(get<typename node::real>(n));
                    case node_kind::uint:       return make_hash(get<typename node::uint>(n));
                    case node_kind::sint:       return make_hash(get<typename node::sint>(n));
                    case node_kind::boolean:    return make_hash(get<typename node::boolean>(n));
                    // g++-8: error: use of deleted function ‘std::hash<std::nullptr_t>::hash()’
                    case node_kind::null:       return 0/*make_hash(get<typename node::null>(n))*/;
                }
                return 0; // LCOV_EXCL_LINE
            }

            template <typename T>
                static size_t make_hash(const T& t) {
                    return hash<T>()(t);
                }

            static size_t combine(size_t h0, size_t h1) {
                return h0 ^ (h1 + 0x9e3779b9 + (h0 << 6) + (h0 >> 2));
            }
        };

}

#endif // CXON_JSON_NODE_HXX_
