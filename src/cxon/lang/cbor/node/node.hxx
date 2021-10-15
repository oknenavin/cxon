// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_NODE_HXX_
#define CXON_CBOR_NODE_HXX_

#include "cxon/lang/cbor/cbor.hxx"
#include "cxon/lang/common/hash.hxx"

#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"


// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // node

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

namespace cxon { namespace cbor { // helpers

    struct undefined;

    template <typename T>
        struct simple;

    template <typename N, typename T, typename A = std::allocator<T>>
        struct taggle;

}}

namespace cxon { namespace cbor { // node traits

    enum class node_kind { sint, uint, bytes, text, array, map, tag, boolean, null, undefined, real, simple };

    template <typename Al>
        struct node_traits {
            using                                       allocator_type  = alc::rebind_t<Al, basic_node<node_traits>>;
            using                                       sint_type       = long long;
            using                                       uint_type       = unsigned long long;
            using                                       bytes_type      = std::vector<unsigned char, alc::rebind_t<Al, unsigned char>>;
            using                                       text_type       = std::basic_string<char, std::char_traits<char>, alc::rebind_t<Al, char>>;
            template <typename T> using                 array_type      = std::vector<T, alc::rebind_t<Al, T>>;
            template <typename K, typename V> using     map_type        = std::map<K, V, std::less<K>, alc::rebind_t<Al, std::pair<const K, V>>>;
            template <typename N, typename T> using     tag_type        = taggle<N, T, alc::rebind_t<Al, T>>;
            using                                       boolean_type    = bool;
            using                                       null_type       = std::nullptr_t;
            using                                       undefined_type  = undefined;
            using                                       real_type       = double;
            using                                       simple_type     = simple<unsigned char>;
        };

}}

namespace std {
    template <typename Tr>
        struct hash<cxon::cbor::basic_node<Tr>>;
}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // node

    namespace imp {

        template <typename N, typename T>
            struct node_kind_;
        template <typename N> struct node_kind_<N, typename N::sint>        { static constexpr node_kind value = node_kind::sint; };
        template <typename N> struct node_kind_<N, typename N::uint>        { static constexpr node_kind value = node_kind::uint; };
        template <typename N> struct node_kind_<N, typename N::bytes>       { static constexpr node_kind value = node_kind::bytes; };
        template <typename N> struct node_kind_<N, typename N::text>        { static constexpr node_kind value = node_kind::text; };
        template <typename N> struct node_kind_<N, typename N::array>       { static constexpr node_kind value = node_kind::array; };
        template <typename N> struct node_kind_<N, typename N::map>         { static constexpr node_kind value = node_kind::map; };
        template <typename N> struct node_kind_<N, typename N::tag>         { static constexpr node_kind value = node_kind::tag; };
        template <typename N> struct node_kind_<N, typename N::null>        { static constexpr node_kind value = node_kind::null; };
        template <typename N> struct node_kind_<N, typename N::undefined>   { static constexpr node_kind value = node_kind::undefined; };
        template <typename N> struct node_kind_<N, typename N::boolean>     { static constexpr node_kind value = node_kind::boolean; };
        template <typename N> struct node_kind_<N, typename N::real>        { static constexpr node_kind value = node_kind::real; };
        template <typename N> struct node_kind_<N, typename N::simple>      { static constexpr node_kind value = node_kind::simple; };

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
            using sint      = typename Tr::sint_type;
            using uint      = typename Tr::uint_type;
            using bytes     = typename Tr::bytes_type;
            using text      = typename Tr::text_type;
            using array     = typename Tr::template array_type<basic_node>;
            using map       = typename Tr::template map_type<basic_node, basic_node>;
            using tag       = typename Tr::template tag_type<uint, basic_node>;
            using boolean   = typename Tr::boolean_type;
            using null      = typename Tr::null_type;
            using undefined = typename Tr::undefined_type;
            using real      = typename Tr::real_type;
            using simple    = typename Tr::simple_type;

            using allocator_type = alc::rebind_t<typename Tr::allocator_type, basic_node>;

            private:
                using value_type = typename std::aligned_union<0, sint, uint, bytes, text, array, map, tag, boolean, null, undefined, real, simple>::type;
                value_type      value_;
                node_kind       kind_;
                allocator_type  alloc_;

            private:
#               ifdef _MSC_VER // std::map move copy/assign are not noexcept, force
                    template <template <typename C> class X, bool = false>
                        struct msvc_map_override            : imp::is_nothrow_x_<X, sint, uint, bytes, text, array, map, tag, boolean, null, undefined, real, simple> {};
                    template <template <typename C> class X>
                        struct msvc_map_override<X, true>   : imp::is_nothrow_x_<X, sint, uint, bytes, text, array, /*map, */tag, boolean, null, undefined, real, simple> {};
                    using is_nothrow_move_constructible = msvc_map_override<std::is_nothrow_move_constructible, std::is_same<map, std::map<basic_node, basic_node>>::value>;
                    using is_nothrow_move_assignable    = msvc_map_override<std::is_nothrow_move_assignable,    std::is_same<map, std::map<basic_node, basic_node>>::value>;
#               else
                    using is_nothrow_move_constructible = imp::is_nothrow_x_<std::is_nothrow_move_constructible, sint, uint, bytes, text, array, map, tag, boolean, null, undefined, real, simple>;
                    using is_nothrow_move_assignable    = imp::is_nothrow_x_<std::is_nothrow_move_assignable, sint, uint, bytes, text, array, map, tag, boolean, null, undefined, real, simple>;
#               endif
                    using is_nothrow_copy_constructible = imp::is_nothrow_x_<std::is_nothrow_copy_constructible, sint, uint, bytes, text, array, map, tag, boolean, null, undefined, real, simple>;
                    using is_nothrow_copy_assignable    = imp::is_nothrow_x_<std::is_nothrow_copy_assignable, sint, uint, bytes, text, array, map, tag, boolean, null, undefined, real, simple>;
            public:

            basic_node() noexcept
            :   kind_(node_kind::undefined)
            {
            }
            basic_node(const allocator_type& al) noexcept
            :   kind_(node_kind::undefined),
                alloc_(al)
            {
            }
            ~basic_node() {
                reset();
            }

            basic_node(basic_node&& o) noexcept(is_nothrow_move_constructible::value)
            :   kind_(o.kind_)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: alc::uninitialized_construct_using_allocator<T>((T*)&value_, alloc_, std::move(o.get<T>())); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(simple);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }
            basic_node(basic_node&& o, const allocator_type& al) noexcept(is_nothrow_move_constructible::value)
            :   kind_(o.kind_),
                alloc_(al)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: alc::uninitialized_construct_using_allocator<T>((T*)&value_, al, std::move(o.get<T>())); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(simple);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }
            basic_node& operator =(basic_node&& o) noexcept(is_nothrow_move_assignable::value) {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: imbue<T>() = std::move(o.get<T>()); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(simple);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return *this;
            }

            basic_node(const basic_node& o) noexcept(is_nothrow_copy_constructible::value)
            :   kind_(o.kind_)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: alc::uninitialized_construct_using_allocator<T>((T*)&value_, alloc_, o.get<T>()); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(simple);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }
            basic_node(const basic_node& o, const allocator_type& al) noexcept(is_nothrow_copy_constructible::value)
            :   kind_(o.kind_),
                alloc_(al)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: alc::uninitialized_construct_using_allocator<T>((T*)&value_, al, o.get<T>()); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(simple);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }
            basic_node& operator =(const basic_node& o) noexcept(is_nothrow_copy_assignable::value) {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: imbue<T>() = o.get<T>(); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(simple);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return *this;
            }

#           define CXON_CBOR_TYPE_DEF(T)\
                    basic_node(T&& v)                                   : kind_(node_kind::T)                   { alc::uninitialized_construct_using_allocator<T>((T*)&value_, alloc_, std::forward<T>(v)); } \
                    basic_node(T&& v, const allocator_type& al)         : kind_(node_kind::T), alloc_(al)       { alc::uninitialized_construct_using_allocator<T>((T*)&value_, al, std::forward<T>(v)); } \
                    basic_node& operator =(T&& v)                                                               { imbue<T>() = std::forward<T>(v); return *this; }\
                    basic_node(const T& v)                              : kind_(node_kind::T)                   { alc::uninitialized_construct_using_allocator<T>((T*)&value_, alloc_, v); } \
                    basic_node(const T& v, const allocator_type& al)    : kind_(node_kind::T), alloc_(al)       { alc::uninitialized_construct_using_allocator<T>((T*)&value_, al, v); } \
                    basic_node& operator =(const T& v)                                                          { imbue<T>() = v; return *this; }
                CXON_CBOR_TYPE_DEF(sint);
                CXON_CBOR_TYPE_DEF(uint);
                CXON_CBOR_TYPE_DEF(bytes);
                CXON_CBOR_TYPE_DEF(text);
                CXON_CBOR_TYPE_DEF(array);
                CXON_CBOR_TYPE_DEF(map);
                CXON_CBOR_TYPE_DEF(tag);
                CXON_CBOR_TYPE_DEF(boolean);
                CXON_CBOR_TYPE_DEF(null);
                CXON_CBOR_TYPE_DEF(undefined);
                CXON_CBOR_TYPE_DEF(real);
                CXON_CBOR_TYPE_DEF(simple);
#           undef CXON_CBOR_TYPE_DEF

            // map if empty or array(s) of two elements, array otherwise
                basic_node(std::initializer_list<basic_node> l)
                :   kind_(node_kind::undefined)
                {
                    *this = l;
                }
                basic_node(std::initializer_list<basic_node> l, const allocator_type& al)
                :   kind_(node_kind::undefined),
                    alloc_(al)
                {
                    *this = l;
                }
                basic_node& operator =(std::initializer_list<basic_node> l) {
                    bool const obj = l.size() == 0 || std::all_of(l.begin(), l.end(), [&](const basic_node& e) {
                        return e.template is<array>() && e.template get<array>().size() == 2;
                    });
                    if (obj) {
                        auto& o = imbue<map>();
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
                        struct int_kind             { static constexpr node_kind value = node_kind::sint; };
                    template <typename T>
                        struct int_kind<T, false>   { static constexpr node_kind value = node_kind::uint; };

                    template <typename T, bool E = std::is_signed<T>::value && !is_char<T>::value>
                        struct int_type_            { using type = sint; };
                    template <typename T>
                        struct int_type_<T, false>  { using type = uint; };
                    template <typename T>
                        using int_type = typename int_type_<T>::type;

                    template <typename T>
                        struct is_int_unique {
                            static constexpr bool value = std::is_integral<T>::value && !std::is_same<T, int_type<T>>::value;
                        };
                public:
            // integrals
                template <typename T, typename = enable_if_t<is_int_unique<T>::value>>
                    basic_node(T t) noexcept
                    :   kind_(int_kind<T>::value)
                    {
                        alc::uninitialized_construct_using_allocator<int_type<T>>((int_type<T>*)&value_, alloc_, t);
                    }
                template <typename T, typename = enable_if_t<is_int_unique<T>::value>>
                    basic_node(T t, const allocator_type& al) noexcept
                    :   kind_(int_kind<T>::value),
                        alloc_(al)
                    {
                        alc::uninitialized_construct_using_allocator<int_type<T>>((int_type<T>*)&value_, al, t);
                    }
                template <typename T, typename = enable_if_t<is_int_unique<T>::value>>
                    basic_node& operator =(T t) {
                        imbue<int_type<T>>() = t; return *this;
                    }
            // string
                basic_node(const typename text::value_type* s)
                :   kind_(node_kind::text)
                {
                    alc::uninitialized_construct_using_allocator<text>((text*)&value_, alloc_, s);
                }
                basic_node(const typename text::value_type* s, const allocator_type& al)
                :   kind_(node_kind::text),
                    alloc_(al)
                {
                    alc::uninitialized_construct_using_allocator<text>((text*)&value_, al, s);
                }
                basic_node& operator =(const typename text::value_type* s) {
                    imbue<text>() = s; return *this;
                }

            void reset() noexcept {
                switch (kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: ((T*)&value_)->~T(); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(simple);
#                   undef CXON_CBOR_TYPE_DEF
                }
                kind_ = node_kind::undefined;
            }

            node_kind kind() const noexcept { return kind_; }

            allocator_type get_allocator() const noexcept {
                return alloc_;
            }

            template <typename T> bool  is() const noexcept {
                return kind_ == imp::node_kind_from_<basic_node, T>();
            }

            template <typename T> T& imbue()/* noexcept(std::is_nothrow_default_constructible<T>::value)*/ {
                if (!is<T>()) {
                    reset(), kind_ = imp::node_kind_from_<basic_node, T>();
                    alc::uninitialized_construct_using_allocator<T>((T*)&value_, alloc_);
                }
                return *(T*)&value_;
            }

            template <typename T> T& get() noexcept {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return *(T*)&value_;
            }
            template <typename T> const T& get() const noexcept {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return *(T*)&value_;
            }

            template <typename T> T* get_if() noexcept {
                return is<T>() ? (T*)&value_ : nullptr;
            }
            template <typename T> const T* get_if() const noexcept {
                return is<T>() ? (T*)&value_ : nullptr;
            }

            bool operator == (const basic_node& n) const noexcept {
                if (kind() != n.kind()) return false;
                switch (kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return get<T>() == n.get<T>()
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(simple);
#                   undef CXON_CBOR_TYPE_DEF
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
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return get<T>() < n.get<T>()
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(boolean);
                        case node_kind::null: return false;
                        CXON_CBOR_TYPE_DEF(undefined);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(simple);
#                   undef CXON_CBOR_TYPE_DEF
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

namespace cxon { namespace cbor { // helpers

    struct undefined {
        constexpr bool operator ==(const undefined&) const { return true; }
        constexpr bool operator  <(const undefined&) const { return false; }
    };

    template <typename T>
        struct simple {
            T value;

            simple(T t = {}) noexcept : value(t) {}
            operator T() const noexcept { return value; }
        };

    template <typename N, typename T, typename Al>
        struct taggle {
            public:     N tag;
            private:    Al al_; // oops, initialization order
            public:     T& value;

            taggle()
            :   tag(-1), al_(), value(*std::allocator_traits<Al>::allocate(al_, 1))
            {   std::allocator_traits<Al>::construct(al_, &value); }
            taggle(const Al& al)
            :   tag(-1), al_(al), value(*std::allocator_traits<Al>::allocate(al_, 1))
            {   std::allocator_traits<Al>::construct(al_, &value); }

            taggle(N n, T&& t)
            :   tag(n), al_(), value(*std::allocator_traits<Al>::allocate(al_, 1))
            {   std::allocator_traits<Al>::construct(al_, &value, std::move(t)); }
            taggle(N n, T&& t, const Al& al)
            :   tag(n), al_(al), value(*std::allocator_traits<Al>::allocate(al_, 1))
            {   std::allocator_traits<Al>::construct(al_, &value, std::move(t)); }

            taggle(N n, const T& t)
            :   tag(n), al_(), value(*std::allocator_traits<Al>::allocate(al_, 1))
            {   std::allocator_traits<Al>::construct(al_, &value, t); }
            taggle(N n, const T& t, const Al& al)
            :   tag(n), al_(al), value(*std::allocator_traits<Al>::allocate(al_, 1))
            {   std::allocator_traits<Al>::construct(al_, &value, t); }

            ~taggle() {
                std::allocator_traits<Al>::destroy(al_, &value);
                std::allocator_traits<Al>::deallocate(al_, &value, 1);
            }

            taggle(taggle&& t)
            :   tag(t.tag), al_(), value(*std::allocator_traits<Al>::allocate(al_, 1))
            {
                std::allocator_traits<Al>::construct(al_, &value);
                value = std::move(t.value);
            }
            taggle(taggle&& t, const Al& al)
            :   tag(t.tag), al_(al), value(*std::allocator_traits<Al>::allocate(al_, 1))
            {
                std::allocator_traits<Al>::construct(al_, &value);
                value = std::move(t.value);
            }
            taggle& operator =(taggle&& t) { return tag = t.tag, value = std::move(t.value), *this; }

            taggle(const taggle& t)
            :   tag(t.tag), al_(), value(*std::allocator_traits<Al>::allocate(al_, 1))
            {
                std::allocator_traits<Al>::construct(al_, &value);
                value = t.value;
            }
            taggle(const taggle& t, const Al& al)
            :   tag(t.tag), al_(al), value(*std::allocator_traits<Al>::allocate(al_, 1))
            {
                std::allocator_traits<Al>::construct(al_, &value);
                value = t.value;
            }
            taggle& operator =(const taggle& t) { return tag = t.tag, value = t.value, *this; }

            taggle& operator =(T&& t) { return value = std::move(t), *this; }
            taggle& operator =(const T& t) { return value = t, *this; }

            bool operator ==(const taggle& t) const noexcept { return tag == t.tag && value == t.value; }
            bool operator  <(const taggle& t) const noexcept { return tag < t.tag || (tag == t.tag && value < t.value); }
        };

}}

namespace cxon { // hash

    template <typename T, typename V>
        struct hash<cbor::taggle<T, V>> {
            std::size_t operator ()(const cbor::taggle<T, V>& t) const noexcept {
                return make_hash(t.tag, t.value);
            }
        };
    template <typename T>
        struct hash<cbor::simple<T>> {
            std::size_t operator ()(const cbor::simple<T>& t) const noexcept {
                return make_hash(t.value);
            }
        };

    template <typename Tr>
        struct hash<cbor::basic_node<Tr>> {
            std::size_t operator ()(const cbor::basic_node<Tr>& n) const noexcept {
                switch (n.kind()) {
#                   define CXON_CBOR_TYPE_DEF(T)            case cbor::node_kind::T: return make_hash(cbor::get<typename cbor::basic_node<Tr>::T>(n))
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(boolean);
                        case cbor::node_kind::null:         return  1; //CXON_CBOR_TYPE_DEF(null); // g++-8: error: use of deleted function ‘std::hash<std::nullptr_t>::hash()’
                        case cbor::node_kind::undefined:    return -1;
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(simple);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return 0; // LCOV_EXCL_LINE
            }
        };

}

namespace std { // hash

    template <typename Tr>
        struct hash<cxon::cbor::basic_node<Tr>> {
            std::size_t operator ()(const cxon::cbor::basic_node<Tr>& n) const noexcept {
                return cxon::make_hash(n);
            }
        };

}

#endif // CXON_CBOR_NODE_HXX_
