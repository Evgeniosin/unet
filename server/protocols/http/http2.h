#ifndef HTTP2_H
#define HTTP2_H

#include "http_base.h"

namespace usub::server::protocols::http {

    class Http2Response final : public HttpResponse {
    public:
        explicit Http2Response(int32_t stream_id);
        ~Http2Response() override;

        bool writeFile(const std::string &file_path) override;

        bool writeRawData(const uint8_t *buf, size_t length) override;

        [[nodiscard]] std::optional<std::shared_ptr<std::vector<uint8_t>>> getBuffer() const override;

        [[nodiscard]] int getFD() const;

        void addHeader(char *name, const char *value) override;

        void addHeaders(const std::vector<std::pair<std::string, std::string>> &headers) override;

        void setStatus(const char* status) override;

        std::vector<nghttp2_nv> &getHeaders();

        void setReason(const char *reason) override;

    private:
        static nghttp2_nv makeNv(const char *name, const char *value);

    public:
        core::DATA_TYPE type = core::NONE;
        size_t read_offset = 0;
        const int32_t stream_id;
        std::string path;

    private:
        std::variant<std::shared_ptr<std::vector<uint8_t>>, int> data;
        std::vector<nghttp2_nv> headers;
    };

    class Http2StreamData {
    public:
        explicit Http2StreamData(int32_t stream_id);

        ~Http2StreamData();

        HttpRequest http_request;
        Http2Response *http_response;
        int32_t stream_id;
        int fd;
    };

    class Http2Context : public HttpContext {
    private:
    protected:
    public:
        Http2Context(HttpContext *base, bufferevent *bev);

        Http2Context(HttpContext &&base, bufferevent *bev);

        ~Http2Context() override;

        static void eventCallback(bufferevent *bev, short events, void *ptr);

        static void readCallback(bufferevent *bev, void *ptr);

        static void writeCallback(bufferevent *bev, void *ptr);

        int sessionSend();

        int sessionRecv();

        int onRequestRecieve(Http2StreamData *stream_data);

        void removeStream(int32_t stream_id);

        int sdResponse(int32_t stream_id, Http2Response *http_response);

        int sendServerConnectionHeader();

        static ssize_t rawDataProviderCallback(nghttp2_session *session, int32_t stream_id, uint8_t *buf,
            size_t length, uint32_t *data_flags, nghttp2_data_source *source, void *user_data);

        static ssize_t sendCallback(nghttp2_session *session, const uint8_t *data,
            size_t length, int flags, void *user_data);

        static int onFrameRecieveCallback(nghttp2_session *session,
            const nghttp2_frame *frame, void *user_data);

        static int onStreamCloseCallback(nghttp2_session *session, int32_t stream_id,
            uint32_t error_code, void *user_data);

        static int onHeaderCallback(nghttp2_session *session,
            const nghttp2_frame *frame, const uint8_t *name,
            size_t namelen, const uint8_t *value,
            size_t valuelen, uint8_t flags, void *user_data);

        static int onBeginHeadersCallback(nghttp2_session *session,
            const nghttp2_frame *frame,
            void *user_data);

        void initializeNghttp2Session();

        Http2Response *getErrorPage(int32_t stream_id);

        std::list<Http2StreamData *> streams;
        core::RequestTypeStr req_str{};
        bufferevent *bev;
        nghttp2_session *session;

    };
}

#endif //HTTP2_H
