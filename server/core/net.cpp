#include "net.h"

namespace usub::server::core {
    REQUEST_TYPE RequestTypeStr::getType(const std::string &type) {
        if (types.find(type) != types.end()) {
            return types.at(type);
        }
        return GET;
    }

    const std::unordered_map<std::string, REQUEST_TYPE> RequestTypeStr::types = {
        {"GET", GET},
        {"POST", POST},
        {"HEAD", HEAD},
        {"PUT", PUT},
        {"DELETE", DELETE},
        {"OPTIONS", OPTIONS},
        {"TRACE", TRACE},
        {"CONNECT", CONNECT},
        {"PATCH", PATCH}
    };
}