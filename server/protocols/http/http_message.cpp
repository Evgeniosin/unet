#include "http_message.h"

namespace usub::server::protocols::http {

    HttpRequest::HttpRequest(bool isFinished, const std::vector<unsigned char> &data, int32_t stream_id)
        : data(data), stream_id(stream_id) {
    }

    HttpRequest::HttpRequest(int32_t stream_id) : stream_id(stream_id) {}

    int32_t HttpRequest::getStreamId() const {
        return stream_id;
    }

    const std::vector<unsigned char> &HttpRequest::getData() const {
        return data;
    }

    std::string HttpRequest::getURL() const {
        return this->url;
    }

} // namespace usub::server::protocols::http


