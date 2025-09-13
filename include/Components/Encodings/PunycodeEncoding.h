#ifndef SERVER_PUNYCODE_H
#define SERVER_PUNYCODE_H

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <limits>

namespace usub::server::component {

    class PunycodeEncoding {
    public:
        /**
     * @brief Encodes a Unicode (UTF-8) string to Punycode with "xn--" prefix.
     *
     * @param input The input Unicode string (UTF-8).
     * @param output The output Punycode string. Empty on failure.
     * @return true If encoding was successful.
     * @return false If encoding failed.
     */
        static bool encode(const std::string& input, std::string& output);

        /**
     * @brief Decodes a Punycode string (with or without "xn--" prefix) to a Unicode (UTF-8) string.
     *
     * @param input The input Punycode string (UTF-8).
     * @param output The output Unicode string. Empty on failure.
     * @return true If decoding was successful.
     * @return false If decoding failed.
     */
        static bool decode(const std::string& input, std::string& output);

        /**
     * @brief Checks if a string is Punycode encoded (starts with "xn--", case-insensitive).
     *
     * @param input The input string to check.
     * @return true If the string starts with "xn--".
     * @return false Otherwise.
     */
        static bool is_punycode(const std::string& input);
    };

}

#endif//SERVER_PUNYCODE_H