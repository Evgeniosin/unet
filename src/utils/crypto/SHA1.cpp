//
// Created by Kirill Zhukov on 11.05.2024.
//

#include "utils/crypto/SHA1.h"

namespace usub::crypto {
    SHA1::SHA1() {
        h0 = 0x67452301;
        h1 = 0xEFCDAB89;
        h2 = 0x98BADCFE;
        h3 = 0x10325476;
        h4 = 0xC3D2E1F0;
        unprocessedBytes = 0;
        size = 0;
    }

    void SHA1::update(const std::string &s) {
        std::istringstream is(s);
        update(is);
    }

    void SHA1::update(std::istream &is) {
        while (true) {
            is.read(reinterpret_cast<char *>(bytes + unprocessedBytes), 64 - unprocessedBytes);
            size_t read = is.gcount();
            if (read == 0) break;
            size += read;
            unprocessedBytes += read;
            if (unprocessedBytes == 64) {
                processBlock(bytes);
                unprocessedBytes = 0;
            }
        }
    }

    void SHA1::pad() {
        size_t originalLength = size;
        bytes[unprocessedBytes++] = 0x80;
        if (unprocessedBytes > 56) {
            while (unprocessedBytes < 64) {
                bytes[unprocessedBytes++] = 0;
            }
            processBlock(bytes);
            unprocessedBytes = 0;
        }
        while (unprocessedBytes < 56) {
            bytes[unprocessedBytes++] = 0;
        }
        for (int i = 0; i < 8; i++) {
            bytes[unprocessedBytes++] = (originalLength * 8) >> (56 - i * 8);
        }
        processBlock(bytes);
    }

    std::string SHA1::final() {
        pad();
        std::ostringstream result;
        result << std::hex << std::setfill('0');
        for (uint32_t *ptr: {&h0, &h1, &h2, &h3, &h4}) {
            result << std::setw(8) << *ptr;
        }
        return result.str();
    }

    void SHA1::processBlock(const unsigned char *block) {
        uint32_t w[80];
        for (size_t i = 0; i < 16; i++) {
            w[i] = (block[i * 4 + 0] << 24)
                   + (block[i * 4 + 1] << 16)
                   + (block[i * 4 + 2] << 8)
                   + (block[i * 4 + 3]);
        }
        for (size_t i = 16; i < 80; i++) {
            w[i] = rotate_left(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;

        for (size_t i = 0; i < 80; i++) {
            uint32_t f, k;
            if (i < 20) {
                f = (b & c) | (~b & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }
            uint32_t temp = rotate_left(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = rotate_left(b, 30);
            b = a;
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    uint32_t SHA1::rotate_left(uint32_t value, size_t count) {
        return (value << count) | (value >> (32 - count));
    }

    std::vector<unsigned char> SHA1::final_bytes() {
        pad();

        std::vector<unsigned char> hash;
        hash.reserve(20);

        auto add_to_hash = [&hash](uint32_t part) {
            hash.push_back(static_cast<unsigned char>((part >> 24) & 0xFF));
            hash.push_back(static_cast<unsigned char>((part >> 16) & 0xFF));
            hash.push_back(static_cast<unsigned char>((part >> 8) & 0xFF));
            hash.push_back(static_cast<unsigned char>(part & 0xFF));
        };

        add_to_hash(h0);
        add_to_hash(h1);
        add_to_hash(h2);
        add_to_hash(h3);
        add_to_hash(h4);

        return hash;
    }
}
