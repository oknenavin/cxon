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

namespace cxon { namespace cbor { // errors

    enum class node_error : int {
        ok,                         // no error
        invalid,                    // invalid cbor value
        recursion_depth_exceeded    // recursion depth exceeded
    };

    struct node_error_category;
    std::error_condition make_error_condition(node_error e) noexcept;

}}

namespace cxon { namespace cbor { // node traits

    enum class node_kind { sint, uint, bytes, text, array, map, boolean, null, real };

    struct node_traits {
        using                                       sint_type       = long long;
        using                                       uint_type       = unsigned long long;
        using                                       bytes_type      = std::vector<unsigned char>;
        using                                       text_type       = std::basic_string<char>;
        template <typename T> using                 array_type      = std::vector<T>;
        template <typename K, typename V> using     map_type        = std::map<K, V>;
        using                                       boolean_type    = bool;
        using                                       null_type       = std::nullptr_t;
        using                                       real_type       = double;
    };

}}

namespace cxon { namespace cbor { // context parameters

    CXON_PARAMETER(recursion_guard, unsigned);    // read/write
    CXON_PARAMETER(recursion_depth, unsigned);    // read/write: constexpr
        
}}

namespace cxon {

    template <typename X = CBOR<>, typename Tr, typename InIt, typename ...CxPs>
        inline auto     from_bytes(cbor::basic_node<Tr>& t, InIt b, InIt e, CxPs... p)      -> from_bytes_result<InIt>;
    template <typename X = CBOR<>, typename Tr, typename Iterable, typename ...CxPs>
        inline auto     from_bytes(cbor::basic_node<Tr>& t, const Iterable& i, CxPs... p)   -> from_bytes_result<decltype(std::begin(i))>;

    template <typename X = CBOR<>, typename Tr, typename OutIt, typename ...CxPs>
        inline auto     to_bytes(OutIt o, const cbor::basic_node<Tr>& t, CxPs... p)         -> enable_if_t<is_output_iterator<OutIt>::value, to_bytes_result<OutIt>>;
    template <typename X = CBOR<>, typename Tr, typename Insertable, typename ...CxPs>
        inline auto     to_bytes(Insertable& i, const cbor::basic_node<Tr>& t, CxPs... p)   -> enable_if_t<is_back_insertable<Insertable>::value, to_bytes_result<decltype(std::begin(i))>>;
    template <typename X = CBOR<>, typename Tr, typename FwIt, typename ...CxPs>
        inline auto     to_bytes(FwIt b, FwIt e, const cbor::basic_node<Tr>& t, CxPs... p)  -> to_bytes_result<FwIt>;

}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { // node

    namespace bits {

        template <typename N, typename T>
            struct node_kind_t;
        template <typename N> struct node_kind_t<N, typename N::sint>       { static constexpr node_kind value = node_kind::sint; };
        template <typename N> struct node_kind_t<N, typename N::uint>       { static constexpr node_kind value = node_kind::uint; };
        template <typename N> struct node_kind_t<N, typename N::bytes>      { static constexpr node_kind value = node_kind::bytes; };
        template <typename N> struct node_kind_t<N, typename N::text>       { static constexpr node_kind value = node_kind::text; };
        template <typename N> struct node_kind_t<N, typename N::array>      { static constexpr node_kind value = node_kind::array; };
        template <typename N> struct node_kind_t<N, typename N::map>        { static constexpr node_kind value = node_kind::map; };
        template <typename N> struct node_kind_t<N, typename N::null>       { static constexpr node_kind value = node_kind::null; };
        template <typename N> struct node_kind_t<N, typename N::boolean>    { static constexpr node_kind value = node_kind::boolean; };
        template <typename N> struct node_kind_t<N, typename N::real>       { static constexpr node_kind value = node_kind::real; };

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
            using sint      = typename Tr::sint_type;
            using uint      = typename Tr::uint_type;
            using bytes     = typename Tr::bytes_type;
            using text      = typename Tr::text_type;
            using array     = typename Tr::template array_type<basic_node>;
            using map       = typename Tr::template map_type<basic_node, basic_node>;
            using boolean   = typename Tr::boolean_type;
            using null      = typename Tr::null_type;
            using real      = typename Tr::real_type;

            private:
#               ifdef _MSC_VER // std::map move copy/assign are not noexcept, force
                    template <template <typename C> class X, bool = false>
                        struct msvc_map_override            : bits::is_nothrow_x<X, sint, uint, bytes, text, array, map, boolean, null, real> {};
                    template <template <typename C> class X>
                        struct msvc_map_override<X, true>   : bits::is_nothrow_x<X, sint, uint, bytes, text, array, /*map, */boolean, null, real> {};
                    using is_nothrow_move_constructible = msvc_map_override<std::is_nothrow_move_constructible, std::is_same<map, std::map<basic_node, basic_node>>::value>;
                    using is_nothrow_move_assignable    = msvc_map_override<std::is_nothrow_move_assignable,    std::is_same<map, std::map<basic_node, basic_node>>::value>;
#               else
                    using is_nothrow_move_constructible = bits::is_nothrow_x<std::is_nothrow_move_constructible, sint, uint, bytes, text, array, map, boolean, null, real>;
                    using is_nothrow_move_assignable    = bits::is_nothrow_x<std::is_nothrow_move_assignable, sint, uint, bytes, text, array, map, boolean, null, real>;
#               endif
            public:

            basic_node() noexcept : kind_(node_kind::null)  {}
            ~basic_node()                                   { reset(); }

            basic_node(basic_node&& o) noexcept(is_nothrow_move_constructible::value) : kind_(o.kind_) {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: new (&reinterpret_cast<T&>(value_)) T(std::move(o.get<T>())); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(real);
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
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(real);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return *this;
            }

            basic_node(const basic_node& o) : kind_(o.kind_) {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: new (&reinterpret_cast<T&>(value_)) T(o.get<T>()); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(real);
#                   undef CXON_CBOR_TYPE_DEF
                }
            }
            basic_node& operator =(const basic_node& o) {
                switch (o.kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: imbue<T>() = o.get<T>(); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(real);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return *this;
            }

#           define CXON_CBOR_TYPE_DEF(T)\
                    basic_node(T&& v) : kind_(node_kind::null)      { imbue<T>() = std::forward<T>(v); }\
                    basic_node& operator =(T&& v)                   { imbue<T>() = std::forward<T>(v); return *this; }\
                    basic_node(const T& v) : kind_(node_kind::null) { imbue<T>() = v; }\
                    basic_node& operator =(const T& v)              { imbue<T>() = v; return *this; }
                CXON_CBOR_TYPE_DEF(sint);
                CXON_CBOR_TYPE_DEF(uint);
                CXON_CBOR_TYPE_DEF(bytes);
                CXON_CBOR_TYPE_DEF(text);
                CXON_CBOR_TYPE_DEF(array);
                CXON_CBOR_TYPE_DEF(map);
                CXON_CBOR_TYPE_DEF(boolean);
                CXON_CBOR_TYPE_DEF(null);
                CXON_CBOR_TYPE_DEF(real);
#           undef CXON_CBOR_TYPE_DEF

            // handle literals
            private:
                template <typename T, bool E = std::is_signed<T>::value>
                    struct int_type             { using type = sint; };
                template <typename T>
                    struct int_type<T, false>   { using type = uint; };

                template <typename T>
                    struct is_int_unique {
                        static constexpr bool value = std::is_integral<T>::value && !std::is_same<T, typename int_type<T>::type>::value;
                    };
            public:
                // numeric
                template <typename T, typename = enable_if_t<is_int_unique<T>::value>>
                    basic_node(T t) : kind_(node_kind::null) {
                        imbue<typename int_type<T>::type>() = t;
                    }
                template <typename T, typename = enable_if_t<is_int_unique<T>::value>>
                    basic_node& operator =(T t) {
                        imbue<typename int_type<T>::type>() = t; return *this;
                    }
                // string
                basic_node(const char* s) : kind_(node_kind::null)          { imbue<text>() = s; }
                basic_node& operator =(const char* s)                       { imbue<text>() = s; return *this; }

            void reset() {
                switch (kind_) {
#                   define CXON_CBOR_TYPE_DEF(T)    case node_kind::T: reinterpret_cast<T&>(value_).~T(); break
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(real);
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
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        CXON_CBOR_TYPE_DEF(null);
                        CXON_CBOR_TYPE_DEF(real);
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
                        CXON_CBOR_TYPE_DEF(sint);
                        CXON_CBOR_TYPE_DEF(uint);
                        CXON_CBOR_TYPE_DEF(bytes);
                        CXON_CBOR_TYPE_DEF(text);
                        CXON_CBOR_TYPE_DEF(array);
                        CXON_CBOR_TYPE_DEF(map);
                        CXON_CBOR_TYPE_DEF(boolean);
                        case node_kind::null: return false;
                        CXON_CBOR_TYPE_DEF(real);
#                   undef CXON_CBOR_TYPE_DEF
                }
                return false; // LCOV_EXCL_LINE
            }

        private:
            using value_type = typename std::aligned_union<0, sint, uint, bytes, text, array, map, boolean, null, real>::type;
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

namespace cxon { namespace cbor { // errors

    struct node_error_category : std::error_category {
        const char* name() const noexcept override {
            return "cxon/cbor/node";
        }
        std::string message(int ev) const override {
            switch (static_cast<node_error>(ev)) {
                case node_error::ok:                        return "no error";
                case node_error::invalid:                   return "invalid cbor";
                case node_error::recursion_depth_exceeded:  return "recursion depth limit exceeded";
                default:                                    return "unknown error";
            }
        }
        static const node_error_category& value() {
            static node_error_category const v{};
            return v;
        }
    };

    std::error_condition make_error_condition(node_error e) noexcept {
        return { static_cast<int>(e), node_error_category::value() };
    }

}}

namespace std { // cxon/cbor errors
    template <> struct is_error_condition_enum<cxon::cbor::node_error> : true_type {};
}

namespace cxon { namespace cbor { namespace bits {

    template <typename Cx, bool G = recursion_guard::in<napa_type<Cx>>::value>
        struct scinc {
            Cx& cx;
            scinc(Cx& cx) : cx(cx)  { ++recursion_guard::reference(cx.px); }
            ~scinc()                { --recursion_guard::reference(cx.px); }
            bool check() const      { return recursion_guard::value(cx.px) < recursion_depth::constant<napa_type<Cx>>(64); }
        };
    template <typename Cx>
        struct scinc<Cx, false> {
            scinc(Cx&)              {}
            ~scinc()                {}
            bool check() const      { return true; }
        };

}}}

namespace cxon {

    template <typename X, typename Tr, typename II, typename ...CxPs>
        inline auto from_bytes(cbor::basic_node<Tr>& t, II b, II e, CxPs... p) -> from_bytes_result<II> {
            return interface::from_bytes<X>(t, b, e, cbor::recursion_guard::set(0), std::forward<CxPs>(p)...);
        }
    template <typename X, typename Tr, typename I, typename ...CxPs>
        inline auto from_bytes(cbor::basic_node<Tr>& t, const I& i, CxPs... p) -> from_bytes_result<decltype(std::begin(i))> {
            return interface::from_bytes<X>(t, i, cbor::recursion_guard::set(0), std::forward<CxPs>(p)...);
        }

    template <typename X, typename Tr, typename OI, typename ...CxPs>
        inline auto to_bytes(OI o, const cbor::basic_node<Tr>& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
            return interface::to_bytes<X>(o, t, cbor::recursion_guard::set(0), std::forward<CxPs>(p)...);
        }
    template <typename X, typename Tr, typename I, typename ...CxPs>
        inline auto to_bytes(I& i, const cbor::basic_node<Tr>& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
            return interface::to_bytes<X>(i, t, cbor::recursion_guard::set(0), std::forward<CxPs>(p)...);
        }
    template <typename X, typename Tr, typename FI, typename ...CxPs>
        inline auto to_bytes(FI b, FI e, const cbor::basic_node<Tr>& t, CxPs... p) -> to_bytes_result<FI> {
            return interface::to_bytes<X>(b, e, t, cbor::recursion_guard::set(0), std::forward<CxPs>(p)...);
        }

#   define CXON_CBOR_NODE_RG()\
        cbor::bits::scinc<Cx> RG__(cx);\
        if (!RG__.check()) return cx|cbor::node_error::recursion_depth_exceeded, false

        template <typename X, typename Tr>
            struct read<X, cbor::basic_node<Tr>> {
                template <typename II, typename Cx>
                    static bool value(cbor::basic_node<Tr>& t, II& i, II e, Cx& cx) {
                        size_t tag;
                        if (!cbor::tag::read<X>(tag, i, e, cx))
                            return false;
                        bio::byte const b = bio::peek(i, e);
                        switch (b & X::mjr) {
#                           define CXON_READ(T)             read_value<X>(t.template imbue<typename cbor::basic_node<Tr>::T>(), i, e, cx)
#                           define CXON_READ_ARR(T, tag)    cbor::cnt::read_array<X>(t.template imbue<typename cbor::basic_node<Tr>::T>(), tag, i, e, cx)
                                case X::pint                                        :                           return CXON_READ(uint);
                                case X::nint                                        :                           return CXON_READ(sint);
                                case X::bstr                                        :                           return CXON_READ(bytes);
                                case X::tstr                                        :                           return CXON_READ(text);
                                case X::arr                                         : { CXON_CBOR_NODE_RG();    return CXON_READ_ARR(array, tag); }
                                case X::map                                         : { CXON_CBOR_NODE_RG();    return CXON_READ_ARR(map, tag); }
                                case X::tag                                         : { CXON_CBOR_NODE_RG();    return value(t, i, e, cx); }
                                case X::svn:
                                    switch (b) {
                                        case X::neg: case X::pos                    :                           return CXON_READ(boolean);
                                        case X::nil                                 :                           return CXON_READ(null);
                                        case X::fp16: case X::fp32: case X::fp64    :                           return CXON_READ(real);
                                    }
#                           undef CXON_READ_ARR
#                           undef CXON_READ
                        }
                        return cx|cbor::node_error::invalid, false;
                    }
            };

        template <typename X, typename Tr>
            struct write<X, cbor::basic_node<Tr>> {
                template <typename O, typename Cx>
                    static bool value(O& o, const cbor::basic_node<Tr>& t, Cx& cx) {
                        using cbor::node_kind;
                        switch (t.kind()) {
#                           define CXON_WRITE(T) write_value<X>(o, t.template get<typename cbor::basic_node<Tr>::T>(), cx)
                                case node_kind::uint    :                           return CXON_WRITE(uint);
                                case node_kind::sint    :                           return CXON_WRITE(sint);
                                case node_kind::bytes   :                           return CXON_WRITE(bytes);
                                case node_kind::text    :                           return CXON_WRITE(text);
                                case node_kind::array   : { CXON_CBOR_NODE_RG();    return CXON_WRITE(array); }
                                case node_kind::map     : { CXON_CBOR_NODE_RG();    return CXON_WRITE(map);   }
                                case node_kind::boolean :                           return CXON_WRITE(boolean);
                                case node_kind::null    :                           return CXON_WRITE(null);
                                case node_kind::real    :                           return CXON_WRITE(real);
#                           undef CXON_WRITE
                        }
                        return false; // LCOV_EXCL_LINE
                    }
            };

#   undef CXON_CBOR_NODE_RG

}

#endif // CXON_CBOR_NODE_HXX_
