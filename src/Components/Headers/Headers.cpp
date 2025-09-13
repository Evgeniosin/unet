#include "Components/Headers/Headers.h"
#include "utils/utils.h"

const std::set<std::string> usub::server::component::Headers::unique_header_names = {
        "host",             // Only one Host header is allowed
        "content-length",   // Single Content-Length header
        "transfer-encoding",// Single Transfer-Encoding header
        "content-type",     // Single Content-Type header
        "authorization",    // Single Authorization header
        "user-agent",       // Single User-Agent header
        "accept",           // Single Accept header
        "accept-encoding",  // Single Accept-Encoding header
        "accept-language",  // Single Accept-Language header
        "referer",          // Single Referer header
        "date",             // Single Date header
        "server",           // Single Server header
        "connection",       // Single Connection header
        "cache-control"     // Single Cache-Control header
};

usub::server::component::Headers *usub::server::component::Headers::setUniqueHeader(std::string key, std::string value) {
    std::string key_lower = usub::utils::toLower(key);
    if (this->unique_header_names.contains(key_lower) && this->unique_headers_.contains(key_lower)) {
        auto it = this->unique_headers_.find(key_lower);
        this->size_ -= it->first.size() + it->second.size() + 4;// 2 for ": " between key and value and 2 for "\r\n" at the end of the header
        this->size_ += key_lower.size() + value.size() + 4;     // 2 for ": " between key and value and 2 for "\r\n" at the end of the header
        this->unique_headers_[key_lower] = value;
    } else if (this->unique_header_names.contains(key_lower)) {
        this->size_ += key_lower.size() + value.size() + 4;// 2 for ": " between key and value and 2 for "\r\n" at the end of the header
        this->unique_headers_.emplace(key_lower, value);
    } else {
        return this;
    }
    return this;
}

void usub::server::component::Headers::clear() {
    this->size_ = 0;
    this->unique_headers_.clear();
    this->headers_.clear();
}


std::string usub::server::component::Headers::string() const {
    std::string res;
    res.reserve(this->size_);
    for (auto &header: this->unique_headers_) {
        res.append(header.first);
        res.append(": ");
        res.append(header.second);
        res.append("\r\n");
    }
    for (auto header = headers_.begin(); header != headers_.end();) {
        const auto key = header->first;
        res.append(key);
        res.append(": ");

        auto range = headers_.equal_range(key);
        bool first = true;
        for (auto valIt = range.first; valIt != range.second; ++valIt) {
            if (!first) {
                res.append("; ");
            }
            first = false;
            res.append(valIt->second);
        }
        res.append("\r\n");

        header = range.second;
    }

    return res;
}

size_t usub::server::component::Headers::size() const {
    return this->size_;
}

bool usub::server::component::Headers::contains(std::string_view key) {
    std::string key_str = usub::utils::toLower(key.data());
    return (this->unique_headers_.contains(key_str) || this->headers_.contains(key_str));
}

void usub::server::component::Headers::erase(std::string_view key) {
    std::string key_str = usub::utils::toLower(key.data());
    if (this->unique_headers_.contains(key_str)) {
        this->size_ -= key_str.size() + this->unique_headers_.at(key_str).size() + 4;// 2 for ": " between key and value and 2 for "\r\n" at the end of the header
        this->unique_headers_.erase(key_str);
    } else {
        auto range = this->headers_.equal_range(key_str);
        for (auto it = range.first; it != range.second; ++it) {
            this->size_ -= key_str.size() + it->second.size() + 4;// 2 for ": " between key and value and 2 for "\r\n" at the end of the header
        }
        this->headers_.erase(key_str);
    }
    return;
}

void usub::server::component::Headers::erase(std::string_view key, std::string_view value) {
    std::string key_str = usub::utils::toLower(key.data());
    if (this->unique_headers_.contains(key_str) && this->unique_headers_.at(key_str) == value) {
        this->size_ -= key_str.size() + value.size() + 4;// 2 for ": " between key and value and 2 for "\r\n" at the end of the header
        this->unique_headers_.erase(key_str);
    } else {
        auto range = this->headers_.equal_range(key_str);
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == value) {
                this->size_ -= key_str.size() + value.size() + 4;// 2 for ": " between key and value and 2 for "\r\n" at the end of the header
                this->headers_.erase(it);
                return;
            }
        }
    }
    return;
}

void usub::server::component::Headers::emplace(std::string_view key, std::string_view value) {
    std::string key_str = usub::utils::toLower(key.data());
    if (this->unique_header_names.contains(key_str) && !this->unique_headers_.contains(key_str)) {
        this->size_ += key_str.size() + value.size() + 4;// 2 for ": " between key and value and 2 for "\r\n" at the end of the header
        this->unique_headers_.emplace(key_str, value);
    } else {

        this->headers_.contains(key_str) ? this->size_ += key_str.size() + value.size() + 4 : this->size_ += value.size() + 2;// 2 for ": " between key and value or ", " and 2
                                                                                                                              // for "\r\n" at the end of the header
                                                                                                                              // but "\r\n" is only once per header
        this->headers_.emplace(key_str, value);
    }
    return;
}

void usub::server::component::Headers::insert(std::string_view key, std::string_view value) {
    this->emplace(key, value);
}

std::vector<std::string> usub::server::component::Headers::getHeaders(std::string_view key) {
    std::vector<std::string> rv;
    auto range = this->headers_.equal_range(key.data());
    for (auto it = range.first; it != range.second; ++it) {
        rv.push_back(it->second);
    }
    if (this->unique_header_names.contains(key.data())) rv.push_back(this->unique_headers_.at(key.data()));
    return rv;
}

std::string usub::server::component::Headers::getUniqueHeader(std::string_view key) {
    std::string key_str = usub::utils::toLower(key.data());
    return this->unique_headers_.contains(key_str) ? this->unique_headers_.at(key_str) : "";
}

std::vector<std::string> usub::server::component::Headers::getMultuHeaders(std::string_view key) {
    std::string key_str = usub::utils::toLower(key.data());
    std::vector<std::string> rv;
    auto range = this->headers_.equal_range(key_str);
    for (auto it = range.first; it != range.second; ++it) {
        rv.push_back(it->second);
    }
    return rv;
}
