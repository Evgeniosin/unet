//
// Created by Kirill Zhukov on 20.11.2023.
//

#include "http_basic_endpoint_handler.h"

namespace usub::server::protocols::http {
    std::future<std::optional<std::function<void(HttpRequest &, HttpResponse &)>>> HttpBasicEndpointHandler::
        matchAsync(const core::REQUEST_TYPE &reqType, const std::string &data) {
        return std::async(std::launch::async, [this, reqType, data]() {
            return match(reqType, data); // Call the synchronous match function
            });
    }

    std::optional<std::function<void(HttpRequest &, HttpResponse &)>> HttpBasicEndpointHandler::match(
        core::REQUEST_TYPE request_type, const std::string &data) {
        if (const auto it = std::ranges::find_if(regexes, [&request_type, &data, this](const auto &tuple) {
            return std::get<0>(tuple) == request_type && equalsFunction(request_type, data, std::get<1>(tuple));
            }); it != regexes.end()) {
            return std::get<2>(*it);
        }
        return std::nullopt;
    }

    void HttpBasicEndpointHandler::addHandler(core::REQUEST_TYPE request_type, const std::regex &regular_expression,
        std::function<void(HttpRequest &,
            HttpResponse &)> function) {
        this->regexes.emplace_back(request_type, regular_expression, function);
    }

    void HttpBasicEndpointHandler::addModule(const std::shared_ptr<HttpEndpointHandler> &module_handler) {
        auto vec = module_handler->getHandlers();
        this->regexes.insert(this->regexes.end(), vec.begin(), vec.end());
    }

    std::vector<std::tuple<core::REQUEST_TYPE, std::regex, std::function<void(HttpRequest &, HttpResponse &)>>>
        HttpBasicEndpointHandler::getHandlers() {
        return this->regexes;
    }

    size_t HttpBasicEndpointHandler::getSize() const {
        return this->regexes.size();
    }

    bool HttpBasicEndpointHandler::equalsFunction(core::REQUEST_TYPE request_type, const std::string &data,
        const std::regex &e) {
        return std::regex_match(data, e);
    }
}; // usub::server::regex::basic
