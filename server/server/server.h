#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <functional>
#include <thread>
#include <memory>
#include <event2/thread.h>
#include <event2/thread.h>
#include <openssl/ssl.h>
#include <nghttp2/nghttp2.h>



#include "configuration/ConfigReader.h"
#include "core/application.h"
#include "core/net.h"
#include "protocols/protocols.h"
#include "globals.h"
#include "protocols/http/http_handler_module.h"
#include "ssl/CTX_util.h"

namespace usub::server {

    namespace http = ::usub::server::protocols::http;

    // TODO this is a temporary solution and needs to be changed to better name, however for now it's ok
    typedef struct ServerData {
        ServerData(core::AppContext *app_ctx, std::shared_ptr<http::HttpBasicEndpointHandler> &basic_endpoint_handler);

        core::AppContext *app_ctx;
        std::shared_ptr<http::HttpBasicEndpointHandler> endpoint_handler;
    } ServerData;

    class Server : std::enable_shared_from_this<Server> {
    public:
        explicit Server(const std::string &config_path);

        ~Server();

        void handle(core::REQUEST_TYPE request_type, const std::string &endpoint,
            const std::function<void(http::HttpRequest &, http::HttpResponse &)> &function);

        void addModule(http::HttpHandlerModule &module);

        size_t getHandlerSize() const;

        static void eventCallback(bufferevent *bev, short events, void *ptr);

        static void acceptCallback(evconnlistener *listener, evutil_socket_t fd,
            sockaddr *addr, int addrlen, void *ptr);

        void start();
        friend void startServerThread(Server *server);

    private:
        void startListen(event_base *evbase, SSL_CTX *ssl_ctx, core::AppContext *app_ctx);
    public:
        std::shared_ptr<configuration::ConfigReader> config;
//        evconnlistener *listener;
        ServerData *server_dt;

    private:
        std::shared_ptr<http::HttpBasicEndpointHandler> endpoint_handler;
        std::vector<std::thread> handlers;
        std::vector<evconnlistener*> listeners;
    };

    SSL_CTX *create_ssl_ctx(const char *key_file, const char *cert_file);

    // TODO: What the hell is this ffs?

    static unsigned char next_proto_list[256];
    static size_t next_proto_list_len;

#ifndef OPENSSL_NO_NEXTPROTONEG
    extern int next_proto_cb(SSL *ssl, const unsigned char **data,
        unsigned int *len, void *arg);
#endif /* !OPENSSL_NO_NEXTPROTONEG */

}

#endif // SERVER_H
