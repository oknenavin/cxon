// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_MEMORY_HXX_
#   define CXON_LIB_STD_MEMORY_HXX_

#   include <memory>

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/std/memory.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/std/memory.hxx"
#   endif

#if defined(CXON_JSON_DEFINED) || defined(CXON_CBOR_DEFINED)
    namespace cxon {

        template <typename X, typename T, typename D>
            struct read<X, std::unique_ptr<T[], D>> {
                template <typename II, typename Cx>
                    static bool value(std::unique_ptr<T[], D>&, II&, II, Cx&) {
                        static_assert(unexpected<std::unique_ptr<T[], D>>(), "std::unique_ptr<T[], D> not supported");
                        return false;
                    }
            };
        template <typename X, typename T, typename D>
            struct write<X, std::unique_ptr<T[], D>> {
                template <typename O, typename Cx>
                    static bool value(O&, const std::unique_ptr<T[], D>&, Cx&) {
                        static_assert(unexpected<std::unique_ptr<T[], D>>(), "std::unique_ptr<T[], D> not supported");
                        return false;
                    }
            };

        template <typename X, typename T>
            struct read<X, std::shared_ptr<T[]>> {
                template <typename II, typename Cx>
                    static bool value(std::shared_ptr<T[]>&, II&, II, Cx&) {
                        static_assert(unexpected<std::shared_ptr<T[]>>(), "std::shared_ptr<T[]> not supported");
                        return false;
                    }
            };

        template <typename X, typename T>
            struct write<X, std::shared_ptr<T[]>> {
                template <typename O, typename Cx>
                    static bool value(O&, const std::shared_ptr<T[]>&, Cx&) {
                        static_assert(unexpected<std::shared_ptr<T[]>>(), "std::shared_ptr<T[]> not supported");
                        return false;
                    }
            };

    }
#endif

#endif // CXON_LIB_STD_MEMORY_HXX_
