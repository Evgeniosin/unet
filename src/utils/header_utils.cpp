#include "utils/header_utils.h"

bool usub::utils::isToken(std::string_view value) {
    auto isValidChar = [](char ch) {
        return std::isalnum(static_cast<unsigned char>(ch)) ||                                                                                                                                                  // 0-9, A-Z, a-z
               ch == '!' || ch == '#' || ch == '$' || ch == '%' || ch == '&' || ch == '\'' || ch == '*' || ch == '+' || ch == '-' || ch == '.' || ch == '^' || ch == '_' || ch == '`' || ch == '|' || ch == '~';// special characters
    };

    // Check if all characters in the string are valid
    return std::all_of(str.begin(), str.end(), isValidChar);
}

bool usub::utils::areTokens(std::string_view value) {
    auto isValidChar = [](char ch) {
        return std::isalnum(static_cast<unsigned char>(ch)) ||                                                                                                                                                               // 0-9, A-Z, a-z
               ch == '!' || ch == '#' || ch == '$' || ch == '%' || ch == '&' || ch == '\'' || ch == '*' || ch == '+' || ch == '-' || ch == '.' || ch == '^' || ch == '_' || ch == '`' || ch == '|' || ch == '~' || ch == ',';// special characters + comma
    };

    // Check if all characters in the string are valid
    return std::all_of(str.begin(), str.end(), isValidChar);
}

bool usub::utils::isValidETag(std::string_view value) {
    // Check if the string is empty
    if (etag.empty()) {
        return false;
    }

    size_t pos = 0;

    // Check for optional weak indicator "W/"
    if (etag.size() >= 2 && etag[0] == 'W' && etag[1] == '/') {
        pos = 2;// Skip "W/"
    }

    // Check if the opaque-tag starts and ends with a double quote
    if (etag.size() < pos + 2 || etag[pos] != '"' || etag.back() != '"') {
        return false;
    }

    // Check each character in the opaque-tag
    for (size_t i = pos + 1; i < etag.size() - 1; ++i) {
        char ch = etag[i];
        // Check if the character is a valid etagc
        if (!(ch >= 0x21 && ch <= 0x7E && ch != '"')) {
            return false;
        }
    }

    return true;
}

bool usub::utils::areValidETags(std::string_view value) {// TODO: Optimize & test
    size_t start = 0;
    while (start < value.size()) {
        size_t end = value.find(',', start);
        if (end == std::string_view::npos) {
            end = value.size();
        }
        // Extract and trim the ETag
        std::string_view etag = value.substr(start, end - start);
        size_t first = etag.find_first_not_of(' ');
        size_t last = etag.find_last_not_of(' ');
        if (first == std::string_view::npos) {
            return false;
        }
        etag = etag.substr(first, last - first + 1);
        if (!isValidETag(etag)) {
            return false;
        }
        start = end + 1;
    }
    return true;
}