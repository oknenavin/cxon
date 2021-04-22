// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CONTAINER_HXX_
#define CXON_CONTAINER_HXX_

//#include "cxon/utility.hxx"
#include <memory>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { // container mutation

    template <typename T>
        struct container_traits;

    template <typename C>
        struct container_traits;
        //  static bool reserve(C& c, size_t s);
        //  static auto emplace(C& c) -> typename C::reference;
        //  template <typename II>
        //      static bool append(C& c, II f, II l);

    template <typename C>
        inline bool container_reserve(C& c, size_t s);
    template <typename C>
        inline auto container_emplace(C& c) -> typename C::reference;
    template <typename C, typename II>
        inline bool container_append(C& c, II f, II l);
        
    template <typename C>
        inline auto adaptor_container(      C& c) ->       typename C::container_type&;
    template <typename C>
        inline auto adaptor_container(const C& c) -> const typename C::container_type&;

}

namespace cxon { // container element read/write

    template <typename X, typename C>
        struct element_reader {
            template <typename II, typename Cx>
                static bool read(C& c, II& i, II e, Cx& cx);
        };
    template <typename X, typename C, typename II, typename Cx>
        inline bool element_read(C& c, II& i, II e, Cx& cx);

    template <typename X, typename C>
        struct element_writer {
            template <typename O, typename E, typename Cx>
                static bool write(O& o, const E& e, Cx& cx);
        };
    template <typename X, typename C, typename E, typename O, typename Cx>
        inline bool element_write(O& o, const E& e, Cx& cx);

}

// implementation //////////////////////////////////////////////////////////////

namespace cxon {

    namespace imp {

        template <typename C>
            inline auto reserve_(option<2>, C& c, size_t s)
                -> enable_if_t<std::is_same<decltype(container_traits<C>::reserve(c, s)), bool>::value, bool>
            {
                return container_traits<C>::reserve(c, s);
            }
        template <typename C>
            inline auto reserve_(option<1>, C& c, size_t s)
                -> decltype(c.reserve(s), bool())
            {
                return c.reserve(s), true;
            }
        template <typename C>
            inline constexpr bool reserve_(option<0>, C&, size_t) {
                return true;
            }

    }
    template <typename C>
        inline bool container_reserve(C& c, size_t s) {
            return imp::reserve_(option<1>(), c, s);
        }


    namespace imp {

        template <typename C>
            inline auto emplace_(option<3>, C& c)
                -> enable_if_t<std::is_same<decltype(container_traits<C>::emplace(c)), typename C::reference>::value, typename C::reference>
            {
                return container_traits<C>::emplace(c);
            }
        template <typename C>
            inline auto emplace_(option<2>, C& c)
                -> enable_if_t<has_emplace_back<C>::value, typename C::reference>
            {
                return c.emplace_back();
            }
        template <typename C>
            inline auto emplace_(option<1>, C& c)
                -> enable_if_t<has_emplace_back_void<C>::value && has_back<C>::value, typename C::reference>
            {
                return c.emplace_back(), c.back();
            }
        template <typename C>
            inline auto emplace_(option<0>, C& c)
                -> enable_if_t<has_push_back<C>::value && has_back<C>::value, typename C::reference>
            {
                return c.push_back({}), c.back();
            }

    }
    template <typename C>
        inline auto container_emplace(C& c) -> typename C::reference {
            return imp::emplace_(option<3>(), c);
        }

    namespace imp {

        template <typename C, typename II>
            inline auto append_(option<1>, C& c, II f, II l)
                -> enable_if_t<std::is_same<decltype(container_traits<C>::append(c, f, l)), bool>::value, bool>
            {
                return container_traits<C>::append(c, f, l);
            }
        template <typename C, typename II>
            inline auto append_(option<0>, C& c, II f, II l)
                -> enable_if_t<std::is_same<decltype(container_emplace(c)), typename C::reference>::value, bool>
            {
                for ( ; f != l; ++f)
                    container_emplace(c) = *f;
                return f == l;
            }

    }
    template <typename C, typename II>
        inline bool container_append(C& c, II f, II l) {
            return imp::append_(option<1>(), c, f, l);
        }

    

    namespace imp {

        template <typename A>
            struct adaptor : A {
                static const typename A::container_type& container(const A& a) noexcept { return ((adaptor&)a).c; }
                static       typename A::container_type& container(      A& a) noexcept { return ((adaptor&)a).c; }
            };

    }
    template <typename C>
        inline auto adaptor_container(      C& c) ->       typename C::container_type& {
            return imp::adaptor<C>::container(c);
        }
    template <typename C>
        inline auto adaptor_container(const C& c) -> const typename C::container_type& {
            return imp::adaptor<C>::container(c);
        }

}

namespace cxon { // container element read/write

    template <typename X, typename C>
        template <typename II, typename Cx>
            bool element_reader<X, C>::read(C& c, II& i, II e, Cx& cx) {
                return read_value<X>(container_emplace(c), i, e, cx);
            }
    template <typename X, typename C, typename II, typename Cx>
        inline bool element_read(C& c, II& i, II e, Cx& cx) {
            return element_reader<X, C>::read(c, i, e, cx);
        }

    template <typename X, typename C>
        template <typename O, typename E, typename Cx>
            bool element_writer<X, C>::write(O& o, const E& e, Cx& cx) {
                return write_value<X>(o, e, cx);
            }
    template <typename X, typename C, typename E, typename O, typename Cx>
        inline bool element_write(O& o, const E& e, Cx& cx) {
            return element_writer<X, C>::write(o, e, cx);
        }

}

#endif // CXON_CONTAINER_HXX_
