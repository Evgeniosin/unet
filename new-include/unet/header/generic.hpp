#pragma once

#include <algorithm>
#include <cctype>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "unet/header/enum.hpp"
#include "unet/utils/error.hpp"

namespace usub::unet::header {
    //     //TODO: HAVE SPECIALISATION FOR ALL HEADERS, SO THAT ASSERTS ARE NOT SILENCED!!! then change to the forward declaration of struct Rule
    //     template<typename T, ENUM E>
    // struct Rule {
    //     static constexpr bool allow_in_context  = true;
    //     static constexpr bool forbid_duplicates = false;
    //     static constexpr bool comma_split       = false;

    //     static std::expected<void, usub::unet::utils::UnetError>
    //     validate_value(std::string_view) {
    //         return {};
    //     }
    // };
    // template<typename T, ENUM E>
    // struct Rule;

    struct Generic;

    struct Header {
        std::string key;
        std::string value;
    };

    namespace helpers {
        inline void ltrim_inplace(std::string &s) {
            auto it = std::find_if_not(s.begin(), s.end(),
                                       [](unsigned char c) { return std::isspace(c) != 0; });
            s.erase(s.begin(), it);
        }

        inline void rtrim_inplace(std::string &s) {
            auto it = std::find_if_not(s.rbegin(), s.rend(),
                                       [](unsigned char c) { return std::isspace(c) != 0; })
                              .base();
            s.erase(it, s.end());
        }

        inline void trim_inplace(std::string &s) {
            ltrim_inplace(s);
            rtrim_inplace(s);
        }

        inline std::string to_lower(std::string_view sv) {
            std::string out;
            out.resize(sv.size());
            std::transform(sv.begin(), sv.end(), out.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return out;
        }

        inline void to_lower_inplace(std::string &s) {
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        }

        inline std::vector<std::string> split_comma_trim(std::string_view sv) {
            std::vector<std::string> out;
            std::string cur;
            cur.reserve(sv.size());

            for (char ch: sv) {
                if (ch == ',') {
                    trim_inplace(cur);
                    if (!cur.empty()) out.push_back(std::move(cur));
                    cur.clear();
                } else {
                    cur.push_back(ch);
                }
            }
            trim_inplace(cur);
            if (!cur.empty()) out.push_back(std::move(cur));
            return out;
        }

        inline std::optional<ENUM> try_parse_enum(std::string_view normalized_lower_key) {
            static const std::unordered_map<std::string_view, ENUM> m = [] {
                std::unordered_map<std::string_view, ENUM> tmp;
                tmp.reserve(std::size(header_enum_to_string_lower));
                for (std::size_t i = 0; i < std::size(header_enum_to_string_lower); ++i) {
                    tmp.emplace(header_enum_to_string_lower[i], static_cast<ENUM>(i));
                }
                return tmp;
            }();

            auto it = m.find(normalized_lower_key);
            if (it == m.end()) return std::nullopt;
            return it->second;
        }


    }// namespace helpers

    template<typename Derived>
    class HeadersBase {

    public:
        // template<ENUM E>
        // struct Rule {
        //     static constexpr bool implemented = false;
        //     static constexpr bool allow_in_context = false;
        //     static constexpr bool forbid_duplicates = false;
        //     static constexpr bool comma_split = false;

        //     static std::expected<void, usub::unet::utils::UnetError>
        //     validate_value(std::string_view) {
        //         return std::unexpected(usub::unet::utils::UnetError{
        //                 usub::unet::utils::UnetError::DEFAULT,
        //                 "Rule not implemented"});
        //     }
        // };

        using iterator = std::vector<Header>::iterator;
        using const_iterator = std::vector<Header>::const_iterator;

        HeadersBase() = default;
        ~HeadersBase() = default;

        [[nodiscard]] std::size_t size() const noexcept { return header_list_.size(); }
        [[nodiscard]] bool empty() const noexcept { return header_list_.empty(); }

        iterator begin() noexcept { return header_list_.begin(); }
        iterator end() noexcept { return header_list_.end(); }
        const_iterator begin() const noexcept { return header_list_.begin(); }
        const_iterator end() const noexcept { return header_list_.end(); }
        const_iterator cbegin() const noexcept { return header_list_.cbegin(); }
        const_iterator cend() const noexcept { return header_list_.cend(); }

        [[nodiscard]] bool contains(std::string_view key) const {
            const std::string k = normalizeKey(key);
            return contains_normalized_(k);
        }

        iterator find(std::string_view key) {
            const std::string k = normalizeKey(key);
            return find_normalized_(k);
        }

        const_iterator find(std::string_view key) const {
            const std::string k = normalizeKey(key);
            return find_normalized_(k);
        }

        [[nodiscard]] bool contains(ENUM e) const {
            return contains(std::string_view(header_enum_to_string_lower[static_cast<std::size_t>(e)]));
        }

        iterator find(ENUM e) {
            return find(std::string_view(header_enum_to_string_lower[static_cast<std::size_t>(e)]));
        }

        const_iterator find(ENUM e) const {
            return find(std::string_view(header_enum_to_string_lower[static_cast<std::size_t>(e)]));
        }

        [[nodiscard]] std::vector<std::string_view> getAll(std::string_view key) const {
            const std::string k = normalizeKey(key);
            std::vector<std::string_view> out;
            for (const auto &h: header_list_) {
                if (h.key == k) out.push_back(h.value);
            }
            return out;
        }

        [[nodiscard]] std::vector<std::string_view> getAll(ENUM e) const {
            return getAll(std::string_view(header_enum_to_string_lower[static_cast<std::size_t>(e)]));
        }

        [[nodiscard]] std::expected<std::string_view, usub::unet::utils::UnetError>
        at(std::string_view key) const {
            auto it = this->find(key);
            if (it == this->end()) {
                return std::unexpected(make_error_not_found_(key));
            }
            return it->value;
        }

        [[nodiscard]] std::expected<std::string_view, usub::unet::utils::UnetError>
        at(ENUM e) const {
            return at(std::string_view(header_enum_to_string_lower[static_cast<std::size_t>(e)]));
        }

        std::expected<void, usub::unet::utils::UnetError>
        add(std::string_view key, std::string_view value) {
            std::string k = normalizeKey(key);
            std::string v(value);
            return add_normalized_(std::move(k), std::move(v));
        }

        std::expected<void, usub::unet::utils::UnetError>
        emplace(std::string &&key, std::string &&value) {
            helpers::to_lower_inplace(key);
            return add_normalized_(std::move(key), std::move(value));
        }

        std::expected<void, usub::unet::utils::UnetError>
        add(ENUM e, std::string_view value) {
            return add(std::string_view(header_enum_to_string_lower[static_cast<std::size_t>(e)]), value);
        }

    protected:
        std::vector<Header> header_list_;
        std::vector<Header> trailers;

        static std::string normalizeKey(std::string_view key) {
            return helpers::to_lower(key);
        }

    private:
        [[nodiscard]] bool contains_normalized_(const std::string &normalized) const {
            return std::any_of(header_list_.begin(), header_list_.end(),
                               [&](const Header &h) { return h.key == normalized; });
        }

        iterator find_normalized_(const std::string &normalized) {
            return std::find_if(header_list_.begin(), header_list_.end(),
                                [&](const Header &h) { return h.key == normalized; });
        }

        const_iterator find_normalized_(const std::string &normalized) const {
            return std::find_if(header_list_.begin(), header_list_.end(),
                                [&](const Header &h) { return h.key == normalized; });
        }

        [[nodiscard]] usub::unet::utils::UnetError make_error_not_found_(std::string_view key) const {
            return usub::unet::utils::UnetError{/*code=*/usub::unet::utils::UnetError::DEFAULT, std::string("Header not found: ") + std::string(key)};
        }

        std::expected<void, usub::unet::utils::UnetError>
        add_normalized_(std::string &&normalized_key, std::string &&value) {
            if (normalized_key.empty()) {
                return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Header key is empty"});
            }
            if (value.empty()) {
                return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Header value is empty"});
            }

            if (auto maybe_e = helpers::try_parse_enum(normalized_key)) {
                const ENUM e = *maybe_e;
                return add_known_enum_(e, std::move(normalized_key), std::move(value));
            }

            helpers::trim_inplace(value);
            header_list_.push_back(Header{std::move(normalized_key), std::move(value)});
            return {};
        }

        std::expected<void, usub::unet::utils::UnetError>
        add_known_enum_(ENUM e, std::string &&normalized_key, std::string &&value) {
            switch (e) {
                default:
                    return add_known_enum_via_rule_(e, std::move(normalized_key), std::move(value));
            }
        }

        std::expected<void, usub::unet::utils::UnetError>
        add_known_enum_via_rule_(ENUM e, std::string &&normalized_key, std::string &&value) {

            constexpr std::size_t N = std::size(header_enum_to_string_lower);

            return dispatch_by_index_<N>(
                    static_cast<std::size_t>(e),
                    std::move(normalized_key),
                    std::move(value));
        }

        template<std::size_t N>
        std::expected<void, usub::unet::utils::UnetError>
        dispatch_by_index_(std::size_t idx, std::string &&normalized_key, std::string &&value) {
            return dispatch_impl_(idx, std::move(normalized_key), std::move(value),
                                  std::make_index_sequence<N>{});
        }

        template<std::size_t... I>
        std::expected<void, usub::unet::utils::UnetError>
        dispatch_impl_(std::size_t idx, std::string &&normalized_key, std::string &&value,
                       std::index_sequence<I...>) {
            std::expected<void, usub::unet::utils::UnetError> rv =
                    std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Invalid header enum index"});

            (void) ((idx == I
                             ? (rv = apply_rule_<static_cast<ENUM>(I)>(std::move(normalized_key), std::move(value)), true)
                             : false) ||
                    ...);

            return rv;
        }

        template<ENUM E>
        std::expected<void, usub::unet::utils::UnetError>
        apply_rule_(std::string &&normalized_key, std::string &&value) {
            using R = typename Derived::template Rule<E>;
            static_assert(R::implemented, "Missing Rule specialization for this header ENUM in this context");

            if constexpr (!R::allow_in_context) {
                return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Header not allowed in this context"});
            }

            if constexpr (R::forbid_duplicates) {
                if (this->contains(std::string_view(normalized_key))) {
                    return std::unexpected(usub::unet::utils::UnetError{usub::unet::utils::UnetError::DEFAULT, "Duplicate header is forbidden"});
                }
            }

            if constexpr (R::comma_split) {
                auto parts = helpers::split_comma_trim(value);
                for (auto &part: parts) {
                    if (auto ok = R::validate_value(part); !ok) return ok;
                    header_list_.push_back(Header{normalized_key, std::move(part)});
                }
                return {};
            } else {
                helpers::trim_inplace(value);
                if (auto ok = R::validate_value(value); !ok) return ok;
                header_list_.push_back(Header{std::move(normalized_key), std::move(value)});
                return {};
            }
        }
    };
    template<typename T>
    class Headers : public HeadersBase<Headers<T>> {
    public:
        using Base = HeadersBase<Headers<T>>;
        using Base::Base;

        template<ENUM E>
        struct Rule {
            static constexpr bool implemented = false;
            static constexpr bool allow_in_context = true;
            static constexpr bool forbid_duplicates = false;
            static constexpr bool comma_split = false;

            static std::expected<void, usub::unet::utils::UnetError>
            validate_value(std::string_view) {
                return std::unexpected(usub::unet::utils::UnetError{
                        usub::unet::utils::UnetError::DEFAULT,
                        "Rule not implemented"});
            }
        };
    };

}// namespace usub::unet::header
