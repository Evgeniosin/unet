//
// Created by Kirill Zhukov on 11.05.2024.
//

#include "Websocket.h"

namespace usub::server::protocols::ws {
    std::string base64_encode(const std::vector<unsigned char> &data) {
        std::string encoded;
        int val = 0;
        int valb = -6;
        for (unsigned char c: data) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (encoded.size() % 4) encoded.push_back('=');
        return encoded;
    }

    std::string create_websocket_accept(const std::string &key) {
        std::string combined = key + WEBSOCKET_GUID;

        usub::crypto::SHA1 sha1;
        sha1.update(combined);
        std::vector<unsigned char> hash = sha1.final_bytes();

        return base64_encode(hash);
    }

    std::vector<uint8_t> createWebSocketFrame(bool fin, Opcode opcode, const std::vector<uint8_t>& data, bool mask, const std::vector<uint8_t>& masking_key) {
        std::vector<uint8_t> frame;
        size_t payload_length = data.size();
        uint8_t fin_and_opcode = (fin ? 0x80 : 0x00) | (static_cast<uint8_t>(opcode) & 0x0F);
        frame.push_back(fin_and_opcode);

        uint8_t mask_bit = mask ? 0x80 : 0x00;

        if (payload_length <= 125) {
            frame.push_back(static_cast<uint8_t>(payload_length | mask_bit));
        } else if (payload_length <= 65535) {
            frame.push_back(126 | mask_bit);
            frame.push_back((payload_length >> 8) & 0xFF);
            frame.push_back(payload_length & 0xFF);
        } else {
            frame.push_back(127 | mask_bit);
            for (int i = 7; i >= 0; --i) {
                frame.push_back((payload_length >> (i * 8)) & 0xFF);
            }
        }

        if (mask && masking_key.size() == 4) {
            frame.insert(frame.end(), masking_key.begin(), masking_key.end());
            for (size_t i = 0; i < data.size(); ++i) {
                frame.push_back(data[i] ^ masking_key[i % 4]);
            }
        } else {
            frame.insert(frame.end(), data.begin(), data.end());
        }

        return frame;
    }

    WebSocketFrame parseWebSocketFrame(const std::vector<uint8_t>& frame_bytes) {
        if (frame_bytes.size() < 2) {
            throw std::invalid_argument("Incomplete frame.");
        }

        WebSocketFrame frame;
        frame.fin = (frame_bytes[0] >> 7) & 1;
        frame.opcode = static_cast<Opcode>(frame_bytes[0] & 0x0F);
        frame.mask = (frame_bytes[1] >> 7) & 1;
        size_t payload_length = frame_bytes[1] & 0x7F;
        size_t index = 2;

        if (payload_length == 126) {
            if (frame_bytes.size() < 4) {
                throw std::invalid_argument("Incomplete frame.");
            }
            payload_length = (frame_bytes[index] << 8) + frame_bytes[index + 1];
            index += 2;
        } else if (payload_length == 127) {
            if (frame_bytes.size() < 10) {
                throw std::invalid_argument("Incomplete frame.");
            }
            payload_length = 0;
            for (int i = 0; i < 8; ++i) {
                payload_length = (payload_length << 8) + frame_bytes[index + i];
            }
            index += 8;
        }

        if (frame.mask) {
            if (frame_bytes.size() < index + 4 + payload_length) {
                throw std::invalid_argument("Incomplete frame.");
            }
            frame.masking_key.assign(frame_bytes.begin() + index, frame_bytes.begin() + index + 4);
            index += 4;
            frame.payload.resize(payload_length);
            for (size_t i = 0; i < payload_length; ++i) {
                frame.payload[i] = frame_bytes[index + i] ^ frame.masking_key[i % 4];
            }
        } else {
            if (frame_bytes.size() < index + payload_length) {
                throw std::invalid_argument("Incomplete frame.");
            }
            frame.payload.assign(frame_bytes.begin() + index, frame_bytes.begin() + index + payload_length);
        }

        return frame;
    }

    WebSocketFrame::WebSocketFrame(bool fin, bool mask, Opcode opcode, std::vector<uint8_t> payload,
                                   std::vector<uint8_t> &masking_key) : fin(fin), mask(mask), opcode(opcode), payload(std::move(payload)), masking_key(std::move(masking_key)) {}
}
