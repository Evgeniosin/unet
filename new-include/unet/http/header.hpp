
#pragma once

#include <expected>
#include <type_traits>
#include <utility>

#include "unet/header/enum.hpp"
#include "unet/header/generic.hpp"
#include "unet/http/request.hpp"
#include "unet/http/response.hpp"

namespace usub::unet::header {

    template<> template<>
    struct Headers<usub::unet::http::Request>::Rule<ENUM::Host> {
        static constexpr bool implemented       = true;
        static constexpr bool allow_in_context  = true;
        static constexpr bool forbid_duplicates = true;
        static constexpr bool comma_split       = false;

        static std::expected<void, usub::unet::utils::UnetError>
        validate_value(std::string_view v) {
            if (v.empty())
                return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Host value is empty"});
            if (v.find(' ') != std::string_view::npos || v.find('\t') != std::string_view::npos)
                return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Host contains whitespace"});
            return {};
        }
    };

    template<> template<>
    struct Headers<usub::unet::http::Response>::Rule<ENUM::Host> {
        static constexpr bool implemented       = true;
        static constexpr bool allow_in_context  = false;
        static constexpr bool forbid_duplicates = true;
        static constexpr bool comma_split       = false;

        static std::expected<void, usub::unet::utils::UnetError>
        validate_value(std::string_view) {
            return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Host is Request-only"});
        }
    };

    template<> template<>
    struct Headers<usub::unet::http::Request>::Rule<ENUM::Content_Length> {
        static constexpr bool implemented       = true;
        static constexpr bool allow_in_context  = true;
        static constexpr bool forbid_duplicates = true;
        static constexpr bool comma_split       = false;

        static std::expected<void, usub::unet::utils::UnetError>
        validate_value(std::string_view v) {
            if (v.empty())
                return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Content-Length is empty"});
            for (char c : v) {
                if (c < '0' || c > '9')
                    return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Content-Length must be digits only"});
            }
            return {};
        }
    };

    template<> template<>
    struct Headers<usub::unet::http::Response>::Rule<ENUM::Content_Length>
        : Headers<usub::unet::http::Request>::Rule<ENUM::Content_Length> {};

    template<> template<>
    struct Headers<usub::unet::http::Request>::Rule<ENUM::Accept> {
        static constexpr bool implemented       = true;
        static constexpr bool allow_in_context  = true;
        static constexpr bool forbid_duplicates = false;
        static constexpr bool comma_split       = true;

        static std::expected<void, usub::unet::utils::UnetError>
        validate_value(std::string_view v) {
            if (v.empty())
                return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Accept token is empty"});
            return {};
        }
    };

    template<> template<>
    struct Headers<usub::unet::http::Response>::Rule<ENUM::Accept>
        : Headers<usub::unet::http::Request>::Rule<ENUM::Accept> {};

    template<> template<>
    struct Headers<usub::unet::http::Request>::Rule<ENUM::Accept_Encoding>
        : Headers<usub::unet::http::Request>::Rule<ENUM::Accept> {};

    template<> template<>
    struct Headers<usub::unet::http::Response>::Rule<ENUM::Accept_Encoding>
        : Headers<usub::unet::http::Request>::Rule<ENUM::Accept> {};

    template<> template<>
    struct Headers<usub::unet::http::Response>::Rule<ENUM::Set_Cookie> {
        static constexpr bool implemented       = true;
        static constexpr bool allow_in_context  = true;
        static constexpr bool forbid_duplicates = false;
        static constexpr bool comma_split       = false;

        static std::expected<void, usub::unet::utils::UnetError>
        validate_value(std::string_view v) {
            if (v.empty())
                return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Set-Cookie is empty"});
            return {};
        }
    };

    template<> template<>
    struct Headers<usub::unet::http::Request>::Rule<ENUM::Set_Cookie> {
        static constexpr bool implemented       = true;
        static constexpr bool allow_in_context  = false;
        static constexpr bool forbid_duplicates = false;
        static constexpr bool comma_split       = false;

        static std::expected<void, usub::unet::utils::UnetError>
        validate_value(std::string_view) {
            return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Set-Cookie is Response-only"});
        }
    };

    template<> template<>
    struct Headers<usub::unet::http::Response>::Rule<ENUM::Server> {
        static constexpr bool implemented       = true;
        static constexpr bool allow_in_context  = true;
        static constexpr bool forbid_duplicates = true;
        static constexpr bool comma_split       = false;

        static std::expected<void, usub::unet::utils::UnetError>
        validate_value(std::string_view v) {
            if (v.empty())
                return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Server is empty"});
            return {};
        }
    };

    template<> template<>
    struct Headers<usub::unet::http::Request>::Rule<ENUM::Server> {
        static constexpr bool implemented       = true;
        static constexpr bool allow_in_context  = false;
        static constexpr bool forbid_duplicates = true;
        static constexpr bool comma_split       = false;

        static std::expected<void, usub::unet::utils::UnetError>
        validate_value(std::string_view) {
            return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Server is Response-only"});
        }
    };


//TODO: uncomment this after im done with enforcing rules for every enum
    // template<typename Ctx, std::size_t... I>
    // consteval bool all_rules_exist(std::index_sequence<I...>) {
    //     return ((Headers<Ctx>::template Rule<static_cast<ENUM>(I)>::implemented) && ...);
    // }

    // static_assert(all_rules_exist<usub::unet::http::Request>(
    //                   std::make_index_sequence<std::size(header_enum_to_string_lower)>{}),
    //               "Not all header ENUM values are handled for http::Request");

    // static_assert(all_rules_exist<usub::unet::http::Response>(
    //                   std::make_index_sequence<std::size(header_enum_to_string_lower)>{}),
    //               "Not all header ENUM values are handled for http::Response");

} // namespace usub::unet::header
