#ifndef USUB_NET_H
#define USUB_NET_H

#include <cstdint>
#include <unordered_map>
#include <string>

namespace usub::server::core {

    enum IPV : uint16_t {
        IPV4 = 4,
        IPV6 = 6
    };

    enum DATA_TYPE {
        NONE = 0,
        BUFFER = 1,
        FD = 2
    };

    enum REQUEST_TYPE : int {
        GET = 1 << 0,
        POST = 1 << 1,
        HEAD = 1 << 2,
        PUT = 1 << 3,
        DELETE = 1 << 4,
        OPTIONS = 1 << 5,
        TRACE = 1 << 6,
        CONNECT = 1 << 7,
        PATCH = 1 << 8
    };

    class RequestTypeStr {
    public:

        [[nodiscard]] static REQUEST_TYPE getType(const std::string &type);

        static const std::unordered_map<std::string, REQUEST_TYPE> types;

    };

}

#endif // USUB_NET_H
