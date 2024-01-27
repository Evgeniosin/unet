#include "http2.h"

namespace usub::server::protocols::http {

#define OUTPUT_WOULDBLOCK_THRESHOLD (1 << 16)

#define ARRLEN(x) (sizeof(x) / sizeof(x[0]))

    Http2StreamData::Http2StreamData(int32_t stream_id) : stream_id(stream_id), fd(-1), http_request(stream_id) {}

    Http2StreamData::~Http2StreamData() {}

    Http2Response::Http2Response(const int32_t stream_id) : stream_id(stream_id) {
    }

    Http2Response::~Http2Response() {
        if (this->type == core::FD) {
            close(this->getFD());
        }
    }

    bool Http2Response::writeFile(const std::string &file_path) {
        if (this->type != core::NONE) {
            return false;
        }
        int fd = open(file_path.c_str(), O_RDONLY);

        if (fd == -1) {
            const std::string data = "<html><head><title>404</title></head>"
                "<body><h1>404 Not Found</h1></body></html>";
            error_log("File not opened: %s, error: %s", file_path.c_str(), strerror(errno));
            this->writeRawData(reinterpret_cast<const uint8_t *>(data.data()), data.length());
            this->addHeader((char *)":status", (char *)"404");
            this->addHeader((char *)"Content-Type", (char *)"text/html; charset=utf-8");
            return false;
        }
        this->data = fd;
        this->type = core::FD;
        this->path = file_path;
        return true;
    }

    bool Http2Response::writeRawData(const uint8_t *buf, const size_t length) {
        if (this->type != core::NONE) {
            return false;
        }
        this->data = std::make_shared<std::vector<uint8_t>>(buf, buf + length);
        this->type = core::BUFFER;
        return true;
    }

    std::optional<std::shared_ptr<std::vector<uint8_t>>> Http2Response::getBuffer() const {
        if (std::holds_alternative<std::shared_ptr<std::vector<uint8_t>>>(data)) {
            return std::get<std::shared_ptr<std::vector<uint8_t>>>(data);
        }
        return std::nullopt;
    }

    int Http2Response::getFD() const {
        if (std::holds_alternative<int>(this->data)) {
            return std::get<int>(this->data);
        }
        return -1;
    }

    void Http2Response::addHeader(char *name, const char *value) {
        this->headers.push_back({
            (uint8_t *)name, (uint8_t *)value, std::strlen(name), std::strlen(value), NGHTTP2_NV_FLAG_NONE
            });
    }

    void Http2Response::addHeaders(const std::vector<std::pair<std::string, std::string>> &headers) {
        for (auto &[name, value] : headers) {
            this->headers.push_back({
                (uint8_t *)name.c_str(), (uint8_t *)value.c_str(), std::strlen(name.c_str()), std::strlen(value.c_str()),
                NGHTTP2_NV_FLAG_NONE
                });
        }
    }

    void Http2Response::setStatus(const char* status) {
        this->headers.push_back(usub::server::protocols::http::Http2Response::makeNv(":status", status));
    }

    nghttp2_nv Http2Response::makeNv(const char *name, const char *value) {
        return {
            (uint8_t *)(name),
            (uint8_t *)(value),
            strlen(name),
            strlen(value),
            NGHTTP2_NV_FLAG_NONE
        };
    }

    std::vector<nghttp2_nv> &Http2Response::getHeaders() {
        if (this->headers.empty()) {
            this->headers.push_back(makeNv(":status", "200"));
            this->headers.push_back(makeNv("content-type", "text/plain"));
        }
        return this->headers;
    }

    void Http2Response::setReason(const char *reason) {
    }

    Http2Context::Http2Context(HttpContext *base, bufferevent *bev) : HttpContext(*base), bev(std::move(bev)) {
        int rv;
        char host[NI_MAXHOST];
        
	timeval timeout = { 10, 0 };
	bufferevent_set_timeouts(this->bev, &timeout, &timeout);
    }

    Http2Context::Http2Context(HttpContext &&base, bufferevent *bev) : HttpContext(std::move(base)), bev(std::move(bev)) {
        int rv;
        char host[NI_MAXHOST];
        
	timeval timeout = { 10, 0 };
	bufferevent_set_timeouts(this->bev, &timeout, &timeout);
    }

    Http2Context::~Http2Context() {
        SSL *ssl = bufferevent_openssl_get_ssl(this->bev);
	bufferevent_setcb(this->bev, NULL, NULL, NULL, NULL);
        for (auto it = streams.begin(); it != streams.end(); ++it) {
            delete *it; // Delete the object pointed to by the iterator
        }
        info_log("%s disconnected", this->client_addr);

        shutdownSSL(ssl);

        bufferevent_free(this->bev);
        if (this->session) {
            nghttp2_session_del(this->session);
        }
    }

    void Http2Context::eventCallback(bufferevent *bev, short events, void *ptr) {
        bool shouldTerminate = false;
        Http2Context *ctx = (Http2Context *)ptr;
        if (events & BEV_EVENT_TIMEOUT) {
            error_log("Timeout, disconnecting: %s", ctx->client_addr);
            shouldTerminate = true;
        }
        else if (events & (BEV_EVENT_READING | BEV_EVENT_WRITING)) {
            error_log("Error during %s data from client: %s",
                (events & BEV_EVENT_READING) ? "reading" : "writing",
                ctx->client_addr);
            shouldTerminate = true;
        }
        else if (events & BEV_EVENT_EOF) {
            error_log("Received EOF from client: %s", ctx->client_addr);
            shouldTerminate = true;
        }
        else if (events & BEV_EVENT_ERROR) {
            error_log("Error: %s", ctx->client_addr);
            shouldTerminate = true;
        }
	ctx->events = events;
        if (shouldTerminate) {
            bufferevent_setcb(bev, nullptr, nullptr, nullptr, nullptr);
            delete ctx;
            ctx = nullptr;
            return;
        }

    }

    void Http2Context::readCallback(bufferevent *bev, void *ptr) {
        Http2Context *ctx = (Http2Context *)ptr;
        if (ctx->sessionRecv() != 0) {
            delete ctx;
            ctx = nullptr;
        }
    }

    void Http2Context::writeCallback(bufferevent *bev, void *ptr) {
        Http2Context *ctx = (Http2Context *)ptr;
        SSL *ssl = bufferevent_openssl_get_ssl(bev);
        if (evbuffer_get_length(bufferevent_get_output(bev)) > 0) {
            return;
        }
        if (nghttp2_session_want_read(ctx->session) == 0 &&
            nghttp2_session_want_write(ctx->session) == 0) {
            delete ctx;
            ctx = nullptr;
            return;
        }
        if (ctx->sessionSend() != 0) {
            delete ctx;
            ctx = nullptr;
            return;
        }
    }

    int Http2Context::sessionSend() {
        int rv;
        rv = nghttp2_session_send(this->session);
        if (rv != 0) {
            error_log("Fatal error: %s, %d", nghttp2_strerror(rv), __LINE__);
            return -1;
        }
        return 0;
    }

    int Http2Context::sessionRecv() {
        ssize_t readlen;
        struct evbuffer *input = bufferevent_get_input(this->bev);
        size_t datalen = evbuffer_get_length(input);
        unsigned char *data = evbuffer_pullup(input, -1);

        readlen = nghttp2_session_mem_recv(this->session, data, datalen);
#if DEBUG_EXTENDED_INFO
        printf("Recieved data: %.*s\n", datalen, data);
#endif
        if (readlen < 0) {
            error_log("Fatal error: %s, %d", nghttp2_strerror((int)readlen), __LINE__);
            return -1;
        }
        if (evbuffer_drain(input, (size_t)readlen) != 0) {
            error_log("Fatal error: evbuffer_drain failed");
            return -1;
        }
        if (this->sessionSend() != 0) {
            return -1;
        }
        return 0;
    }


    int Http2Context::onRequestRecieve(Http2StreamData *stream_data) {
        int rv;
        auto match = this->endpoint_handler->match(
            this->req_str.getType(stream_data->http_request.headers.at(":method")),
            stream_data->http_request.headers.at(":path"));
        if (match.has_value()) {
            auto *res = new Http2Response(stream_data->stream_id);
            stream_data->http_request.url = stream_data->http_request.headers.at(":path");
            match.value()(stream_data->http_request, *res);
            stream_data->http_response = res;
            rv = this->sdResponse(stream_data->stream_id, res);
        }
        else {
            auto res = this->getErrorPage(stream_data->stream_id);
            stream_data->http_response = res;
            rv = this->sdResponse(stream_data->stream_id, res);
        }
        if (rv != 0) {
            error_log("Error: %s", nghttp2_strerror(rv));
            error_log("Error code: %d", rv);
            return NGHTTP2_ERR_CALLBACK_FAILURE;
        }

        return 0;
    }

    void Http2Context::removeStream(int32_t stream_id) {
        auto &streams = this->streams;
        for (auto it = streams.begin(); it != streams.end(); ++it) {
            if ((*it)->stream_id == stream_id) {
                delete (*it)->http_response;
                return;
            }
        }
    }

    int Http2Context::sdResponse(int32_t stream_id, Http2Response *http_response) {
        int rv;
        nghttp2_data_provider prd;
        prd.source.ptr = http_response;
        prd.read_callback = Http2Context::rawDataProviderCallback;

        auto headers = http_response->getHeaders();
        rv = nghttp2_submit_response(this->session, stream_id, headers.data(), headers.size(), &prd);
        if (rv != 0) {
            error_log("Fatal error: %s, %d", nghttp2_strerror(rv), __LINE__);
            return -1;
        }
        return 0;
    }

    int Http2Context::sendServerConnectionHeader() {
        nghttp2_settings_entry iv[1] = {
            {NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS, 100}
        };
        int rv;

        rv = nghttp2_submit_settings(this->session, NGHTTP2_FLAG_NONE, iv,
            ARRLEN(iv));
        if (rv != 0) {
            error_log("Fatal error: %s, %d", nghttp2_strerror(rv), __LINE__);
            return -1;
        }
        return 0;
    }

    ssize_t Http2Context::rawDataProviderCallback(nghttp2_session *session, int32_t stream_id, uint8_t *buf,
        size_t length, uint32_t *data_flags, nghttp2_data_source *source, void *user_data) {
        (void)session;
        (void)stream_id;
        auto *data = static_cast<Http2Response *>(source->ptr);

        if (data->type == core::BUFFER) {
            auto buffer = data->getBuffer();
            if (buffer.has_value()) {
                if (data->read_offset >= buffer->get()->size()) {
                    *data_flags |= NGHTTP2_DATA_FLAG_EOF;
                    return 0; // Все данные отправлены
                }

                size_t data_left = buffer->get()->size() - data->read_offset;
                size_t copy_len = (data_left < length) ? data_left : length;

                memcpy(buf, buffer->get()->data() + data->read_offset, copy_len);
                data->read_offset += copy_len;

                if (data->read_offset >= buffer->get()->size()) {
                    *data_flags |= NGHTTP2_DATA_FLAG_EOF;
                }
                return static_cast<ssize_t>(copy_len);
            }
        }
        else {
            const int fd = data->getFD();
            if (fd < 0) {
                error_log("File descriptor corrupted: %s", data->path.c_str());
            }
            ssize_t r;

            while ((r = read(fd, buf, length)) == -1 && errno == EINTR) {
            }

            if (r == -1) {
                return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
            }

            if (r == 0) {
                *data_flags |= NGHTTP2_DATA_FLAG_EOF;
            }

            return r;
        }

        //TODO temporary fix?
        return -1;
    }

    ssize_t Http2Context::sendCallback(nghttp2_session *session, const uint8_t *data,
        size_t length, int flags, void *user_data) {
        Http2Context *ctx = (Http2Context *)user_data;
        struct bufferevent *bev = ctx->bev;
        (void)session;
        (void)flags;

        /* Avoid excessive buffering in server side. */
        if (evbuffer_get_length(bufferevent_get_output(ctx->bev)) >=
            OUTPUT_WOULDBLOCK_THRESHOLD) {
            return NGHTTP2_ERR_WOULDBLOCK;
        }
        bufferevent_write(bev, data, length);

        return (ssize_t)length;
    }

    int Http2Context::onFrameRecieveCallback(nghttp2_session *session,
        const nghttp2_frame *frame, void *user_data) {
        auto *ctx = static_cast<Http2Context *>(user_data);
        Http2StreamData *stream_data;
        switch (frame->hd.type) {
            case NGHTTP2_DATA:
            case NGHTTP2_HEADERS:
#if DEBUG_EXTENDED_INFO
                printf("Recieved HEAdERS frame\n");
#endif
                /* Check that the client request has finished */
                if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
                    stream_data =
                        static_cast<Http2StreamData *>(nghttp2_session_get_stream_user_data(session,
                            frame->hd.stream_id));
                    /* For DATA and HEADERS frame, this callback may be called after
                       on_stream_close_callback. Check that stream still alive. */
                    if (!stream_data) {
                        return 0;
                    }
                    return ctx->onRequestRecieve(stream_data);
                }
                break;
            case NGHTTP2_GOAWAY: {
                SSL *ssl = bufferevent_openssl_get_ssl(ctx->bev);
#if DEBUG_EXTENDED_INFO
                printf("Recieved GOAWAY frame, SSL state\tSSL_in_init: %d, SSL_in_before: %d, SSL_is_init_finished: %d, SSL_in_connect_init: %d, SSL_in_accept_init: %d, SSL_get_state: %d\n",
                    SSL_in_init(ssl), SSL_in_before(ssl), SSL_is_init_finished(ssl), SSL_in_connect_init(ssl), SSL_in_accept_init(ssl), SSL_get_state(ssl));
#endif
                break;
            }
            default:
                break;
        }
        return 0;
    }

    int Http2Context::onStreamCloseCallback(nghttp2_session *session, int32_t stream_id,
        uint32_t error_code, void *user_data) {
        Http2Context *ctx = (Http2Context *)user_data;
        Http2StreamData *stream_data;
        (void)error_code;

        stream_data = static_cast<Http2StreamData *>(nghttp2_session_get_stream_user_data(session, stream_id));
        if (!stream_data) {
            return 0;
        }
        ctx->removeStream(stream_id);
        return 0;
    }

    int Http2Context::onBeginHeadersCallback(nghttp2_session *session,
        const nghttp2_frame *frame,
        void *user_data) {
        Http2Context *ctx = (Http2Context *)user_data;
        auto stream_data = new Http2StreamData(frame->hd.stream_id);

        if (frame->hd.type != NGHTTP2_HEADERS ||
            frame->headers.cat != NGHTTP2_HCAT_REQUEST) {
            return 0;
        }
        ctx->streams.push_back(stream_data);
        nghttp2_session_set_stream_user_data(session, frame->hd.stream_id,
            stream_data);
        return 0;
    }

    int Http2Context::onHeaderCallback(nghttp2_session *session,
        const nghttp2_frame *frame, const uint8_t *name,
        size_t namelen, const uint8_t *value,
        size_t valuelen, uint8_t flags, void *user_data) {
        Http2StreamData *stream_data;
        const char PATH[] = ":path";
        (void)flags;
        (void)user_data;

        switch (frame->hd.type) {
            case NGHTTP2_HEADERS:
                if (frame->headers.cat != NGHTTP2_HCAT_REQUEST) {
                    break;
                }
                stream_data =
                    static_cast<Http2StreamData *>(nghttp2_session_get_stream_user_data(session,
                        frame->hd.stream_id));
                if (!stream_data) {
                    break;
                }
                stream_data->http_request.headers.try_emplace(std::string((char *)name, namelen),
                    std::string((char *)value, valuelen));
                if (namelen == sizeof(PATH) - 1 && memcmp(PATH, name, namelen) == 0) {
                    size_t j;
                    for (j = 0; j < valuelen && value[j] != '?'; ++j);
                }
                break;
        }
        return 0;
    }

    void Http2Context::initializeNghttp2Session() {
        nghttp2_session_callbacks *callbacks;

        nghttp2_session_callbacks_new(&callbacks);

        nghttp2_session_callbacks_set_send_callback(callbacks, Http2Context::sendCallback);

        nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks,
            Http2Context::onFrameRecieveCallback);

        nghttp2_session_callbacks_set_on_stream_close_callback(
            callbacks, Http2Context::onStreamCloseCallback);

        nghttp2_session_callbacks_set_on_header_callback(callbacks,
            Http2Context::onHeaderCallback);

        nghttp2_session_callbacks_set_on_begin_headers_callback(
            callbacks, Http2Context::onBeginHeadersCallback);

        nghttp2_session_server_new(&this->session, callbacks, this);

        nghttp2_session_callbacks_del(callbacks);
    }

    Http2Response *Http2Context::getErrorPage(int32_t stream_id) {
        auto httpRes = new Http2Response(stream_id);
        if (/*this->error_path.empty()*/true) {
            const std::string data = "<html><head><title>404</title></head>"
                "<body><h1>404 Not Found</h1></body></html>";
            httpRes->writeRawData(reinterpret_cast<const uint8_t *>(data.data()), data.length());
            httpRes->addHeader((char *)":status", (char *)"404");
            httpRes->addHeader((char *)"Content-Type", (char *)"text/html; charset=utf-8");
        }
        // else {
        //     httpRes->writeFile(this->error_path);
        //     httpRes->addHeader((char *)":status", (char *)"404");
        //     httpRes->addHeader((char *)"Content-Type", (char *)"text/html; charset=utf-8");
        // }
        return httpRes;
    }

}
