// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_LIB_STD_VECTOR_HXX_
#   define CXON_LIB_STD_VECTOR_HXX_

#   include <vector>

#   ifdef CXON_JSON_DEFINED
#       include "cxon/lang/json/lib/std/vector.hxx"
#   endif
#   ifdef CXON_CBOR_DEFINED
#       include "cxon/lang/cbor/lib/std/vector.hxx"
#   endif

namespace cxon { namespace cnt {

    template <typename T, typename ...R>
        struct traits<std::vector<T, R...>> {
            template <typename II>
                static bool append(std::vector<T, R...>& c, II f, II l) {
                    return c.insert(c.end(), f, l), true;
                }
        };

    template <typename X, typename ...A>
        struct element_reader<X, std::vector<bool, A...>> {
            template <typename II, typename Cx>
                static bool read(std::vector<bool, A...>& t, II& i, II e, Cx& cx) {
                    bool b; return read_value<X>(b, i, e, cx) && (t.push_back(b), true);
                }
        };
    template <typename X, typename ...A>
        struct element_writer<X, std::vector<bool, A...>> {
            template <typename O, typename Cx>
                static bool write(O& o, bool e, Cx& cx) {
                    return write_value<X>(o, e, cx);
                }
        };

}}

#endif // CXON_LIB_STD_VECTOR_HXX_
