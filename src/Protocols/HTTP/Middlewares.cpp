#include "Protocols/HTTP/Middlewares.h"

usub::server::protocols::http::MiddlewareChain &usub::server::protocols::http::MiddlewareChain::addMiddleware(usub::server::protocols::http::MiddlewarePhase phase, std::function<MiddlewareFunctionType> middleware) {
    switch (phase) {
        case usub::server::protocols::http::MiddlewarePhase::SETTINGS:
            this->settings_middlewares_.emplace_back(std::move(middleware));
            break;
        case usub::server::protocols::http::MiddlewarePhase::HEADER:
            this->header_middlewares_.emplace_back(std::move(middleware));
            break;
        case usub::server::protocols::http::MiddlewarePhase::BODY:
            this->body_middlewares_.emplace_back(std::move(middleware));
            break;
        case usub::server::protocols::http::MiddlewarePhase::RESPONSE:
            this->response_middlewares_.emplace_back(std::move(middleware));
            break;
    }
    return *this;
}
usub::server::protocols::http::MiddlewareChain &usub::server::protocols::http::MiddlewareChain::emplace_back(usub::server::protocols::http::MiddlewarePhase phase, std::function<MiddlewareFunctionType> middleware) {
    return this->addMiddleware(phase, std::move(middleware));
}

bool usub::server::protocols::http::MiddlewareChain::execute(usub::server::protocols::http::MiddlewarePhase phase, const usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) const {
    const std::vector<std::function<MiddlewareFunctionType>> *middlewares = nullptr;

    switch (phase) {
        case usub::server::protocols::http::MiddlewarePhase::SETTINGS:
            middlewares = &this->settings_middlewares_;
            break;
        case MiddlewarePhase::HEADER:
            middlewares = &this->header_middlewares_;
            break;
        case MiddlewarePhase::BODY:
            middlewares = &this->body_middlewares_;
            break;
        case MiddlewarePhase::RESPONSE:
            middlewares = &this->response_middlewares_;
            break;
    }

    if (middlewares) {
        for (const auto &middleware: *middlewares) {
            if (!middleware(request, response)) {
                // Middleware has handled the response; halt the chain
                return false;
            }
            if (response.isSent()) {
                // Response has been sent; halt further processing
                return false;
            }
        }
    }
    return true;
}
