#ifndef USUB_STRING_UTILS
#define USUB_STRING_UTILS

#include <algorithm>
#include <cctype>
#include <iterator>
#include <locale>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace usub::utils {

    // Trim from start (in-place)
    inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                        [](unsigned char ch) { return !std::isspace(ch); }));
    }

    // Trim from end (in-place)
    inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](unsigned char ch) { return !std::isspace(ch); })
                        .base(),
                s.end());
    }

    // Trim both ends (in-place)
    inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    // Return a trimmed copy (by value)
    inline std::string trim_copy(std::string_view s) {
        auto start = std::find_if_not(s.begin(), s.end(), [](unsigned char c) { return std::isspace(c); });
        auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char c) { return std::isspace(c); }).base();
        if (start >= end) return {};
        return std::string(start, end);
    }

    // Case-insensitive compare
    inline bool icmp(std::string_view a, std::string_view b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (std::tolower(static_cast<unsigned char>(a[i])) !=
                std::tolower(static_cast<unsigned char>(b[i]))) {
                return false;
            }
        }
        return true;
    }

    // Lowercase copy (returns a new string)
    inline std::string toLower(std::string_view s) {
        std::string result(s);
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    // Lowercase in-place
    inline void toLowerInPlace(std::string &s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    }

    // Check if string is a positive integer
    inline bool isPositiveIntegerString(std::string_view s) {
        return !s.empty() && std::all_of(s.begin(), s.end(), [](char c) { return std::isdigit(c); });
    }

    // Check if string is a integer
    inline bool isIntegerString(std::string_view s) {
        if (s.empty()) return false;
        auto it = s.begin();
        if (*it == '-' || *it == '+') ++it;// Allow leading sign
        return std::all_of(it, s.end(), [](char c) { return std::isdigit(c); });
    }

    // Split by delimiter and trim each token
    inline std::vector<std::string> splitAndTrim(std::string_view input, char delimiter = ',') {
        std::vector<std::string> result;
        size_t start = 0;
        while (start < input.size()) {
            size_t end = input.find(delimiter, start);
            auto token = input.substr(start, end == std::string_view::npos ? input.size() - start : end - start);
            result.emplace_back(trim_copy(token));
            if (end == std::string_view::npos) break;
            start = end + 1;
        }
        return result;
    }

    // Case-insensitive hash
    struct CaseInsensitiveHash {
        std::size_t operator()(const std::string &key) const {
            std::string lower;
            lower.reserve(key.size());
            for (char c: key) {
                lower.push_back(std::tolower(static_cast<unsigned char>(c)));
            }
            return std::hash<std::string>{}(lower);
        }
    };

    // Case-insensitive comparison
    struct CaseInsensitiveEqual {
        bool operator()(const std::string &lhs, const std::string &rhs) const {
            return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                              [](char a, char b) {
                                  return std::tolower(static_cast<unsigned char>(a)) ==
                                         std::tolower(static_cast<unsigned char>(b));
                              });
        }
    };

    // Case-insensitive string comparison for fixed-length strings
    inline bool case_insensitive_equal(const char *a, const char *b, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            if (std::tolower(static_cast<unsigned char>(a[i])) !=
                std::tolower(static_cast<unsigned char>(b[i]))) {
                return false;
            }
        }
        return true;
    }

    // Split a string by a delimiter
    inline std::vector<std::string> split(const std::string &s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.emplace_back(usub::utils::trim_copy(token));
        }
        return tokens;
    }


    inline std::vector<std::string> split(std::string_view s, char delimiter) {
        std::vector<std::string> tokens;
        size_t start = 0;
        while (start < s.size()) {
            size_t end = s.find(delimiter, start);
            auto token = s.substr(start, end == std::string_view::npos ? s.size() - start : end - start);
            tokens.emplace_back(usub::utils::trim_copy(token));
            if (end == std::string_view::npos) break;
            start = end + 1;
        }
        return tokens;
    }

}// namespace usub::utils

#endif// USUB_STRING_UTILS
