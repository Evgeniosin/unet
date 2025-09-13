#ifdef USE_OPEN_SSL
#include <Modules/StreamHandlers/OpenSSL.h>
#endif

#include "Protocols/HTTP/Message.h"
#include "server/server.h"

struct BalanceInfoRequest {
    bool is_null_balance_showed{true};
};
struct BalanceInfoResponse {
    std::string currency;
    std::string amount;
};


template<class Handler>
using PlainServer = usub::server::ServerImpl<Handler, usub::server::PlainHTTPStreamHandler>;

using RegexServer = PlainServer<usub::server::protocols::http::HTTPEndpointHandler>;
using RadixServer = PlainServer<usub::server::protocols::http::RadixRouter>;


#ifdef USE_OPEN_SSL
template<class Handler>
using TLSServer = usub::server::ServerImpl<Handler, usub::server::TLSHTTPStreamHandler>;

using RegexServerWithSSL = TLSServer<usub::server::protocols::http::HTTPEndpointHandler>;
using RadixServerWithSSL = TLSServer<usub::server::protocols::http::RadixRouter>;

using MixedServer = usub::server::Server<usub::server::protocols::http::HTTPEndpointHandler,
                                         usub::server::PlainHTTPStreamHandler,
                                         usub::server::TLSHTTPStreamHandler>;

using MixedServerRadix = usub::server::Server<usub::server::protocols::http::RadixRouter,
                                              usub::server::PlainHTTPStreamHandler,
                                              usub::server::TLSHTTPStreamHandler>;
#endif

uint64_t request_cntr{};

bool headerMiddle(const usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) {
    std::cout << "header middleware reached" << std::endl;
    return true;
}

bool globalMiddle(const usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) {
    request_cntr++;
    //    std::cout<<"global middleware reached"<<std::endl;
    return true;
}


bool bodyMiddle(const usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) {
    //    std::cout<<"body middleware reached"<<std::endl;
    return true;
}

bool responseMiddle(const usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) {
    // std::cout << "request middleware reached " << request_cntr << std::endl;
    return true;
}

void handlerFunction(usub::server::protocols::http::Request &request, usub::server::protocols::http::Response &response) {

    auto headers = request.getHeaders();
    for (const auto &[name, values]: headers) {
        std::cout << "Header: " << name << "\n";
        for (const auto &val: values) {
            std::cout << "  Value: " << val << "\n";
        }
    }
    std::cout << "Matched :" << request.getURL() << std::endl;
    for (auto &[k, v]: request.uri_params) {
        std::cout << "param[" << k << "] = " << v << '\n';
    }
    std::cout << "Query Params:\n";
    for (const auto &[key, values]: request.getQueryParams()) {
        std::cout << "key: " << key << '\n';
        for (const auto &value: values) {
            std::cout << "  value: " << value << '\n';
        }
    }
    response.setStatus(200)
            .setMessage("OK")
            .addHeader("Content-Type", "text/html")
            .setBody("Hello World! How are you \n");
    return;
}

const usub::server::protocols::http::param_constraint uuid{
        R"([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})",
        "Must be a valid UUID"};

int main() {
    // RegexServer server("../config/config.toml");
    RadixServer server("../config/config.toml");
    // RegexServerWithSSL server("../config/https.toml");
    // MixedServer server("../config/https.toml");
    // MixedServerRadix server("../config/https.toml");

    server.addMiddleware(usub::server::protocols::http::MiddlewarePhase::HEADER, globalMiddle);

    // server.handle({"*"}, R"(/hello)", handlerFunction, {}) // Radix

    // server.handle({"*"}, R"(/hello)", handlerFunction) // Regex
    //         .addMiddleware(usub::server::protocols::http::MiddlewarePhase::HEADER, headerMiddle)
    //         .addMiddleware(usub::server::protocols::http::MiddlewarePhase::RESPONSE, responseMiddle);

    // addRoute(server, handlerFunction, "HelloWorld", "GET", "/hello", JSON(HelloRequest), JSON(HelloResponse));
    //         .addMiddleware(usub::server::protocols::http::MiddlewarePhase::HEADER, headerMiddle)
    //         .addMiddleware(usub::server::protocols::http::MiddlewarePhase::RESPONSE, responseMiddle);

    std::unordered_map<std::string_view, const usub::server::protocols::http::param_constraint *> constraints = {
            {"id", &uuid}};


    server.run();
}

// curl -v https://127.0.0.1:8111/hello
// curl --cacert ./build/cert.pem -v https://localhost:8111/hello
