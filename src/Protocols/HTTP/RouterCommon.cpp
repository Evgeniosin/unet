#include "Protocols/HTTP/RouterCommon.h"

namespace usub::server::protocols::http {

    Route::Route(const std::set<std::string> &methods,
                 const std::regex regex,
                 const std::vector<std::string> &params,
                 std::function<FunctionType> handler,
                 bool accept_all)
        : param_names(params)
        , handler(handler)
        {
            pathRegex = std::regex(regex);
            if (accept_all) {
                this->accept_all_methods = accept_all;
            }
            this->allowed_method_tokenns = std::move(methods);
        }

    Route &Route::addMiddleware(MiddlewarePhase phase, std::function<MiddlewareFunctionType> middleware) {
        this->middleware_chain.addMiddleware(phase, std::move(middleware));
        return *this;
    }

}
