#ifndef URL_H
#define URL_H

#include <array>
#include <cstdint>
#include <string>


#include "Components/Encodings/PercentEncoded.h"
#include "Components/URL/QueryParams.h"

namespace usub::server::component {


    /*
         foo://example.com:8042/over/there?name=ferret#nose
         \_/   \______________/\_________/ \_________/ \__/
          |           |            |            |        |
       scheme     authority       path        query   fragment
          |   _____________________|__
         / \ /                        \
         urn:example:animal:ferret:nose
*/
    class URN {
    private:
        std::string scheme_;
        std::string authority_;
        std::string path_;
        url::QueryParams query_;
        std::string fragment_;

    public:
        URN() = default;
        ~URN() = default;

        std::string &getScheme();
        std::string &getAuthority();
        std::string &getPath();
        const std::string &getPath() const;
        url::QueryParams &getQueryParams();
        const url::QueryParams &getQueryParams() const;
        std::string &getFragment();

        URN &setScheme(std::string_view scheme);
        URN &setAuthority(std::string_view authority);
        URN &setPath(std::string_view path);
        URN &setQueryParams(url::QueryParams query_params);
        URN &addQueryParam(std::string_view key, std::string_view value);
        URN &setFragment(std::string_view fragment);

        bool parse(std::string_view urn);
        bool parse(std::string &&urn);

        static bool isSchemeChar(char c);
        static bool isAuthorityChar(char c);
        static bool isPathChar(char c);
        static bool isQueryChar(char c);
        static bool isFragmentChar(char c);

        // std compatability
        std::string string() const;
        std::vector<std::string> &operator[](const std::string &key);
        const std::vector<std::string> &operator[](const std::string &key) const;
    };

}// namespace usub::server::component

#endif// URL_H
