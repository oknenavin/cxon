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

#ifndef CXON_JSON_ERROR_HXX_
#define CXON_JSON_ERROR_HXX_

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace json {

    enum class read_error : int {
        ok,                     // no error
        unexpected,             // unexpected input
        character_invalid,      // character invalid
        integral_invalid,       // integral invalid or out of range
        floating_point_invalid, // floating-point invalid
        boolean_invalid,        // boolean invalid
        escape_invalid,         // invalid escape sequence
        surrogate_invalid,      // invalid surrogate
        overflow                // buffer overflow
    };

    enum class write_error : int {
        ok,                     // no error
        output_failure,         // output cannot be written
        argument_invalid        // argument invalid
    };

    struct read_error_category : std::error_category {
        const char* name() const noexcept override {
            return "cxon/json/read";
        }
        std::string message(int ev) const override {
            switch (static_cast<read_error>(ev)) {
                case read_error::ok:                        return "no error";
                case read_error::unexpected:                return "unexpected input";
                case read_error::character_invalid:         return "invalid character";
                case read_error::integral_invalid:          return "invalid integral or value out of range";
                case read_error::floating_point_invalid:    return "invalid floating point";
                case read_error::boolean_invalid:           return "invalid boolean";
                case read_error::escape_invalid:            return "invalid escape sequence";
                case read_error::surrogate_invalid:         return "invalid surrogate";
                case read_error::overflow:                  return "buffer overflow";
                default:                                    return "unknown error";
            }
        }
        static const read_error_category& value() {
            static read_error_category const v{};
            return v;
        }
    };

    inline std::error_condition make_error_condition(read_error e) noexcept {
        return { static_cast<int>(e), read_error_category::value() };
    }

    struct write_error_category : std::error_category {
        const char* name() const noexcept override {
            return "cxon/json/write";
        }
        std::string message(int ev) const override {
            switch (static_cast<write_error>(ev)) {
                case write_error::ok:               return "no error";
                case write_error::output_failure:   return "output cannot be written";
                case write_error::argument_invalid: return "invalid argument";
                default:                            return "unknown error";
            }
        }
        static const write_error_category& value() {
            static write_error_category const v{};
            return v;
        }
    };

    inline std::error_condition make_error_condition(write_error e) noexcept {
        return { static_cast<int>(e), write_error_category::value() };
    }

}}

namespace std { // cxon errors
    template <> struct is_error_condition_enum<cxon::json::read_error> : true_type {};
    template <> struct is_error_condition_enum<cxon::json::write_error> : true_type {};
}

#endif // CXON_JSON_ERROR_HXX_
