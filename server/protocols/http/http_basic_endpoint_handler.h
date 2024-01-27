//
// Created by Kirill Zhukov on 20.11.2023.
//

#ifndef HTTPBASICENDPOINTHANDLER_H
#define HTTPBASICENDPOINTHANDLER_H

#include <future>
#include <functional>
#include <optional>
#include "http_endpoint_handler.h"

namespace usub::server::protocols::http {
    class HttpBasicEndpointHandler final : public HttpEndpointHandler<> {
    public:
        HttpBasicEndpointHandler() = default;
        ~HttpBasicEndpointHandler() override = default;

        std::future<std::optional<std::function<void(HttpRequest &, HttpResponse &)>>> matchAsync(
            const core::REQUEST_TYPE &reqType, const std::string &data);

        std::optional<std::function<void(HttpRequest &, HttpResponse &)>> match(
            core::REQUEST_TYPE request_type, const std::string &data) override;

        void addHandler(core::REQUEST_TYPE request_type, const std::regex &regular_expression,
            std::function<void(HttpRequest &, HttpResponse &)> function) override;

        void addModule(const std::shared_ptr<HttpEndpointHandler> &module_handler) override;

        std::vector<std::tuple<core::REQUEST_TYPE, std::regex, std::function<void
        (HttpRequest &, HttpResponse &)>>> getHandlers() override;

        size_t getSize() const;

    private:
        bool equalsFunction(core::REQUEST_TYPE request_type, const std::string &data, const std::regex &e) override;

    protected:
        std::vector<std::tuple<core::REQUEST_TYPE, std::regex, std::function<void
        (HttpRequest &, HttpResponse &)>>> regexes;
    };
}; // usub::server::regex::basic

#endif //HTTPBASICENDPOINTHANDLER_H
