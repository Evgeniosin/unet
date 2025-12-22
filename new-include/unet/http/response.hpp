#pragma once

#include <string>

#include "unet/header/generic.hpp"
#include "unet/http/message.hpp"


namespace usub::unet::http {

    struct ResponseMetadata {
        VERSION version{};
        STATUS_CODE status_code{};
        std::optional<std::string> status_message{};
    };

    struct Response {
        ResponseMetadata metadata{};
        usub::unet::header::Headers headers{};
        std::string body{};
        MessagePolicy policy{};// keep it last for easier initialization
    };

}// namespace usub::unet::http