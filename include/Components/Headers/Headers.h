#ifndef HEADERS_H
#define HEADERS_H

#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

// Local Includes
#include "ContentDisposition.h"
#include "ContentType.h"

#include "utils/placeholder.h"

namespace usub::server::component {


    class Headers {
    public:
        size_t size_{};

    public:
        Headers() = default;
        virtual Headers() = 0;

        virtual void clear() = 0;
        virtual std::string string() = 0;
        virtual size_t size() = 0;

        static bool isFieldNameChar(char c);
        static bool isFieldName(std::string_view value);
        static bool isFieldValueChar(char c);
        static bool isFieldValue(std::string_view value);
    };

}// namespace usub::server::component


#endif//HEADERS_H
