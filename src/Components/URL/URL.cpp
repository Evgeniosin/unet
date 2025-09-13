#include "Components/URL/URL.h"

std::string &usub::server::component::URN::getPath() {
    return this->path_;
}

const std::string &usub::server::component::URN::getPath() const {
    return this->path_;
}

usub::server::component::url::QueryParams &usub::server::component::URN::getQueryParams() {
    return this->query_;
}

const usub::server::component::url::QueryParams &usub::server::component::URN::getQueryParams() const {
    return this->query_;
}

std::string &usub::server::component::URN::getFragment() {
    return this->fragment_;
}

std::string usub::server::component::URN::string() const {
    return PercentEncoded::decode(this->path_) + PercentEncoded::decode(this->query_.string()) /* + this->fragment_*/;
}

bool usub::server::component::URN::isSchemeChar(char c) {
    static constexpr std::array<uint8_t, 256> validChars = [] {
        std::array<uint8_t, 256> table = {};
        for (char c = 'A'; c <= 'Z'; ++c) table[static_cast<unsigned char>(c)] = 1;
        for (char c = 'a'; c <= 'z'; ++c) table[static_cast<unsigned char>(c)] = 1;
        for (char c = '0'; c <= '9'; ++c) table[static_cast<unsigned char>(c)] = 1;
        for (char c: {'+', '-', '.'}) {
            table[static_cast<unsigned char>(c)] = 1;
        }
        return table;
    }();
    return validChars[static_cast<unsigned char>(c)] != 0;
}

bool usub::server::component::URN::isPathChar(char c) {
    static constexpr std::array<uint8_t, 256> validChars = [] {
        std::array<uint8_t, 256> table = {};

        // Unreserved characters (A-Z, a-z, 0-9, -, ., _, ~)
        for (char c = 'A'; c <= 'Z'; ++c) table[static_cast<unsigned char>(c)] = 1;
        for (char c = 'a'; c <= 'z'; ++c) table[static_cast<unsigned char>(c)] = 1;
        for (char c = '0'; c <= '9'; ++c) table[static_cast<unsigned char>(c)] = 1;
        for (char c: {'-', '.', '_', '~'}) {
            table[static_cast<unsigned char>(c)] = 1;
        }

        // Sub-delimiters (!, $, &, ', (, ), *, +, ,, ;, =)
        for (char c: {'!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='}) {
            table[static_cast<unsigned char>(c)] = 1;
        }

        // Reserved characters allowed in path (:, @, /)
        for (char c: {':', '@', '/'}) {
            table[static_cast<unsigned char>(c)] = 1;
        }

        return table;
    }();
    return validChars[static_cast<unsigned char>(c)] != 0;
}

bool usub::server::component::URN::isQueryChar(char c) {
    static constexpr std::array<uint8_t, 256> validChars = [] {
        std::array<uint8_t, 256> table = {};

        // Unreserved characters (A-Z, a-z, 0-9, -, ., _, ~)
        for (char c = 'A'; c <= 'Z'; ++c) table[static_cast<unsigned char>(c)] = 1;
        for (char c = 'a'; c <= 'z'; ++c) table[static_cast<unsigned char>(c)] = 1;
        for (char c = '0'; c <= '9'; ++c) table[static_cast<unsigned char>(c)] = 1;
        for (char c: {'-', '.', '_', '~'}) {
            table[static_cast<unsigned char>(c)] = 1;
        }

        // Sub-delimiters (!, $, &, ', (, ), *, +, ,, ;, =)
        for (char c: {'!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='}) {
            table[static_cast<unsigned char>(c)] = 1;
        }

        // Reserved characters allowed in queries (:, @, /, ?)
        for (char c: {':', '@', '/', '?', '%'}) {
            table[static_cast<unsigned char>(c)] = 1;
        }

        // Fragment delimiter (#) is NOT included → disallowed
        return table;
    }();
    return validChars[static_cast<unsigned char>(c)] != 0;
}
