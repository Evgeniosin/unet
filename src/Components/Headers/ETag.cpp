#include "Components/Headers/ETag.h"

usub::server::component::ETag::ETag(usub::server::component::ETag &&other) noexcept {
    this->etag_ = std::move(other.etag_);
    this->weak_ = other.weak_;
}

usub::server::component::ETag::ETag(const std::string &etag) {
    this->parse(etag);
}

usub::server::component::ETag::ETag(const usub::server::component::ETag &other) {
    this->etag_ = other.etag_;
    this->weak_ = other.weak_;
}

usub::server::component::ETag &usub::server::component::ETag::operator=(const usub::server::component::ETag &other) {
    if (this != &other) {
        this->etag_ = other.etag_;
        this->weak_ = other.weak_;
    }
    return *this;
}

usub::server::component::ETag &usub::server::component::ETag::operator=(usub::server::component::ETag &&other) noexcept {
    if (this != &other) {
        this->etag_ = std::move(other.etag_);
        this->weak_ = other.weak_;
    }
    return *this;
}

usub::server::component::ETag &usub::server::component::ETag::operator=(const std::string_view &etag) {
    this->parse(etag);
    return *this;
}

usub::server::component::ETag &usub::server::component::ETag::clear() {
    this->etag_.clear();
    this->weak_ = false;
    return *this;
}

bool usub::server::component::ETag::isWeak() const {
    return this->weak_;
}

bool usub::server::component::ETag::parse(std::string_view etag) {
    if (etag.empty()) return false;

    if (etag[0] == 'W' && etag[1] == '/') {
        this->weak_ = true;
        etag.remove_prefix(2);
    }

    if (etag[0] != '"') return false;
    etag.remove_prefix(1);

    size_t pos = etag.find('"');
    if (pos == std::string_view::npos) return false;

    this->etag_ = std::string(etag.substr(0, pos));
    etag.remove_prefix(pos + 1);

    if (!etag.empty()) return false;

    return true;
}

std::string usub::server::component::ETag::string() const {
    std::string etag;
    if (this->weak_) etag += "W/";
    etag += '"';
    etag += this->etag_;
    etag += '"';
    return etag;
}
