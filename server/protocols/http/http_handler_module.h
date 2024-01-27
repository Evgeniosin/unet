//
// Created by Kirill Zhukov on 29.11.2023.
//

#ifndef HTTPHANDLERMODULE_H
#define HTTPHANDLERMODULE_H

#include <functional>
#include "core/net.h"
#include "http_endpoint_handler.h"

namespace usub::server::protocols::http {

    class HttpHandlerModule {
    public:
        virtual ~HttpHandlerModule() = default;

        virtual void handle(core::REQUEST_TYPE request_type, const std::string &endpoint,
            const std::function<void(HttpRequest &, HttpResponse &)> &function) = 0;

        virtual std::shared_ptr<HttpEndpointHandler<>> getNativeHandler() = 0;
    };
}; // usub::server::protocols::http

#endif //HTTPHANDLERMODULE_H
