#include "Components/Headers/ContentType.h"

bool usub::server::component::header::ContentType::is_number(const std::string &s) const {
    if (s.empty()) return false;

    size_t start = 0;
    // Check for optional leading minus
    if (s[0] == '-') {
        if (s.size() == 1) return false;// Only "-" is not a number
        start = 1;
    }

    bool decimal_point = false;
    return std::all_of(s.begin() + start, s.end(), [&](char c) -> bool {
        if (c == '.') {
            if (decimal_point) return false;// More than one decimal point
            decimal_point = true;
            return true;
        }
        return std::isdigit(static_cast<unsigned char>(c));
    });
}

// Parses a single parameter key-value pair
void usub::server::component::header::ContentType::parseParam(const std::string &token) {
    size_t eq_pos = token.find('=');
    if (eq_pos == std::string::npos)
        return;

    std::string key = usub::utils::toLower(usub::utils::trim(token.substr(0, eq_pos)));
    std::string value = usub::utils::trim(token.substr(eq_pos + 1));

    // Remove surrounding quotes if present
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.size() - 2);
    }

    if (key == "charset") {
        this->charset_ = value;
    } else if (key == "boundary") {
        this->boundary_ = value;
    } else {
        this->params_[key] = value;
    }
}

// Constructors

usub::server::component::header::ContentType::ContentType() = default;

usub::server::component::header::ContentType::ContentType(const std::string &content_type) {
    this->parse(content_type);
}

// Getter Method Definitions

std::string usub::server::component::header::ContentType::getFullType() const {
    return this->type_ + "/" + this->subtype_;
}

const std::string &usub::server::component::header::ContentType::getType() const {
    return this->type_;
}

const std::string &usub::server::component::header::ContentType::getSubtype() const {
    return this->subtype_;
}

const std::string &usub::server::component::header::ContentType::getCharset() const {
    return this->charset_;
}

const std::string &usub::server::component::header::ContentType::getBoundary() const {
    return this->boundary_;
}

const std::string &usub::server::component::header::ContentType::getParam(const std::string &param_name) const {
    std::string lower_param = usub::utils::toLower(param_name);
    auto it = this->params_.find(lower_param);
    return it != this->params_.end() ? it->second : "";
}

// Check if the object is empty
bool usub::server::component::header::ContentType::empty() const {
    return this->type_.empty() || this->subtype_.empty();
}

void usub::server::component::header::ContentType::clear() {
    this->type_.clear();
    this->subtype_.clear();
    this->charset_.clear();
    this->boundary_.clear();
    this->params_.clear();
}

// Access parameter by name
const std::string &usub::server::component::header::ContentType::at(const std::string &param_name) const {
    return this->params_.at(usub::utils::toLower(param_name));
}

std::string &usub::server::component::header::ContentType::at(const std::string &param_name) {
    return this->params_.at(usub::utils::toLower(param_name));
}

// Overloaded subscript operator
std::string &usub::server::component::header::ContentType::operator[](const std::string &param_name) {
    return this->params_[usub::utils::toLower(param_name)];
}

bool usub::server::component::header::ContentType::operator==(const ContentType &other) const {
    if (this == &other) {
        return true;
    }
    return this->type_ == other.type_ && this->subtype_ == other.subtype_ && this->charset_ == other.charset_ &&
           this->boundary_ == other.boundary_ && this->params_ == other.params_;
}

usub::server::component::header::ContentType &usub::server::component::header::ContentType::operator=(const ContentType &other) {
    if (this == &other) {
        return *this;
    }
    this->clear();
    this->type_ = other.type_;
    this->subtype_ = other.subtype_;
    this->charset_ = other.charset_;
    this->boundary_ = other.boundary_;
    this->params_ = other.params_;
    return *this;
}

// Reconstruct the Content-Type header as a string
std::string usub::server::component::header::ContentType::string() const {
    // Estimate the final string size
    if (this->empty()) return "";
    size_t estimated_size = this->size();

    std::string result;
    result.reserve(estimated_size);

    result += "Content-Type: ";
    result += this->getFullType();

    if (!this->charset_.empty()) {
        result += "; charset=\"";
        result += this->charset_;
        result += "\"";
    }

    if (!this->boundary_.empty()) {
        result += "; boundary=\"";
        result += this->boundary_;
        result += "\"";
    }

    for (const auto &[key, value]: this->params_) {
        result += "; ";
        result += key;
        result += "=";
        if (this->is_number(value)) {
            result += value;
        } else {
            result += "\"";
            result += value;
            result += "\"";
        }
    }

    result += "\r\n";

    return result;
}

size_t usub::server::component::header::ContentType::size() const {
    if (this->empty()) return 0;
    size_t rv = 14 + this->type_.size();                             // "Content-Type: " + type
    rv += this->subtype_.empty() ? 2 : 1 + this->subtype_.size() + 2;// "; " or "/" + subtype + "; "
    rv += this->charset_.empty() ? 0 : this->charset_.size() + 2;    // charset + "; "
    rv += this->boundary_.empty() ? 0 : this->boundary_.size() + 2;  // boundary + "; "
    for (const auto &[key, value]: this->params_) {
        rv += key.size() + 1 + value.size() + 2;// key + "=" + value + "; "
    }// we dont remove last 2 because they are used in "/r/n"
    return rv;
}


// Parse method
uint8_t usub::server::component::header::ContentType::parse(const std::string &content_type) {
    // Clear existing data
    this->clear();

    size_t pos = 0;
    size_t len = content_type.length();
    if (len > 8192) return 0;
    std::string content_type_lower = usub::utils::toLower(content_type);

    // Check and remove "Content-Type:" prefix if present
    const std::string prefix = "content-type:";
    pos = content_type_lower.find(prefix);
    if (pos == std::string::npos) pos = 0;
    if (len >= prefix.size()) {
        std::string start = usub::utils::toLower(content_type_lower.substr(pos, pos + prefix.size()));
        if (start == prefix) {
            pos = prefix.size();
        }
    }

    // Parse the type and subtype
    size_t semicolon = content_type_lower.find(';', pos);
    std::string type_subtype;
    if (semicolon == std::string::npos) {
        type_subtype = usub::utils::trim(content_type_lower.substr(pos));
        pos = len;
    } else {
        type_subtype = usub::utils::trim(content_type_lower.substr(pos, semicolon - pos));
        pos = semicolon + 1;
    }

    // Split type and subtype
    size_t slash_pos = type_subtype.find('/');
    if (slash_pos != std::string::npos) {
        this->type_ = usub::utils::toLower(usub::utils::trim(type_subtype.substr(0, slash_pos)));
        this->subtype_ = usub::utils::toLower(usub::utils::trim(type_subtype.substr(slash_pos + 1)));
    } else {
        // Handle cases where '/' is missing
        this->type_ = usub::utils::toLower(type_subtype);
        this->subtype_.clear();
    }

    // Parse parameters
    while (pos < len) {
        // Find the next semicolon or end of string
        size_t quote = content_type.find('\"', pos);
        size_t next_semicolon = content_type.find(';', pos);
        if (quote <= next_semicolon)
            quote = content_type.find('\"', quote + 1);
        else
            quote = 0;
        std::string token;
        if (next_semicolon == std::string::npos && quote <= next_semicolon) {
            token = content_type.substr(pos);
            pos = len;
        } else if (quote > next_semicolon) {
            token = content_type.substr(pos, quote + 1 - pos);
            pos = quote + 2;
        } else {
            token = content_type.substr(pos, next_semicolon - pos);
            pos = next_semicolon + 1;
        }

        token = usub::utils::trim(token);
        if (!token.empty()) {
            this->parseParam(token);
        }
    }
    return 1;
}