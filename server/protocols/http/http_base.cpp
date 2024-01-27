#include "http_base.h"

namespace usub::server::protocols::http {

    StatusCodeHandler::StatusCodeHandler(bool status_error) : status_error(status_error) {};

    std::string StatusCodeHandler::getMessage(const char *statusCode) {
#if 0
        if (statusCodeMessages.find(statusCode) != statusCodeMessages.end()) {
#endif
        if (statusCodeMessages.contains(statusCode)) {
            return statusCodeMessages.at(statusCode);
        }
        if (this->status_error) {
            error_log("Status code isn't found: %s", statusCode);
        }
        return "";
    }

    void StatusCodeHandler::setStatusError(bool statusError) {
        this->status_error = statusError;
    }

    const std::unordered_map<std::string, std::string> StatusCodeHandler::statusCodeMessages = {
            {"100", "Continue"},
            {"101", "Switching Protocols"},
            {"102", "Processing"},
            {"103", "Early Hints"},
            {"200", "OK"},
            {"201", "Created"},
            {"202", "Accepted"},
            {"203", "Non-Authoritative Information"},
            {"204", "No Content"},
            {"205", "Reset Content"},
            {"206", "Partial Content"},
            {"207", "Multi-Status"},
            {"208", "Already Reported"},
            {"226", "IM Used"},
            {"300", "Multiple Choices"},
            {"301", "Moved Permanently"},
            {"302", "Found"},
            {"303", "See Other"},
            {"304", "Not Modified"},
            {"306", "Switch Proxy"},
            {"307", "Temporary Redirect"},
            {"308", "Resume Incomplete"},
            {"400", "Bad Request"},
            {"401", "Unauthorized"},
            {"402", "Payment Required"},
            {"403", "Forbidden"},
            {"404", "Not Found"},
            {"405", "Method Not Allowed"},
            {"406", "Not Acceptable"},
            {"407", "Proxy Authentication Required"},
            {"408", "Request Timeout"},
            {"409", "Conflict"},
            {"410", "Gone"},
            {"411", "Length Required"},
            {"412", "Precondition Failed"},
            {"413", "Request Entity Too Large"},
            {"414", "Request-URI Too Long"},
            {"415", "Unsupported Media Type"},
            {"416", "Requested Range Not Satisfiable"},
            {"417", "Expectation Failed"},
            {"418", "I'm a teapot"},
            {"421", "Misdirected Request"},
            {"422", "Unprocessable Entity"},
            {"423", "Locked"},
            {"424", "Failed Dependency"},
            {"426", "Upgrade Required"},
            {"428", "Precondition Required"},
            {"429", "Too Many Requests"},
            {"431", "Request Header Fields Too Large"},
            {"440", "Login Time-out"},
            {"444", "Connection Closed Without Response"},
            {"449", "Retry With"},
            {"450", "Blocked by Windows Parental Controls"},
            {"451", "Unavailable For Legal Reasons"},
            {"494", "Request Header Too Large"},
            {"495", "SSL Certificate Error"},
            {"496", "SSL Certificate Required"},
            {"497", "HTTP Request Sent to HTTPS Port"},
            {"499", "Client Closed Request"},
            {"500", "Internal Server Error"},
            {"501", "Not Implemented"},
            {"502", "Bad Gateway"},
            {"503", "Service Unavailable"},
            {"504", "Gateway Timeout"},
            {"505", "HTTP Version Not Supported"},
            {"506", "Variant Also Negotiates"},
            {"507", "Insufficient Storage"},
            {"508", "Loop Detected"},
            {"509", "Bandwidth Limit Exceeded"},
            {"510", "Not Extended"},
            {"511", "Network Authentication Required"},
            {"520", "Unknown Error"},
            {"521", "Web Server Is Down"},
            {"522", "Connection Timed Out"},
            {"523", "Origin Is Unreachable"},
            {"524", "A Timeout Occurred"},
            {"525", "SSL Handshake Failed"},
            {"526", "Invalid SSL Certificate"},
            {"527", "Railgun Listener to Origin Error"},
            {"530", "Origin DNS Error"},
            {"598", "Network Read Timeout Error"},
    };

    HttpContext::HttpContext(core::AppContext *app_ctx,
                             std::shared_ptr<HttpBasicEndpointHandler> &basic_endpoint_handler,
                             sockaddr *addr, int addrlen) : app_ctx(app_ctx), endpoint_handler(basic_endpoint_handler),
                                                            client_addr(nullptr) {
        int rv;
        char host[NI_MAXHOST];
        this->app_ctx = app_ctx;
        rv = getnameinfo(addr, (socklen_t) addrlen, host, sizeof(host), NULL, 0,
                         NI_NUMERICHOST);
        if (rv != 0) {
            this->client_addr = strdup("(unknown)");
        } else {
            this->client_addr = strdup(host);
        }
    }

    HttpContext::HttpContext(HttpContext &&other) noexcept
            : app_ctx(other.app_ctx),
              endpoint_handler(std::move(other.endpoint_handler)),
              client_addr(other.client_addr),
              events(other.events) {
        other.client_addr = nullptr;
    }

    HttpContext::HttpContext(HttpContext &other) : app_ctx(other.app_ctx), endpoint_handler(other.endpoint_handler),
                                                   client_addr(strdup(other.client_addr)), events(other.events) {
    }


    HttpContext::~HttpContext() {
        if (this->client_addr != nullptr) {
            free(this->client_addr);
            this->client_addr = nullptr;
        }
    }


    HttpContext &HttpContext::operator=(HttpContext &&other) noexcept {
        if (this != &other) {
            if (client_addr != nullptr) {
                free(client_addr);
            }

            app_ctx = other.app_ctx;
            endpoint_handler = std::move(other.endpoint_handler);
            client_addr = other.client_addr;
            events = other.events;

            other.client_addr = nullptr;
        }
        return *this;
    }

    // HttpResponse HttpContext::getErrorPage() {
    //     return HttpResponse();
    // }

    void HttpContext::shutdownSSL(SSL *ssl) {
        if (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) {
            info_log("Shutdown alert recieved from peer.");
            int res = SSL_shutdown(ssl);
            if (res == 1) {
                info_log("shutdown was succesfull in first attempt.");
            } else if (res == -1) {
                error_log("res: %d", res);
                unsigned long err = ERR_get_error();
                char *msg = ERR_error_string(err, nullptr);
                error_log("SSL error: %s, err_code: %lu", msg, err);
            }
        } else if (!(SSL_get_shutdown(ssl) & (SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN)) &&
                   !(events & BEV_EVENT_EOF) &&
                   !(events & BEV_EVENT_ERROR) && !(SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN)) {
            info_log("Shutdown alert sent to peer: %s.", this->client_addr);
            int res = SSL_shutdown(ssl);
            if (res == 1) {
                info_log("shutdown was succesfull in first attempt");
            } else if (res == 0) {
                char buf[1024];
                for (int i = 0; i <= 100; i++) {
                    if (SSL_get_error(ssl, res) == SSL_ERROR_WANT_READ || SSL_get_error(ssl, res) ==
                                                                          SSL_ERROR_WANT_WRITE) {
                        for (int result = SSL_get_error(ssl, SSL_read(ssl, buf, sizeof(buf)));
                             result != SSL_ERROR_ZERO_RETURN;) {
                            info_log("waiting for conn to be closed, %d", result);
                        }
                    }
                }
            } else {
                error_log("res: %d", res);
                unsigned long err = ERR_get_error();
                char *msg = ERR_error_string(err, nullptr);
                error_log("SSL error: %s, err_code: %lu", msg, err);
            }
        } else {
            info_log("Shutdown request ignored.");
        }
    }

}
