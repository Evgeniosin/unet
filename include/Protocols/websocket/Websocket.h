//
// Created by Kirill Zhukov on 11.05.2024.
//

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <string>
#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <functional>
#include "utils/crypto/SHA1.h"

namespace usub::server::protocols::ws {
    constexpr char WEBSOCKET_GUID[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    static constexpr char base64_chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

    // According to RFC6455
    enum class Opcode : uint8_t {
        CONTINUATION    = 0x0,
        TEXT            = 0x1,
        BINARY          = 0x2,
        CLOSE           = 0x8,
        PING            = 0x9,
        PONG            = 0xA
    };

    class WebSocketFrame {
    public:
        WebSocketFrame() = default;
        WebSocketFrame(bool fin, bool mask, Opcode opcode, std::vector<uint8_t> payload, std::vector<uint8_t>& masking_key);
        ~WebSocketFrame() = default;
    public:
        bool fin;
        bool mask;
        Opcode opcode;
        std::vector<uint8_t> payload;
        std::vector<uint8_t> masking_key;
    };

    static std::string base64_encode(const std::vector<unsigned char> &data);

    std::string create_websocket_accept(const std::string &key);

    std::vector<uint8_t> createWebSocketFrame(bool fin, Opcode opcode, const std::vector<uint8_t>& data, bool mask = false, const std::vector<uint8_t>& masking_key = {});

    WebSocketFrame parseWebSocketFrame(const std::vector<uint8_t>& frame_bytes);
}

#endif //WEBSOCKET_H

