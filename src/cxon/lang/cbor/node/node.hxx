// Copyright (c) 2017-2021 oknenavin.
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

    template <typename Al = std::allocator<void>>
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
            template <typename N, typename T> using     tag_type        = taggle<N, T>;
            using                                       boolean_type    = bool;
            using                                       null_type       = std::nullptr_t;
            using                                       undefined_type  = undefined;
            using                                       real_type       = double;
            using                                       simple_type     = simple<unsigned char>;
            using                                       dynamic_types   = integer_sequence<node_kind, node_kind::array, node_kind::map, node_kind::tag>;
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
            struct node_kind_; // value
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

            private: // value
                template <typename T>
                    using is_dynamic_type_ = integer_sequence_contains<
                        typename Tr::dynamic_types, imp::node_kind_<basic_node, typename std::remove_pointer<T>::type>::value
                    >;
                template <typename T>
                    using dt_ = typename std::conditional<is_dynamic_type_<T>::value, T*, T>::type;

#               define CXON_TYPES_DEF dt_<sint>, dt_<uint>, dt_<bytes>, dt_<text>, dt_<array>, dt_<map>, dt_<tag>, dt_<boolean>, dt_<null>, dt_<undefined>, dt_<real>, dt_<simple>
                    using dynamic_types_ = type_sequence<CXON_TYPES_DEF>;
                    using value_type_ = typename std::aligned_union<0, CXON_TYPES_DEF>::type;
#               undef CXON_TYPES_DEF

                value_type_     value_;
                node_kind       kind_;
                allocator_type  alloc_;

                // value access
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

            basic_node(basic_node&& o)
                noexcept(value::is_nothrow_move_constructible<basic_node>::value)
            :   kind_(o.kind_)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: value::move_construct<T>(*this, std::forward<basic_node>(o)); break
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
            basic_node(basic_node&& o, const allocator_type& al)
                noexcept(value::is_nothrow_move_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(al)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: value::move_construct<T>(*this, std::forward<basic_node>(o)); break
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
            basic_node& operator =(basic_node&& o)
                noexcept(value::is_nothrow_move_assignable<basic_node>::value)
            {   // TODO: alloc_ != o.alloc_
                this->~basic_node(), kind_ = o.kind_;
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return   alloc_ == o.alloc_ ? \
                                                                                    value::move_assign<T>(*this, std::forward<basic_node>(o)) : \
                                                                                    value::copy_assign<T>(*this, o)
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
                return *this; // LCOV_EXCL_LINE
            }

            basic_node(const basic_node& o)
                noexcept(value::is_nothrow_copy_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(std::allocator_traits<allocator_type>::select_on_container_copy_construction(o.alloc_))
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: value::copy_construct<T>(*this, o); break
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

            basic_node(const basic_node& o, const allocator_type& al)
                noexcept(value::is_nothrow_copy_constructible<basic_node>::value)
            :   kind_(o.kind_), alloc_(al)
            {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: value::copy_construct<T>(*this, o); break
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
            basic_node& operator =(const basic_node& o)
                noexcept(value::is_nothrow_copy_assignable<basic_node>::value)
            {   // TODO: alloc_ != o.alloc_
                if (kind_ != o.kind_) {
                    this->~basic_node(), kind_ = o.kind_;
                    switch (o.kind_) {
#                       define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return value::copy_assign<T>(*this, o), *this
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
#                       undef CXON_CBOR_TYPE_DEF
                    }
                }
                else {
                    switch (kind_) {
#                       define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return value::get<T>(*this) = value::get<T>(o), *this
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
#                       undef CXON_CBOR_TYPE_DEF
                    }
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
                        struct int_kind_            { static constexpr node_kind value = node_kind::sint; };
                    template <typename T>
                        struct int_kind_<T, false>  { static constexpr node_kind value = node_kind::uint; };

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
                        return imbue<null>() = nullptr, *this;
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
                return kind_ == imp::node_kind_<basic_node, T>::value;
            }

            template <typename T> T& imbue()
                noexcept(noexcept(value::construct<T>(std::declval<basic_node&>())))
            {
                if (!is<T>()) {
                    reset(), kind_ = imp::node_kind_<basic_node, T>::value;
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

    namespace imp {
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
    }

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

            taggle& operator =(T&& t) noexcept(std::is_nothrow_move_assignable<T>::value) {
                return value = std::move(t), *this;
            }
            taggle& operator =(const T& t) noexcept(std::is_nothrow_copy_assignable<T>::value) {
                return value = t, *this;
            }

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
