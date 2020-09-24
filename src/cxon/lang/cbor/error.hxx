// Copyright (c) 2017-2020 oknenavin.
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
        u8_invalid,             // uint8_t invalid
        u16_invalid,            // uint16_t invalid
        u32_invalid,            // uint32_t invalid
        u64_invalid,            // uint64_t invalid
        u_invalid               // unsigned invalid
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
                case read_error::u8_invalid:                return "invalid 8-bit unsigned integer";
                case read_error::u16_invalid:               return "invalid 16-bit unsigned integer";
                case read_error::u32_invalid:               return "invalid 32-bit unsigned integer";
                case read_error::u64_invalid:               return "invalid 64-bit unsigned integer";
                case read_error::u_invalid:                 return "invalid unsigned integer";
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
