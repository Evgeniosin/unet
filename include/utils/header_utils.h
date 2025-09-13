#include <string>

namespace usub::utils {

    bool isToken(std::string_view value);
    bool areTokens(std::string_view value);
    bool isValidEtag(std::string_view value);
    bool areValidEtags(std::string_view value);// TODO: Optimize & test
}// namespace usub::utils