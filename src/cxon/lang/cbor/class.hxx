// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_CLASS_HXX_
#define CXON_CBOR_CLASS_HXX_

#include <tuple>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cls {

    template <typename O = void>    struct sink;
    template <typename T>           struct is_sink;

    template <typename X, typename S>
        struct sink_reader;

    template <typename X, typename T, typename II, typename Cx>
        inline bool sink_read(sink<T>& s, II& i, II e, Cx& cx);

}}}

namespace cxon { namespace cbor { namespace cls {

    template <typename F>
        struct field;
    template <typename F = sink<>>
        constexpr field<F> make_field(const char* name, F f = {});

    template <typename X, typename S, typename F, typename II, typename Cx>
        inline bool read_field(S& s, F f, II& i, II e, Cx& cx);
    template <typename X, typename O, typename S, typename F, typename Cx>
        inline bool write_field(O& o, const S& s, F f, Cx& cx);

    template <typename ...F>
        using fields = std::tuple<F...>;
    template <typename ...F> 
        constexpr fields<F...> make_fields(F... f);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cls {

    template <typename O>
        struct sink {
            O value;
            using value_type = typename O::value_type;
            void push_back(value_type v) const { value.push_back(v); }
        };
    template <>
        struct sink<void> {
            template <typename T>
                constexpr void push_back(T) const {}
        };

    template <typename T>
        struct is_sink          : std::false_type {};
    template <typename T>
        struct is_sink<sink<T>> : std::true_type {};

    template <typename X, typename T, typename II, typename Cx>
        inline bool sink_read(sink<T>& s, II& i, II e, Cx& cx) {
            return sink_reader<X>::read(s, i, e, cx);
        }

}}}

namespace cxon { namespace cbor { namespace cls {

    // field

    template <typename F>
        struct field {
            using type = F;
            char const*const name;
            type value;
        };

    template <typename O>
        struct field<sink<O>> {
            using type = sink<O>;
            char const*const name;
            type const value;
        };

    template <typename F>
        constexpr field<F> make_field(const char* name, F f) {
            return { name, f };
        }

    namespace bits {

        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field_(S& s, F f, II& i, II e, Cx& cx)
                -> enable_if_t<!is_sink<typename F::type>::value &&  std::is_member_pointer<typename F::type>::value, bool>
            {
                return read_value<X>(s.*f.value, i, e, cx);
            }
        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field_(S&, F f, II& i, II e, Cx& cx)
                -> enable_if_t<is_sink<typename F::type>::value && !std::is_member_pointer<typename F::type>::value, bool>
            {
                return read_value<X>(*f.value, i, e, cx);
            }

        template <typename X, typename O, typename S, typename F, typename Cx>
            inline auto write_field_(O& o, const S& s, F f, Cx& cx)
                -> enable_if_t<is_sink<typename F::type>::value &&  std::is_member_pointer<typename F::type>::value, bool>
            {
                return write_key<X>(o, f.name, cx) && write_value<X>(o, s.*f.value, cx);
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            inline auto write_field_(O& o, const S&, F f, Cx& cx)
                -> enable_if_t<is_sink<typename F::type>::value && !std::is_member_pointer<typename F::type>::value, bool>
            {
                return write_key<X>(o, f.name, cx) && write_value<X>(o, *f.value, cx);
            }

        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field_(S&, F f, II& i, II e, Cx& cx)
                -> enable_if_t< is_sink<typename F::type>::value, bool>
            {
                return sink_read<X>(f.value, i, e, cx);
            }

        template <typename X, typename O, typename S, typename F, typename Cx>
            constexpr auto write_field_(O&, const S&, F, Cx&)
                -> enable_if_t< is_sink<typename F::type>::value, bool>
            {
                return true;
            }

    }
    template <typename X, typename S, typename F, typename II, typename Cx>
        inline bool read_field(S& s, F f, II& i, II e, Cx& cx) {
            return bits::read_field_<X>(s, f, i, e, cx);
        }
    template <typename X, typename O, typename S, typename F, typename Cx>
        inline bool write_field(O& o, const S& s, F f, Cx& cx) {
            return bits::write_field_<X>(o, s, f, cx);
        }

    // fields

    template <typename ...F> 
        constexpr fields<F...> make_fields(F... f) {
            return { f... };
        }

}}}

#endif // CXON_CBOR_CLASS_HXX_
