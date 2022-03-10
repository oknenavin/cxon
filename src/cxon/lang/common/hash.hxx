// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_HASH_HXX_
#define CXON_HASH_HXX_

#include <functional>
#include <iterator>
#include <utility>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon {

    template <typename T>
        struct hash;

    template <typename ...T>
        inline std::size_t make_hash(const T&... t) noexcept;
    template <typename ...T>
        inline std::size_t make_hash(std::size_t s, const T&... t) noexcept;

}

// implementation //////////////////////////////////////////////////////////////

namespace cxon {

    template <typename T>
        struct hash {
            std::size_t operator ()(const T& t) const noexcept {
                return h_(t);
            }
            private: std::hash<T> h_;
        };

    template <> // g++-8: error: use of deleted function ‘std::hash<std::nullptr_t>::hash()’
        struct hash<std::nullptr_t> {
            std::size_t operator ()(std::nullptr_t) const noexcept {
                return make_hash(0);
            }
        };

    template <typename K, typename V>
        struct hash<std::pair<K, V>> {
            std::size_t operator ()(const std::pair<K, V>& t) const noexcept {
                return make_hash(t.first, t.second);
            }
        };

    namespace imp {

        template <typename T, typename E = void_t<>>
            struct is_iterable_                                         : std::false_type {};
        template <typename T>
            struct is_iterable_<T, void_t<typename T::const_iterator>>  : std::true_type {};

        template <typename T>
            inline auto make_hash_(const T& t) noexcept -> enable_if_t< is_iterable_<T>::value, std::size_t> {
                hash<typename T::value_type> hs;
                std::size_t s = 0;
                for (auto& v: t)
                    s = make_hash(s, hs(v));
                return s;
            }
        template <typename T>
            inline auto make_hash_(const T& t) noexcept -> enable_if_t<!is_iterable_<T>::value, std::size_t> {
                return hash<T>()(t);
            }

        template <typename ...>
            constexpr std::size_t make_hash_(std::size_t s) noexcept {
                return s;
            }
        template <typename H, typename ...T>
            inline std::size_t make_hash_(std::size_t s, const H& h, const T&... t) noexcept {
                s ^= make_hash_(h) + 0x9e3779b9 + (s << 6) + (s >> 2);
                return make_hash_(s, t...);
            }

    }

    template <typename ...T>
        inline std::size_t make_hash(const T&... t) noexcept {
            return imp::make_hash_(0, t...);
        }

    template <typename ...T>
        inline std::size_t make_hash(std::size_t s, const T&... t) noexcept {
            return imp::make_hash_(s, t...);
        }

}

#endif // CXON_HASH_HXX_
