#ifndef SERVER_QUERY_PARAMS_H
#define SERVER_QUERY_PARAMS_H

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Components/Encodings/PercentEncoded.h"

namespace usub::server::component::url {

    class QueryParams {
    private:
        std::string query_params_string_;
        std::unordered_map<std::string, std::vector<std::string>> query_params_map_;
        using iterator = decltype(query_params_map_)::iterator;
        using const_iterator = decltype(query_params_map_)::const_iterator;

    public:
        void addQueryParam(std::string_view key, std::string_view value);

        std::string &string();
        std::string string() const;

        std::vector<std::string> &operator[](const std::string &key);

        std::vector<std::string> at(const std::string &key);

        bool contains(const std::string &key) const;

        const std::vector<std::string> &at(const std::string &key) const;


        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator cbegin() const;
        const_iterator cend() const;
    };
}// namespace usub::server::component::url

#endif//SERVER_QUERY_PARAMS_H
