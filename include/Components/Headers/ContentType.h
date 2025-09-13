#ifndef CONTENT_TYPE_H
#define CONTENT_TYPE_H

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>


//local headers
#include <utils/utils.h>

namespace usub::server::component::header {
    class ContentType {
    private:
        // Member Variables
        std::string type_;
        std::string subtype_;
        std::string charset_;
        std::string boundary_;
        std::unordered_map<std::string, std::string> params_;

        // Utility Functions
        bool is_number(const std::string &s) const;

        // Parses a single parameter key-value pair
        void parseParam(const std::string &token);

    public:
        // Constructors and Destructor
        ContentType();
        explicit ContentType(const std::string &content_type);
        ~ContentType() = default;

        // Getter Methods
        std::string getFullType() const;
        const std::string &getType() const;
        const std::string &getSubtype() const;
        const std::string &getCharset() const;
        const std::string &getBoundary() const;
        const std::string &getParam(const std::string &param_name) const;

        void setType(const std::string &type) { this->type_ = type; };
        void setSubtype(const std::string &subtype) { this->subtype_ = subtype; };

        // Check if the object is empty
        bool empty() const;

        void clear();
        // Access parameter by name
        const std::string &at(const std::string &param_name) const;
        std::string &at(const std::string &param_name);

        // Overloaded subscript operator
        std::string &operator[](const std::string &param_name);
        ContentType &operator=(const ContentType &other);
        bool operator==(const ContentType &other) const;

        // Reconstruct the Content-Type header as a string
        std::string string() const;
        size_t size() const;

        // Parse method
        [[maybe_unused]] uint8_t parse(const std::string &content_type_str);
    };

}// namespace usub::server::protocols::format

#endif// CONTENT_TYPE_H
