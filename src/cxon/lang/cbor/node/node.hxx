// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_NODE_HXX_
#define CXON_CBOR_NODE_HXX_

#include "cxon/lang/cbor/cbor.hxx"

#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // node

    template <typename Tr>
        struct basic_node;

    using node = basic_node<struct node_traits>;

    template <typename T, typename N> inline        bool    is(const N& n);
    template <typename T, typename N> inline        T&      imbue(N& n);
    template <typename T, typename N> inline        T&      get(N& n);
    template <typename T, typename N> inline const  T&      get(const N& n);
    template <typename T, typename N> inline        T*      get_if(N& n);
    template <typename T, typename N> inline const  T*      get_if(const N& n);
}}

namespace cxon { namespace cbor { // node traits

    enum class node_kind { positive, negative, bytes, string, array, map, boolean, null, floating };

    struct node_traits {
        using                                       positive_type   = long long;
        using                                       negative_type   = unsigned long long;
        using                                       bytes_type      = std::basic_string<unsigned char>;
        using                                       string_type     = std::basic_string<char>;
        template <typename T> using                 array_type      = std::vector<T>;
        template <typename K, typename V> using     map_type        = std::map<K, V>;
        using                                       boolean_type    = bool;
        using                                       null_type       = std::nullptr_t;
        using                                       floating_type   = double;
    };

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // node

    namespace bits {

        template <typename N, typename T>
            struct node_kind_t;
        template <typename N> struct node_kind_t<N, typename N::positive>   { static constexpr node_kind value = node_kind::positive; };
        template <typename N> struct node_kind_t<N, typename N::negative>   { static constexpr node_kind value = node_kind::negative; };
        template <typename N> struct node_kind_t<N, typename N::bytes>      { static constexpr node_kind value = node_kind::bytes; };
        template <typename N> struct node_kind_t<N, typename N::string>     { static constexpr node_kind value = node_kind::string; };
        template <typename N> struct node_kind_t<N, typename N::array>      { static constexpr node_kind value = node_kind::array; };
        template <typename N> struct node_kind_t<N, typename N::map>        { static constexpr node_kind value = node_kind::map; };
        template <typename N> struct node_kind_t<N, typename N::null>       { static constexpr node_kind value = node_kind::null; };
        template <typename N> struct node_kind_t<N, typename N::boolean>    { static constexpr node_kind value = node_kind::boolean; };
        template <typename N> struct node_kind_t<N, typename N::floating>   { static constexpr node_kind value = node_kind::floating; };

        template <typename N, typename T>
            constexpr node_kind node_kind_from() noexcept { return node_kind_t<N, T>::value; }

        template <template <typename C> class X, typename ...>
            struct is_nothrow_x             { static constexpr bool value = true; };
        template <template <typename C> class X, typename T>
            struct is_nothrow_x<X, T>       { static constexpr bool value = X<T>::value; };
        template <template <typename C> class X, typename H, typename ...T>
            struct is_nothrow_x<X, H, T...> { static constexpr bool value = is_nothrow_x<X, H>::value && is_nothrow_x<X, T...>::value; };

    }

    template <typename Tr>
        struct basic_node {
            using positive  = typename Tr::positive_type;
            using negative  = typename Tr::negative_type;
            using bytes     = typename Tr::bytes_type;
            using string    = typename Tr::string_type;
            using array     = typename Tr::template array_type<basic_node>;
            using map       = typename Tr::template map_type<basic_node, basic_node>;
            using boolean   = typename Tr::boolean_type;
            using null      = typename Tr::null_type;
            using floating  = typename Tr::floating_type;

#           ifdef _MSC_VER // std::map move copy/assign are not noexcept, force
                template <template <typename C> class X, bool = false>
                    struct msvc_map_override            : bits::is_nothrow_x<X, positive, negative, bytes, string, array, map, boolean, null, floating> {};
                template <template <typename C> class X>
                    struct msvc_map_override<X, true>   : bits::is_nothrow_x<X, positive, negative, bytes, string, array, /*map, */boolean, null, floating> {};
                using is_nothrow_move_constructible = msvc_map_override<std::is_nothrow_move_constructible, std::is_same<map, std::map<string, basic_node>>::value>;
                using is_nothrow_move_assignable    = msvc_map_override<std::is_nothrow_move_assignable,    std::is_same<map, std::map<string, basic_node>>::value>;
#           else
                using is_nothrow_move_constructible = bits::is_nothrow_x<std::is_nothrow_move_constructible, positive, negative, bytes, string, array, map, boolean, null, floating>;
                using is_nothrow_move_assignable    = bits::is_nothrow_x<std::is_nothrow_move_assignable, positive, negative, bytes, string, array, map, boolean, null, floating>;
#           endif

            basic_node() noexcept : kind_(node_kind::null)  {}
            ~basic_node()                                   { reset(); }

            basic_node(basic_node&& o) noexcept(is_nothrow_move_constructible::value) : kind_(o.kind_) {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: new (&reinterpret_cast<T&>(value_)) T(std::move(o.get<T>())); break
                        CXON_CBOR_TYPE_DEF(positive);
                        CXON_CBOR_TYPE_DEF(negative);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(string);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(floating);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }
            basic_node& operator =(basic_node&& o) noexcept(is_nothrow_move_assignable::value) {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: get<T>() = std::move(o.get<T>()); break
                        CXON_CBOR_TYPE_DEF(positive);
                        CXON_CBOR_TYPE_DEF(negative);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(string);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(floating);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return *this;
            }

            basic_node(const basic_node& o) : kind_(o.kind_) {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: new (&reinterpret_cast<T&>(value_)) T(o.get<T>()); break
                        CXON_CBOR_TYPE_DEF(positive);
                        CXON_CBOR_TYPE_DEF(negative);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(string);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(floating);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }
            basic_node& operator =(const basic_node& o) {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: imbue<T>() = o.get<T>(); break
                        CXON_CBOR_TYPE_DEF(positive);
                        CXON_CBOR_TYPE_DEF(negative);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(string);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(floating);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return *this;
            }

#           define CXON_CBOR_TYPE_DEF(T)\
                    basic_node(T&& v) : kind_(node_kind::null)      { imbue<T>() = std::forward<T>(v); }\
                    basic_node& operator =(T&& v)                   { imbue<T>() = std::forward<T>(v); return *this; }\
                    basic_node(const T& v) : kind_(node_kind::null) { imbue<T>() = v; }\
                    basic_node& operator =(const T& v)              { imbue<T>() = v; return *this; }
                CXON_CBOR_TYPE_DEF(positive);
                CXON_CBOR_TYPE_DEF(negative);
                CXON_CBOR_TYPE_DEF(bytes);
                CXON_CBOR_TYPE_DEF(string);
                CXON_CBOR_TYPE_DEF(array);
                CXON_CBOR_TYPE_DEF(map);
                CXON_CBOR_TYPE_DEF(boolean);
                CXON_CBOR_TYPE_DEF(null);
                CXON_CBOR_TYPE_DEF(floating);
#           undef CXON_CBOR_TYPE_DEF
                basic_node(int n) : kind_(node_kind::null)          { imbue<negative>() = n; }
                basic_node& operator =(int n)                       { imbue<negative>() = n; return *this; }
                basic_node(const char* s) : kind_(node_kind::null)  { imbue<string>() = s; }
                basic_node& operator =(const char* s)               { imbue<string>() = s; return *this; }

            void reset() {
                switch (kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: reinterpret_cast<T&>(value_).~T(); break
                        CXON_CBOR_TYPE_DEF(positive);
                        CXON_CBOR_TYPE_DEF(negative);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(string);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(floating);
#                   undef CXON_CBOR_TYPE_DEF
                }
                kind_ = node_kind::null;
            }

            node_kind kind() const noexcept { return kind_; }

            template <typename T> bool  is() const {
                return kind_ == bits::node_kind_from<basic_node, T>();
            }

            template <typename T> T& imbue() {
                if (!is<T>()) {
                    reset(), kind_ = bits::node_kind_from<basic_node, T>();
                    new (&value_) T();
                }
                return reinterpret_cast<T&>(value_);
            }

            template <typename T> T& get() {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return reinterpret_cast<T&>(value_);
            }
            template <typename T> const T& get() const {
                CXON_ASSERT(is<T>(), "node type mismatch");
                return reinterpret_cast<const T&>(value_);
            }

            template <typename T> T* get_if() noexcept {
                return is<T>() ? &reinterpret_cast<T&>(value_) : nullptr;
            }
            template <typename T> const T* get_if() const noexcept {
                return is<T>() ? &reinterpret_cast<const T&>(value_) : nullptr;
            }

            bool operator == (const basic_node& n) const {
                if (kind() != n.kind()) return false;
                switch (kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return get<T>() == n.get<T>()
                        CXON_CBOR_TYPE_DEF(positive);
                        CXON_CBOR_TYPE_DEF(negative);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(string);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(floating);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return false; // LCOV_EXCL_LINE
            }
            bool operator != (const basic_node& n) const {
                return !operator ==(n);
            }

            bool operator < (const basic_node& n) const {
                if (kind() != n.kind())
                    return kind() < n.kind();
                switch (kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: return get<T>() < n.get<T>()
                        CXON_CBOR_TYPE_DEF(positive);
                        CXON_CBOR_TYPE_DEF(negative);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(string);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        case node_kind::null: return false;
                        CXON_CBOR_TYPE_DEF(floating);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return false; // LCOV_EXCL_LINE
            }

        private:
            using value_type = typename std::aligned_union<0, positive, negative, bytes, string, array, map, boolean, null, floating>::type;
            value_type  value_;
            node_kind   kind_;
        };

    template <typename T, typename N>
        inline bool is(const N& n)          { return n.template is<T>(); }

    template <typename T, typename N>
        inline T& imbue(N& n)               { return n.template imbue<T>(); }

    template <typename T, typename N>
        inline T& get(N& n)                 { return n.template get<T>(); }
    template <typename T, typename N>
        inline const T& get(const N& n)     { return n.template get<T>(); }

    template <typename T, typename N>
        inline T* get_if(N& n)              { return n.template get_if<T>(); }
    template <typename T, typename N>
        inline const T* get_if(const N& n)  { return n.template get_if<T>(); }

}}

#endif // CXON_CBOR_NODE_HXX_
