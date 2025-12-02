// Concepts for BodyType
// Okay, this requires a lot of explanation. I'll clarify on some of the behaviour andthe meaning I put into those parser types.
// First of all, the BodyType class is only responsible for parsing the pure body content, without any framing.
// Framing (Transfer-Encoding, Content-Length) is handled by the library, and the body parser is only fed the actual body data.
// Secondly parsing is done in a single pass, without any resets or rewinds. So chunks of chunked encoding are parsed in the next order
// assuming SAX or MANUAL parsers: read size -> feed chunk to parser (min(chunk_size, remaining_packet_size)) -> read next size ->
// repeat until 0 size chunk is read. You should keep that in mind when implementing your BodyType parsers.
// DOM - expects to receive the whole body at once, and parse it in one go. The library is responsible for accumulating
// and feeding the whole body. move semantics is enforced to try and avoid copies. This may not be possible in all cases,
// but at least we try. It will never go into body Middleware, no matter how big the body is, and how many frames or packets
// it was split into.
// Manual - same as DOM, but the user BodyType is responsible for accumulation. The library will just feed it the body subview, where the
// data must be. Since the library can't be fully responsible for validation of framing in that case due to it being a single pass,
// the framing may bleed into the body data if the client is malicious or buggy.
// It's likely that such behaviour will break parsing on the subsequent chunk size read attempt. But this is an important part for safety.
// SAX - the library will feed body chunks to BodyType::parse as they arrive. And the intermediate results are shown in BODY middleware.
// Every parser is expected to fully consume the chunk it is given, otherwise the data WILL be lost. The library won't try to re-feed any unconsumed data.
// At least for now. We may switch to iterators or something similar in the future to allow partial consumption.
// But that would mean that user parser would be partially responsible for framing validation, which is not ideal.

template<class T>
concept has_type_tag = requires { { T::type } -> std::convertible_to<TYPE>; };

template<class T>
concept is_dom_parser = has_type_tag<T> && (T::type == TYPE::DOM);

template<class T>
concept is_manual_parser = has_type_tag<T> && (T::type == TYPE::MANUAL);

template<class T>
concept is_sax_parser = has_type_tag<T> && (T::type == TYPE::SAX);

template<class T>
concept dom_parser_compatable = requires(T t) {
    { t.parse(std::declval<std::string &&>()) } -> std::same_as<std::expected<void, ParseErrorInfo>>;
};

template<class T>
concept manual_parser_compatable = requires(T t, std::string_view view) {
    { t.parse(view) } -> std::same_as<std::expected<void, ParseErrorInfo>>;
};

template<class T>
concept sax_parser_compatable = requires(T t, std::string_view chunk) {
    { t.parse(chunk) } -> std::same_as<std::expected<void, ParseErrorInfo>>;// TODO: maybe some other signature? Dafuq
};

template<class T>
concept body_type_compatible =
        (is_dom_parser<T> && dom_parser_compatable<T>) ||
        (is_sax_parser<T> && sax_parser_compatable<T>) ||
        (is_manual_parser<T> && manual_parser_compatable<T>);
