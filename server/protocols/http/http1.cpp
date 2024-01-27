#include "http1.h"

namespace usub::server::protocols::http {

    Http1Response::~Http1Response() {
        if (this->reason)
            std::free(const_cast<char *>(this->reason));
    }

    bool Http1Response::writeFile(const std::string &file_path) {
        if (this->type != core::NONE) {
            return false;
        }
        int fd = open(file_path.c_str(), O_RDONLY);
        if (fd < 0) {
            error_log("File not opened: %s, error: %s", file_path.c_str(), strerror(errno));
            return false;
        }
        if (fstat(fd, &this->st) < 0) {
            close(fd);
            error_log("File corrupted: %s, file size: %d", file_path.c_str(), fstat(fd, &this->st));
            return false;
        }
        this->data = fd;
        this->type = core::FD;
        this->path = file_path;
        return true;
    }

    bool Http1Response::writeRawData(const uint8_t *buf, size_t length) {
        if (this->type != core::NONE) {
            return false;
        }
        this->data = std::make_shared<std::vector<uint8_t>>(buf, buf + length);
        this->type = core::BUFFER;
        return true;
    }

    std::optional<std::shared_ptr<std::vector<uint8_t>>> Http1Response::getBuffer() const {
        if (std::holds_alternative<std::shared_ptr<std::vector<uint8_t>>>(data)) {
            return std::get<std::shared_ptr<std::vector<uint8_t>>>(data);
        }
        return std::nullopt;
    }

    int Http1Response::getFD() const {
        if (std::holds_alternative<int>(this->data)) {
            return std::get<int>(this->data);
        }
        return -1;
    }

    void Http1Response::addHeader(char *name, const char *value) {
        this->headers.emplace_back(name, value);
    }

    void Http1Response::addHeaders(const std::vector<std::pair<std::string, std::string>> &headers) {
        this->headers = std::move(headers);
    }

    void Http1Response::setStatus(const char *status) {
        this->status = status;
        this->reason = strdup(this->status_code_handler.getMessage(status).c_str());
    }


    void Http1Response::write_chunked_response(std::vector<uint8_t> &auxBuf, const uint8_t *data, size_t size) {
        char chunk_size[10];
        int len = snprintf(chunk_size, sizeof(chunk_size), "%zx\r\n", size);
        if (len > 0 && len < sizeof(chunk_size)) {
            auxBuf.insert(auxBuf.end(), chunk_size, chunk_size + len);
            auxBuf.insert(auxBuf.end(), data, data + size);
            auxBuf.insert(auxBuf.end(), "\r\n", &"\r\n"[2]);
        }
    }

    void Http1Response::prepare_buffer(bufferevent *bev) {
        std::vector<uint8_t> auxiliary_buf;
        std::string statusLine = "HTTP/1.1 ";
        statusLine.append((this->status != nullptr) ? this->status : "500").append(" ").append(
                (this->reason != nullptr) ? this->reason : "Internal Server Error").append("\r\n");
        auxiliary_buf.reserve(statusLine.size() + this->headers.size() * 20);
        auxiliary_buf.insert(auxiliary_buf.end(), statusLine.begin(), statusLine.end());
        for (auto &[name, value]: headers) {
            std::string header = name + ": " + value + "\r\n";
            auxiliary_buf.insert(auxiliary_buf.end(), header.begin(), header.end());
        }
        std::string tr_en = "Transfer-Encoding: chunked\r\n\r\n";
        auxiliary_buf.insert(auxiliary_buf.end(), tr_en.begin(), tr_en.end());

        switch (this->type) {
            case core::FD: {
                FILE *file = fopen(this->path.c_str(), "rb");
                uint8_t *file_data;

                if (file) {
                    file_data = (uint8_t *) malloc(this->st.st_size);
                    if (file_data) {
                        fread(file_data, 1, this->st.st_size, file);
                        this->write_chunked_response(auxiliary_buf, file_data, this->st.st_size);
                        fclose(file);
                        free(file_data);
                    }
                }
                break;
            }
            case core::BUFFER: {
                auto res_buf = this->getBuffer().value();
                this->write_chunked_response(auxiliary_buf, res_buf->data(), res_buf->size());
                break;
            }
            default:
                break;
        }
        auxiliary_buf.insert(auxiliary_buf.end(), "0\r\n\r\n", &"0\r\n\r\n"[5]);
        bufferevent_write(bev, auxiliary_buf.data(), auxiliary_buf.size());
    }

    void Http1Response::setReason(const char *reason) {
        this->reason = reason;
    }


    Http1Context::Http1Context(HttpContext *base, bufferevent *bev) : HttpContext(*base), bev(std::move(bev)) {
        this->http_response = new Http1Response();
        char host[NI_MAXHOST];

        timeval timeout = {10, 0};
        bufferevent_set_timeouts(this->bev, &timeout, &timeout);
    }

    Http1Context::~Http1Context() {
        delete this->http_response;
        SSL *ssl = bufferevent_openssl_get_ssl(this->bev);
        info_log("%s disconnected", this->client_addr);
#if DEBUG_EXTENDED_INFO
        printf("destructor\tSSL_in_init: %d, SSL_in_before: %d, SSL_is_init_finished: %d, SSL_in_connect_init: %d, SSL_in_accept_init: %d, SSL_get_state: %d\n",
            SSL_in_init(ssl), SSL_in_before(ssl), SSL_is_init_finished(ssl), SSL_in_connect_init(ssl), SSL_in_accept_init(ssl), SSL_get_state(ssl));
#endif
        shutdownSSL(ssl);

        bufferevent_free(this->bev);
        delete this->parser;
        this->parser = nullptr;
    }

    void Http1Context::eventCallback(bufferevent *bev, short events, void *ptr) {
        bool shouldTerminate = false;
        Http1Context *ctx = (Http1Context *) ptr;
        if (events & BEV_EVENT_TIMEOUT) {
            error_log("Timeout, disconnecting: %s", ctx->client_addr);
            shouldTerminate = true;
        } else if (events & (BEV_EVENT_READING | BEV_EVENT_WRITING)) {
            error_log("Error during %s data from client: %s",
                      (events & BEV_EVENT_READING) ? "reading" : "writing",
                      (ctx) ? ctx->client_addr : "null");
            shouldTerminate = true;
        } else if (events & BEV_EVENT_EOF) {
            error_log("Received EOF from client: %s", ctx->client_addr);
            shouldTerminate = true;
        } else if (events & BEV_EVENT_ERROR) {
            error_log("Error: %s", ctx->client_addr);
            shouldTerminate = true;
        }
        if (ctx) {
            ctx->events = events;
            if (shouldTerminate) {
                bufferevent_setcb(bev, nullptr, nullptr, nullptr, nullptr);
                delete ctx;
                ctx = nullptr;
                return;
            }
        }
    }

    void Http1Context::readCallback(bufferevent *bev, void *ptr) {
        auto *ctx = (Http1Context *) ptr;
        auto parser = new http::Http1Parser(bufferevent_get_input(bev));
        ctx->parser = parser;
        if (parser->parse_request() != -1) {
            auto match = ctx->endpoint_handler->match(
                    parser->request_type,
                    parser->uri);
            if (match.has_value()) {
                auto *res = ctx->http_response;
                auto req = parser->getRequest();
                req.url = parser->uri;
                req.headers.emplace((char *) ":path", parser->uri);
                try {
                    match.value()(req, *res);
                    res->prepare_buffer(bev);
                } catch (std::exception &e) {
                    error_log("Error: %s", e.what());
                    auto err_res = ctx->getErrorPage(-1);
                    err_res.prepare_buffer(bev);
                }
            } else {
                auto res = ctx->getErrorPage(-1);
                res.prepare_buffer(bev);
            }
        } else {
            delete ctx;
            ctx = nullptr;
        }
    }

    void Http1Context::writeCallback(bufferevent *bev, void *ptr) {
        auto *ctx = (Http1Context *) ptr;

    }

    Http1Response Http1Context::getErrorPage(int32_t stream_id) {
        const std::string data = "<html><head><title>404</title></head>"
                                 "<body><h1>404 Not Found</h1></body></html>";
        this->http_response->writeRawData(reinterpret_cast<const uint8_t *>(data.data()), data.length());
        this->http_response->addHeader((char *) "Content-Type", (char *) "text/html; charset=utf-8");
        this->http_response->setStatus("404");
        return *this->http_response;
    }

    // TODO: EVERYTHING BELOW THIS MUST AND WILL BE BE CHANGED 

    Http1ModuleResponse::Http1ModuleResponse(evhttp_request *request) : req(request) {
        this->buf = evbuffer_new();
    }

    Http1ModuleResponse::~Http1ModuleResponse() {
        if (this->type == core::FD) {
            close(this->getFD());
        }
        if (this->reason) {
            free((void *) this->reason);
        }
    }

    bool Http1ModuleResponse::writeFile(const std::string &file_path) {
        if (this->type != core::NONE) {
            return false;
        }
        int fd = open(file_path.c_str(), O_RDONLY);
        if (fd < 0) {
            error_log("File not opened: %s, error: %s", file_path.c_str(), strerror(errno));
            evhttp_send_error(this->req, HTTP_NOTFOUND, "File not found");
            return false;
        }
        if (fstat(fd, &this->st) < 0) {
            close(fd);
            error_log("File corrupted: %s, file size: %d", file_path.c_str(), fstat(fd, &this->st));
            evhttp_send_error(this->req, HTTP_INTERNAL, "Internal Server Error");
            return false;
        }
        this->data = fd;
        this->type = core::FD;
        return true;
    }

    bool Http1ModuleResponse::writeRawData(const uint8_t *buf, size_t length) {
        if (this->type != core::NONE) {
            return false;
        }
        this->data = std::make_shared<std::vector<uint8_t>>(buf, buf + length);
        this->type = core::BUFFER;
        return true;
    }

    std::optional<std::shared_ptr<std::vector<uint8_t>>> Http1ModuleResponse::getBuffer() const {
        if (std::holds_alternative<std::shared_ptr<std::vector<uint8_t>>>(data)) {
            return std::get<std::shared_ptr<std::vector<uint8_t>>>(data);
        }
        return std::nullopt;
    }

    int Http1ModuleResponse::getFD() const {
        if (std::holds_alternative<int>(this->data)) {
            return std::get<int>(this->data);
        }
        return -1;
    }

    void Http1ModuleResponse::addHeader(char *name, const char *value) {
        evhttp_add_header(evhttp_request_get_output_headers(this->req), name, value);
    }

    void
    Http1ModuleResponse::addHeaders(const std::vector<std::pair<std::string, std::string>> &headers) {
        for (auto &[name, value]: headers) {
            evhttp_add_header(evhttp_request_get_output_headers(this->req), name.c_str(), value.c_str());
        }
    }

    void Http1ModuleResponse::setStatus(const char *status) {
        this->status = status;
        this->reason = strdup(this->status_code_handler.getMessage(status).c_str());
    }

    void Http1ModuleResponse::send() {
        if (this->type == core::FD) {
            const int fd = this->getFD();
            evhttp_send_reply_start(this->req, (this->status != nullptr) ? std::stoi(this->status) : 200,
                                    (this->reason) ? this->reason : "OK");
            evbuffer_add_file(this->buf, fd, 0, this->st.st_size);
            evhttp_send_reply_chunk(this->req, this->buf);
            evbuffer_free(this->buf);
            evhttp_send_reply_end(this->req);
            close(fd);
        } else if (this->type == core::BUFFER) {
            auto buffer = this->getBuffer();
            if (buffer.has_value()) {
                evbuffer_add(this->buf, buffer.value()->data(), buffer.value()->size());
                evhttp_send_reply(req, (this->status != nullptr) ? std::stoi(this->status) : 200,
                                  (this->reason) ? this->reason : "OK",
                                  this->buf);
                evbuffer_free(this->buf);
            }
        } else if (this->type == core::NONE) {
            evhttp_send_reply(this->req, (this->status != nullptr) ? std::stoi(this->status) : HTTP_INTERNAL,
                              (this->reason != nullptr) ? this->reason : "Internal Server Error", nullptr);
            evbuffer_free(this->buf);
        }
    }

    void Http1ModuleResponse::setReq(evhttp_request *req) {
        this->req = req;
    }

    void Http1ModuleResponse::setReason(const char *reason) {
        this->reason = reason;
    }

    void Http1ModuleResponse::setStatusError(bool status_error) {
        this->status_code_handler.setStatusError(status_error);
    }

    void request_handler(evhttp_request *req, void *arg) {
        const evkeyvalq *headers = evhttp_request_get_input_headers(req);
        auto http_request = HttpRequest(-1);
        for (const evkeyval *header = headers->tqh_first; header != nullptr; header = header->next.tqe_next) {
            http_request.headers.emplace(header->key, header->value);
        }
        evhttp_uri *parsed_uri = evhttp_uri_parse(evhttp_request_get_uri(req));
        if (!parsed_uri) {
            evhttp_send_error(req, HTTP_NOTFOUND, "Document not found");
            return;
        }
        const char *path = evhttp_uri_get_path(parsed_uri);
        http_request.headers.emplace(":path", path);
        auto global_handler = static_cast<Http1Handler *>(arg);
        auto type = static_cast<core::REQUEST_TYPE>(evhttp_request_get_command(req));
        auto match = global_handler->endpoint_handler->match(type, path);
        Http1ModuleResponse res = Http1ModuleResponse(req);
        res.setStatusError(global_handler->statusError);
        if (match.has_value()) {
            match.value()(http_request, res);
        } else {
            global_handler->setErrorPage(&res);
            res.setStatus("404");
        }
        evhttp_uri_free(parsed_uri);
        res.send();
    }

    bufferevent *bevcb(event_base *base, void *arg) {
        auto ctx = static_cast<std::pair<SSL_CTX *, int> *>(arg);
        return bufferevent_openssl_socket_new(base, ctx->second, SSL_new(ctx->first), BUFFEREVENT_SSL_ACCEPTING,
                                              BEV_OPT_CLOSE_ON_FREE);
    }


    Http1Handler::Http1Handler(std::shared_ptr<HttpBasicEndpointHandler> &basic_endpoint_handler,
                               event_base *event_base, SSL_CTX *ssl_ctx, int port, const std::string &ip_addr,
                               int backlog, core::IPV ipv, bool is_ssl, bool statusError,
                               const std::string &err_path) : endpoint_handler(basic_endpoint_handler),
                                                              ctx(ssl_ctx),
                                                              evbase(event_base), port(port), ip_addr(ip_addr),
                                                              statusError(statusError),
                                                              error_path(err_path) {
        this->soc_fd = usub::server::net::createSocket(port, ip_addr, backlog, ipv);
        this->http = evhttp_new(this->evbase);
        if (!this->http) {
            error_log("Couldn't create an event_base: Exiting");
            exit(9);
        }
        if (evhttp_accept_socket(this->http, this->soc_fd) != 0) {
            error_log("Couldn't bind to port: %d. Exiting", this->port);
            close(this->soc_fd);
            evhttp_free(this->http);
            event_base_free(this->evbase);
            exit(9);
        }

        evhttp_set_gencb(this->http, request_handler, this);
        if (is_ssl) {
            std::pair data = {this->ctx, this->soc_fd};
            evhttp_set_bevcb(this->http, bevcb, &data);
        }
        info_log("Server is running on: %d", this->port);
        event_base_dispatch(this->evbase);
        {
            std::lock_guard<std::mutex> lock(eventBasesMutex);
            eventBases.push_back(this->evbase);
        }
    }

    Http1Handler::~Http1Handler() {
        if (this->soc_fd) {
            close(this->soc_fd);
        }
        if (this->http) {
            evhttp_free(this->http);
        }
    }

    void Http1Handler::setErrorPage(Http1ModuleResponse *http_response) const {
        if (this->error_path.empty()) {
            const std::string data = "<html><head><title>404</title></head>"
                                     "<body><h1>404 Not Found</h1></body></html>";
            http_response->writeRawData(reinterpret_cast<const uint8_t *>(data.data()), data.length());
            http_response->addHeader((char *) "Content-Type", (char *) "text/html; charset=utf-8");
        } else {
            http_response->writeFile(this->error_path);
            http_response->addHeader((char *) ":status", (char *) "404");
            http_response->addHeader((char *) "Content-Type", (char *) "text/html; charset=utf-8");
        }
    }

    Http1Parser::Http1Parser(evbuffer *in) : in(in) {
        size_t len = evbuffer_get_length(this->in);
        this->raw_data = new uint8_t[len + 1];
        this->raw_data = evbuffer_pullup(this->in, len);
        std::memcpy(this->raw_data, evbuffer_pullup(in, len), len);
        this->raw_data[len] = '\0';
    }

    Http1Parser::~Http1Parser() {
    }

    int Http1Parser::parse_request() {
        auto line_ptr = reinterpret_cast<char *>(this->raw_data);
        char *method = strsep(&line_ptr, " ");
        if (!method) return -1;
        this->request_type = this->request_type_str.getType(method);
        this->uri = strsep(&line_ptr, " ");
        if (!this->uri) return -1;
        this->version = strsep(&line_ptr, "\r\n");
        if (!this->version) return -1;
        size_t skip = strspn(line_ptr, "\r\n");
        line_ptr[skip - 1] += 2;
        while (strncmp(line_ptr, "\n\r\n", 1) != 0) {
            while (*line_ptr == '\f' || *line_ptr == ' ' || *line_ptr == '\v') {
                ++line_ptr;
            }
            char *key = strsep(&line_ptr, ":");
            while (*line_ptr == '\f' || *line_ptr == ' ' || *line_ptr == '\v') {
                ++line_ptr;
            }
            char *value = strsep(&line_ptr, "\r\n");
            if (!line_ptr) {
                break;
            }
            skip = strspn(line_ptr, "\r\n");
            line_ptr[skip - 1] += 1;
        }
        if (line_ptr) {
            while (*line_ptr == '\r' || *line_ptr == '\n' || *line_ptr == '\v') {
                ++line_ptr;
            }
            this->body = reinterpret_cast<uint8_t *>(line_ptr);
        }
        return 1;
    }

    HttpRequest Http1Parser::getRequest() {
        HttpRequest request(-1);
        request.data.assign(reinterpret_cast<const unsigned char *>(this->body),
                            reinterpret_cast<const unsigned char *>(this->body) +
                            strlen(reinterpret_cast<const char *>(this->body)));
        request.headers = std::move(this->headers);
        return request;
    }

    int Http1Parser::header_is_valid(char *line) {
        const char *p = line;

        while ((p = strpbrk(p, "\r\n")) != NULL) {
            /* we really expect only one new line */
            p += strspn(p, "\r\n");
            /* we expect a space or tab for continuation */
            if (*p != ' ' && *p != '\t')
                return (0);
        }
        return (1);
    }
} // usub::server::protocol::http1
