#include "Components/Encodings/PercentEncoded.h"


std::string usub::server::component::PercentEncoded::encode(std::string_view input) {
    std::string output;
    output.reserve(input.size());// Reserve space to improve performance

    for (char c: input) {
        if (isUnreserved(c)) {
            output += c;
        } else {
            // Convert character to its hexadecimal representation
            output += '%';
            output += toHex(static_cast<unsigned char>(c) >> 4);
            output += toHex(static_cast<unsigned char>(c) & 0x0F);
        }
    }

    return output;
}

std::string usub::server::component::PercentEncoded::encodeNonASCII(std::string_view input) {
    std::string output;
    output.reserve(input.size());// Reserve space to improve performance

    for (unsigned char c: input) {
        if (c <= 127 && isUnreserved(c)) {
            // ASCII unreserved characters are left as-is
            output += static_cast<char>(c);
        } else {
            // Percent-encode non-ASCII characters and reserved ASCII characters
            output += '%';
            output += toHex(c >> 4);
            output += toHex(c & 0x0F);
        }
    }

    return output;
}

std::string usub::server::component::PercentEncoded::decode(std::string_view input) {
    std::string output;
    output.reserve(input.size());// Reserve space to improve performance

    size_t i = 0;
    while (i < input.size()) {
        if (input[i] == '%') {
            if (i + 2 >= input.size()) {
                throw std::invalid_argument("Invalid percent-encoding: Incomplete escape sequence.");
            }
            char high = input[i + 1];
            char low = input[i + 2];
            if (!isHexDigit(high) || !isHexDigit(low)) {
                throw std::invalid_argument("Invalid percent-encoding: Non-hexadecimal characters.");
            }
            // Convert hex digits to a single character
            char decoded_char = (fromHex(high) << 4) | fromHex(low);
            output += decoded_char;
            i += 3;// Skip past the escape sequence
        } else if (input[i] == '+') {
            // Optional: Convert '+' to space if handling application/x-www-form-urlencoded
            output += ' ';
            i++;
        } else {
            output += input[i];
            i++;
        }
    }

    return output;
}

bool usub::server::component::PercentEncoded::isUnreserved(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) ||
           c == '-' || c == '_' || c == '.' || c == '~';
}

char usub::server::component::PercentEncoded::toHex(char value) {
    if (value >= 0 && value <= 9) {
        return '0' + value;
    } else {
        return 'A' + (value - 10);
    }
}

bool usub::server::component::PercentEncoded::isHexDigit(char c) {
    return std::isxdigit(static_cast<unsigned char>(c));
}

char usub::server::component::PercentEncoded::fromHex(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else {
        throw std::invalid_argument("Invalid hexadecimal digit.");
    }
}
