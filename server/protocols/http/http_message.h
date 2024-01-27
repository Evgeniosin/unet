#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <vector>
#include <cstdint>
#include <string>
#include <optional>
#include <memory>
#include <unordered_map>

namespace usub::server::protocols::http {

    class HttpResponse {
    public:
        virtual ~HttpResponse() = default;
        /**
         * Used for sending files.
         * @param file_path path to file to be sent as reponse
         */
        virtual bool writeFile(const std::string &file_path) = 0;
        /**
         * Used for writing text reponses such as JSON, XML, etc.
         * @param buf buffer to be written
         * @param length size of buffer
         */
        virtual bool writeRawData(const uint8_t *buf, size_t length) = 0;
        /**
         * Adds header to response.
         * @param name name of header
         * @param value value of header
         */
        virtual void addHeader(char *name, const char *value) = 0;
        /**
         * Allows to set several headers by single function call.
         * @param headers vector of pairs: <b>header name : header value</b>
         */
        virtual void addHeaders(const std::vector<std::pair<std::string, std::string>> &headers) = 0;
        /**
         * Set status to response. Is status code is described in RFC, there is no need to set reason manually.
         * @param status status to be set
         */
        virtual void setStatus(const char* status) = 0;

        /**
         * Set reason to status code (should be used for only non-RFC status codes) for HTTP/1.1.
         * @param reason reason which should be sent with status in HTTP/1.1
         */
        virtual void setReason(const char *reason) = 0;

        [[nodiscard]] virtual std::optional<std::shared_ptr<std::vector<uint8_t>>> getBuffer() const = 0;
    };

    class HttpRequest {
    public:
        HttpRequest(bool isFinished, const std::vector<unsigned char> &data, int32_t stream_id);

        explicit HttpRequest(int32_t stream_id);

        [[nodiscard]] const std::vector<unsigned char> &getData() const;

        [[nodiscard]] int32_t getStreamId() const;

        [[nodiscard]] std::string getURL() const;

    public:
        std::string url;
        std::unordered_map<std::string, std::string> headers{};
        std::vector<unsigned char> data;
    private:
        const int32_t stream_id;
    };

} // namespace usub::server::protocols::http


#endif // HTTP_MESSAGE_H