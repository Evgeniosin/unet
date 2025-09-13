#include "Components/DataTypes/Multipart/FormData.h"

void usub::server::protocols::format::multipart::FormDataData::parseHeader(
        const std::string &line) {
    size_t colon = line.find(':');
    if (colon == std::string::npos)
        return;
    std::string key = usub::utils::toLower(usub::utils::trim(line.substr(0, colon)));
    std::string value = usub::utils::trim(line.substr(colon + 1));
    this->form_data_headers_[key] = value;
}

void usub::server::protocols::format::multipart::FormDataData::done() {
    this->done_ = true;
}

void usub::server::protocols::format::multipart::FormDataData::clear() {
    this->content_disposition_.clear();
    this->content_type_.clear();
    this->content_.clear();
    this->form_data_headers_.clear();
    this->done_ = false;
}

std::string usub::server::protocols::format::multipart::FormDataData::string() const {
    size_t estimated_size = this->size();
    std::string rv;
    rv.reserve(estimated_size);
    rv.append(this->content_disposition_.string());
    rv.append(this->content_type_.string());
    for (const auto &header: this->form_data_headers_) {
        rv.append(header.first);
        rv.append(": ");
        rv.append(header.second);
        rv.append("\r\n");
    }
    rv.append("\r\n");
    rv.append(this->content_);
    rv.append("\r\n");

    return rv;
}

size_t usub::server::protocols::format::multipart::FormDataData::size() const {
    size_t rv = 0;
    rv += this->content_disposition_.size() + this->content_type_.size();// Content-Disposition + Content-Type
    for (const auto &header: this->form_data_headers_) {
        rv += header.first.size() + header.second.size() + 4;// ": " + "\r\n"
    }
    rv += 2;// "\r\n"
    rv += this->content_.size();
    return rv;
}

bool usub::server::protocols::format::multipart::FormDataData::operator==(const FormDataData &other) const {
    return this->content_disposition_ == other.content_disposition_ &&
           this->content_type_ == other.content_type_ &&
           this->content_ == other.content_ &&
           this->form_data_headers_ == other.form_data_headers_;
}

uint8_t usub::server::protocols::format::multipart::FormData::parse(
        const std::string &form_data, const std::string &boundary) {
    if (form_data.empty())
        return this->state_;
    if (this->state_ == FORM_DATA_PARSER_STATE::FINISHED)
        return this->state_;
    if (boundary.empty() && this->boundary_.empty()) {
        this->state_ = FORM_DATA_PARSER_STATE::ERROR;
        return this->state_;
    } else if (this->boundary_.empty()) {
        this->boundary_ = boundary;
    }

    std::string current_boundary = "--" + this->boundary_;

    bool newline = false;
    bool carriage_return = false;
    std::istringstream stream(form_data);
    std::string line{};
    size_t vec_size{};

    while (std::getline(stream, line)) {
        newline = false;
        carriage_return = false;
        std::streampos p = stream.tellg();
        stream.seekg(p - std::streamoff(1));
        if (stream.peek() == '\n') {
            newline = true;
        }
        stream.seekg(p);
        if (line.back() == '\r') {
            carriage_return = true;
            line.pop_back();
            if (!unfinished_string_.empty()) {
                line = unfinished_string_ + line;
                unfinished_string_.clear();
            }
        } else {
            unfinished_string_ += line;
            if (this->state_ != FORM_DATA_PARSER_STATE::READING_CONTENT) {
                if (unfinished_string_.size() >= 8192) {
                    this->state_ = FORM_DATA_PARSER_STATE::ERROR;
                    return this->state_;
                }
                break;
            } else {
                unfinished_string_.clear();
            }
        }
        switch (this->state_) {
            case FORM_DATA_PARSER_STATE::WAITING_FOR_BOUNDARY:
                if (line == current_boundary) {
                    this->state_ = FORM_DATA_PARSER_STATE::READING_HEADERS;
                    break;
                } else if (line == current_boundary + "--") {
                    this->state_ = FORM_DATA_PARSER_STATE::ERROR;
                    break;
                }
                break;
            case FORM_DATA_PARSER_STATE::READING_HEADERS:
                if (line.size() >= 19 &&
                    usub::utils::toLower(line.substr(0, 19)) == "content-disposition") {
                    this->unfinished_part_.second.getContentDisposition().parse(line);
                    this->unfinished_part_.first =
                            this->unfinished_part_.second.getContentDisposition().getName();
                } else if (line.size() >= 12 &&
                           usub::utils::toLower(line.substr(0, 12)) == "content-type") {
                    this->unfinished_part_.second.getContentType().parse(line);
                } else if (line.empty()) {
                    if (this->unfinished_part_.second.getContentType().empty()) {
                        this->unfinished_part_.second.getContentType().setType("text");
                        this->unfinished_part_.second.getContentType().setSubtype("plain");
                    }
                    if (this->unfinished_part_.first.empty() ||
                        this->unfinished_part_.second.getContentDisposition().empty()) {
                        this->state_ = FORM_DATA_PARSER_STATE::ERROR;
                        return this->state_;
                    }
                    this->parts_[this->unfinished_part_.first].push_back(
                            this->unfinished_part_.second);
                    this->unfinished_part_.second.clear();
                    this->state_ = FORM_DATA_PARSER_STATE::READING_CONTENT;
                } else {
                    this->unfinished_part_.second.parseHeader(line);
                }
                break;
            case FORM_DATA_PARSER_STATE::READING_CONTENT: {
                vec_size = this->parts_[this->unfinished_part_.first].size() - 1;
                usub::server::protocols::format::multipart::FormDataData &part = this->parts_[this->unfinished_part_.first][vec_size];
                if (line == current_boundary) {
                    part.done();
                    std::string &str = part.getContent();
                    str.resize(str.size() - 2);
                    this->unfinished_part_.first.clear();
                    this->state_ = FORM_DATA_PARSER_STATE::READING_HEADERS;
                } else if (line == current_boundary + "--") {
                    std::string &str = part.getContent();
                    str.resize(str.size() - 2);
                    this->unfinished_part_.first.clear();
                    this->state_ = FORM_DATA_PARSER_STATE::FINISHED;
                } else {
                    part.getContent() += line;
                    if (carriage_return) {
                        part.getContent() += '\r';
                    }
                    if (newline) {
                        part.getContent() += '\n';
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    return this->state_;
}

std::vector<usub::server::protocols::format::multipart::FormDataData> usub::server::protocols::format::multipart::FormData::getParts(std::string part_name) {
    return this->parts_.contains(part_name) ? this->parts_[part_name] : std::vector<usub::server::protocols::format::multipart::FormDataData>{};
}

std::string usub::server::protocols::format::multipart::FormData::string() const {
    if (this->boundary_.empty() ||
        this->state_ == FORM_DATA_PARSER_STATE::ERROR ||
        this->state_ != FORM_DATA_PARSER_STATE::FINISHED) {
        return "";
    }
    size_t estimated_size = this->size();
    std::string rv;
    rv.reserve(estimated_size);
    for (const auto &[key, value]: this->parts_) {
        for (const auto &part: value) {
            rv.append("--");
            rv.append(this->boundary_);
            rv.append("\r\n");
            rv.append(part.string());
        }
    }
    rv.append("--");
    rv.append(this->boundary_);
    rv.append("--\r\n");
    return rv;
}

size_t usub::server::protocols::format::multipart::FormData::size() const {
    if (this->boundary_.empty() ||
        this->state_ == FORM_DATA_PARSER_STATE::ERROR ||
        this->state_ != FORM_DATA_PARSER_STATE::FINISHED) {
        return 0;
    }
    size_t rv = 0;
    for (const auto &[key, value]: this->parts_) {
        for (const auto &part: value) {
            rv += 2 + this->boundary_.size() + 2;// "--" + boundary + "\r\n"
            rv += part.size();
        }
    }
    rv += 2 + this->boundary_.size() + 4;// "--" + boundary + "--" + "\r\n"
    return rv;
}

std::vector<usub::server::protocols::format::multipart::FormDataData> &usub::server::protocols::format::multipart::FormData::at(const std::string &part_name) {
    return this->parts_.at(part_name);
}

const std::vector<usub::server::protocols::format::multipart::FormDataData> &usub::server::protocols::format::multipart::FormData::at(const std::string &part_name) const {
    return this->parts_.at(part_name);
}

std::vector<usub::server::protocols::format::multipart::FormDataData> &usub::server::protocols::format::multipart::FormData::operator[](const std::string &part_name) {
    return this->parts_[part_name];
}
