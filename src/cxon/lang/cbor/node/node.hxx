// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_NODE_HXX_
#define CXON_CBOR_NODE_HXX_

#include "cxon/lang/cbor/cbor.hxx"
#include "cxon/lang/common/hash.hxx"
#include "cxon/lang/common/node-value.hxx"

#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // node

    template <typename Al = std::allocator<char>>
        struct node_traits;

    template <typename Tr>
        struct basic_node;

    using node = basic_node<node_traits<>>;

    template <typename T, typename N> inline        bool    is(const N& n) noexcept;
#   if !defined(__GNUC__) || (__GNUC__ > 7 || (__GNUC__ == 7 && __GNUC_MINOR__ >= 5)) || defined(__clang__)
        // error: declaration of ‘template<class T, class N> T& cxon::cbor::imbue(N&) noexcept (noexcept (declval<N&>().imbue<T>()))’ has a different exception specifier
        template <typename T, typename N> inline        T&      imbue(N& n) noexcept(noexcept(std::declval<N&>().template imbue<T>()));
#   endif
    template <typename T, typename N> inline        T&      get(N& n) noexcept;
    template <typename T, typename N> inline const  T&      get(const N& n) noexcept;
    template <typename T, typename N> inline        T*      get_if(N& n) noexcept;
    template <typename T, typename N> inline const  T*      get_if(const N& n) noexcept;

}}

namespace cxon { namespace cbor { // helpers

    struct undefined;

    template <typename T>
        struct simple;

    template <typename N, typename T>
        struct taggle;

}}

namespace cxon { namespace cbor { // node traits

    enum class node_kind { map, array, tag, bytes, text, real, sint, uint, simple, boolean, null, undefined };

    template <typename Al>
        struct node_traits {
            using                                       allocator_type  = Al;
            template <typename K, typename V> using     map_type        = std::map<K, V, std::less<K>, alc::rebind_t<Al, std::pair<const K, V>>>;
            template <typename T> using                 array_type      = std::vector<T, alc::rebind_t<Al, T>>;
            template <typename N, typename T> using     tag_type        = taggle<N, T>;
            using                                       bytes_type      = std::vector<unsigned char, alc::rebind_t<Al, unsigned char>>;
            using                                       text_type       = std::basic_string<char, std::char_traits<char>, alc::rebind_t<Al, char>>;
            using                                       real_type       = double;
            using                                       sint_type       = long long;
            using                                       uint_type       = unsigned long long;
            using                                       simple_type     = simple<unsigned char>;
            using                                       boolean_type    = bool;
            using                                       null_type       = std::nullptr_t;
            using                                       undefined_type  = undefined;
            using                                       dynamic_types   = integer_sequence<node_kind, node_kind::map, node_kind::array, node_kind::tag>;
        };

}}

namespace std {
    template <typename Tr>
        struct hash<cxon::cbor::basic_node<Tr>>;
}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // node

    template <typename Tr>
        struct basic_node {
            using allocator_type    = alc::rebind_t<typename Tr::allocator_type, basic_node>;
            using map               = typename Tr::template map_type<basic_node, basic_node>;
            using array             = typename Tr::template array_type<basic_node>;
            using tag               = typename Tr::template tag_type<typename Tr::uint_type, basic_node>;
            using bytes             = typename Tr::bytes_type;
            using text              = typename Tr::text_type;
            using real              = typename Tr::real_type;
            using sint              = typename Tr::sint_type;
            using uint              = typename Tr::uint_type;
            using simple            = typename Tr::simple_type;
            using boolean           = typename Tr::boolean_type;
            using null              = typename Tr::null_type;
            using undefined         = typename Tr::undefined_type;

            private: // value
                template <typename N, typename T> struct node_kind_;
                template <typename N> struct node_kind_<N, typename N::map>         : std::integral_constant<node_kind, node_kind::map>         {};
                template <typename N> struct node_kind_<N, typename N::array>       : std::integral_constant<node_kind, node_kind::array>       {};
                template <typename N> struct node_kind_<N, typename N::tag>         : std::integral_constant<node_kind, node_kind::tag>         {};
                template <typename N> struct node_kind_<N, typename N::bytes>       : std::integral_constant<node_kind, node_kind::bytes>       {};
                template <typename N> struct node_kind_<N, typename N::text>        : std::integral_constant<node_kind, node_kind::text>        {};
                template <typename N> struct node_kind_<N, typename N::real>        : std::integral_constant<node_kind, node_kind::real>        {};
                template <typename N> struct node_kind_<N, typename N::sint>        : std::integral_constant<node_kind, node_kind::sint>        {};
                template <typename N> struct node_kind_<N, typename N::uint>        : std::integral_constant<node_kind, node_kind::uint>        {};
                template <typename N> struct node_kind_<N, typename N::simple>      : std::integral_constant<node_kind, node_kind::simple>      {};
                template <typename N> struct node_kind_<N, typename N::boolean>     : std::integral_constant<node_kind, node_kind::boolean>     {};
                template <typename N> struct node_kind_<N, typename N::null>        : std::integral_constant<node_kind, node_kind::null>        {};
                template <typename N> struct node_kind_<N, typename N::undefined>   : std::integral_constant<node_kind, node_kind::undefined>   {};

                template <typename T>
                    using is_dynamic_type_ = integer_sequence_contains<
                        typename Tr::dynamic_types, node_kind_<basic_node, typename std::remove_pointer<T>::type>::value
                    >;
                template <typename T>
                    using dt_       = typename std::conditional<is_dynamic_type_<T>::value, T*, T>::type;
                using map_          = dt_<map>;
                using array_        = dt_<array>;
                using tag_          = dt_<tag>;
                using bytes_        = dt_<bytes>;
                using text_         = dt_<text>;
                using real_         = dt_<real>;
                using sint_         = dt_<sint>;
                using uint_         = dt_<uint>;
                using simple_       = dt_<simple>;
                using boolean_      = dt_<boolean>;
                using null_         = dt_<null>;
                using undefined_    = dt_<undefined>;

                template <typename T>
                    using dt_dbg_       = typename std::conditional<is_dynamic_type_<T>::value, T*, char>::type;
                using map_dbg_          = dt_dbg_<map>;
                using array_dbg_        = dt_dbg_<array>;
                using tag_dbg_          = dt_dbg_<tag>;
                using bytes_dbg_        = dt_dbg_<bytes>;
                using text_dbg_         = dt_dbg_<text>;
                using real_dbg_         = dt_dbg_<real>;
                using sint_dbg_         = dt_dbg_<sint>;
                using uint_dbg_         = dt_dbg_<uint>;
                using simple_dbg_       = dt_dbg_<simple>;
                using boolean_dbg_      = dt_dbg_<boolean>;
                using null_dbg_         = dt_dbg_<null>;
                using undefined_dbg_    = dt_dbg_<undefined>;

                using value_type_ = typename aligned_union<map_, array_, tag_, bytes_, text_, real_, sint_, uint_, simple_, boolean_, null_, undefined_>::type;

                value_type_     value_;
                node_kind       kind_;
                allocator_type  alloc_;

                // value access
                    using dynamic_types_ = type_sequence<map_, array_, tag_, bytes_, text_, real_, sint_, uint_, simple_, boolean_, null_, undefined_>;
                    static constexpr node_kind kind_default_ = node_kind::undefined;
                    template <typename N> friend struct value::controller;
            public:

            constexpr basic_node() noexcept
            :   kind_(node_kind::undefined)
            {
            }
            constexpr basic_node(const allocator_type& al) noexcept
            :   kind_(node_kind::undefined),
                alloc_(al)
            {
            }
            ~basic_node() {
                switch (kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: value::destruct<T>(*this); break
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(simple);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }

            basic_node(basic_node&& o)
                noexcept(value::is_nothrow_move_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(std::move(o.alloc_))
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: value::move_construct<T>(*this, std::forward<basic_node>(o)); break
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(simple);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }
            basic_node(basic_node&& o, const allocator_type& al)
                noexcept(value::is_nothrow_move_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(al)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: value::move_construct<T>(*this, std::forward<basic_node>(o)); break
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(simple);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }
            basic_node& operator =(basic_node&& o)
                noexcept(value::is_nothrow_move_assignable<basic_node>::value)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return value::move_assign<T>(*this, std::forward<basic_node>(o))
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(simple);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return *this; // LCOV_EXCL_LINE
            }

            basic_node(const basic_node& o)
                noexcept(value::is_nothrow_copy_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(std::allocator_traits<allocator_type>::select_on_container_copy_construction(o.alloc_))
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: value::copy_construct<T>(*this, o); break
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(simple);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }

            basic_node(const basic_node& o, const allocator_type& al)
                noexcept(value::is_nothrow_copy_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(al)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: value::copy_construct<T>(*this, o); break
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(simple);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }
            basic_node& operator =(const basic_node& o)
                noexcept(value::is_nothrow_copy_assignable<basic_node>::value)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return value::copy_assign<T>(*this, o)
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(simple);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return *this; // LCOV_EXCL_LINE
            }

#           define CXON_CBOR_TYPE_DEF(T)\
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
                CXON_CBOR_TYPE_DEF(map);
                CXON_CBOR_TYPE_DEF(array);
                CXON_CBOR_TYPE_DEF(tag);
                CXON_CBOR_TYPE_DEF(bytes);
                CXON_CBOR_TYPE_DEF(text);
                CXON_CBOR_TYPE_DEF(real);
                CXON_CBOR_TYPE_DEF(sint);
                CXON_CBOR_TYPE_DEF(uint);
                CXON_CBOR_TYPE_DEF(simple);
                CXON_CBOR_TYPE_DEF(boolean);
                CXON_CBOR_TYPE_DEF(null);
                CXON_CBOR_TYPE_DEF(undefined);
#           undef CXON_CBOR_TYPE_DEF

            // map if empty or array(s) of two elements, array otherwise
                basic_node(std::initializer_list<basic_node> l)
                :   kind_(node_kind::undefined)
                {
                    *this = l;
                }
                basic_node(std::initializer_list<basic_node> l, const allocator_type& al)
                :   kind_(node_kind::undefined), alloc_(al)
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
            // integrals
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
                    basic_node& operator =(T t) {
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
                basic_node(const typename text::value_type* s)
                :   kind_(node_kind::text)
                {
                    value::construct<text>(*this, s);
                }
                basic_node(const typename text::value_type* s, const allocator_type& al)
                :   kind_(node_kind::text), alloc_(al)
                {
                    value::construct<text>(*this, s);
                }
                basic_node& operator =(const typename text::value_type* s) {
                    return imbue<text>() = s, *this;
                }

            void reset() noexcept {
                this->~basic_node(), kind_ = node_kind::undefined;
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
                static constexpr st swap[12][12] = {
#                   define S(...) value::swap<basic_node, __VA_ARGS__>
                        {S(map, map), S(map,   array), S(map,   tag), S(map,   bytes), S(map,   text), S(map,   real), S(map,   sint), S(map,   uint), S(map,    simple), S(map,     boolean), S(map,     null), S(map,       undefined)},
                        {    nullptr, S(array, array), S(array, tag), S(array, bytes), S(array, text), S(array, real), S(array, sint), S(array, uint), S(array,  simple), S(array,   boolean), S(array,   null), S(array,     undefined)},
                        {    nullptr,         nullptr, S(tag,   tag), S(tag,   bytes), S(tag,   text), S(tag,   real), S(tag,   sint), S(tag,   uint), S(tag,    simple), S(tag,     boolean), S(tag,     null), S(tag,       undefined)},
                        {    nullptr,         nullptr,       nullptr, S(bytes, bytes), S(bytes, text), S(bytes, real), S(bytes, sint), S(bytes, uint), S(bytes,  simple), S(bytes,   boolean), S(bytes,   null), S(bytes,     undefined)},
                        {    nullptr,         nullptr,       nullptr,         nullptr, S(text,  text), S(text,  real), S(text,  sint), S(text,  uint), S(text,   simple), S(text,    boolean), S(text,    null), S(text,      undefined)},
                        {    nullptr,         nullptr,       nullptr,         nullptr,        nullptr, S(real,  real), S(real,  sint), S(real,  uint), S(real,   simple), S(real,    boolean), S(real,    null), S(real,      undefined)},
                        {    nullptr,         nullptr,       nullptr,         nullptr,        nullptr,        nullptr, S(sint,  sint), S(sint,  uint), S(sint,   simple), S(sint,    boolean), S(sint,    null), S(sint,      undefined)},
                        {    nullptr,         nullptr,       nullptr,         nullptr,        nullptr,        nullptr,        nullptr, S(uint,  uint), S(uint,   simple), S(uint,    boolean), S(uint,    null), S(uint,      undefined)},
                        {    nullptr,         nullptr,       nullptr,         nullptr,        nullptr,        nullptr,        nullptr,        nullptr, S(simple, simple), S(simple,  boolean), S(simple,  null), S(simple,    undefined)},
                        {    nullptr,         nullptr,       nullptr,         nullptr,        nullptr,        nullptr,        nullptr,        nullptr,           nullptr, S(boolean, boolean), S(boolean, null), S(boolean,   undefined)},
                        {    nullptr,         nullptr,       nullptr,         nullptr,        nullptr,        nullptr,        nullptr,        nullptr,           nullptr,             nullptr, S(null,    null), S(null,      undefined)},
                        {    nullptr,         nullptr,       nullptr,         nullptr,        nullptr,        nullptr,        nullptr,        nullptr,           nullptr,             nullptr,          nullptr, S(undefined, undefined)}
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
#                       define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return f.get<T>() == s.get<T>()
                            CXON_CBOR_TYPE_DEF(map);
                            CXON_CBOR_TYPE_DEF(array);
                            CXON_CBOR_TYPE_DEF(tag);
                            CXON_CBOR_TYPE_DEF(bytes);
                            CXON_CBOR_TYPE_DEF(text);
                            CXON_CBOR_TYPE_DEF(real);
                            CXON_CBOR_TYPE_DEF(sint);
                            CXON_CBOR_TYPE_DEF(uint);
                            CXON_CBOR_TYPE_DEF(simple);
                            CXON_CBOR_TYPE_DEF(boolean);
                            CXON_CBOR_TYPE_DEF(null);
                            CXON_CBOR_TYPE_DEF(undefined);
#                       undef CXON_CBOR_TYPE_DEF
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
#                       define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return f.get<T>() < s.get<T>()
                            CXON_CBOR_TYPE_DEF(map);
                            CXON_CBOR_TYPE_DEF(array);
                            CXON_CBOR_TYPE_DEF(tag);
                            CXON_CBOR_TYPE_DEF(bytes);
                            CXON_CBOR_TYPE_DEF(text);
                            CXON_CBOR_TYPE_DEF(real);
                            CXON_CBOR_TYPE_DEF(sint);
                            CXON_CBOR_TYPE_DEF(uint);
                            CXON_CBOR_TYPE_DEF(simple);
                            CXON_CBOR_TYPE_DEF(boolean);
                            case node_kind::null: return false;
                            CXON_CBOR_TYPE_DEF(undefined);
#                       undef CXON_CBOR_TYPE_DEF
                    }
                    return false; // LCOV_EXCL_LINE
                }

#               define CXON_JSON_DEF(T)\
                    friend bool operator ==(const basic_node& n, const T& v) noexcept { return  n.is<T>() && n.get<T>() == v; } \
                    friend bool operator ==(const T& v, const basic_node& n) noexcept { return  n.is<T>() && n.get<T>() == v; } \
                    friend bool operator !=(const basic_node& n, const T& v) noexcept { return !n.is<T>() || n.get<T>() != v; } \
                    friend bool operator !=(const T& v, const basic_node& n) noexcept { return !n.is<T>() || n.get<T>() != v; }
                    CXON_JSON_DEF(map)
                    CXON_JSON_DEF(array)
                    CXON_JSON_DEF(tag)
                    CXON_JSON_DEF(bytes)
                    CXON_JSON_DEF(text)
                    // const string::value_type*
                    friend bool operator ==(const basic_node& n, const typename text::value_type* v) noexcept {
                        return n.is<text>() && n.get<text>() == v;
                    }
                    friend bool operator ==(const typename text::value_type* v, const basic_node& n) noexcept {
                        return n.is<text>() && n.get<text>() == v;
                    }
                    friend bool operator !=(const basic_node& n, const typename text::value_type* v) noexcept {
                        return !n.is<text>() || n.get<text>() != v;
                    }
                    friend bool operator !=(const typename text::value_type* v, const basic_node& n) noexcept {
                        return !n.is<text>() || n.get<text>() != v;
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
                    CXON_JSON_DEF(simple)
                    CXON_JSON_DEF(boolean)
                    //CXON_JSON_DEF(null)
                    friend bool operator ==(const basic_node& n, null) noexcept { return  n.is<null>(); }
                    friend bool operator ==(null, const basic_node& n) noexcept { return  n.is<null>(); }
                    //CXON_JSON_DEF(undefined)
                    friend bool operator ==(const basic_node& n, undefined) noexcept { return  n.is<undefined>(); }
                    friend bool operator ==(undefined, const basic_node& n) noexcept { return  n.is<undefined>(); }
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

    template <typename N, typename T>
        struct taggle {
            public:     N tag;
            public:     T value;

            constexpr taggle() noexcept(std::is_nothrow_constructible<T>::value)
            :   tag(-1), value()
            {
            }
            taggle(N n, T&& t) noexcept(std::is_nothrow_move_constructible<T>::value)
            :   tag(n), value(std::move(t)) {}
            taggle(N n, const T& t) noexcept(std::is_nothrow_copy_constructible<T>::value)
            :   tag(n), value(t)
            {
            }

            ~taggle() = default;

            taggle(taggle&& t) noexcept(std::is_nothrow_move_constructible<T>::value)
            :   tag(t.tag), value(std::move(t.value))
            {
            }
            taggle& operator =(taggle&& t) noexcept(std::is_nothrow_move_assignable<T>::value) {
                return tag = t.tag, value = std::move(t.value), *this;
            }

            taggle(const taggle& t) noexcept(std::is_nothrow_copy_constructible<T>::value)
            :   tag(t.tag), value(t.value)
            {
            }
            taggle& operator =(const taggle& t) noexcept(std::is_nothrow_copy_assignable<T>::value) {
                return tag = t.tag, value = t.value, *this;
            }

            bool operator ==(const taggle& t) const noexcept { return tag == t.tag && value == t.value; }
            bool operator !=(const taggle& t) const noexcept { return tag != t.tag || value != t.value; }
            bool operator  <(const taggle& t) const noexcept { return tag < t.tag || (tag == t.tag && value < t.value); }
        };

}}

namespace cxon { // hash

    template <>
        struct hash<cbor::undefined> {
            std::size_t operator ()(cbor::undefined) const noexcept {
                return make_hash(-1);
            }
        };
    template <typename T>
        struct hash<cbor::simple<T>> {
            std::size_t operator ()(const cbor::simple<T>& t) const noexcept {
                return make_hash(t.value);
            }
        };
    template <typename T, typename V>
        struct hash<cbor::taggle<T, V>> {
            std::size_t operator ()(const cbor::taggle<T, V>& t) const noexcept {
                return make_hash(t.tag, t.value);
            }
        };

    template <typename Tr>
        struct hash<cbor::basic_node<Tr>> {
            std::size_t operator ()(const cbor::basic_node<Tr>& n) const noexcept {
                switch (n.kind()) {
#                   define CXON_CBOR_TYPE_DEF(T)            case cbor::node_kind::T: return make_hash(cbor::get<typename cbor::basic_node<Tr>::T>(n))
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(tag);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(real);
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(simple);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(undefined);
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
