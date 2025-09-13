//
// Created by Kirill Zhukov on 13.05.2024.
//

#include "WebsocketBase.h"

namespace usub::server::protocols::ws {
    void WebSocketMessage::writeRawData(Opcode opcode, const std::vector<uint8_t> &data, bool isFin, bool isMask,
                                        const std::vector<uint8_t> &maskingKey) {
        this->rawFrame = ws::createWebSocketFrame(isFin, opcode, data, isMask, maskingKey);
    }

    void readCallbackWS(usub::core::ClientData &clientData) {
        auto frame = ws::parseWebSocketFrame(clientData.inData);
        auto message = ws::WebSocketMessage(clientData);
        static_cast<WebSocketContext *>(clientData.userData)->function(frame, message);
        clientData.writeData(message.rawFrame.data(), message.rawFrame.size());
    }

    void readCallbackWSS(usub::core::ClientData &clientData) {
        info_log("websocket HTTPS read");
        SSL *ssl = static_cast<SSL *>(clientData.ssl);
        std::vector<uint8_t> decrypted_data{};
        int ret{};
        BIO *rbio = SSL_get_rbio(ssl);

        if (!rbio) {
            std::cerr << "Read BIO not set." << std::endl;
            return;
        }

        decrypted_data.resize(clientData.inData.size());

        std::cout << "Encrypted data: ";
     	   for (auto byte: clientData.inData) std::cout << std::hex << static_cast<int>(byte) << " ";
        std::cout << std::endl;
        ret = BIO_write(rbio, clientData.inData.data(), clientData.inData.size());
        //Write the encrypted data to the existing rbio
        if (ret <= 0) {
	    info_log("%d", ret);
            std::cerr << "Failed to load encrypted data into BIO." << std::endl;
            return;
        }

        do {
            ret = SSL_read(ssl, decrypted_data.data(), decrypted_data.size());
        } while (ret > 0);
	
        std::cout << "Decryoted data: ";
     	   for (auto byte: decrypted_data) std::cout << std::hex << static_cast<int>(byte) << " ";
        std::cout << std::endl;


        int ssl_err = SSL_get_error(ssl, ret);
        if (ssl_err == SSL_ERROR_WANT_READ || ssl_err == SSL_ERROR_WANT_WRITE) {
            do {
                ret = BIO_read(rbio, decrypted_data.data(), decrypted_data.size());
            } while (ret > 0);
            info_log("ret is %d, error is %s", ret, ssl_err == SSL_ERROR_WANT_READ ? "Want Read" : "Want Write");
        } else {
            ERR_print_errors_fp(stderr);
        }

        auto frame = ws::parseWebSocketFrame(decrypted_data);
        auto message = ws::WebSocketMessage(clientData);
        static_cast<WebSocketContext *>(clientData.userData)->function(frame, message);
        usub::utils::ssl::encrypt_data_SSL(ssl, message.rawFrame);
        clientData.writeData(message.rawFrame.data(), message.rawFrame.size());
    }

    void WebSocketMessage::sendMessage(const std::vector<uint8_t> &data) {
        this->writeRawData(usub::server::protocols::ws::Opcode::TEXT, data, true, false);
    }

    WebSocketMessage::WebSocketMessage(core::ClientData &clientData) : clientData(clientData) {}

    usub::core::ConnectionID WebSocketMessage::getConnectionID() {
        return this->clientData.getConnectionID();
    }

    void writeCallback(usub::core::ClientData &clientData) {info_log("ws_write");}

    void errorCallback(usub::core::ClientData &clientData, ErrorStatus status) {
        auto ctx = static_cast<WebSocketContext *>(clientData.userData);
        delete ctx;
    }

    WebSocketContext::WebSocketContext(const std::function<void(ws::WebSocketFrame&, ws::WebSocketMessage&)> &function)
            : function(function) {}
}
