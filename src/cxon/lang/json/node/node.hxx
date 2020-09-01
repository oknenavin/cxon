// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_NODE_HXX_
#define CXON_JSON_NODE_HXX_

#include "cxon/lang/json/json.hxx"

#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace json { // node

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

namespace cxon { namespace json { // errors

    enum error : int {
        ok,                         // no error
        invalid,                    // invalid json value
        recursion_depth_exceeded    // recursion depth exceeded
    };

}}

namespace cxon { namespace json { // node traits

    enum class node_kind { object, array, string, number, boolean, null };

    struct node_traits {
        using                                       null_type       = std::nullptr_t;
        using                                       boolean_type    = bool;
        using                                       number_type     = double;
        using                                       string_type     = std::basic_string<char>;
        template <typename T> using                 array_type      = std::vector<T>;
        template <typename K, typename V> using     object_type     = std::map<K, V>;
    };

}}

namespace cxon { namespace json { // context parameters

    CXON_PARAMETER(recursion_guard, unsigned);    // read/write
    CXON_PARAMETER(recursion_depth, unsigned);    // read/write: constexpr
        
}}  // cxon::json contexts

namespace cxon {

    using json::basic_node;

    template <typename X = JSON<>, typename Tr, typename InIt, typename ...CxPs>
        inline auto     from_bytes(basic_node<Tr>& t, InIt b, InIt e, CxPs... p)      -> from_bytes_result<InIt>;
    template <typename X = JSON<>, typename Tr, typename Iterable, typename ...CxPs>
        inline auto     from_bytes(basic_node<Tr>& t, const Iterable& i, CxPs... p)   -> from_bytes_result<decltype(std::begin(i))>;

    template <typename X = JSON<>, typename Tr, typename OutIt, typename ...CxPs>
        inline auto     to_bytes(OutIt o, const basic_node<Tr>& t, CxPs... p)         -> enable_if_t<is_output_iterator<OutIt>::value, to_bytes_result<OutIt>>;
    template <typename X = JSON<>, typename Tr, typename Insertable, typename ...CxPs>
        inline auto     to_bytes(Insertable& i, const basic_node<Tr>& t, CxPs... p)   -> enable_if_t<is_back_insertable<Insertable>::value, to_bytes_result<decltype(std::begin(i))>>;
    template <typename X = JSON<>, typename Tr, typename FwIt, typename ...CxPs>
        inline auto     to_bytes(FwIt b, FwIt e, const basic_node<Tr>& t, CxPs... p)  -> to_bytes_result<FwIt>;

}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace json { // node

    namespace bits {

        template <typename N, typename T>
            struct node_kind_t;
        template <typename N> struct node_kind_t<N, typename N::null>     { static constexpr node_kind value = node_kind::null; };
        template <typename N> struct node_kind_t<N, typename N::boolean>  { static constexpr node_kind value = node_kind::boolean; };
        template <typename N> struct node_kind_t<N, typename N::number>   { static constexpr node_kind value = node_kind::number; };
        template <typename N> struct node_kind_t<N, typename N::string>   { static constexpr node_kind value = node_kind::string; };
        template <typename N> struct node_kind_t<N, typename N::array>    { static constexpr node_kind value = node_kind::array; };
        template <typename N> struct node_kind_t<N, typename N::object>   { static constexpr node_kind value = node_kind::object; };

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
            using null      = typename Tr::null_type;
            using boolean   = typename Tr::boolean_type;
            using number    = typename Tr::number_type;
            using string    = typename Tr::string_type;
            using array     = typename Tr::template array_type<basic_node>;
            using object    = typename Tr::template object_type<string, basic_node>;

#           ifdef _MSC_VER // std::map move copy/assign are not noexcept, force
                template <template <typename C> class X, bool = false>
                    struct msvc_map_override            : bits::is_nothrow_x<X, object, array, string, number, boolean, null> {};
                template <template <typename C> class X>
                    struct msvc_map_override<X, true>   : bits::is_nothrow_x<X, /*object, */array, string, number, boolean, null> {};
                using is_nothrow_move_constructible = msvc_map_override<std::is_nothrow_move_constructible, std::is_same<object, std::map<string, basic_node>>::value>;
                using is_nothrow_move_assignable    = msvc_map_override<std::is_nothrow_move_assignable,    std::is_same<object, std::map<string, basic_node>>::value>;
#           else
                using is_nothrow_move_constructible = bits::is_nothrow_x<std::is_nothrow_move_constructible, object, array, string, number, boolean, null>;
                using is_nothrow_move_assignable    = bits::is_nothrow_x<std::is_nothrow_move_assignable, object, array, string, number, boolean, null>;
#           endif

            basic_node() noexcept : kind_(node_kind::null)  {}
            ~basic_node()                                   { reset(); }

            basic_node(basic_node&& o) noexcept(is_nothrow_move_constructible::value) : kind_(o.kind_) {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: new (&reinterpret_cast<T&>(value_)) T(std::move(o.get<T>())); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(number);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
            }
            basic_node& operator =(basic_node&& o) noexcept(is_nothrow_move_assignable::value) {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: get<T>() = std::move(o.get<T>()); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(number);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
                return *this;
            }

            basic_node(const basic_node& o) : kind_(o.kind_) {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: new (&reinterpret_cast<T&>(value_)) T(o.get<T>()); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(number);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
            }
            basic_node& operator =(const basic_node& o) {
                switch (o.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: imbue<T>() = o.get<T>(); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(number);
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
                CXON_JSON_TYPE_DEF(number)
                CXON_JSON_TYPE_DEF(boolean)
                CXON_JSON_TYPE_DEF(null)
#           undef CXON_JSON_TYPE_DEF
                basic_node(int n) : kind_(node_kind::null)          { imbue<number>() = n; }
                basic_node& operator =(int n)                       { imbue<number>() = n; return *this; }
                basic_node(const char* s) : kind_(node_kind::null)  { imbue<string>() = s; }
                basic_node& operator =(const char* s)               { imbue<string>() = s; return *this; }

            void reset() {
                switch (kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: reinterpret_cast<T&>(value_).~T(); break
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(number);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
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
                switch (n.kind_) {
#                   define CXON_JSON_TYPE_DEF(T)    case node_kind::T: return get<T>() == n.get<T>()
                        CXON_JSON_TYPE_DEF(object);
                        CXON_JSON_TYPE_DEF(array);
                        CXON_JSON_TYPE_DEF(string);
                        CXON_JSON_TYPE_DEF(number);
                        CXON_JSON_TYPE_DEF(boolean);
                        CXON_JSON_TYPE_DEF(null);
#                   undef CXON_JSON_TYPE_DEF
                }
                return false; // LCOV_EXCL_LINE
            }
            bool operator != (const basic_node& n) const {
                return !operator ==(n);
            }

        private:
            using value_type = typename std::aligned_union<0, object, array, string, number, boolean, null>::type;
            value_type  value_;
            node_kind   kind_;
        };

    template <typename T, typename N>
        inline bool is(const N& n) { return n.template is<T>(); }

    template <typename T, typename N>
        inline T& imbue(N& n) { return n.template imbue<T>(); }

    template <typename T, typename N>
        inline T& get(N& n) { return n.template get<T>(); }
    template <typename T, typename N>
        inline const T& get(const N& n) { return n.template get<T>(); }

    template <typename T, typename N>
        inline T* get_if(N& n) { return n.template get_if<T>(); }
    template <typename T, typename N>
        inline const T* get_if(const N& n) { return n.template get_if<T>(); }

}}

namespace cxon { namespace json { // errors

    struct error_category : std::error_category {
        const char* name() const noexcept override {
            return "cxon/json/node";
        }
        std::string message(int ev) const override {
            switch (static_cast<error>(ev)) {
                case error::ok:                         return "no error";
                case error::invalid:                    return "invalid json";
                case error::recursion_depth_exceeded:   return "recursion depth limit exceeded";
                default:                                return "unknown error";
            }
        }
        static const error_category& value() {
            static error_category const v{};
            return v;
        }
    };

    std::error_condition make_error_condition(error e) noexcept {
        return { static_cast<int>(e), error_category::value() };
    }

}}

namespace std { // cxon/json errors
    template <> struct is_error_condition_enum<cxon::json::error> : true_type {};
}

namespace cxon { namespace json { namespace bits {

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

    using json::basic_node;
    using json::error;
    using json::recursion_guard;

    template <typename X, typename Tr, typename II, typename ...CxPs>
        inline auto from_bytes(basic_node<Tr>& t, II b, II e, CxPs... p) -> from_bytes_result<II> {
            return interface::from_bytes<X>(t, b, e, recursion_guard::set(0), std::forward<CxPs>(p)...);
        }
    template <typename X, typename Tr, typename I, typename ...CxPs>
        inline auto from_bytes(basic_node<Tr>& t, const I& i, CxPs... p) -> from_bytes_result<decltype(std::begin(i))> {
            return interface::from_bytes<X>(t, i, recursion_guard::set(0), std::forward<CxPs>(p)...);
        }

    template <typename X, typename Tr, typename OI, typename ...CxPs>
        inline auto to_bytes(OI o, const basic_node<Tr>& t, CxPs... p) -> enable_if_t<is_output_iterator<OI>::value, to_bytes_result<OI>> {
            return interface::to_bytes<X>(o, t, recursion_guard::set(0), std::forward<CxPs>(p)...);
        }
    template <typename X, typename Tr, typename I, typename ...CxPs>
        inline auto to_bytes(I& i, const basic_node<Tr>& t, CxPs... p) -> enable_if_t<is_back_insertable<I>::value, to_bytes_result<decltype(std::begin(i))>> {
            return interface::to_bytes<X>(i, t, recursion_guard::set(0), std::forward<CxPs>(p)...);
        }
    template <typename X, typename Tr, typename FI, typename ...CxPs>
        inline auto to_bytes(FI b, FI e, const basic_node<Tr>& t, CxPs... p) -> to_bytes_result<FI> {
            return interface::to_bytes<X>(b, e, t, recursion_guard::set(0), std::forward<CxPs>(p)...);
        }

#   define CXON_JSON_NODE_RG()\
        json::bits::scinc<Cx> RG__(cx);\
        if (!RG__.check()) return cx|error::recursion_depth_exceeded, false

        template <typename X, typename Tr>
            struct read<X, basic_node<Tr>> {
                template <typename II, typename Cx>
                    static bool value(basic_node<Tr>& t, II& i, II e, Cx& cx) {
                        chio::consume<X>(i, e);
                        switch (chio::peek(i, e)) {
#                           define CXON_READ(T) read_value<X>(t.template imbue<typename basic_node<Tr>::T>(), i, e, cx)
                                case '{'                : { CXON_JSON_NODE_RG();    return CXON_READ(object); }
                                case '['                : { CXON_JSON_NODE_RG();    return CXON_READ(array);  }
                                case '\"'               :                           return CXON_READ(string);
                                case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
                                                        :                           return CXON_READ(number);
                                case 't': case 'f'      :                           return CXON_READ(boolean);
                                case 'n'                :                           return CXON_READ(null);
#                           undef CXON_READ
                        }
                        return cx|error::invalid, false;
                    }
            };

        template <typename X, typename Tr>
            struct write<X, basic_node<Tr>> {
                template <typename O, typename Cx>
                    static bool value(O& o, const basic_node<Tr>& t, Cx& cx) {
                        using json::node_kind;
                        switch (t.kind()) {
#                           define CXON_WRITE(T) write_value<X>(o, t.template get<typename basic_node<Tr>::T>(), cx)
                                case node_kind::object  : { CXON_JSON_NODE_RG();    return CXON_WRITE(object); }
                                case node_kind::array   : { CXON_JSON_NODE_RG();    return CXON_WRITE(array); }
                                case node_kind::string  :                           return CXON_WRITE(string);
                                case node_kind::number  :                           return CXON_WRITE(number);
                                case node_kind::boolean :                           return CXON_WRITE(boolean);
                                case node_kind::null    :                           return CXON_WRITE(null);
#                           undef CXON_WRITE
                        }
                        return false; // LCOV_EXCL_LINE
                    }
            };

#   undef CXON_JSON_NODE_RG

}

#endif // CXON_JSON_NODE_HXX_
