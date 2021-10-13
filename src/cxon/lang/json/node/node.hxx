// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_NODE_HXX_
#define CXON_JSON_NODE_HXX_

#include "cxon/lang/json/json.hxx"
#include "cxon/lang/common/hash.hxx"

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
    template <typename T, typename N> inline        T&      imbue(N& n)/* noexcept(std::is_nothrow_default_constructible<T>::value)*/;
    template <typename T, typename N> inline        T&      get(N& n) noexcept;
    template <typename T, typename N> inline const  T&      get(const N& n) noexcept;
    template <typename T, typename N> inline        T*      get_if(N& n) noexcept;
    template <typename T, typename N> inline const  T*      get_if(const N& n) noexcept;

}}

namespace cxon { namespace json { // helpers
    struct null;
}}

namespace cxon { namespace json { // node traits

    enum class node_kind { object, array, string, sint, uint, real, boolean, null };

    template <typename Al>
        struct node_traits {
            using                                       allocator_type  = alc::rebind_t<Al, basic_node<node_traits>>;
            template <typename K, typename V> using     object_type     = std::map<K, V, std::less<K>, alc::rebind_t<Al, std::pair<const K, V>>>;
            template <typename T> using                 array_type      = std::vector<T, alc::rebind_t<Al, T>>;
            using                                       string_type     = std::basic_string<char, std::char_traits<char>, alc::rebind_t<Al, char>>;
            using                                       real_type       = double;
            using                                       sint_type       = long long;
            using                                       uint_type       = unsigned long long;
            using                                       boolean_type    = bool;
            using                                       null_type       = std::nullptr_t;
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
            struct is_nothrow_x_<X, H, T...>    { static constexpr bool value = X<H>::value && is_nothrow_x_<X, T...>::value; };

        template <typename T, typename ...U>
            struct is_from              { static constexpr bool value = false; };
        template <typename T, typename U>
            struct is_from<T, U>        { static constexpr bool value = std::is_same<T, U>:: value; };
        template <typename T, typename U, typename ...R>
            struct is_from<T, U, R...>  { static constexpr bool value = std::is_same<T, U>:: value || is_from<T, R...>::value; };

        //template<typename T, typename ...U> // C++17
        //    struct is_from { static constexpr bool value {(std::is_same_v<T, U> || ...)}; };

        template <typename T, typename U, bool C>
            struct select;
        template <typename T, typename U>
            struct select<T, U, true>   { using type = U; };
        template <typename T, typename U>
            struct select<T, U, false>  { using type = T; };

    }

    template <typename Tr>
        struct basic_node {
            using object    = typename Tr::template object_type<basic_node, basic_node>;
            using array     = typename Tr::template array_type<basic_node>;
            using string    = typename Tr::string_type;
            using real      = typename Tr::real_type;
            using sint      = typename Tr::sint_type;
            using uint      = typename Tr::uint_type;
            using boolean   = typename Tr::boolean_type;
            using null      = typename Tr::null_type;

            using allocator_type = alc::rebind_t<typename Tr::allocator_type, basic_node>;

            private:
                template <typename T>
                    struct is_dynamic : std::integral_constant<bool, imp::is_from<T, object, array>::value> {};
                template <typename T>
                    using dynamic_t = typename imp::select<T, T*, is_dynamic<T>::value>::type;

                using value_type = typename std::aligned_union<0, dynamic_t<object>, dynamic_t<array>, dynamic_t<string>, sint, uint, real, boolean, null>::type;
                value_type      value_;
                node_kind       kind_;
                allocator_type  alloc_;

            private:
                using is_nothrow_move_constructible = imp::is_nothrow_x_<std::is_nothrow_move_constructible, object, array, string, sint, uint, real, boolean, null>;
                using is_nothrow_move_assignable    = imp::is_nothrow_x_<std::is_nothrow_move_assignable, object, array, string, sint, uint, real, boolean, null>;
                using is_nothrow_copy_constructible = imp::is_nothrow_x_<std::is_nothrow_copy_constructible, object, array, string, sint, uint, real, boolean, null>;
                using is_nothrow_copy_assignable    = imp::is_nothrow_x_<std::is_nothrow_copy_assignable, object, array, string, sint, uint, real, boolean, null>;
            public:

            // value
                private:
                    template <typename T, typename N>
                        static constexpr auto value(N& n) noexcept
                            -> enable_if_t<!is_dynamic<T>(), decltype(*(T*)&n.value_)>
                        {
                            return *(T*)&n.value_;
                        }
                    template <typename T, typename N>
                        static constexpr auto value(N& n) noexcept
                            -> enable_if_t< is_dynamic<T>::value, decltype(**(T**)&n.value_)>
                        {
                            return **(T**)&n.value_;
                        }
                public:

            // move construct
                private:
                    template <typename T>
                        static auto move_construct_(basic_node& n, basic_node&& o) noexcept(std::is_nothrow_move_constructible<T>::value)
                            -> enable_if_t<!is_dynamic<T>::value && !std::is_trivially_copyable<T>::value>
                        {
                            alc::uninitialized_construct_using_allocator<T>(&value<T>(n), n.alloc_, std::move(value<T>(o)));
                        }
                    template <typename T>
                        static auto move_construct_(basic_node& n, basic_node&& o) noexcept
                            -> enable_if_t<!is_dynamic<T>::value &&  std::is_trivially_copyable<T>::value>
                        {
                            value<T>(n) = std::move(value<T>(o));
                        }
                    template <typename T>
                        static auto move_construct_(basic_node& n, basic_node&& o) noexcept
                            -> enable_if_t< is_dynamic<T>::value>
                        {
                            value<T*>(n) = value<T*>(o), o.kind_ = node_kind::null;
                        }
                    static void move_construct(basic_node& n, basic_node&& o) {
                        switch (o.kind_) {
#                           define CXON_JSON_TYPE_DEF(T)    case node_kind::T: move_construct_<T>(n, std::forward<basic_node>(o)); break
                                CXON_JSON_TYPE_DEF(object);
                                CXON_JSON_TYPE_DEF(array);
                                CXON_JSON_TYPE_DEF(string);
                                CXON_JSON_TYPE_DEF(sint);
                                CXON_JSON_TYPE_DEF(uint);
                                CXON_JSON_TYPE_DEF(real);
                                CXON_JSON_TYPE_DEF(boolean);
                                CXON_JSON_TYPE_DEF(null);
#                           undef CXON_JSON_TYPE_DEF
                        }
                    }
                public:

            // move assign
                private:
                    static void move_assign(basic_node& n, basic_node&& o) {
                        destruct(n), n.kind_ = o.kind_;
                        switch (o.kind_) {
#                           define CXON_JSON_TYPE_DEF(T)    case node_kind::T: move_construct(n, std::forward<basic_node>(o)); break
                                CXON_JSON_TYPE_DEF(object);
                                CXON_JSON_TYPE_DEF(array);
                                CXON_JSON_TYPE_DEF(string);
                                CXON_JSON_TYPE_DEF(sint);
                                CXON_JSON_TYPE_DEF(uint);
                                CXON_JSON_TYPE_DEF(real);
                                CXON_JSON_TYPE_DEF(boolean);
                                CXON_JSON_TYPE_DEF(null);
#                           undef CXON_JSON_TYPE_DEF
                        }
                    }
                public:

            // copy construct
                private:
                    template <typename T, typename ...A>
                        static auto copy_construct_(basic_node& n, A&&... t) noexcept(std::is_nothrow_copy_constructible<T>::value)
                            -> enable_if_t<!is_dynamic<T>::value>
                        {
                            alc::uninitialized_construct_using_allocator<T>(&value<T>(n), n.alloc_, std::forward<A>(t)...);
                        }
                    template <typename T, typename ...A>
                        static auto copy_construct_(basic_node& n, A&&... t)
                            -> enable_if_t< is_dynamic<T>::value>
                        {
                            using at = alc::rebind_t<allocator_type, T>;
                            at    al(n.alloc_);
                            value<T*>(n) = std::allocator_traits<at>::allocate(al, 1);
                            alc::uninitialized_construct_using_allocator<T>(value<T*>(n), al, std::forward<A>(t)...);
                        }
                    static void copy_construct(basic_node& n, const basic_node& o) {
                        switch (o.kind_) {
#                           define CXON_JSON_TYPE_DEF(T)    case node_kind::T: copy_construct_<T>(n, o.get<T>()); break
                                CXON_JSON_TYPE_DEF(object);
                                CXON_JSON_TYPE_DEF(array);
                                CXON_JSON_TYPE_DEF(string);
                                CXON_JSON_TYPE_DEF(sint);
                                CXON_JSON_TYPE_DEF(uint);
                                CXON_JSON_TYPE_DEF(real);
                                CXON_JSON_TYPE_DEF(boolean);
                                CXON_JSON_TYPE_DEF(null);
#                           undef CXON_JSON_TYPE_DEF
                        }
                    }
                public:

            // copy assign
                private:
                    static void copy_assign(basic_node& n, const basic_node& o) {
                        destruct(n), n.kind_ = o.kind_;
                        switch (o.kind_) {
#                           define CXON_JSON_TYPE_DEF(T)    case node_kind::T: copy_construct(n, o.get<T>()); break
                                CXON_JSON_TYPE_DEF(object);
                                CXON_JSON_TYPE_DEF(array);
                                CXON_JSON_TYPE_DEF(string);
                                CXON_JSON_TYPE_DEF(sint);
                                CXON_JSON_TYPE_DEF(uint);
                                CXON_JSON_TYPE_DEF(real);
                                CXON_JSON_TYPE_DEF(boolean);
                                CXON_JSON_TYPE_DEF(null);
#                           undef CXON_JSON_TYPE_DEF
                        }
                    }
                public:

            // construct
                private:
                    template <typename T, typename ...A>
                        static void construct(basic_node& n, A&&... t) noexcept(noexcept(copy_construct_<T>(n, std::forward<A>(t)...))) {
                            copy_construct_<T>(n, std::forward<A>(t)...);
                        }
                public:

            // destruct
                private:
                    template <typename T>
                        static auto destruct(basic_node& n) noexcept
                            -> enable_if_t<!is_dynamic<T>::value>
                        {
                            value<T>(n).~T();
                        }
                    template <typename T>
                        static auto destruct(basic_node& n) noexcept
                            -> enable_if_t< is_dynamic<T>::value>
                        {
                            using at = alc::rebind_t<allocator_type, T>;
                            at    al(n.alloc_);
                            std::allocator_traits<at>::destroy(al, value<T*>(n));
                            std::allocator_traits<at>::deallocate(al, value<T*>(n), 1);
                        }
                    static void destruct(basic_node& n) noexcept {
                        switch (n.kind_) {
#                           define CXON_JSON_TYPE_DEF(T)    case node_kind::T: destruct<T>(n); break
                                CXON_JSON_TYPE_DEF(object);
                                CXON_JSON_TYPE_DEF(array);
                                CXON_JSON_TYPE_DEF(string);
                                CXON_JSON_TYPE_DEF(sint);
                                CXON_JSON_TYPE_DEF(uint);
                                CXON_JSON_TYPE_DEF(real);
                                CXON_JSON_TYPE_DEF(boolean);
                                CXON_JSON_TYPE_DEF(null);
#                           undef CXON_JSON_TYPE_DEF
                        }
                    }
                public:

            constexpr basic_node() noexcept
            :   kind_(node_kind::null)
            {
            }
            constexpr basic_node(const allocator_type& al) noexcept
            :   kind_(node_kind::null),
                alloc_(al)
            {
            }
            ~basic_node() noexcept {
                reset();
            }

            /*constexpr */basic_node(basic_node&& o) noexcept(is_nothrow_move_constructible::value)
            :   kind_(o.kind_)
            {
                move_construct(*this, std::forward<basic_node>(o));
            }
            /*constexpr */basic_node(basic_node&& o, const allocator_type& al) noexcept(is_nothrow_move_constructible::value)
            :   kind_(o.kind_),
                alloc_(al)
            {
                move_construct(*this, std::forward<basic_node>(o));
            }
            /*constexpr */basic_node& operator =(basic_node&& o) noexcept(is_nothrow_move_constructible::value) {
                return move_assign(*this, std::forward<basic_node>(o)), *this;
            }

            /*constexpr */basic_node(const basic_node& o)
            :   kind_(o.kind_)
            {
                copy_construct(*this, o);
            }
            /*constexpr */basic_node(const basic_node& o, const allocator_type& al)
            :   kind_(o.kind_),
                alloc_(al)
            {
                copy_construct(*this, o);
            }
            /*constexpr */basic_node& operator =(const basic_node& o) {
                return copy_assign(*this, o), *this;
            }

#           define CXON_JSON_TYPE_DEF(T)\
                    basic_node(T&& v)                                   : kind_(node_kind::T)               { construct<T>(*this, std::forward<T>(v)); } \
                    basic_node(T&& v, const allocator_type& al)         : kind_(node_kind::T), alloc_(al)   { construct<T>(*this, std::forward<T>(v)); } \
                    basic_node& operator =(T&& v)                                                           { return imbue<T>() = std::forward<T>(v), *this; } \
                    basic_node(const T& v)                              : kind_(node_kind::T)               { construct<T>(*this, v); } \
                    basic_node(const T& v, const allocator_type& al)    : kind_(node_kind::T), alloc_(al)   { construct<T>(*this, v); } \
                    basic_node& operator =(const T& v)                                                      { return imbue<T>() = v, *this; }
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
                basic_node(std::initializer_list<basic_node> l)
                :   kind_(node_kind::null)
                {
                    *this = l;
                }
                basic_node(std::initializer_list<basic_node> l, const allocator_type& al)
                :   kind_(node_kind::null),
                    alloc_(al)
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
                        struct int_kind             { static constexpr node_kind value = node_kind::sint; };
                    template <typename T>
                        struct int_kind<T, false>   { static constexpr node_kind value = node_kind::uint; };

                    template <typename T, bool E = std::is_signed<T>::value && !is_char<T>::value>
                        struct int_type_            { using type = sint; };
                    template <typename T>
                        struct int_type_<T, false>  { using type = uint; };
                    template <typename T>
                        using int_type = typename int_type_<T>::type;
                public:
            // integral
                template <typename T, typename = enable_if_t<std::is_integral<T>::value>>
                    basic_node(T t) noexcept
                    :   kind_(int_kind<T>::value)
                    {
                        construct<int_type<T>>(*this, t);
                    }
                template <typename T, typename = enable_if_t<std::is_integral<T>::value>>
                    basic_node(T t, const allocator_type& al) noexcept
                    :   kind_(int_kind<T>::value),
                        alloc_(al)
                    {
                        construct<int_type<T>>(*this, t);
                    }
                template <typename T, typename = enable_if_t<std::is_integral<T>::value>>
                    basic_node& operator =(T t) noexcept {
                        return imbue<int_type<T>>() = t, *this;
                    }
            // string
                basic_node(const typename string::value_type* s)
                :   kind_(node_kind::string)
                {
                    construct<string>(*this, s);
                }
                basic_node(const typename string::value_type* s, const allocator_type& al)
                :   kind_(node_kind::string),
                    alloc_(al)
                {
                    construct<string>(*this, s);
                }
                basic_node& operator =(const typename string::value_type* s) {
                    return imbue<string>() = s, *this;
                }
            // std::nullptr_t
                //template <typename T, typename = enable_if_t<!std::is_same<T, std::nullptr_t>::value>>
                template <typename ...>
                    constexpr basic_node(std::nullptr_t) noexcept
                    :   kind_(node_kind::null)
                    {
                    }
                //template <typename T, typename = enable_if_t<!std::is_same<T, std::nullptr_t>::value>>
                template <typename ...>
                    constexpr basic_node(std::nullptr_t, const allocator_type& al) noexcept
                    :   kind_(node_kind::null),
                        alloc_(al)
                    {
                    }
                //template <typename T, typename = enable_if_t<!std::is_same<T, std::nullptr_t>::value>>
                template <typename ...>
                    basic_node& operator =(std::nullptr_t) noexcept {
                        return destruct(*this), kind_ = node_kind::null, *this;
                    }

            void reset() noexcept {
                destruct(*this), kind_ = node_kind::null;
            }

            node_kind kind() const noexcept { return kind_; }

            allocator_type get_allocator() const noexcept {
                return alloc_;
            }

            template <typename T> bool  is() const noexcept {
                return kind_ == imp::node_kind_from_<basic_node, T>();
            }

            template <typename T> T& imbue() {
                if (!is<T>()) {
                    reset(), kind_ = imp::node_kind_from_<basic_node, T>();
                    construct<T>(*this);
                }
                return value<T>(*this);
            }

            template <typename T> T& get() noexcept {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return value<T>(*this);
            }
            template <typename T> const T& get() const noexcept {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return value<T>(*this);
            }

            template <typename T> T* get_if() noexcept {
                return is<T>() ? &value<T>(*this) : nullptr;
            }
            template <typename T> const T* get_if() const noexcept {
                return is<T>() ? &value<T>(*this) : nullptr;
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
                        case node_kind::null: return true;
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

namespace cxon { namespace json { // helpers

    struct null {
        constexpr bool operator ==(const null&) const noexcept { return true; }
        constexpr bool operator  <(const null&) const noexcept { return false; }
        private:
            friend constexpr bool operator ==(null, std::nullptr_t) noexcept { return true; }
            friend constexpr bool operator ==(std::nullptr_t, null) noexcept { return true; }
    };

}}

namespace cxon { // hash

    template <typename Tr>
        struct hash<json::basic_node<Tr>> {
            size_t operator ()(const json::basic_node<Tr>& n) const noexcept {
                switch (n.kind()) {
#                   define CXON_JSON_TYPE_DEF(T)        case json::node_kind::T: return make_hash(json::get<typename json::basic_node<Tr>::T>(n))
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(boolean);
                        case json::node_kind::null:     return 0;
#                   undef CXON_JSON_TYPE_DEF
                }
                return 0; // LCOV_EXCL_LINE
            }
        };

}

namespace std { // hash

    template <typename Tr>
        struct hash<cxon::json::basic_node<Tr>> {
            size_t operator ()(const cxon::json::basic_node<Tr>& n) const noexcept {
                return cxon::make_hash(n);
            }
        };

}

#endif // CXON_JSON_NODE_HXX_
