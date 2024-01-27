//
// Based on Vadim Zhukov's code, 20.11.2023.
//

#ifndef HTTPENDPOINTHANDLER_H
#define HTTPENDPOINTHANDLER_H

#include <memory>
#include <regex>
#include "http_message.h"
#include "core/net.h"

namespace usub::server::protocols::http {
    template<class UserData = std::string>
    class HttpEndpointHandler {
    public:
        virtual ~HttpEndpointHandler() = default;

        virtual std::optional<std::function<void(HttpRequest &, HttpResponse &)>> match(core::REQUEST_TYPE request_type, const UserData &data) = 0;

        virtual void addHandler(core::REQUEST_TYPE request_type, const std::regex &regular_expression, std::function<void(HttpRequest &, HttpResponse &)> function) = 0;

        virtual void addModule(const std::shared_ptr<HttpEndpointHandler> &module_handler) = 0;

        virtual std::vector<std::tuple<core::REQUEST_TYPE, std::regex, std::function<void
        (HttpRequest &, HttpResponse &)>>> getHandlers() = 0;

    private:
        virtual bool equalsFunction(core::REQUEST_TYPE request_type, const UserData &data, const std::regex &e) = 0;
    };
}; // usub::server::regex
#endif //HTTPENDPOINTHANDLER_H
