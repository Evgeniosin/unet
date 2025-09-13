#include "utils/utils.h"

bool usub::utils::isPositiveIntegerString(const std::string &str) {
    if (str.empty()) return false;

    for (char c: str) {
        if (!std::isdigit(c)) return false;// Return false if any character is not a digit
    }

    return true;
}

std::vector<unsigned char> usub::utils::toLower(const std::vector<unsigned char> &vec) {
    std::vector<unsigned char> lower;
    lower.reserve(vec.size());
    for (unsigned char c: vec) {
        lower.push_back(static_cast<unsigned char>(std::tolower(c)));
    }
    return lower;
}

std::string usub::utils::toLower(const std::string &str) {
    std::string lower;
    lower.reserve(str.size());
    for (char c: str) {
        lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return lower;
}

std::string usub::utils::deleteSpaces(std::string_view str) {
    std::string result;
    result.reserve(str.size());
    std::copy_if(str.begin(), str.end(), std::back_inserter(result), [](char ch) {
        return !std::isspace(static_cast<unsigned char>(ch));
    });
    return result;
}

// Overload for std::string (if needed)
std::string usub::utils::deleteSpaces(const std::string &str) {
    return deleteSpaces(std::string_view(str));
}

bool usub::utils::operator==(const std::vector<unsigned char> &vec, const std::string &str) {
    if (vec.size() != str.size()) {
        return false;
    }
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] != static_cast<unsigned char>(str[i])) {
            return false;
        }
    }
    return true;
}

bool usub::utils::operator==(const std::string &str, const std::vector<unsigned char> &vec) {
    return vec == str;
}
