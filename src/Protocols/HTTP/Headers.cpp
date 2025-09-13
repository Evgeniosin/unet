#include "Protocols/HTTP/Headers.h"
#include "Protocols/HTTP/header_lookup.h"

usub::server::protocols::http::Headers &usub::server::protocols::http::Headers::clear() {
    this->known_headers_map_.clear();
    this->unknown_headers_map_.clear();
    return *this;
}

bool usub::server::protocols::http::Headers::contains(std::string_view key) const {
    auto lookup = HTTPHeaderLookup::lookupHeader(key.data(), key.size());
    if (lookup) [[likely]] {
        return this->known_headers_map_.contains(lookup->id);
    } else {
        return this->unknown_headers_map_.contains(usub::utils::toLower(std::string(key)));
    }
}

bool usub::server::protocols::http::Headers::containsValue(std::string_view key, std::string_view token, bool ignore_case) const {
    auto lookup = HTTPHeaderLookup::lookupHeader(key.data(), key.size());

    if (lookup) [[likely]] {
        auto it = known_headers_map_.find(lookup->id);
        if (it == known_headers_map_.end()) return false;

        for (const auto &val: it->second) {
            if (ignore_case ? usub::utils::icmp(val, token) : val == token)
                return true;
        }
        return false;
    } else {
        auto it = unknown_headers_map_.find(usub::utils::toLower(std::string(key)));
        if (it == unknown_headers_map_.end()) return false;

        for (const auto &val: it->second) {
            if (ignore_case ? usub::utils::icmp(val, token) : val == token)
                return true;
        }
        return false;
    }
}

bool usub::server::protocols::http::Headers::containsValue(usub::server::component::HeaderEnum key, std::string_view token, bool ignore_case) const {
    auto it = known_headers_map_.find(key);
    if (it == known_headers_map_.end()) return false;

    for (const auto &val: it->second) {
        if (ignore_case ? usub::utils::icmp(val, token) : val == token)
            return true;
    }
    return false;
}


bool usub::server::protocols::http::Headers::contains(usub::server::component::HeaderEnum key) const {
    return this->known_headers_map_.contains(key);
}

const std::vector<std::string> &usub::server::protocols::http::Headers::at(std::string_view key) const {
    auto lookup = HTTPHeaderLookup::lookupHeader(key.data(), key.size());
    if (lookup) [[likely]] {
        return this->known_headers_map_.at(lookup->id);
    } else {
        return this->unknown_headers_map_.at(usub::utils::toLower(std::string(key)));
    }
}

const std::vector<std::string> &usub::server::protocols::http::Headers::at(usub::server::component::HeaderEnum key) const {
    return this->known_headers_map_.at(key);
}

usub::server::protocols::http::Headers &usub::server::protocols::http::Headers::erase(std::string_view key_view) {
    auto lookup = HTTPHeaderLookup::lookupHeader(key_view.data(), key_view.size());
    if (lookup) [[likely]] {
        auto it = known_headers_map_.find(lookup->id);
        if (it != known_headers_map_.end()) {
            known_headers_map_.erase(it);
        }
    } else {
        unknown_headers_map_.erase(usub::utils::toLower(std::string(key_view)));
    }
    return *this;
}


const size_t usub::server::protocols::http::Headers::size() const {
    size_t total_size = 0;

    // Iterate over known_headers_map_
    for (const auto &pair: known_headers_map_) {
        total_size += 2;                                                                                                 // Add 2 for the key (':' and space optionally)
        total_size += std::strlen(usub::server::component::header_enum_to_string_lower[static_cast<size_t>(pair.first)]);// Add the size of the key
        for (const auto &value: pair.second) {
            total_size += value.size() + 2;// Add the size of each value + 2 (for the comma and space optionally)
        }
    }

    // Iterate over unknown_headers_map_
    for (const auto &pair: unknown_headers_map_) {
        total_size += pair.first.size() + 2;// Add the size of the key + 2
        total_size += pair.second.size();   // Add the number of values in the vector
        for (const auto &value: pair.second) {
            total_size += value.size() + 2;// Add the size of each value
        }
    }

    return total_size;
}

std::vector<std::string> &usub::server::protocols::http::Headers::operator[](std::string_view key) {
    auto lookup = HTTPHeaderLookup::lookupHeader(key.data(), key.size());
    if (lookup) [[likely]] {
        return known_headers_map_[lookup->id];
    } else {
        return unknown_headers_map_[usub::utils::toLower(std::string(key))];
    }
}

std::vector<std::string> &usub::server::protocols::http::Headers::operator[](usub::server::component::HeaderEnum key) {
    return known_headers_map_[key];
}
