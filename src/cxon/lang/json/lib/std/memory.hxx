// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_MEMORY_HXX_
#define CXON_JSON_LIB_STD_MEMORY_HXX_

#include "cxon/lang/common/allocator.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, typename D> struct is_map<std::unique_ptr<T, D>>      : is_map<T> {};
    template <typename T, typename D> struct is_list<std::unique_ptr<T, D>>     : is_list<T> {};
    template <typename T, typename D> struct is_string<std::unique_ptr<T, D>>   : is_string<T> {};
    template <typename T, typename D> struct is_number<std::unique_ptr<T, D>>   : is_number<T> {};
    template <typename T, typename D> struct is_bool<std::unique_ptr<T, D>>     : is_bool<T> {};
    template <typename T, typename D> struct is_null<std::unique_ptr<T, D>>     : std::true_type {};

    template <typename T> struct is_map<std::shared_ptr<T>>     : is_map<T> {};
    template <typename T> struct is_list<std::shared_ptr<T>>    : is_list<T> {};
    template <typename T> struct is_string<std::shared_ptr<T>>  : is_string<T> {};
    template <typename T> struct is_number<std::shared_ptr<T>>  : is_number<T> {};
    template <typename T> struct is_bool<std::shared_ptr<T>>    : is_bool<T> {};
    template <typename T> struct is_null<std::shared_ptr<T>>    : std::true_type {};
}}

namespace cxon {

    template <typename X, typename T, typename D>
        struct read<JSON<X>, std::unique_ptr<T, D>, enable_if_t<!std::is_array<T>::value>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::unique_ptr<T, D>& t, II& i, II e, Cx& cx) {
                        T* p;
                    return read_value<Y>(p, i, e, cx) && (t.reset(p), true);
                }
        };

    template <typename X, typename T, typename D>
        struct write<JSON<X>, std::unique_ptr<T, D>, enable_if_t<!std::is_array<T>::value>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::unique_ptr<T, D>& t, Cx& cx) {
                    return write_value<Y>(o, t.get(), cx);
                }
        };

    template <typename X, typename T>
        struct read<JSON<X>, std::shared_ptr<T>, enable_if_t<!std::is_array<T>::value>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::shared_ptr<T>& t, II& i, II e, Cx& cx) {
                    auto al = alc::make_context_allocator<T>(cx);
                        T* p;
#                   if __cplusplus < 201402L
                        return read_value<Y>(p, i, e, cx) && (t.reset(p, [al](T* p) mutable { al.release(p); }), true);
#                   else
                        return read_value<Y>(p, i, e, cx) && (t.reset(p, [al = std::move(al)](T* p) mutable { al.release(p); }), true);
#                   endif
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, std::shared_ptr<T>, enable_if_t<!std::is_array<T>::value>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::shared_ptr<T>& t, Cx& cx) {
                    return write_value<Y>(o, t.get(), cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_MEMORY_HXX_
