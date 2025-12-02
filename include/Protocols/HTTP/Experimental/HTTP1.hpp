#pragma once

#include "Protocols/HTTP/Experimental/Request.hpp"
#include "Protocols/HTTP/Experimental/Response.hpp"

namespace usub::server::protocols::http {

class HTTP1Parser {
public:
    enum class STATE : uint8_t {
        METHOD,
        URI_ORIGIN_FORM,
        URI_ABSOLUTE_FORM,
        URI_AUTHORITY_FORM,
        URI_ASTERISK_FORM,
        QUERY,
        FRAGMENT, // That's here for completeness, but HTTP1 doesn't use fragments during transfer
        VERSION,
        HEADERS_KEY,
        HEADERS_VALUE,
        HEADERS_CR,
        HEADERS_LF,
        HEADERS_END_CR,
        HEADERS_END_LF,
        HEADERS_DONE,
        BODY_LENGTH,
        BODY_CHUNK_SIZE,
        BODY_CHUNK_DATA,
        BODY_CHUNK_DATA_CR,
        BODY_CHUNK_DATA_LF,
        DONE,
        ERROR
    };

    STATE state{STATE::METHOD};
    std::size_t header_count{0};
    std::size_t bytes_total{0};
    std::size_t body_bytes{0};
    std::optional<std::size_t> content_length_expected;
    
    // current in-flight header 
    std::string cur_header_name;
    std::string cur_header_value;

    // optional bounded tail buffer for debugging
    static constexpr std::size_t kTail{256};
    std::array<char, kTail> tail{};
    std::size_t tail_pos{0};
    
    inline void operator()(Request& request, std::string_view chunk, std::string_view::const_iterator& iterator_pos) {
        if (this->state == STATE::ERROR) {
            iterator_pos = chunk.end();
            return;
        }
        if (this->state == STATE::DONE) {
            request.clear();
            this->clear();
        }
        while (iterator_pos != chunk.end()) {
            char c = *iterator_pos;
            switch (this->state) {
                case STATE::METHOD:
                    // parse method
                    break;
                case STATE::URI_ORIGIN_FORM:
                    // parse origin form URI
                    break;
                // ... other states ...
                case STATE::DONE:
                    // finished parsing
                    break;
                case STATE::ERROR:
                    // handle error
                    iterator_pos = chunk.end();
                    return;
            }
            ++iterator_pos;
        }
    };
private:

};

class HTTP1 {

    // Request, Response and parser will live here
private:
public:
    HTTP1() = default;
    ~HTTP1() = default;
}
}// namespace usub::server::protocols::http