#include "utils/HTTPUtils/HTTPUtils.h"

bool usub::utils::isLanguageTag(std::string_view value) {
    // Lambda function to check if a character is valid
    auto isValidChar = [](char ch) {
        return std::isalnum(static_cast<unsigned char>(ch)) ||                            // 0-9, A-Z, a-z
               ch == '*' || ch == ',' || ch == '-' || ch == '.' || ch == ';' || ch == '=';// special characters
    };

    // Check if all characters in the string are valid
    return std::all_of(value.begin(), value.end(), isValidChar);
}

bool usub::utils::areLanguageTags(std::string_view value) {
    // Lambda function to check if a character is valid
    auto isValidChar = [](char ch) {
        return std::isalnum(static_cast<unsigned char>(ch)) ||                                         // 0-9, A-Z, a-z
               ch == '*' || ch == ',' || ch == '-' || ch == '.' || ch == ';' || ch == '=' || ch == ',';// special characters
    };

    // Check if all characters in the string are valid
    return std::all_of(value.begin(), value.end(), isValidChar);
}

constexpr std::array<uint8_t, 256> generateValidTChars() {
    std::array<uint8_t, 256> table = {};
    for (char c = 'A'; c <= 'Z'; ++c) table[static_cast<unsigned char>(c)] = 1;
    for (char c = 'a'; c <= 'z'; ++c) table[static_cast<unsigned char>(c)] = 1;
    for (char c = '0'; c <= '9'; ++c) table[static_cast<unsigned char>(c)] = 1;
    for (char c: {'!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '^', '_', '`', '|', '~'}) {
        table[static_cast<unsigned char>(c)] = 1;
    }
    return table;
};

constexpr std::array<uint8_t, 256> generateValidVChars() {
    std::array<uint8_t, 256> table = {};
    for (char c = '!'; c <= '~'; ++c) table[static_cast<unsigned char>(c)] = 1;
    return table;
};

constexpr std::array<uint8_t, 256> generateValidVCharsOrObsText() {
    std::array<uint8_t, 256> table = {};
    for (char c = '!'; c <= '~'; ++c) table[static_cast<unsigned char>(c)] = 1;
    for (unsigned char c = 128; c <= 255 && c >= 128; ++c) table[static_cast<unsigned char>(c)] = 1;
    return table;
};

static constexpr std::array<uint8_t, 256> validTChars = generateValidTChars();

static constexpr std::array<uint8_t, 256> validVChars = generateValidVChars();

static constexpr std::array<uint8_t, 256> validVCharsOrObsText = generateValidVCharsOrObsText();

bool usub::utils::isTchar(char ch) {
    return validTChars[static_cast<unsigned char>(ch)] != 0;
}

bool usub::utils::isTchar(std::string_view value) {
    for (char ch: value) {
        if (validTChars[static_cast<unsigned char>(ch)] == 0 || ch == ' ' || ch == '\t') {
            return false;
        }
    }
    return true;
}

bool usub::utils::isVchar(char ch) {
    return validVChars[static_cast<unsigned char>(ch)] != 0;
}

bool usub::utils::isVchar(std::string_view value) {
    for (char ch: value) {
        if (validVChars[static_cast<unsigned char>(ch)] == 0 || ch == ' ' || ch == '\t') {
            return false;
        }
    }
    return true;
}

bool usub::utils::isObsText(char ch) {
    return (static_cast<unsigned char>(ch) >= 128);
}

bool usub::utils::isVcharOrObsText(char ch) {
    return validVCharsOrObsText[static_cast<unsigned char>(ch)] != 0;
}

bool usub::utils::isVcharOrObsText(std::string_view value) {
    for (char ch: value) {
        if (validVCharsOrObsText[static_cast<unsigned char>(ch)] == 0 || ch == ' ' || ch == '\t') {
            return false;
        }
    }
    return true;
}
