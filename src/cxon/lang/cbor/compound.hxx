// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMPOUND_HXX_
#define CXON_CBOR_COMPOUND_HXX_

#include "cxon/lang/cbor/common/container.hxx"
#include "cxon/lang/common/allocator.hxx"
#include <cstring>

namespace cxon { // array/read

    template <typename X, typename T, size_t N>
        struct read<CBOR<X>, T[N]> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(T (&t)[N], II& i, II e, Cx& cx) {
                    size_t tag;
                    return  cbor::tag::read<Y>(tag, i, e, cx) &&
                            cbor::cnt::read_array<Y>(std::begin(t), std::end(t), tag, i, e, cx)
                    ;
                }
        };

}

namespace cxon { // array/write

    template <typename X, typename T, size_t N>
        struct write<CBOR<X>, T[N]> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    return cbor::cnt::write_array<J>(o, std::begin(t), std::end(t), cx);
                }
        };

}

namespace cxon { // pointer/read

    namespace cbor { namespace bits {

        template <typename T, typename A>
            struct pointer_container {
                using value_type = T;
                using pointer = value_type*;
                using reference = value_type&;

                // coverity[assign_zero]
                // coverity[var_deref_model]
                pointer_container(const A& a) : a_(a), f_(), l_(), e_() { grow(8); }

                pointer release() { return f_; }

                size_t size() const     { return std::distance(f_, e_); }
                size_t max_size() const { return std::numeric_limits<size_t>::max(); }

                pointer begin()  { return f_; }
                pointer end()    { return e_; }

                reference emplace_back()            { return grow(), *e_++; }
                void push_back(const value_type& t) { grow(), *e_ = t, ++e_; }
                void push_back(value_type&& t)      { grow(), *e_ = std::move(t), ++e_; }

                void reserve(size_t n)              { n > size_t(l_ - f_) ? grow(n) : void(); }

                private:
                    void grow()                     { e_ == l_ ? grow((l_ - f_) * 2) : void(); }
                    
                    void grow(size_t n) {
                        CXON_ASSERT(n > size_t(l_ - f_), "unexpected");
                        auto const p = a_.create(n);
                            // coverity[deref_parm_in_call] - f_ == e_ in this case
                            std::move(f_, e_, p);
                            a_.release(f_, l_ - f_);
                        e_ = p + (e_ - f_), l_ = p + n, f_ = p;
                    }

                private:
                    A a_;
                    pointer f_, l_, e_;
            };
        template <typename X, typename T, typename Cx>
            auto make_pointer_container(Cx& cx) -> pointer_container<T, decltype(make_context_allocator<X, T>(cx))> {
                return { make_context_allocator<X, T>(cx) };
            }

    }}

    namespace cbor { namespace bits {

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_pointer_t_(T*& t, II& i, II e, Cx& cx) {
                if (bio::peek(i, e) == X::nil)
                    return bio::get(i, e), t = nullptr, true;
                auto al = make_context_allocator<X, T>(cx);
                T *const v = al.create();
                    if (!v || !read_value<X>(*v, i, e, cx))
                        return al.release(v), false;
                return t = v, true;
            }

        template <typename X, typename T, typename II, typename Cx>
            static auto read_pointer_(T*& t, II& i, II e, Cx& cx)
                -> enable_if_t< std::is_assignable<decltype(*t), decltype(*i)>::value, bool>
            {
                size_t tag;
                if (!tag::read<X>(tag, i, e, cx))
                    return false;
                switch (bio::peek(i, e) & X::mjr) {
                    case X::bstr: case X::tstr: case X::arr: {
                                auto c = make_pointer_container<X, T>(cx);
                                return cbor::cnt::read_array<X>(c, tag, i, e, cx) && (c.push_back({}), t = c.release());
                    }
                    default:    return read_pointer_t_<X>(t, i, e, cx);
                }
            }
        template <typename X, typename T, typename II, typename Cx>
            static auto read_pointer_(T*& t, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_assignable<decltype(*t), decltype(*i)>::value, bool>
            {
                size_t tag;
                if (!tag::read<X>(tag, i, e, cx))
                    return false;
                switch (bio::peek(i, e) & X::mjr) {
                    case X::arr: {
                                auto c = make_pointer_container<X, T>(cx);
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
                    return cbor::bits::read_pointer_<Y>(t, i, e, cx);
                }
        };
    template <typename X, typename T>
        struct read<CBOR<X>, const T*> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(const T*& t, II& i, II e, Cx& cx) {
                    return cbor::bits::read_pointer_<Y>((T*&)t, i, e, cx);
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
