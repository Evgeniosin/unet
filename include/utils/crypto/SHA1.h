//
// Created by Kirill Zhukov on 11.05.2024.
//

#ifndef WEBSOCKETPARSER_SHA1_H
#define WEBSOCKETPARSER_SHA1_H

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace usub::crypto {
    class SHA1 {
    public:
        SHA1();

        void update(const std::string &s);

        void update(std::istream &is);

        std::string final();

        std::vector<unsigned char> final_bytes();

    private:
        uint32_t h0, h1, h2, h3, h4;
        unsigned char bytes[64];
        uint32_t unprocessedBytes;
        uint64_t size;

        void processBlock(const unsigned char *block);

        void pad();

        static uint32_t rotate_left(uint32_t value, size_t count);
    };
}

#endif //WEBSOCKETPARSER_SHA1_H
