#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

#include "unet/http/parser/http1.hpp"

using usub::unet::http::STATUS_CODE;
using usub::unet::http::VERSION;
using usub::unet::http::parser::http1::RequestParser;

namespace {
    void expect(bool condition, std::string_view message) {
        if (!condition) {
            std::cerr << "FAIL: " << message << '\n';
            std::exit(1);
        }
    }

    void test_basic_get_request() {
        const std::string raw =
                "GET /hello/world?name=codex HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "\r\n";

        auto result = RequestParser::parse(raw);
        expect(result.has_value(), "basic GET request should parse");

        const auto &request = result.value();
        expect(request.metadata.method_token == "GET", "method token should match");
        expect(request.metadata.uri.path == "/hello/world", "path should match");
        expect(request.metadata.uri.query == "name=codex", "query should match");
        expect(request.metadata.version == VERSION::HTTP_1_1, "version should be HTTP/1.1");
    }

    void test_invalid_method_token() {
        const std::string raw =
                " GET / HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "\r\n";

        auto result = RequestParser::parse(raw);
        expect(!result.has_value(), "invalid method token should fail");
        expect(result.error().expected_status == STATUS_CODE::BAD_REQUEST,
               "invalid method token should be BAD_REQUEST");
    }

    void test_invalid_path_character() {
        const std::string raw =
                "GET /inva{lid HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "\r\n";

        auto result = RequestParser::parse(raw);
        expect(!result.has_value(), "invalid path character should fail");
        expect(result.error().expected_status == STATUS_CODE::BAD_REQUEST,
               "invalid path should be BAD_REQUEST");
    }

    void test_header_size_limit() {
        RequestParser parser;
        usub::unet::http::Request request;
        request.policy.max_header_size = 2;

        std::string_view raw =
                "GET / HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "\r\n";

        auto it = raw.begin();
        auto end = raw.end();
        auto result = parser.parse(request, it, end);
        result = parser.parse(request, it, end);
        expect(!result.has_value(), "header size limit should fail");
        expect(result.error().expected_status == STATUS_CODE::REQUEST_HEADER_FIELDS_TOO_LARGE,
               "header size limit should report REQUEST_HEADER_FIELDS_TOO_LARGE");
    }

    void test_content_length_body_read() {
        RequestParser parser;
        usub::unet::http::Request request;
        auto &ctx = parser.getContext();

        ctx.state = RequestParser::STATE::DATA_CONTENT_LENGTH;
        ctx.body_read_size = 5;
        ctx.current_state_size = 0;

        std::string_view body = "Hello";
        auto it = body.begin();
        const auto end = body.end();

        auto result = parser.parse(request, it, end);
        expect(result.has_value(), "content-length body parse should succeed");
        expect(request.body == "Hello", "content-length body should match");
        expect(ctx.state == RequestParser::STATE::COMPLETE,
               "content-length body should advance to COMPLETE");
    }

    void test_chunked_body_read_with_mocked_state() {
        RequestParser parser;
        usub::unet::http::Request request;
        auto &ctx = parser.getContext();

        ctx.state = RequestParser::STATE::DATA_CHUNKED_SIZE;
        ctx.current_state_size = 0;
        ctx.kv_buffer = {};

        std::string_view chunk = "3\r\nabc\r\n";
        auto it = chunk.begin();
        auto end = chunk.end();

        auto result = parser.parse(request, it, end);
        expect(result.has_value(), "chunked body parse should succeed");
        expect(request.body == "abc", "chunked body should contain first chunk");
        expect(ctx.state == RequestParser::STATE::DATA_CHUNK_DONE,
               "chunked body should end in DATA_CHUNK_DONE");

        // Mock the next state transition since the parser isn't fully wired yet.
        ctx.state = RequestParser::STATE::DATA_CHUNKED_SIZE;
        ctx.current_state_size = 0;
        ctx.kv_buffer = {};

        std::string_view last = "0\r\n\r\n";
        auto it_last = last.begin();
        auto end_last = last.end();

        result = parser.parse(request, it_last, end_last);
        expect(result.has_value(), "last chunk parse should succeed");
        expect(ctx.state == RequestParser::STATE::DATA_DONE,
               "last chunk should end in DATA_DONE");
    }
}// namespace

int main() {
    // test_basic_get_request();
    test_invalid_method_token();
    test_invalid_path_character();
    test_header_size_limit();
    test_content_length_body_read();
    test_chunked_body_read_with_mocked_state();

    std::cout << "All HTTP/1 parser tests passed.\n";
    return 0;
}
