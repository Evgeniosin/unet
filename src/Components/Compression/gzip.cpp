#include "Components/Compression/gzip.h"

usub::server::component::Gzip::Gzip()
    : CompressionBase("gzip") {}

void usub::server::component::Gzip::decompress(std::string &data) const {
    if (data.empty()) {
        this->state_ = STATE::ERROR;
        return;
    }

    z_stream strm = {};
    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) {
        this->state_ = STATE::ERROR;
        return;
    }

    // Estimate decompressed size and resize the string
    std::string originalData = std::move(data);// Backup the original data
    data.resize(originalData.size() * 2);      // Start with double the size

    strm.next_in = reinterpret_cast<Bytef *>(originalData.data());
    strm.avail_in = originalData.size();
    strm.next_out = reinterpret_cast<Bytef *>(data.data());
    strm.avail_out = data.size();

    while (true) {
        int ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_END) {
            break;
        } else if (ret != Z_OK) {
            inflateEnd(&strm);
            this->state_ = STATE::ERROR;
            data = std::move(originalData);// Restore the original data
            return;
        }

        // Expand buffer if needed
        if (strm.avail_out == 0) {
            size_t currentSize = data.size();
            data.resize(currentSize * 2);// Double the buffer size
            strm.next_out = reinterpret_cast<Bytef *>(data.data() + currentSize);
            strm.avail_out = currentSize;
        }
    }

    inflateEnd(&strm);
    data.resize(strm.total_out);// Resize to actual decompressed size
    this->state_ = STATE::COMPLETE;
    return;
}

void usub::server::component::Gzip::compress(std::string &data) const {
    if (data.empty()) {
        this->state_ = STATE::ERROR;
        return;
    }

    z_stream strm = {};
    if (deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        this->state_ = STATE::ERROR;
        return;
    }

    // Preallocate based on deflateBound and resize the string
    std::string originalData = std::move(data);// Backup the original data
    data.resize(deflateBound(&strm, originalData.size()));

    strm.next_in = reinterpret_cast<Bytef *>(originalData.data());
    strm.avail_in = originalData.size();
    strm.next_out = reinterpret_cast<Bytef *>(data.data());
    strm.avail_out = data.size();

    int ret = deflate(&strm, Z_FINISH);
    if (ret != Z_STREAM_END) {
        deflateEnd(&strm);
        this->state_ = STATE::ERROR;
        data = std::move(originalData);// Restore the original data
        return;
    }

    deflateEnd(&strm);
    data.resize(strm.total_out);// Resize to actual compressed size
    this->state_ = STATE::COMPLETE;
    return;
}

uint8_t usub::server::component::Gzip::getState() const {
    return static_cast<uint8_t>(state_);
}

size_t usub::server::component::Gzip::getTypeID() const {
    return TypeID<Gzip>::id();
}

namespace {
    // Register the Gzip class with the DecoderChainFactory
    bool registered = DecoderRegistrar::registerDecoder<usub::server::component::Gzip>("gzip");
}// namespace