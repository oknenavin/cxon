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

#include <memory>

// MOVE ////////////////////////////////////////////////////////////////////////

namespace cxon { namespace imp {

    template <typename N>
        struct node {
            // value access
            template <typename U, typename M>
                static constexpr auto value(M& n) noexcept -> enable_if_t<!M::template is_dynamic_type_<U>::value && !std::is_pointer<U>::value, U&> {
                    return *(U*)&n.value_;
                }
            template <typename U, typename M>
                static constexpr auto value(M& n) noexcept -> enable_if_t< M::template is_dynamic_type_<U>::value && !std::is_pointer<U>::value, U&> {
                    return **(U**)&n.value_;
                }
            template <typename U, typename M>
                static constexpr auto value(M& n) noexcept -> enable_if_t<!M::template is_dynamic_type_<U>::value &&  std::is_pointer<U>::value, U&> {
                    return (U)&n.value_;
                }
            template <typename U, typename M>
                static constexpr auto value(M& n) noexcept -> enable_if_t< M::template is_dynamic_type_<U>::value &&  std::is_pointer<U>::value, U&> {
                    return *(U*)&n.value_;
                }
            // move construct
            template <typename U>
                static auto move_construct(N& n, N&& o)
                       noexcept(std::is_nothrow_move_constructible<U>::value)
                    -> enable_if_t<!N::template is_dynamic_type_<U>::value && !std::is_trivially_assignable<U, U>::value>
                {
                    alc::uninitialized_construct_using_allocator<U>(&value<U>(n), n.alloc_, std::move(value<U>(o)));
                }
            template <typename U>
                static auto move_construct(N& n, N&& o) noexcept
                    -> enable_if_t<!N::template is_dynamic_type_<U>::value &&  std::is_trivially_assignable<U, U>::value>
                {
                    value<U>(n) = value<U>(o);
                }
            template <typename U>
                static auto move_construct(N& n, N&& o) noexcept
                    -> enable_if_t< N::template is_dynamic_type_<U>::value>
                {
                    value<U*>(n) = value<U*>(o), o.kind_ = N::node_kind_default;
                }
            // move assign
            template <typename U>
                static auto move_assign(N& n, N&& o)
                       noexcept(std::is_nothrow_move_constructible<U>::value)
                    -> enable_if_t<!N::template is_dynamic_type_<U>::value &&  std::allocator_traits<typename N::allocator_type>::propagate_on_container_move_assignment::value, N&>
                {
                    n.alloc_ = std::move(o.alloc_);
                    alc::uninitialized_construct_using_allocator<U>(&value<U>(n), n.alloc_, std::move(value<U>(o)));
                    return n;
                }
            template <typename U>
                static auto move_assign(N& n, N&& o) noexcept
                    -> enable_if_t< N::template is_dynamic_type_<U>::value &&  std::allocator_traits<typename N::allocator_type>::propagate_on_container_move_assignment::value, N&>
                {
                    n.alloc_ = std::move(o.alloc_);
                    value<U*>(n) = value<U*>(o), o.kind_ = N::node_kind_default;
                    return n;
                }
            template <typename U>
                static auto move_assign(N& n, N&& o)
                        noexcept(std::is_nothrow_move_assignable<U>::value)
                    -> enable_if_t<!N::template is_dynamic_type_<U>::value && !std::allocator_traits<typename N::allocator_type>::propagate_on_container_move_assignment::value, N&>
                {
                    value<U>(n) = std::move(value<U>(o));
                    return n;
                }
            template <typename U>
                static auto move_assign(N& n, N&& o) noexcept
                    -> enable_if_t< N::template is_dynamic_type_<U>::value && !std::allocator_traits<typename N::allocator_type>::propagate_on_container_move_assignment::value, N&>
                {
                    value<U*>(n) = value<U*>(o), o.kind_ = N::node_kind_default;
                    return n;
                }
            // copy construct
            template <typename U>
                static auto copy_construct(N& n, const N& o)
                        noexcept(std::is_nothrow_copy_constructible<U>::value)
                    -> enable_if_t<!N::template is_dynamic_type_<U>::value && !std::is_trivially_assignable<U, U>::value>
                {
                    alc::uninitialized_construct_using_allocator<U>(&value<U>(n), n.alloc_, value<U>(o));
                }
            template <typename U>
                static auto copy_construct(N& n, const N& o) noexcept
                    -> enable_if_t<!N::template is_dynamic_type_<U>::value &&  std::is_trivially_assignable<U, U>::value>
                {
                    value<U>(n) = value<U>(o);
                }
            template <typename U>
                static auto copy_construct(N& n, const N& o)
                    -> enable_if_t< N::template is_dynamic_type_<U>::value>
                {
                    construct<U>(n, value<U>(o));
                }
            // construct
            template <typename U, typename ...A>
                static auto construct(N& n, A&&... t)
                        noexcept(std::is_nothrow_constructible<U, A&&...>::value)
                    ->  enable_if_t<!N::template is_dynamic_type_<U>::value>
                {
                    alc::uninitialized_construct_using_allocator<U>(&value<U>(n), n.alloc_, std::forward<A>(t)...);
                }
            template <typename U, typename ...A>
                static auto construct(N& n, A&&... t)
                    -> enable_if_t< N::template is_dynamic_type_<U>::value>
                {
                    using at = alc::rebind_t<typename N::allocator_type, U>;
                    at    al(n.alloc_);
                    value<U*>(n) = std::allocator_traits<at>::allocate(al, 1);
                    alc::uninitialized_construct_using_allocator<U>(value<U*>(n), al, std::forward<A>(t)...);
                }
            // destruct
            template <typename U>
                static auto destruct(N& n) noexcept -> enable_if_t<!N::template is_dynamic_type_<U>::value> {
                    value<U>(n).~U();
                }
            template <typename U>
                static auto destruct(N& n) noexcept -> enable_if_t< N::template is_dynamic_type_<U>::value> {
                    using at = alc::rebind_t<typename N::allocator_type, U>;
                    at    al(n.alloc_);
                    std::allocator_traits<at>::destroy(al, value<U*>(n));
                    std::allocator_traits<at>::deallocate(al, value<U*>(n), 1);
                }

        };

    template <typename T, typename N>
        constexpr T& value(N& n) noexcept {
            return node<N>::template value<T>(n);
        }

    template <typename T, typename N>
        inline void move_construct(N& n, N&& o)
            noexcept(noexcept(node<N>::template move_construct<T>(n, std::forward<N>(o))))
        {
            node<N>::template move_construct<T>(n, std::forward<N>(o));
        }

    template <typename T, typename N>
        inline N& move_assign(N& n, N&& o)
            noexcept(noexcept(node<N>::template move_assign<T>(n, std::forward<N>(o))))
        {
            return node<N>::template move_assign<T>(n, std::forward<N>(o));
        }

    template <typename T, typename N>
        inline void copy_construct(N& n, const N& o)
            noexcept(noexcept(node<N>::template copy_construct<T>(n, o)))
        {
            node<N>::template copy_construct<T>(n, o);
        }

    template <typename T, typename N, typename ...A>
        inline void construct(N& n, A&&... t)
            noexcept(noexcept(node<N>::template construct<T, A...>(n, std::forward<A>(t)...)))
        {
            node<N>::template construct<T, A...>(n, std::forward<A>(t)...);
        }

    template <typename T, typename N>
        inline void destruct(N& n) noexcept {
            node<N>::template destruct<T>(n);
        }

}}

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace json { // node

    template <typename Al = std::allocator<void>>
        struct node_traits;

    template <typename Tr>
        struct basic_node;

    using node = basic_node<node_traits<>>;

    template <typename T, typename N> inline        bool    is(const N& n) noexcept;
    template <typename T, typename N> inline        T&      imbue(N& n) noexcept(noexcept(std::declval<N&>().template imbue<T>()));
    template <typename T, typename N> inline        T&      get(N& n) noexcept;
    template <typename T, typename N> inline const  T&      get(const N& n) noexcept;
    template <typename T, typename N> inline        T*      get_if(N& n) noexcept;
    template <typename T, typename N> inline const  T*      get_if(const N& n) noexcept;

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

    namespace imp {
        template <typename N, typename T>
            struct node_kind_; // value
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
                    using is_dynamic_type_ =
                        integer_sequence_contains<typename Tr::dynamic_types, imp::node_kind_<basic_node, typename std::remove_pointer<T>::type>::value>;

                template <typename T>
                    using dt_ = typename std::conditional<is_dynamic_type_<T>::value, T*, T>::type;

#               define CXON_TYPES_DEF dt_<object>, dt_<array>, dt_<string>, dt_<sint>, dt_<uint>, dt_<real>, dt_<boolean>, dt_<null>
                    using dynamic_types_ = type_sequence<CXON_TYPES_DEF>;
                    using value_type_ = typename std::aligned_union<0, CXON_TYPES_DEF>::type;
#               undef CXON_TYPES_DEF

                value_type_     value_;
                node_kind       kind_;
                allocator_type  alloc_;

            private:
                using is_nothrow_move_constructible_ = type_sequence_conjunction<std::is_nothrow_move_constructible, dynamic_types_>;
                using is_nothrow_move_assignable_    = type_sequence_conjunction<std::is_nothrow_move_assignable, dynamic_types_>;
                using is_nothrow_copy_constructible_ = type_sequence_conjunction<std::is_nothrow_copy_constructible, dynamic_types_>;
                using is_nothrow_copy_assignable_    = type_sequence_conjunction<std::is_nothrow_copy_assignable, dynamic_types_>;

                template <typename A>
                    using propagate_on_container_move_assignment_ = typename std::allocator_traits<A>::propagate_on_container_move_assignment;
                template <typename A>
                    using propagate_on_container_copy_assignment_ = typename std::allocator_traits<A>::propagate_on_container_copy_assignment;

                using has_dynamic_tpyes_ = type_sequence_disjunction<is_dynamic_type_, dynamic_types_>;
            public:

            // value
                private:
                    static constexpr node_kind node_kind_default = node_kind::null;
                    template <typename N>
                        friend struct cxon::imp::node;
                public:

            // move construct
                private:
                    static void move_construct(basic_node& n, basic_node&& o)
                        noexcept(is_nothrow_move_constructible_::value)
                    {
                        switch (o.kind_) {
#                           define CXON_JSON_TYPE_DEF(T)    case node_kind::T: cxon::imp::move_construct<T>(n, std::forward<basic_node>(o)); break
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
                    static basic_node& move_assign(basic_node& n, basic_node&& o)
                        noexcept(is_nothrow_move_constructible_::value && is_nothrow_move_assignable_::value)
                    {
                        destruct(n), n.kind_ = o.kind_;
                        switch (o.kind_) {
#                           define CXON_JSON_TYPE_DEF(T)    case node_kind::T: return cxon::imp::move_assign<T>(n, std::forward<basic_node>(o))
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
                        return n; // LCOV_EXCL_LINE
                    }
                public:

            // copy construct
                private:
                    static void copy_construct(basic_node& n, const basic_node& o)
                        noexcept(!has_dynamic_tpyes_::value && is_nothrow_copy_constructible_::value)
                    {
                        switch (o.kind_) {
#                           define CXON_JSON_TYPE_DEF(T)    case node_kind::T: cxon::imp::copy_construct<T>(n, o); break
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
                    static void copy_assign(basic_node& n, const basic_node& o)
                        noexcept(!has_dynamic_tpyes_::value && is_nothrow_copy_constructible_::value)
                    {
                        destruct(n), n.kind_ = o.kind_;
                        switch (o.kind_) {
#                           define CXON_JSON_TYPE_DEF(T)    case node_kind::T: cxon::imp::copy_construct<T>(n, o); break
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
                public:

            // destruct
                private:
                    static void destruct(basic_node& n) noexcept {
                        switch (n.kind_) {
#                           define CXON_JSON_TYPE_DEF(T)    case node_kind::T: cxon::imp::destruct<T>(n); break
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

            /*constexpr */basic_node() noexcept
            :   kind_(node_kind::null)
            {
                cxon::imp::construct<null>(*this, nullptr);
            }
            /*constexpr */basic_node(const allocator_type& al) noexcept
            :   kind_(node_kind::null), alloc_(al)
            {
                cxon::imp::construct<null>(*this, nullptr);
            }
            ~basic_node() noexcept {
                reset();
            }

            /*constexpr */basic_node(basic_node&& o)
                noexcept(noexcept(move_construct(std::declval<basic_node&>(), std::forward<basic_node>(o))))
            :   kind_(o.kind_)
            {
                move_construct(*this, std::forward<basic_node>(o));
            }
            /*constexpr */basic_node(basic_node&& o, const allocator_type& al)
                noexcept(noexcept(move_construct(std::declval<basic_node&>(), std::forward<basic_node>(o))))
            :   kind_(o.kind_), alloc_(al)
            {
                move_construct(*this, std::forward<basic_node>(o));
            }
            /*constexpr */basic_node& operator =(basic_node&& o)
                noexcept(
                    noexcept(move_assign(std::declval<basic_node&>(), std::forward<basic_node>(o))) &&
                    noexcept(copy_assign(std::declval<basic_node&>(), o))
                )
            {
                // TODO: alloc_ != o.alloc_
                return alloc_ == o.alloc_ ?
                    move_assign(*this, std::forward<basic_node>(o)) :
                    (copy_assign(*this, o), *this)
                ;
            }

            /*constexpr */basic_node(const basic_node& o)
                noexcept(noexcept(copy_construct(std::declval<basic_node&>(), o)))
            :   kind_(o.kind_), alloc_(std::allocator_traits<allocator_type>::select_on_container_copy_construction(o.alloc_))
            {
                copy_construct(*this, o);
            }
            /*constexpr */basic_node(const basic_node& o, const allocator_type& al)
                noexcept(noexcept(copy_construct(std::declval<basic_node&>(), o)))
            :   kind_(o.kind_), alloc_(al)
            {
                copy_construct(*this, o);
            }
            /*constexpr */basic_node& operator =(const basic_node& o)
                noexcept(noexcept(copy_assign(std::declval<basic_node&>(), o)))
            {
                // TODO: alloc_ != o.alloc_
                return copy_assign(*this, o), *this;
            }

#           define CXON_JSON_TYPE_DEF(T)\
                    basic_node(T&& v)                                   noexcept(noexcept(cxon::imp::construct<T>(std::declval<basic_node&>(), std::forward<T>(v)))) \
                                                                        : kind_(node_kind::T)               { cxon::imp::construct<T>(*this, std::forward<T>(v)); } \
                    basic_node(T&& v, const allocator_type& al)         noexcept(noexcept(cxon::imp::construct<T>(std::declval<basic_node&>(), std::forward<T>(v)))) \
                                                                        : kind_(node_kind::T), alloc_(al)   { cxon::imp::construct<T>(*this, std::forward<T>(v)); } \
                    basic_node& operator =(T&& v)                       noexcept(noexcept(std::declval<basic_node&>().template imbue<T>() = std::forward<T>(v))) \
                                                                        { return imbue<T>() = std::forward<T>(v), *this; } \
                    basic_node(const T& v)                              noexcept(noexcept(cxon::imp::construct<T>(std::declval<basic_node&>(), v))) \
                                                                        : kind_(node_kind::T)               { cxon::imp::construct<T>(*this, v); } \
                    basic_node(const T& v, const allocator_type& al)    noexcept(noexcept(cxon::imp::construct<T>(std::declval<basic_node&>(), v))) \
                                                                        : kind_(node_kind::T), alloc_(al)   { cxon::imp::construct<T>(*this, v); } \
                    basic_node& operator =(const T& v)                  noexcept(noexcept(std::declval<basic_node&>().template imbue<T>() = v)) \
                                                                        { return imbue<T>() = v, *this; }
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
                        cxon::imp::construct<int_type<T>>(*this, t);
                    }
                template <typename T, typename = enable_if_t<std::is_integral<T>::value>>
                    basic_node(T t, const allocator_type& al) noexcept
                    :   kind_(int_kind<T>::value),
                        alloc_(al)
                    {
                        cxon::imp::construct<int_type<T>>(*this, t);
                    }
                template <typename T, typename = enable_if_t<std::is_integral<T>::value>>
                    basic_node& operator =(T t) noexcept {
                        return imbue<int_type<T>>() = t, *this;
                    }
            // std::nullptr_t
                template <typename ...>
                    constexpr basic_node(std::nullptr_t) noexcept
                    :   kind_(node_kind::null)
                    {
                    }
                template <typename ...>
                    constexpr basic_node(std::nullptr_t, const allocator_type& al) noexcept
                    :   kind_(node_kind::null),
                        alloc_(al)
                    {
                    }
                template <typename ...>
                    basic_node& operator =(std::nullptr_t) noexcept {
                        return destruct(*this), kind_ = node_kind::null, *this; // TODO: assign nullptr
                    }
            // string
                basic_node(const typename string::value_type* s)
                :   kind_(node_kind::string)
                {
                    cxon::imp::construct<string>(*this, s);
                }
                basic_node(const typename string::value_type* s, const allocator_type& al)
                :   kind_(node_kind::string),
                    alloc_(al)
                {
                    cxon::imp::construct<string>(*this, s);
                }
                basic_node& operator =(const typename string::value_type* s) {
                    return imbue<string>() = s, *this;
                }

            void reset() noexcept {
                destruct(*this), kind_ = node_kind::null;
            }

            node_kind kind() const noexcept { return kind_; }

            allocator_type get_allocator() const noexcept {
                return alloc_;
            }

            template <typename T> bool  is() const noexcept {
                return kind_ == imp::node_kind_<basic_node, T>::value;
            }

            template <typename T> T& imbue()
                noexcept(noexcept(cxon::imp::construct<T>(std::declval<basic_node&>())))
            {
                if (!is<T>()) {
                    reset(), kind_ = imp::node_kind_<basic_node, T>::value;
                    cxon::imp::construct<T>(*this);
                }
                return cxon::imp::value<T>(*this);
            }

            template <typename T> T& get() noexcept {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return cxon::imp::value<T>(*this);
            }
            template <typename T> const T& get() const noexcept {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return cxon::imp::value<T>(*this);
            }

            template <typename T> T* get_if() noexcept {
                return is<T>() ? &cxon::imp::value<T>(*this) : nullptr;
            }
            template <typename T> const T* get_if() const noexcept {
                return is<T>() ? &cxon::imp::value<T>(*this) : nullptr;
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

            private:
                friend constexpr bool operator ==(const basic_node& n, null) noexcept { return  n.is<null>(); }
                friend constexpr bool operator ==(null, const basic_node& n) noexcept { return  n.is<null>(); }
                friend constexpr bool operator !=(const basic_node& n, null) noexcept { return !n.is<null>(); }
                friend constexpr bool operator !=(null, const basic_node& n) noexcept { return !n.is<null>(); }
        };

    namespace imp {
        template <typename N> struct node_kind_<N, typename N::null>    { static constexpr node_kind value = node_kind::null; };
        template <typename N> struct node_kind_<N, typename N::boolean> { static constexpr node_kind value = node_kind::boolean; };
        template <typename N> struct node_kind_<N, typename N::sint>    { static constexpr node_kind value = node_kind::sint; };
        template <typename N> struct node_kind_<N, typename N::uint>    { static constexpr node_kind value = node_kind::uint; };
        template <typename N> struct node_kind_<N, typename N::real>    { static constexpr node_kind value = node_kind::real; };
        template <typename N> struct node_kind_<N, typename N::string>  { static constexpr node_kind value = node_kind::string; };
        template <typename N> struct node_kind_<N, typename N::array>   { static constexpr node_kind value = node_kind::array; };
        template <typename N> struct node_kind_<N, typename N::object>  { static constexpr node_kind value = node_kind::object; };
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

namespace cxon { // hash

    template <typename Tr>
        struct hash<json::basic_node<Tr>> {
            std::size_t operator ()(const json::basic_node<Tr>& n) const noexcept {
                switch (n.kind()) {
#                   define CXON_JSON_TYPE_DEF(T)        case json::node_kind::T: return make_hash(json::get<typename json::basic_node<Tr>::T>(n))
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(sint);
                        CXON_JSON_TYPE_DEF(uint);
                        CXON_JSON_TYPE_DEF(real);
                        CXON_JSON_TYPE_DEF(boolean);
                        case json::node_kind::null:     return 0; //CXON_JSON_TYPE_DEF(null); // g++-8: error: use of deleted function ‘std::hash<std::nullptr_t>::hash()’
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
