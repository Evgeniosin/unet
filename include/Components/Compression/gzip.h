//
// Created by kirill on 12/22/24.
//

#ifndef SERVER_GZIP_H
#define SERVER_GZIP_H

#include <stdexcept>
#include <string>
#include <vector>
#include <zlib.h>

#include "Components/Compression/CompressionBase.h"

namespace usub::server::component {
    class Gzip : public CompressionBase {
    public:
        explicit Gzip();
        ~Gzip() override = default;

        // In-place decompression
        void decompress(std::string &data) const override;

        // In-place compression
        void compress(std::string &data) const override;

        uint8_t getState() const override;

        size_t getTypeID() const override;
    };
}// namespace usub::server::component
#endif//SERVER_GZIP_H
