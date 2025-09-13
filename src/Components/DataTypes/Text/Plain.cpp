#include "Components/DataTypes/Text/Plain.h"

usub::server::component::CLTextPlain::CLTextPlain(size_t content_length)
    : ContentLengthTransferBase("text/plain", content_length, PARSE_TYPE::SAX) {
}

usub::server::component::CLTextPlain &usub::server::component::CLTextPlain::append(const char &ch) {
    ContentLengthTransferBase::append(ch);
    return *this;
}

usub::server::component::CLTextPlain &usub::server::component::CLTextPlain::append(const std::string &data) {
    ContentLengthTransferBase::append(data);
    return *this;
}

usub::server::component::CLTextPlain &usub::server::component::CLTextPlain::clear() {
    ContentLengthTransferBase::clear();
    return *this;
}

std::string usub::server::component::CLTextPlain::serialize() {
    return ContentLengthTransferBase::serialize();
}

usub::server::component::CLTextPlain &usub::server::component::CLTextPlain::deserialize() {
    ContentLengthTransferBase::deserialize();
    return *this;
}

usub::server::component::CLTextPlain &usub::server::component::CLTextPlain::deserialize(const std::string &data) {
    ContentLengthTransferBase::deserialize(data);
    return *this;
}

std::string usub::server::component::CLTextPlain::string() {
    return this->data;
}

size_t usub::server::component::CLTextPlain::getTypeID() const {
    return TypeID<CLTextPlain>::id();
}

uint8_t usub::server::component::CLTextPlain::getState() const {
    return ContentLengthTransferBase::getState();
}

size_t usub::server::component::CLTextPlain::size() const {
    return this->bytes_read;
}

// template bool Registrar::registerClass<usub::server::component::ContentLengthTransferBase, usub::server::component::CLTextPlain>(const std::string &);

namespace {
    bool registered = ContentLengthFactory::instance().registerClass<usub::server::component::CLTextPlain>("text/plain");
    // bool registered = Registrar::registerClass<usub::server::component::ContentLengthTransferBase, usub::server::component::CLTextPlain>("text/plain");
}