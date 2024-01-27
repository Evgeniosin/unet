#ifndef HTTP1_H
#define HTTP1_H

#include "http_base.h"


namespace usub::server::protocols::http {
    class Http1Parser;

    class Http1Response final : public HttpResponse {
    public:
        Http1Response() = default;

        ~Http1Response() override;

        bool writeFile(const std::string &file_path) override;

        bool writeRawData(const uint8_t *buf, size_t length) override;

        [[nodiscard]] std::optional<std::shared_ptr<std::vector<uint8_t>>> getBuffer() const override;

        [[nodiscard]] int getFD() const;

        void addHeader(char *name, const char *value) override;

        void addHeaders(const std::vector<std::pair<std::string, std::string>> &headers) override;

        void setStatus(const char* status) override;

        void prepare_buffer(bufferevent* bev);

        void setReason(const char *reason) override;

        void write_chunked_response(std::vector<uint8_t>& auxBuf, const uint8_t *data, size_t size);

    public:
        core::DATA_TYPE type = core::NONE;
        struct stat st;
        std::vector<std::pair<std::string, std::string>> headers;
    private:
        const char* status = nullptr;
        const char *reason = nullptr;
        std::variant<std::shared_ptr<std::vector<uint8_t>>, int> data;
        bool status_error;
        std::string path;
        StatusCodeHandler status_code_handler{ false };
    };

    class Http1Context : public HttpContext {
    private:
    protected:
    public:
        Http1Context(HttpContext *base, bufferevent *bev);

        ~Http1Context() override;

        static void eventCallback(bufferevent *bev, short events, void *ptr);

        static void readCallback(bufferevent *bev, void *ptr);

        static void writeCallback(bufferevent *bev, void *ptr);

        Http1Response getErrorPage(int32_t stream_id);

        core::RequestTypeStr req_str{};
        bufferevent *bev;
        Http1Response* http_response;
        Http1Parser* parser = nullptr;
    };

    class Http1Parser {
    public:
        Http1Parser(evbuffer *in);

        ~Http1Parser();

    public:
        int parse_request();

        HttpRequest getRequest();
    public:
        uint8_t *body{};
        char *version{};
        char *uri{};
        const char *hostname{};
        const char *scheme{};
        core::REQUEST_TYPE request_type;
        std::unordered_map<std::string, std::string> headers;
    private:
        int header_is_valid(char *line);
    private:
        evbuffer *in;
        uint8_t *raw_data;
        core::RequestTypeStr request_type_str{};
    };

    // TODO: EVERYTHING BELOW THIS MUST BE CHANGED 

#ifndef HTTP1LOOP_H
#define HTTP1LOOP_H

    class Http1ModuleResponse final : public HttpResponse {
    public:
        Http1ModuleResponse() = default;

        explicit Http1ModuleResponse(evhttp_request *request);

        ~Http1ModuleResponse() override;

        bool writeFile(const std::string &file_path) override;

        bool writeRawData(const uint8_t *buf, size_t length) override;

        [[nodiscard]] std::optional<std::shared_ptr<std::vector<uint8_t>>> getBuffer() const override;

        [[nodiscard]] int getFD() const;

        void addHeader(char *name, const char *value) override;

        void addHeaders(const std::vector<std::pair<std::string, std::string>> &headers) override;

        void setStatus(const char* status) override;

        void send();

        void setReq(evhttp_request *req);

        void setReason(const char *reason) override;

        void setStatusError(bool status_error);

        core::DATA_TYPE type = core::NONE;

        struct stat st;

        std::list<std::pair<std::string, std::string>> headers;

    private:
        StatusCodeHandler status_code_handler{ false };

        std::variant<std::shared_ptr<std::vector<uint8_t>>, int> data;

        evbuffer *buf = nullptr;
        evhttp_request *req;
        const char* status;
        const char *reason = nullptr;
    };

    bufferevent *bevcb(struct event_base *base, void *arg);

    class Http1Handler {
    public:
        Http1Handler(std::shared_ptr<HttpBasicEndpointHandler> &basic_endpoint_handler, event_base *evbase,
            SSL_CTX *ssl_ctx, int port, const std::string &ip_addr, int backlog, core::IPV ipv, bool is_ssl, bool statusError = true, const std::string &err_path = "");

        ~Http1Handler();

        friend void request_handler(struct evhttp_request *req, void *arg);
    private:
        void setErrorPage(Http1ModuleResponse *http_response) const;
    private:
        std::shared_ptr<HttpBasicEndpointHandler> endpoint_handler;
        SSL_CTX *ctx;
        event_base *evbase;
        evhttp *http;
        int port;
        const std::string &ip_addr;
        int soc_fd;
        bool statusError;
        std::string error_path;
    };


#endif //HTTP1LOOP_H


}

#endif //HTTP1_H
