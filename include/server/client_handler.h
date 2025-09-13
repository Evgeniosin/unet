#ifndef USUB_SERVER_CLIENT_HANDLER_H
#define USUB_SERVER_CLIENT_HANDLER_H

#include <memory>
#include <net/Socket.h>
#include <string>
#include <tasks/Awaitable.h>

#include "Protocols/HTTP/EndpointHandler.h"
#include "Protocols/HTTP/HTTP1.h"


namespace usub::server {
    struct ClientHandler : IEIRAwaitableVoid {
        explicit ClientHandler(usub::uvent::net::Socket *socket, const std::shared_ptr<usub::server::protocols::http::HTTPEndpointHandler> &endpoint_handler);

        bool await_ready() noexcept override;

        bool await_suspend(std::coroutine_handle<> handle) noexcept;

        void await_resume() override;

        IEIRAwaitableVoid run();

    private:
        usub::uvent::net::Socket *socket_{nullptr};
    };
}// namespace usub::server

#endif//USUB_SERVER_CLIENT_HANDLER_H
