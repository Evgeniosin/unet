#pragma once

#include <uvent/Uvent.h>

#include "unet/http/request.hpp"
#include "unet/http/response.hpp"

namespace usub::unet::http {


    template<class RouterType>
    class Dispatcher {
    public:
        Dispatcher() = default;
        ~Dispatcher() = default;

        usub::uvent::task::Awaitable<void> on_read(std::string_view data, usub::uvent::net::TCPClientSocket &socket) {
        }
        usub::uvent::task::Awaitable<void> on_read(std::string_view data, usub::uvent::net::TCPClientSocket &socket) {
        }
        usub::uvent::task::Awaitable<void> on_close();
        usub::uvent::task::Awaitable<void> on_error(int error_code);

    private:
        Request request_;
        Response response_;
        std::shared_ptr<RouterType> router_;
    };

    template<class RouterType>
    class ServerImpl {
    public:
        //TODO: implement constructors
        explicit ServerImpl(const ServerConfig &config);
        explicit ServerImpl(usub::Uvent &uvent);
        ServerImpl(const ServerConfig &config, usub::Uvent &uvent);
        ServerConfig() = default;

        ~ServerImpl() = default;

        auto &handle(auto &&...args) {
            return this->router_->addHandler(std::forward<decltype(args)>(args)...);
        }

        auto &addMiddleware(auto &&...args) {// allow for modifying router when wanted
            return this->router_->addMiddleware(std::forward<decltype(args)>(args)...);
        }

    private:
        ServerConfig config_;
        std::shared_ptr<RouterType> router_;
        std::shared_ptr<usub::Uvent> uvent_;
        Dispatcher<RouterType> dispatcher_;
    };


}// namespace usub::unet::http