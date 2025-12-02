namespace usub::unet::protocols::http {
    
    enum class RESPONSE_CODE : uint16_t { // TODO: complete
      NO_ERROR = 0, 
      OK = 200,
      BAD_REQUEST = 400,
      NOT_FOUND = 404,
      INTERNAL_SERVER_ERROR = 500,
    };

     /**
     * @enum VERSION
     * @brief Enumerates supported HTTP versions.
     */
    enum class VERSION : uint16_t {
        HTTP_0_9 = 9, ///< HTTP/0.9.
        HTTP_1_0 = 10,///< HTTP/1.0.
        HTTP_1_1 = 11,///< HTTP/1.1.
        HTTP_2_0 = 20,///< HTTP/2.0.
        HTTP_3_0 = 30 ///< HTTP/3.0.
    };

    struct ParseErrorInfo {
        RESPONSE_CODE code = RESPONSE_CODE::NO_ERROR; // Error code corresponding to HTTP response code
        std::string message;                          // textual explanation
        std::array<char, 256> place;                  // where the error happened // TODO: maybe make it 128?
        std::size_t position = 0;                     // position in the input where error was detected
    };

    // TODO: Think about FramingPolicy: wether to allow/disallow certain encodings: only chunked, only content-length, both, etc.
    // Allow trailers or not, etc.
    enum class FRAMING_POLICY : uint8_t {
        ALL = 0,
        CONTENT_LENGTH_ONLY = 1,
        CHUNKED_ONLY = 2,
    };

    struct MessagePolicy{
        static const FRAMING_POLICY framing = FRAMING_POLICY::ALL; // Allow both chunked and content-length // NO SUPPORT YET
        static const bool allow_trailers = false; // Disallow trailers for now // NO SUPPORT YET
        std::size_t max_header_size = 64 * 1024;
        std::size_t max_body_size = 8 * 1024 * 1024;
    };

    enum class TYPE : uint8_t {
        DOM = 0,   // Never goes into body Middleware, no matter the Transfer-Encoding
                   // The library will accumulate the whole body before calling parse
                   // parse is called once with the full body and move semantics
        MANUAL = 1,// Same as DOM, but user BodyType is responsible for accumulation
        SAX = 2,   // Will go into body Middleware when Transfer-Encoding: chunked,
                   // the library will feed body chunks to BodyType::parse as they arrive
    };

}