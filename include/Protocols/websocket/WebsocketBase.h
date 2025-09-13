//
// Created by Kirill Zhukov on 13.05.2024.
//

#ifndef SERVER_WEBSOCKETBASE_H
#define SERVER_WEBSOCKETBASE_H

#include "openssl/ssl.h"
#include "openssl/err.h"
#include "Websocket.h"
#include "core/ClientData.h"
#include "utils/ssl/data.h"

namespace usub::server::protocols::ws {
    class WebSocketMessage {
    public:
        WebSocketMessage(usub::core::ClientData &clientData);

        void writeRawData(Opcode opcode, const std::vector<uint8_t> &data = {}, bool isFin = false, bool isMask = false,
                          const std::vector<uint8_t> &maskingKey = {});

        void sendMessage(const std::vector<uint8_t> &data);

        usub::core::ConnectionID getConnectionID();

    private:
        usub::core::ClientData &clientData;

        std::vector<uint8_t> rawFrame;

        friend void readCallbackWS(usub::core::ClientData &clientData);

        friend void readCallbackWSS(usub::core::ClientData &clientData);
    };

    class WebSocketContext {
    public:
        explicit WebSocketContext(const std::function<void(ws::WebSocketFrame &, ws::WebSocketMessage &)> &function);

    public:
        std::function<void(ws::WebSocketFrame &, ws::WebSocketMessage &)> function;
    };

    void readCallbackWS(usub::core::ClientData &clientData);

    void readCallbackWSS(usub::core::ClientData &clientData);

    void writeCallback(usub::core::ClientData &clientData);

    void errorCallback(usub::core::ClientData &clientData, ErrorStatus status);
}

#endif //SERVER_WEBSOCKETBASE_H
