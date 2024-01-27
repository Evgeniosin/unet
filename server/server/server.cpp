#include "server.h"

namespace usub::server {

    ServerData::ServerData(core::AppContext *app_ctx,
                           std::shared_ptr<http::HttpBasicEndpointHandler> &
                           basic_endpoint_handler) : app_ctx(app_ctx),
                                                     endpoint_handler(basic_endpoint_handler) {
    }


    void startServerThread(Server *server) {
        bool is_ssl = server->config->is_ssl();
        auto http_v = static_cast<http::HTTPV>(server->config->getProtocolVersion());
        auto key_file = server->config->getKeyFilePath();
        auto cert_file = server->config->getPemFilePath();
        event_base *evbase = event_base_new();
        {
            std::lock_guard<std::mutex> lock(eventBasesMutex);
            eventBases.push_back(evbase);
        }
        if (!key_file.empty() && !cert_file.empty() && is_ssl) {
            if (http_v == 2) {
                info_log("Uses HTTP2");
                SSL_CTX *ssl_ctx = create_ssl_ctx(key_file.c_str(),
                                                  server->config->getPemFilePath().c_str());
                core::AppContext app_ctx(ssl_ctx, evbase);

                evthread_use_pthreads();

                // int backlog = server->config->getBacklog();
                // auto ipv = static_cast<core::IPV>(server->config->getIPV());
                // std::string ip_addr = server->config->getIPAddr();
                // int soc_fd = usub::server::net::createSocket(std::stoi(server->config->getPort()), ip_addr, backlog, ipv);
                // ServerData *server_dt = new ServerData(&app_ctx, server->endpoint_handler);
                // evconnlistener *listener = evconnlistener_new(evbase, Server::acceptCallback, (void *)server_dt,
                //     LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 50, soc_fd);
                // if (listener) {
                //     info_log("Server is running on: %s", server->config->getPort().c_str());
                //     return;
                // }
                // else {
                //     const char *error_msg = evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
                //     error_log("Could not create a listener: %s", error_msg);
                // }
                server->startListen(evbase, ssl_ctx, &app_ctx);
                event_base_loop(evbase, 0);
                event_base_free(evbase);
                SSL_CTX_free(ssl_ctx);
            } else {
                int backlog = server->config->getBacklog();
                auto ipv = static_cast<core::IPV>(server->config->getIPV());
                std::string ip_addr = server->config->getIPAddr();
                info_log("Uses HTTP/1.1 with SSL");
                SSL_CTX *ssl_ctx = create_http1_ctx(key_file.c_str(),
                                                    cert_file.c_str());
                auto http1_handler = http::Http1Handler(server->endpoint_handler, evbase, ssl_ctx,
                                                        std::stoi(server->config->getPort()), ip_addr, backlog, ipv,
                                                        true,
                                                        server->config->getStatusError());
            }
        } else {
            int backlog = server->config->getBacklog();
            auto ipv = static_cast<core::IPV>(server->config->getIPV());
            std::string ip_addr = server->config->getIPAddr();
            info_log("Uses HTTP/1.1 without SSL");
            auto http1_handler = http::Http1Handler(server->endpoint_handler, evbase, nullptr,
                                                    std::stoi(server->config->getPort()), ip_addr, backlog, ipv, false,
                                                    server->config->getStatusError());
        }
    }

    Server::Server(const std::string &config_path) {
        this->config = std::make_shared<configuration::ConfigReader>(config_path);
        this->endpoint_handler = std::make_shared<http::HttpBasicEndpointHandler>();
    }

    Server::~Server() {
        for (auto &listener : listeners) {
            evconnlistener_free(listener);
        }
    }

    void Server::handle(const core::REQUEST_TYPE request_type, const std::string &endpoint,
                        const std::function<void(http::HttpRequest &,
                                                 http::HttpResponse &)> &function) {
        this->endpoint_handler->addHandler(request_type, std::regex(endpoint), function);
    }

    void Server::addModule(http::HttpHandlerModule &module) {
        this->endpoint_handler->addModule(module.getNativeHandler());
    }

    size_t Server::getHandlerSize() const {
        return this->endpoint_handler->getSize();
    }

    void Server::start() {
        int threads = this->config->getThreads();
        for (int i = 0; i < threads; i++) {
            server_threads.emplace_back(startServerThread, this);
        }
        // startServerThread(this);
    }

    void Server::startListen(event_base *evbase, SSL_CTX *ssl_ctx,
                             core::AppContext *app_ctx) {
        int backlog = this->config->getBacklog();
        auto ipv = static_cast<core::IPV>(this->config->getIPV());
        std::string ip_addr = this->config->getIPAddr();
        int soc_fd = usub::server::net::createSocket(std::stoi(this->config->getPort()), ip_addr, backlog, ipv);
        this->server_dt = new ServerData(app_ctx, this->endpoint_handler);
        auto listener = evconnlistener_new(evbase, Server::acceptCallback, (void *) server_dt,
                                            LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 50, soc_fd);
        if (listener) {
            info_log("Server is running on: %s", this->config->getPort().c_str());
            return;
        } else {
            const char *error_msg = evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
            error_log("Could not create a listener: %s", error_msg);
        }
        this->listeners.push_back(listener);
    }

    void Server::eventCallback(bufferevent *bev, short events, void *ptr) {
        bool shouldTerminate = false;
        http::HttpContext *ctx = (http::HttpContext *) ptr;
        if (events & BEV_EVENT_CONNECTED) {
            const unsigned char *alpn = nullptr;
            unsigned int alpnlen = 0;
	    bool is_http2 = 0;
            SSL *ssl = bufferevent_openssl_get_ssl(bev);

#ifndef OPENSSL_NO_NEXTPROTONEG
            SSL_get0_next_proto_negotiated(ssl, &alpn, &alpnlen);
#endif /* !OPENSSL_NO_NEXTPROTONEG */
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
            if (alpn == nullptr) {
                SSL_get0_alpn_selected(ssl, &alpn, &alpnlen);
            }
#endif /* OPENSSL_VERSION_NUMBER >= 0x10002000L */

            if (alpn != nullptr && alpnlen >= 2 && memcmp("h2", alpn, 2) == 0) {
		is_http2 = 1;
                http::Http2Context *new_ctx = new http::Http2Context(std::move(*ctx), bev);
                delete ctx;
                ctx = new_ctx;
                new_ctx->initializeNghttp2Session();
                bufferevent_setcb(new_ctx->bev, http::Http2Context::readCallback, http::Http2Context::writeCallback,
                                  http::Http2Context::eventCallback, new_ctx);
                if (new_ctx->sendServerConnectionHeader() != 0 ||
                    new_ctx->sessionSend() != 0) {
                    delete (http::Http2Context *) new_ctx;
                    ctx = nullptr;
                    return;
                }
            } else {
                http::Http1Context *new_ctx = new http::Http1Context(ctx, bev);
                delete ctx;
                ctx = new_ctx;
                bufferevent_setcb(new_ctx->bev, http::Http1Context::readCallback, http::Http1Context::writeCallback,
                                  http::Http1Context::eventCallback, new_ctx);
            }
            info_log("%s connected, protocol: %s.", (ctx->client_addr), (is_http2 ? "HTTP2" : "HTTP1 or an unsupported proto"));
            return;
        }

        if (events & BEV_EVENT_TIMEOUT) {
            error_log("Timeout, disconnecting: %s", ctx->client_addr);
            shouldTerminate = true;
        } else if (events & (BEV_EVENT_READING | BEV_EVENT_WRITING)) {
            error_log("Error during %s data from client: %s",
                      (events & BEV_EVENT_READING) ? "reading" : "writing",
                      ctx->client_addr);
            shouldTerminate = true;
        } else if (events & BEV_EVENT_EOF) {
            error_log("Received EOF from client: %s", ctx->client_addr);
            shouldTerminate = true;
        } else if (events & BEV_EVENT_ERROR) {
            error_log("Error: %s", ctx->client_addr);
            shouldTerminate = true;
        }
        if (shouldTerminate) {
            bufferevent_setcb(bev, NULL, NULL, NULL, NULL);
            delete ctx;
            ctx = nullptr;
            return;
        }

    }

    void Server::acceptCallback(evconnlistener *listener, int fd,
                                sockaddr *addr, int addrlen, void *arg) {
        auto *server_dt = (ServerData *) arg;
        http::HttpContext *ctx = new http::HttpContext(server_dt->app_ctx, server_dt->endpoint_handler,
                                                       addr, addrlen);
        SSL *ssl = create_ssl(server_dt->app_ctx->ssl_ctx);
        int val = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &val, sizeof(val));

        bufferevent *bev = bufferevent_openssl_socket_new(
                server_dt->app_ctx->evbase, fd, ssl, BUFFEREVENT_SSL_ACCEPTING,
                BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
        bufferevent_enable(bev, EV_READ | EV_WRITE);

        ssl = bufferevent_openssl_get_ssl(bev);

        bufferevent_setcb(bev, NULL, NULL, Server::eventCallback, ctx);
    }

    SSL_CTX *create_ssl_ctx(const char *key_file, const char *cert_file) {
        SSL_CTX *ssl_ctx;

        ssl_ctx = SSL_CTX_new(TLS_server_method());
        if (!ssl_ctx) {
            error_log("Could not create SSL/TLS context: %s",
                      ERR_error_string(ERR_get_error(), nullptr));
        }
        SSL_CTX_set_options(ssl_ctx,
                            SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 |
                            SSL_OP_NO_COMPRESSION |
                            SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
        if (SSL_CTX_set1_curves_list(ssl_ctx, "P-256") != 1) {
            error_log("SSL_CTX_set1_curves_list failed: %s",
                      ERR_error_string(ERR_get_error(), nullptr));
        }
#else  /* !(OPENSSL_VERSION_NUMBER >= 0x30000000L) */
        {
            EC_KEY *ecdh;
            ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
            if (!ecdh) {
                error_log("EC_KEY_new_by_curv_name failed: %s",
                    ERR_error_string(ERR_get_error(), nullptr));
            }
            SSL_CTX_set_tmp_ecdh(ssl_ctx, ecdh);
            EC_KEY_free(ecdh);
        }
#endif /* !(OPENSSL_VERSION_NUMBER >= 0x30000000L) */

        if (SSL_CTX_use_PrivateKey_file(ssl_ctx, key_file, SSL_FILETYPE_PEM) != 1) {
            error_log("Could not read private key file %s", key_file);
        }
        if (SSL_CTX_use_certificate_chain_file(ssl_ctx, cert_file) != 1) {
            error_log("Could not read certificate file %s", cert_file);
        }

        next_proto_list[0] = NGHTTP2_PROTO_VERSION_ID_LEN;
        memcpy(&next_proto_list[1], NGHTTP2_PROTO_VERSION_ID,
               NGHTTP2_PROTO_VERSION_ID_LEN);
        next_proto_list_len = 1 + NGHTTP2_PROTO_VERSION_ID_LEN;

#ifndef OPENSSL_NO_NEXTPROTONEG
        SSL_CTX_set_next_protos_advertised_cb(ssl_ctx, next_proto_cb, nullptr);
#endif /* !OPENSSL_NO_NEXTPROTONEG */

#if OPENSSL_VERSION_NUMBER >= 0x10002000L
        SSL_CTX_set_alpn_select_cb(ssl_ctx, alpn_select_proto_cb, nullptr);
#endif /* OPENSSL_VERSION_NUMBER >= 0x10002000L */

        return ssl_ctx;
    }

    // TODO: What the hell is this ffs?

#ifndef OPENSSL_NO_NEXTPROTONEG

    int next_proto_cb(SSL *ssl, const unsigned char **data,
                      unsigned int *len, void *arg) {
        (void) ssl;
        (void) arg;

        *data = next_proto_list;
        *len = (unsigned int) next_proto_list_len;
        return SSL_TLSEXT_ERR_OK;
    }

#endif

}
