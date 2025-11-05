#pragma once

#include <string>

namespace usub::server::experimental {

    /**
     * @enum VERSION
     * @brief Enumerates supported HTTP versions.
     */
    enum class VERSION : uint16_t {
        NONE = 0,    ///< No HTTP version specified.
        HTTP_0_9 = 1,///< HTTP/0.9.
        HTTP_1_0 = 2,///< HTTP/1.0.
        HTTP_1_1 = 3,///< HTTP/1.1.
        HTTP_2_0 = 4,///< HTTP/2.0.
        HTTP_3_0 = 5,///< HTTP/3.0.
        BROKEN = 6   ///< Indicates a broken or unsupported HTTP version.
    };

    struct Settings {
        std::string method;   // Method is a string as the implementation before
        std::string path;     // URI class
        std::string scheme;   // h2/h3 can fill it out, h1 requires manual
        std::string authority;// h2/h3 or Host for h1
        VERSION version;      // h1 can fill it out, h2,h3 requires manual feeding
    };

    struct RequestBase {
        Settings settings;
    };

}// namespace usub::server::experimental