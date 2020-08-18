// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHARIO_ENUMS_HXX_
#define CXON_CHARIO_ENUMS_HXX_

#include "chario.hxx"
#include "unquoted-value.hxx"

namespace cxon { namespace enums { // enum reader/writer construction helpers

    template <typename E>
        struct value {
            char const*const name;
            E const value;
        };
    template <typename E>
        constexpr value<E> make_value(const char* name, E value) {
            return { name, value };
        }

    namespace bits {

        template <typename X>
            struct read {
                template <size_t N, typename II, typename Cx>
                    static bool value(char (&t)[N], II& i, II e, Cx& cx) {
                        return unquoted::read_value<X>(t, i, e, cx);
                    }
            };
        template <typename X>
            struct read<JSON<X>> {
                template <size_t N, typename II, typename Cx>
                    static bool value(char (&t)[N], II& i, II e, Cx& cx) {
                        return cxon::read_value<JSON<X>>(t, i, e, cx);
                    }
            };

        template <typename X>
            struct opqt {
                static constexpr std::nullptr_t const beg = nullptr;
                static constexpr std::nullptr_t const end = nullptr;
            };
        template <typename X>
            struct opqt<JSON<X>> : JSON<X>::string {};

    }

    template <typename X, typename E, typename V, typename II, typename Cx>
        inline bool read_value(E& t, V vb, V ve, II& i, II e, Cx& cx) {
            io::consume<X>(i, e);
            II const o = i;
                char id[ids_len_max::constant<prms_type<Cx>>(64)];
                    if (!bits::read<X>::value(id, i, e, cx)) return false;
                for ( ; vb != ve; ++vb) if (std::strcmp(vb->name, id) == 0)
                    return t = vb->value, true;
            return io::rewind(i, o), cx|read_error::unexpected;
        }

    template <typename X, typename E, typename V, typename O, typename Cx>
        inline bool write_value(O& o, E t, V vb, V ve, Cx& cx) {
            for ( ; vb != ve; ++vb) if (t == vb->value)
                return io::poke<X>(o, bits::opqt<X>::beg, cx) && io::poke<X>(o, vb->name, cx) && io::poke<X>(o, bits::opqt<X>::end, cx);
            return cx|write_error::argument_invalid;
        }

}}  // cxon::enums enum reader/writer construction helpers

#if 1 // cxon enumeration

#   define CXON_ENUM_VALUE(T, N, V)     enums::make_value(N, T::V)
#   define CXON_ENUM_VALUE_NAME(N, V)   CXON_ENUM_VALUE(T, N, V)
#   define CXON_ENUM_VALUE_ASIS(V)      CXON_ENUM_VALUE(T, #V, V)

#   define CXON_ENUM_READ(Type, ...)\
        namespace cxon {\
            template <typename X, typename II, typename Cx>\
                inline bool read_value(Type& t, II& i, II e, Cx& cx) {\
                    using T = Type;\
                    static constexpr enums::value<Type> v[] = { __VA_ARGS__ };\
                    return enums::read_value<X>(t, std::begin(v), std::end(v), i, e, cx);\
                }\
        }
#   define CXON_ENUM_WRITE(Type, ...)\
        namespace cxon {\
            template <typename X, typename O, typename Cx>\
                inline bool write_value(O& o, const Type& t, Cx& cx) {\
                    using T = Type;\
                    static constexpr enums::value<Type> v[] = { __VA_ARGS__ };\
                    return enums::write_value<X>(o, t, std::begin(v), std::end(v), cx);\
                }\
        }
#   define CXON_ENUM(Type, ...)\
        CXON_ENUM_READ(Type, __VA_ARGS__)\
        CXON_ENUM_WRITE(Type, __VA_ARGS__)

#endif // enumeration

#endif // CXON_CHARIO_ENUMS_HXX_
