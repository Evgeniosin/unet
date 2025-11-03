#pragma once

#include "Protocols/HTTP/HTTP1.h"
#include "Protocols/HTTP/Message.h"
#include "server/AwaitableHandler.h"
#include <Uvent.h>
#include <iostream>
#include <net/Socket.h>
#include <tasks/Awaitable.h>

namespace usub::client {

    class HttpClient {
    private:
        usub::uvent::net::SocketHolder socket_;
        usub::server::protocols::http::Request request_;
        usub::server::protocols::http::Response response_;

    public:
        HttpClient() = default;
        HttpClient(std::string url) {
            connect(url);
        }
        ~HttpClient() = default;

        usub::uvent::task::Awaitable<bool> connect(const std::string &url) {
            std::string host;
            std::string port = "80";

            size_t protocol_end = url.find("//");
            size_t host_start = (protocol_end != std::string::npos) ? protocol_end + 2 : 0;
            size_t host_end = url.find_first_of(":/", host_start);

            if (host_end == std::string::npos) {
                host = url.substr(host_start);
            } else {
                host = url.substr(host_start, host_end - host_start);
                if (url[host_end] == ':') {
                    size_t port_start = host_end + 1;
                    size_t port_end = url.find('/', port_start);
                    port = url.substr(port_start, port_end - port_start);
                } else if (url.compare(0, 5, "https") == 0) {
                    port = "443";
                }
            }

            auto res = co_await socket_->async_connect(host.c_str(), port.c_str());
            co_return !res.has_value();
        }


        usub::uvent::task::Awaitable<bool> connect(const std::string &host, const std::string &port) {
            auto res = co_await socket_->async_connect(host.c_str(), port.c_str());
            co_return !res.has_value();
        }

        usub::server::protocols::http::Request &getRequest() { return this->request_; }

        usub::uvent::task::Awaitable<usub::server::protocols::http::Response *> send() {
            std::string serialized = request_.string();
            auto result = co_await socket_->async_send(
                    const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(serialized.data())),
                    serialized.size());
            std::cout << "Serialized request:\n"
                      << serialized << std::flush;
            if (!result.has_value()) {
                std::cerr << "Send failed: " << toString(result.error()) << std::endl;
                co_return &response_;
            }

            while (this->response_.getState() != usub::server::protocols::http::RESPONSE_STATE::FINISHED) {
                uint8_t buffer[16384 * 4];
                memset(buffer, '\0', sizeof(buffer));
                std::cout << "Before read" << std::endl;
                ssize_t read_sz = co_await socket_->async_read(buffer, sizeof(buffer));
                if (read_sz < 0) {
                    std::cerr << "Read failed or connection closed." << std::endl;
                    co_return &response_;
                } else if (read_sz == 0) {
                    std::cerr << "No data read." << std::endl;
                    if (buffer[0] != 0) {
                        std::cerr << "Buffer not empty on read_sz == 0: " << buffer << std::endl;
                    }
                } else {
                    std::cout << "Read size: " << read_sz << std::endl;
                    std::string response_text(reinterpret_cast<char *>(buffer), static_cast<size_t>(read_sz));
                    response_.parse<usub::server::protocols::http::VERSION::HTTP_1_X>(response_text);
                }
            }


            co_return &response_;
        }

        usub::server::protocols::http::Response &getResponse() { return response_; }
    };

}// namespace usub::client
