#ifndef MULTIPART_FORM_DATA_H
#define MULTIPART_FORM_DATA_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>


// local includes
#include "Components/Headers/ContentDisposition.h"
#include "Components/Headers/ContentType.h"
#include "utils/utils.h"


namespace usub::server::protocols::format::multipart {

    enum FORM_DATA_PARSER_STATE : uint8_t {
        FINISHED = 0,
        WAITING_FOR_BOUNDARY,
        READING_HEADERS,
        READING_CONTENT,
        ERROR
    };

    class FormDataData {
    private:
        usub::server::component::header::ContentDisposition content_disposition_{};
        usub::server::component::header::ContentType content_type_{};
        std::string content_{};
        bool done_{false};
        std::unordered_map<std::string, std::string> form_data_headers_{};

    public:
        FormDataData() = default;
        ~FormDataData() = default;

        void parseHeader(const std::string &line);
        usub::server::component::header::ContentDisposition &getContentDisposition() {
            return this->content_disposition_;
        };
        usub::server::component::header::ContentType &getContentType() { return this->content_type_; };
        std::string &getContent() { return this->content_; };

        void done();

        // Imitate std behaviour
        void clear();
        std::string string() const;
        size_t size() const;
        bool operator==(const FormDataData &other) const;
    };

    class FormData {
    private:
        std::string boundary_{};
        std::string unfinished_string_{};
        std::pair<std::string, FormDataData> unfinished_part_{};
        uint8_t state_{FORM_DATA_PARSER_STATE::WAITING_FOR_BOUNDARY};
        std::unordered_map<std::string, std::vector<FormDataData>> parts_{};

    public:
        [[maybe_unused]] uint8_t parse(const std::string &form_data,
                                       const std::string &boundary);
        void parseHeaders(const std::string &line);
        std::vector<FormDataData> getParts(std::string part_name);

        // Imitate std behaviour
        std::string string() const;
        size_t size() const;
        std::vector<FormDataData> &at(const std::string &part_name);
        const std::vector<FormDataData> &at(const std::string &part_name) const;
        std::vector<FormDataData> &operator[](const std::string &part_name);
    };

}// namespace usub::server::protocols::format::multipart

#endif// MULTIPART_FORM_DATA_H
