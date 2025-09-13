//
// Created by kirill on 12/23/24.
//
#include "Components/DataTypes/Application/UrlEncoded.h"


usub::server::component::CLURLEncoded::CLURLEncoded(size_t content_length)
    : ContentLengthTransferBase("urlencoded", content_length, PARSE_TYPE::SAX) {
}

usub::server::component::CLURLEncoded& usub::server::component::CLURLEncoded::append(const char& ch) {
    ContentLengthTransferBase::append(ch);
    return *this;
}

usub::server::component::CLURLEncoded& usub::server::component::CLURLEncoded::append(const std::string& data) {
    ContentLengthTransferBase::append(data);
    return *this;
}

usub::server::component::CLURLEncoded& usub::server::component::CLURLEncoded::clear() {
    ContentLengthTransferBase::clear();
    buffer_ = {};
    current_phase_ = ParsePhase::KEY;
    this->state = STATE::OK;
    query_params_ = url::QueryParams();
    return *this;
}

usub::server::component::CLURLEncoded& usub::server::component::CLURLEncoded::deserialize(const std::string& data) {
    for (const auto& ch : data) {
        switch (this->current_phase_) {
            case ParsePhase::KEY:
                switch (ch) {
                    case '=':
                        this->current_phase_ = ParsePhase::VALUE;
                        break;
                    case '&':
                    case '\r':
                    case '\n':
                        this->state = STATE::ERROR;
                        break;
                    default:
                        this->buffer_.first += ch;
                        break;
                }
                break;
            case ParsePhase::VALUE:
                switch (ch) {
                    case '&':
                        this->query_params_.addQueryParam(buffer_.first, buffer_.second);
                        buffer_ = {};
                        current_phase_ = ParsePhase::KEY;
                        break;
                    case '\r':
                    case '\n':
                        this->state = STATE::ERROR;
                        break;
                    default:
                        this->buffer_.second += ch;
                        break;
                }
        }
    }
    if (current_phase_ == ParsePhase::VALUE) {
        this->query_params_.addQueryParam(buffer_.first, buffer_.second);
    }
    this->query_params_.string() = data;
    this->bytes_read = data.size();
    return *this;
}

usub::server::component::CLURLEncoded& usub::server::component::CLURLEncoded::deserialize() {
    for (const auto& ch : this->data) {
        switch (this->current_phase_) {
            case ParsePhase::KEY:
                switch (ch) {
                    case '=':
                        this->current_phase_ = ParsePhase::VALUE;
                        break;
                    case '&':
                    case '\r':
                    case '\n':
                        this->state = STATE::ERROR;
                        break;
                    default:
                        this->buffer_.first += ch;
                        break;
                }
                break;
            case ParsePhase::VALUE:
                switch (ch) {
                    case '&':
                        this->query_params_.addQueryParam(buffer_.first, buffer_.second);
                        buffer_ = {};
                        current_phase_ = ParsePhase::KEY;
                        break;
                    case '\r':
                    case '\n':
                        this->state = STATE::ERROR;
                        break;
                    default:
                        this->buffer_.second += ch;
                        break;
                }
        }
    }
    if (this->current_phase_ == ParsePhase::VALUE) {
        query_params_.addQueryParam(buffer_.first, buffer_.second);
    }
    return *this;
}

std::string usub::server::component::CLURLEncoded::serialize() {
    return query_params_.string();
}

std::string usub::server::component::CLURLEncoded::string() {
    return query_params_.string();
}

size_t usub::server::component::CLURLEncoded::getTypeID() const {
    return TypeID<usub::server::component::CLURLEncoded>::id();
}

uint8_t usub::server::component::CLURLEncoded::getState() const {
    return this->state;
}

size_t usub::server::component::CLURLEncoded::size() const {
    return this->bytes_read;
}

const usub::server::component::url::QueryParams& usub::server::component::CLURLEncoded::getQueryParams() const {
    return this->query_params_;
}
