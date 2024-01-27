#ifndef HTTP_BASE_H
#define HTTP_BASE_H

#include "openssl/ssl.h"

#include <iostream>
#include <utility>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <list>
#include <optional>
#include <variant>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#  include <sys/socket.h>
#endif /* HAVE_SYS_SOCKET_H */
#ifdef HAVE_NETDB_H
#  include <netdb.h>
#endif /* HAVE_NETDB_H */
#include <csignal>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <sys/stat.h>
#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#endif /* HAVE_FCNTL_H */
#include <cctype>
#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */
#include <netinet/tcp.h>
#ifndef __sgi
#  include <err.h>
#endif
#include <cstring>
#include <cerrno>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>

#include <event.h>
#include <event2/http.h>
#include <event2/event.h>
#include <event2/bufferevent_ssl.h>
#include <event2/listener.h>
#include <nghttp2/nghttp2.h>

#include "logging/logger.h"
#include "http_basic_endpoint_handler.h"
#include "http_message.h"
#include "core/net.h"
#include "core/application.h"
#include "globals.h"
#include "socket/socket.h"


namespace usub::server::protocols::http {

    namespace core = ::usub::server::core;

    enum HTTPV : int {
        HTTP1 = 1,
        HTTP2 = 2
#if WITH_HTTP3
        HTTP3 = 2
#endif
    };

    class StatusCodeHandler {
    public:
        StatusCodeHandler(bool status_error);
        std::string getMessage(const char* statusCode);
        void setStatusError(bool statusError);
    private:
        static const std::unordered_map<std::string, std::string> statusCodeMessages;
        bool status_error = false;
    };

    class HttpContext {
    private:
    protected:
    public:
        HttpContext(core::AppContext *app_ctx,
            std::shared_ptr<HttpBasicEndpointHandler> &basic_endpoint_handler, sockaddr *addr, int addrlen);

        HttpContext(HttpContext &&other) noexcept;

        HttpContext(HttpContext &other);

        virtual ~HttpContext();

        // virtual HttpResponse getErrorPage();

        virtual void shutdownSSL(SSL *ssl);

        HttpContext &operator=(HttpContext &&other) noexcept;

        std::shared_ptr<HttpBasicEndpointHandler> endpoint_handler;
        core::AppContext *app_ctx;
        char *client_addr;
        short events = 0;
    };

} // usub::server::protocols::http namespace

#endif //HTTP_BASE_H
