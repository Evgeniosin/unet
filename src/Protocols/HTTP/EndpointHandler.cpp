#include "Protocols/HTTP/EndpointHandler.h"

namespace usub::server::protocols::http {

    void HTTPEndpointHandler::parsePathPattern(const std::string &pathPattern,
                                               std::regex &outRegex,
                                               std::vector<std::string> &outParamNames) const {
        std::string regexPattern = "^";// Start anchor
        size_t position = 0;
        const std::string specialChars = ".^$+?()[]\\|";

        if (containsCapturingGroup(pathPattern)) {
            throw std::runtime_error("Path pattern contains capturing groups, which are not allowed: \"" + pathPattern + "\"");
        }

        while (position < pathPattern.size()) {
            char c = pathPattern[position];

            if (c == ' ') {
                regexPattern += "(?:\\+|%20)";// Match spaces as '+' or '%20'
                position++;
            } else if (c == '{') {
                // Handle parameter
                size_t end = findMatchingBrace(pathPattern, position);
                if (end == std::string::npos) {
                    throw std::runtime_error("Unmatched '{' in path pattern");
                }
                std::string paramContent = pathPattern.substr(position + 1, end - position - 1);
                size_t colon = paramContent.find(':');
                std::string paramName;
                std::string paramRegex;
                if (colon != std::string::npos) {
                    paramName = paramContent.substr(0, colon);
                    paramRegex = paramContent.substr(colon + 1);
                } else {
                    paramName = paramContent;
                    paramRegex = "[^/]+";// Default regex
                }

                if (paramName.empty() || paramName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != std::string::npos) {
                    throw std::runtime_error("Invalid or empty parameter name: \"" + paramName + "\" in path pattern: \"" + pathPattern + "\"");
                }
                outParamNames.push_back(paramName);
                regexPattern += "(" + paramRegex + ")";// Capture group with custom regex
                position = end + 1;
            } else {
                regexPattern += c;
                position++;
            }
            //        } else if (c == '*') {
            //            // Handle wildcards
            //            if (position + 1 < pathPattern.size() && pathPattern[position + 1] == '*') {
            //                regexPattern += ".*"; // Double wildcard '**' matches multiple segments
            //                position += 2;
            //            } else {
            //                regexPattern += "[^/]+"; // Single wildcard '*' matches a single segment
            //                position += 1;
            //            }
            //        } else {
            //            // Escape regex special characters in literals
            //            if (specialChars.find(c) != std::string::npos) {
            //                regexPattern += '\\';
            //            }
            //            regexPattern += c;
            //            position++;
            //        }
        }

        // Optionally, ignore query parameters and fragments
        //    regexPattern += "(?:\\?.*)?$"; // Non-capturing group for query parameters

        std::cout << "Generated Regex Pattern: " << regexPattern << std::endl;// For debugging

        // Compile the regex with ECMAScript syntax
        try {
            // outRegex = std::regex(regexPattern, std::regex::ECMAScript);
            outRegex = std::regex(regexPattern, std::regex::ECMAScript | std::regex::optimize);// Optimize the regex
        } catch (const std::regex_error &e) {
            throw std::runtime_error("Invalid regex pattern: " + regexPattern + " Error: " + e.what());
        }
    }

    size_t HTTPEndpointHandler::findMatchingBrace(const std::string &pathPattern, size_t start) const {
        std::stack<char> braceStack;
        for (size_t i = start; i < pathPattern.size(); ++i) {
            if (pathPattern[i] == '{') {
                braceStack.push('{');
            } else if (pathPattern[i] == '}') {
                if (braceStack.empty()) {
                    throw std::runtime_error("Unmatched '}' in path pattern");
                }
                braceStack.pop();
                if (braceStack.empty()) {
                    return i;
                }
            }
        }
        return std::string::npos;
    }


    bool HTTPEndpointHandler::containsCapturingGroup(const std::string &regex) const {
        for (size_t i = 0; i < regex.size(); ++i) {
            if (regex[i] == '(') {
                if (i + 1 >= regex.size() || regex[i + 1] != '?') {
                    return true;// found a capturing group
                }
            }
        }
        return false;
    }


    Route &HTTPEndpointHandler::addPlainStringHandler(const std::set<std::string> &method, const std::string &pathPattern, std::function<FunctionType> function) {
        this->plainstring_routes_[pathPattern] = Route(method, std::regex(pathPattern), std::vector<std::string>{}, function);
        return this->plainstring_routes_[pathPattern];
    }


    Route &HTTPEndpointHandler::addHandler(const std::set<std::string> &method, const std::string &pathPattern, std::function<FunctionType> function) {
        std::regex pathRegex;
        std::vector<std::string> paramNames;
        parsePathPattern(pathPattern, pathRegex, paramNames);
        method.contains("*") ? this->routes_.emplace_back(method, pathRegex, paramNames, function, true)
                             : this->routes_.emplace_back(method, pathRegex, paramNames, function);
        return this->routes_.back();
    }


    Route &HTTPEndpointHandler::addHandler(std::string_view &method, const std::string &pathPattern, std::function<FunctionType> function) {
        std::regex pathRegex;
        std::vector<std::string> paramNames;
        parsePathPattern(pathPattern, pathRegex, paramNames);
        std::set<std::string> method_set{method.data()};
        method == "*" ? this->routes_.emplace_back(method_set, pathRegex, paramNames, function, true)
                      : this->routes_.emplace_back(method_set, pathRegex, paramNames, function);
        return this->routes_.back();
    }

    MiddlewareChain &HTTPEndpointHandler::addMiddleware(MiddlewarePhase phase, std::function<MiddlewareFunctionType> middleware) {
        if (phase == MiddlewarePhase::HEADER)
            this->middleware_chain_.addMiddleware(phase, std::move(middleware));
        else
            std::cerr << "Non header global middlewares are not supported yet" << std::endl;
        return this->middleware_chain_;
    }


    MiddlewareChain &HTTPEndpointHandler::getMiddlewareChain() {
        return this->middleware_chain_;
    }


    std::optional<std::pair<Route *, bool>> HTTPEndpointHandler::match(Request &request) {
        // Check if the route exists for plain string routes
        if (!this->plainstring_routes_.empty() && this->plainstring_routes_.contains(request.getURL())) {
            auto route = this->plainstring_routes_[request.getURL()];
            if (route.accept_all_methods || route.allowed_method_tokenns.contains(request.getRequestMethod())) {
                return std::make_pair(&route, true);// Route found and method allowed
            }
            return std::make_pair(&route, false);// Route found but method not allowed
        }

        // Check for routes with regular expressions
        for (auto &route: this->routes_) {
            if (!route.accept_all_methods && !route.allowed_method_tokenns.contains(request.getRequestMethod())) {
                continue;// Skip routes where the method is not allowed
            }

            std::smatch matchResult;
            std::string fullURL = request.getURL();
            if (std::regex_match(fullURL, matchResult, route.pathRegex)) {
                // Extract parameters and store them in the Request object
                for (size_t i = 0; i < route.param_names.size(); ++i) {
                    if (i + 1 < matchResult.size()) {
                        request.uri_params[route.param_names[i]] = matchResult[i + 1];
                    }
                }
                return std::make_pair(&route, true);// Route matched and method allowed
            }
        }
        return std::nullopt;// No route matched
    }


    void HTTPEndpointHandler::addErrorHandler(const std::string &error_code, std::function<FunctionType> function) {
        this->error_page_handlers_.emplace(error_code, function);
    }


    void HTTPEndpointHandler::executeErrorChain(Request &request, Response &response) {
        if (this->error_page_handlers_.contains("Log")) {
            this->error_page_handlers_.at("Log")(request, response);
        }
        std::string error = std::to_string((uint16_t) request.getState());
        if (this->error_page_handlers_.contains(error)) {
            this->error_page_handlers_.at(error)(request, response);
        }
    }
}// namespace usub::server::protocols::http