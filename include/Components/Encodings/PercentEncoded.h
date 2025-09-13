#ifndef SERVER_PERCENT_ENCODED_H
#define SERVER_PERCENT_ENCODED_H

#include <string>
#include <string_view>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace usub::server::component {

    class PercentEncoded {
    public:
        static std::string encode(std::string_view input);

        static std::string encodeNonASCII(std::string_view input);

        static std::string decode(std::string_view input);

    private:
        // Checks if a character is unreserved according to RFC 3986
        static bool isUnreserved(char c);

        // Converts a 4-bit value to its hexadecimal character
        static char toHex(char value);

        // Checks if a character is a valid hexadecimal digit
        static bool isHexDigit(char c);

        // Converts a hexadecimal character to its integer value
        static char fromHex(char c);
    };

}

#endif // SERVER_PERCENT_ENCODED_H