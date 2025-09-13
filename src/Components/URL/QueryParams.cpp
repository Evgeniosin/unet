#include "Components/URL/QueryParams.h"

void usub::server::component::url::QueryParams::addQueryParam(std::string_view key, std::string_view value) {
    this->query_params_map_[usub::server::component::PercentEncoded::decode(key)].push_back(usub::server::component::PercentEncoded::decode(value));
}

std::string &usub::server::component::url::QueryParams::string() {
    return this->query_params_string_;
}

std::string usub::server::component::url::QueryParams::string() const {
    return this->query_params_string_;
}
std::vector<std::string> &usub::server::component::url::QueryParams::operator[](const std::string &key) {
    return this->query_params_map_[key];
}

bool usub::server::component::url::QueryParams::contains(const std::string &key) const {
    return this->query_params_map_.contains(key);
}

std::vector<std::string> usub::server::component::url::QueryParams::at(const std::string &key) {
    return this->query_params_map_.at(key);
}

const std::vector<std::string> &usub::server::component::url::QueryParams::at(const std::string &key) const {
    return this->query_params_map_.at(key);
}

usub::server::component::url::QueryParams::iterator usub::server::component::url::QueryParams::begin() { return query_params_map_.begin(); }
usub::server::component::url::QueryParams::iterator usub::server::component::url::QueryParams::end() { return query_params_map_.end(); }

usub::server::component::url::QueryParams::const_iterator usub::server::component::url::QueryParams::begin() const { return query_params_map_.begin(); }
usub::server::component::url::QueryParams::const_iterator usub::server::component::url::QueryParams::end() const { return query_params_map_.end(); }

usub::server::component::url::QueryParams::const_iterator usub::server::component::url::QueryParams::cbegin() const { return query_params_map_.cbegin(); }
usub::server::component::url::QueryParams::const_iterator usub::server::component::url::QueryParams::cend() const { return query_params_map_.cend(); }
