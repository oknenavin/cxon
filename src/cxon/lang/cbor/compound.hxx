// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMPOUND_HXX_
#define CXON_CBOR_COMPOUND_HXX_

#include "cxon/lang/cbor/common/container.hxx"
#include "cxon/lang/common/allocator.hxx"

namespace cxon { // array/read

    template <typename X, typename T, std::size_t N>
        struct read<CBOR<X>, T[N]> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(T (&t)[N], II& i, II e, Cx& cx) {
                    typename X::tag_type tag;
                    return  cbor::tag::read<Y>(tag, i, e, cx) &&
                            cbor::cnt::read_array<Y>(std::begin(t), std::end(t), tag, i, e, cx)
                    ;
                }
        };

}

namespace cxon { // array/write

    template <typename X, typename T, std::size_t N>
        struct write<CBOR<X>, T[N]> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    return cbor::cnt::write_array<Y>(o, std::begin(t), std::end(t), cx);
                }
        };

}

namespace cxon { // pointer/read

    namespace cbor { namespace imp {

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_pointer_t_(T*& t, II& i, II e, Cx& cx) {
                if (bio::peek(i, e) == X::nil)
                    return bio::get(i, e), t = nullptr, true;
                auto al = alc::make_context_allocator<T>(cx);
                T *const v = al.create();
                    if (!v || !read_value<X>(*v, i, e, cx))
                        return al.release(v), false;
                return t = v, true;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline auto read_pointer_(T*& t, II& i, II e, Cx& cx)
                -> enable_if_t< std::is_assignable<decltype(*t), decltype(*i)>::value, bool>
            {
                typename X::tag_type tag;
                if (!tag::read<X>(tag, i, e, cx))
                    return false;
                switch (bio::peek(i, e) & X::mjr) {
                    case X::bstr: case X::tstr: case X::arr: {
                                auto c = cxon::cnt::make_pointer_container<X, T>(cx);
                                return cbor::cnt::read_array<X>(c, tag, i, e, cx) && (c.push_back({}), t = c.release());
                    }
                    default:    return read_pointer_t_<X>(t, i, e, cx);
                }
            }
        template <typename X, typename T, typename II, typename Cx>
            inline auto read_pointer_(T*& t, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_assignable<decltype(*t), decltype(*i)>::value, bool>
            {
                typename X::tag_type tag;
                if (!tag::read<X>(tag, i, e, cx))
                    return false;
                switch (bio::peek(i, e) & X::mjr) {
                    case X::arr: {
                                auto c = cxon::cnt::make_pointer_container<X, T>(cx);
                                return cbor::cnt::read_array<X>(c, tag, i, e, cx) && (c.push_back({}), t = c.release());
                    }
                    default:    return read_pointer_t_<X>(t, i, e, cx);
                }
            }

    }}

    template <typename X, typename T>
        struct read<CBOR<X>, T*> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(T*& t, II& i, II e, Cx& cx) {
                    return cbor::imp::read_pointer_<Y>(t, i, e, cx);
                }
        };
    template <typename X, typename T>
        struct read<CBOR<X>, const T*> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(const T*& t, II& i, II e, Cx& cx) {
                    return cbor::imp::read_pointer_<Y>((T*&)t, i, e, cx);
                }
        };

}

namespace cxon { // pointer/write

    template <typename X, typename T>
        struct write<CBOR<X>, T*> {
            template <typename O, typename Cx, typename U = T, typename Y = CBOR<X>>
                static auto value(O& o, const T* t, Cx& cx)
                    -> enable_if_t<!is_char<U>::value, bool>
                {
                    return t ?
                        write_value<Y>(o, *t, cx) :
                        bio::poke<Y>(o, Y::nil, cx)
                    ;
                }
            template <typename O, typename Cx, typename U = T, typename Y = CBOR<X>>
                static auto value(O& o, const T* t, Cx& cx)
                    -> enable_if_t< is_char<U>::value, bool>
                {
                    return t ?
                        cbor::cnt::write_array<Y>(o, t, t + std::char_traits<T>::length(t) + 1, cx) :
                        bio::poke<Y>(o, Y::nil, cx)
                    ;
                }
        };

}

#endif // CXON_CBOR_COMPOUND_HXX_
