#pragma once

#include <string>
#include "Protocols/HTTP/Experimental/Generic.h"
#include "Components/URI.hpp"

namespace usub::unet::protocols::http {

    static std::size_t max_method_token_size = 40;// Arguably should be much smaller, but let's be generous
    static std::size_t max_uri_size = 16 * 1024;  // Very generous limit for URI
    
    struct Settings {
        std::string method;            // Method is a string as the implementation before
        usub::unet::component::URI uri;// usually just path, but can be full URI 
        VERSION version{VERSION::NONE};// h1 can fill it out, h2,h3 requires manual feeding
    };

    struct RequestBase {
        Settings settings;
    };
    
    template<class BodyType, class HeaderType>
    struct RequestT : public RequestBase {
        BodyType body;
        HeaderType headers;
    };

    template<class BodyType, class HeaderType> /*, class HTTPParser>*/ // TODO: Allow for overloading parser in the future
    struct RequestParserT : public RequestT<BodyType, HeaderType> {
        // friend HTTPParser;

        using RequestT = RequestT<BodyType, HeaderType>;

        MessagePolicy policy;

        void clean() {
            this->progress = ParseProgress{};
            this->settings = Settings{};
            this->headers = HeaderType{};
            this->body = BodyType{};
        }
    };
}