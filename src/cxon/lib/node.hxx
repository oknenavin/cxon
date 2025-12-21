// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CXON_LIB_NODE_HXX_
#define CXON_LIB_NODE_HXX_

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace node { // errors

    enum class error : int {
        ok,                         // no error
        invalid,                    // invalid input
        recursion_depth_exceeded    // recursion depth exceeded
    };

    struct error_category;
    inline std::error_condition make_error_condition(error e) noexcept;

}}

namespace cxon { namespace node { // context parameters

    CXON_PARAMETER(recursion_guard, unsigned);  // read/write
    CXON_PARAMETER(recursion_depth, unsigned);  // read/write: constexpr

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace node { // errors

    struct error_category : std::error_category {
        const char* name() const noexcept override {
            return "cxon/node";
        }
        std::string message(int ev) const override {
            switch (static_cast<error>(ev)) {
                case error::ok:                         return "no error";
                case error::invalid:                    return "invalid input";
                case error::recursion_depth_exceeded:   return "recursion depth limit exceeded";
                default:                                return "unknown error";
            }
        }
        static const error_category& value() {
            static error_category const v{};
            return v;
        }
    };

    inline std::error_condition make_error_condition(error e) noexcept {
        return { static_cast<int>(e), error_category::value() };
    }

}}

namespace std { // cxon/node errors
    template <> struct is_error_condition_enum<cxon::node::error> : true_type {};
}

namespace cxon { namespace node { namespace imp {

    template <typename Cx, bool G = recursion_guard::in<napa_type<Cx>>::value>
        struct scinc_ {
            Cx& cx;
            scinc_(Cx& cx) noexcept : cx(cx)    { ++recursion_guard::reference(cx.px); }
            ~scinc_() noexcept                  { --recursion_guard::reference(cx.px); }
            bool check() const noexcept         { return recursion_guard::value(cx.px) < recursion_depth::constant<napa_type<Cx>>(64); }
        };
    template <typename Cx>
        struct scinc_<Cx, false> {
            scinc_(Cx&) noexcept                {}
            ~scinc_() noexcept                  {}
            bool check() const noexcept         { return true; }
        };

}}}

#ifdef CXON_JSON_DEFINED
#   include "node.json.hxx"
#endif

#endif // CXON_LIB_NODE_HXX_
