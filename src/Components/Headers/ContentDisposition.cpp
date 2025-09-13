#include "Components/Headers/ContentDisposition.h"

usub::server::component::FileInfo::FileInfo(const std::string &filename) {
    this->parse(filename);
}


uint8_t usub::server::component::FileInfo::parse(const std::string &filename) {
    if (filename.empty()) {
        return 0;
    }
    size_t first_quote = filename.find('\'');
    size_t second_quote = filename.find('\'', first_quote + 1);

    if (first_quote == std::string::npos || second_quote == std::string::npos) {
        this->charset_ = "";
        this->language_ = "";
        this->filename_ = filename;
        return 1;
    }
    this->charset_ = filename.substr(0, first_quote);
    this->language_ = filename.substr(first_quote + 1, second_quote - first_quote - 1);
    this->filename_ = filename.substr(second_quote + 1);

    return 1;
}

void usub::server::component::FileInfo::clear() {
    this->charset_.clear();
    this->filename_.clear();
    this->language_.clear();
}

std::string usub::server::component::FileInfo::string() const {
    if (this->charset_.empty()) {
        return "filename=\"" + this->filename_.string() + "\"";
    }

    std::string result;
    result += "filename*=";
    result.reserve(this->charset_.size() + 1 + this->language_.size() + 1 + this->filename_.string().size() + 1);

    result.append(this->charset_);
    result.append("\'");
    result.append(this->language_);
    result.append("\'");
    result.append(this->filename_.string());

    return result;
}

size_t usub::server::component::FileInfo::size() const {
    size_t rv = 0;
    if (this->empty()) {
        return 0;
    }
    if (this->charset_.empty()) {
        return 10 + this->filename_.string().size() + 1;// "filename" + "=" + '"' + filename + '"'
    }
    return 10 + this->charset_.size() + 1 + this->language_.size() + 1 + this->filename_.string().size();// filename* + "=" + charset + "'" + language + "'" + filename
}

std::ostream &usub::server::component::operator<<(std::ostream &os, const usub::server::component::FileInfo &file_info) {
    if (!file_info.charset_.empty()) {
        os << file_info.charset_ << "\'" << file_info.language_ << "\'" << file_info.filename_.string();
    } else {
        os << file_info.filename_.string();
    }

    return os;
}

usub::server::component::FileInfo &usub::server::component::FileInfo::operator=(const FileInfo &other) {
    if (this == &other) {
        return *this;
    }
    this->clear();
    this->charset_ = other.charset_;
    this->language_ = other.language_;
    this->filename_ = other.filename_;
    return *this;
}

bool usub::server::component::FileInfo::operator==(const FileInfo &other) const {
    if (this == &other) {
        return true;
    }
    return this->charset_ == other.charset_ && this->language_ == other.language_ && this->filename_ == other.filename_;
}

bool usub::server::component::header::ContentDisposition::is_number(const std::string &s) const {
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
void usub::server::component::header::ContentDisposition::parseParam(const std::string &token) {
    size_t eq_pos = token.find('=');
    if (eq_pos == std::string::npos)
        return;

    std::string key = usub::utils::toLower(usub::utils::trim(token.substr(0, eq_pos)));
    std::string value = usub::utils::trim(token.substr(eq_pos + 1));

    // Remove surrounding quotes if present
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.size() - 2);
    }

    if (key == "name") {
        this->name_ = value;
    } else if (key == "filename") {
        this->filename_ = value;
    } else if (key == "filename*") {
        this->filename_star_ = value;
    } else {
        this->params_[key] = value;
    }
}

// Constructors

usub::server::component::header::ContentDisposition::ContentDisposition() = default;

usub::server::component::header::ContentDisposition::ContentDisposition(const std::string &content_disposition) {
    this->parse(content_disposition);
}

// Getter Method Definitions
const std::string &usub::server::component::header::ContentDisposition::getDisposition() const {
    return this->disposition_;
}

const std::string &usub::server::component::header::ContentDisposition::getName() const {
    return this->name_;
}

const usub::server::component::FileInfo &usub::server::component::header::ContentDisposition::getFilename() const {
    return this->filename_;
}

const usub::server::component::FileInfo &usub::server::component::header::ContentDisposition::getFilenameStar() const {
    return this->filename_star_;
}

const std::unordered_map<std::string, std::string> &usub::server::component::header::ContentDisposition::getParams() const {
    return this->params_;
}

// Check if the object is empty
bool usub::server::component::header::ContentDisposition::empty() const {
    return this->disposition_.empty();
}

void usub::server::component::header::ContentDisposition::clear() {
    this->disposition_.clear();
    this->name_.clear();
    this->filename_.clear();
    this->filename_star_.clear();
    this->params_.clear();
}

// Access parameter by name
const std::string &usub::server::component::header::ContentDisposition::at(const std::string &param_name) const {
    return this->params_.at(usub::utils::toLower(param_name));
}

std::string &usub::server::component::header::ContentDisposition::at(const std::string &param_name) {
    return this->params_.at(usub::utils::toLower(param_name));
}

// Overloaded subscript operator
std::string &usub::server::component::header::ContentDisposition::operator[](const std::string &param_name) {
    return this->params_[usub::utils::toLower(param_name)];
}

usub::server::component::header::ContentDisposition &usub::server::component::header::ContentDisposition::operator=(const ContentDisposition &other) {
    if (this == &other) {
        return *this;
    }
    this->clear();
    this->disposition_ = other.disposition_;
    this->name_ = other.name_;
    this->filename_ = other.filename_;
    this->filename_star_ = other.filename_star_;
    this->params_ = other.params_;

    return *this;
}

bool usub::server::component::header::ContentDisposition::operator==(const ContentDisposition &other) const {
    if (this == &other) {
        return true;
    }
    return this->disposition_ == other.disposition_ &&
           this->name_ == other.name_ &&
           this->filename_ == other.filename_ &&
           this->filename_star_ == other.filename_star_ &&
           this->params_ == other.params_;
}

// Reconstruct the Content-Disposition header as a string
std::string usub::server::component::header::ContentDisposition::string() const {
    // Estimate the final string size to minimize reallocations
    size_t estimated_size = this->size();
    if (estimated_size == 0) return "";

    std::string result;
    result.reserve(estimated_size);

    // Append "Content-Disposition: " and disposition_
    result.append("Content-Disposition: ");
    result.append(this->disposition_);

    // Append "; name=\"<name>\""
    if (!this->name_.empty()) {
        result.append("; name=\"");
        result.append(this->name_);
        result.append("\"");
    }

    // Append "; filename=\"<filename>\""
    if (!this->filename_.empty()) {
        result.append("; ");
        result.append(this->filename_.string());
    }

    // Append "; filename*=\"<filename_star>\""
    if (!this->filename_star_.empty()) {
        result.append("; ");
        result.append(this->filename_star_.string());
    }

    // Append additional parameters
    for (const auto &[key, value]: this->params_) {
        result.append("; ");
        result.append(key);
        result.append("=");
        if (this->is_number(value)) {
            result.append(value);
        } else {
            result.append("\"");
            result.append(value);
            result.append("\"");
        }
    }
    result.append("\r\n");
    return result;
}

size_t usub::server::component::header::ContentDisposition::size() const {
    if (this->empty()) {
        return 0;
    }
    size_t rv = 20 + this->disposition_.size();                              // "Content-Disposition: " + disposition_
    rv += this->name_.empty() ? 0 : 8 + this->name_.size() + 1;              // '; name="' + name_ + '"'
    rv += this->filename_.empty() ? 0 : 2 + this->filename_.size();          // '; filename="' + filename_ + '"'
    rv += this->filename_star_.empty() ? 0 : 2 + this->filename_star_.size();// '; filename*=' + filename_star_
    for (const auto &[key, value]: this->params_) {
        rv += 2 + key.size() + 1 + value.size();// "; " + key + "=" + value
        if (!this->is_number(value)) {
            rv += 2;// Quotes around non-numeric value
        }
    }
    rv += 2;// For the final "\r\n"
    return rv;
}


// Parse method
uint8_t usub::server::component::header::ContentDisposition::parse(const std::string &content_disposition) {
    // Clear existing data
    this->clear();

    size_t pos = 0;
    size_t len = content_disposition.length();
    if (len > 8192) return 0;
    std::string content_disposition_lower = usub::utils::toLower(content_disposition);

    const std::string prefix = "content-disposition:";
    pos = content_disposition_lower.find(prefix);
    if (pos == std::string::npos) pos = 0;
    else
        pos += prefix.size();

    size_t semicolon = content_disposition_lower.find(';', pos);
    if (semicolon == std::string::npos) {
        this->disposition_ = usub::utils::toLower(usub::utils::trim(content_disposition_lower.substr(pos)));
        return 1;
    }

    this->disposition_ = usub::utils::toLower(usub::utils::trim(content_disposition_lower.substr(pos, semicolon - pos)));
    pos = semicolon + 1;

    while (pos < len) {
        size_t quote = content_disposition_lower.find('\"', pos);
        size_t next_semicolon = content_disposition_lower.find(';', pos);
        if (quote <= next_semicolon)
            quote = content_disposition_lower.find('\"', quote + 1);
        else
            quote = 0;
        std::string token;
        if (next_semicolon == std::string::npos && quote <= next_semicolon) {
            token = content_disposition.substr(pos);
            pos = len;
        } else if (quote > next_semicolon) {
            token = content_disposition.substr(pos, quote + 1 - pos);
            pos = quote + 2;
        } else {
            token = content_disposition.substr(pos, next_semicolon - pos);
            pos = next_semicolon + 1;
        }
        token = usub::utils::trim(token);
        if (!token.empty()) {
            this->parseParam(token);
        }
    }
    return 1;
}
