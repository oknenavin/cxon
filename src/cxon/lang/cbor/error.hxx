// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_ERROR_HXX_
#define CXON_CBOR_ERROR_HXX_

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor {

    enum class read_error : int {
        ok,                     // no error
        unexpected,             // unexpected input
        boolean_invalid,        // boolean invalid
        null_invalid,           // null invalid
        integer_invalid,        // integer invalid
        floating_point_invalid, // integer invalid
        size_invalid,           // size invalid
        array_invalid,          // array invalid
        tag_invalid             // tag invalid
    };

    enum class write_error : int {
        ok,                     // no error
        output_failure,         // output cannot be written
        argument_invalid        // argument invalid
    };

    struct read_error_category : std::error_category {
        const char* name() const noexcept override {
            return "cxon/cbor/read";
        }
        std::string message(int ev) const override {
            switch (static_cast<read_error>(ev)) {
                case read_error::ok:                        return "no error";
                case read_error::unexpected:                return "unexpected input";
                case read_error::boolean_invalid:           return "invalid boolean";
                case read_error::null_invalid:              return "invalid null";
                case read_error::integer_invalid:           return "invalid integer";
                case read_error::floating_point_invalid:    return "invalid floating-point";
                case read_error::size_invalid:              return "invalid size";
                case read_error::array_invalid:             return "invalid array";
                case read_error::tag_invalid:               return "invalid tag";
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
            return "cxon/cbor/write";
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
    template <> struct is_error_condition_enum<cxon::cbor::read_error> : true_type {};
    template <> struct is_error_condition_enum<cxon::cbor::write_error> : true_type {};
}

#endif // CXON_CBOR_ERROR_HXX_
