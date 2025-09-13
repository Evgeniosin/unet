#pragma once

#include <string>
#include <expected>
#include <vector>
#include <unordered_map>
#include "utils/string_utils.h"

struct Part {
    std::string content_type;
    std::unordered_map<std::string,std::string> disposition;
    std::string data;
    std::unordered_map<std::string, std::vector<std::string>, usub::utils::CaseInsensitiveHash, usub::utils::CaseInsensitiveEqual> headers;

};

class MultipartFormData {
public:


    explicit MultipartFormData(const std::string& input, std::string boundary){
        this->boundary_ = boundary;
        parse(input); 
    }

    explicit MultipartFormData(std::string boundary = "") : boundary_(boundary) {}

    std::expected<void, std::string> parse(std::string input);
  

    static void printParsedMultipart(const MultipartFormData& form);

    size_t                   count        (const std::string& name) const;
    bool                     contains     (const std::string& name) const;
    std::string&             getPart      (const std::string& name)      ;
    std::string              getPart      (const std::string& name) const;
    size_t                   size         ()                        const;
    std::vector<Part>&       at           (const std::string& name);
    const std::vector<Part>& at           (const std::string& name) const;
    void                     addPart      (const Part& part);
    void                     addPart      (Part&& part);
    void                     clear        ();

    auto                     begin()      { return parts_by_name_.begin(); }
    auto                     end()        { return parts_by_name_.end(); }
    auto                     begin()      const { return parts_by_name_.begin(); }
    auto                     end()        const { return parts_by_name_.end(); }
    auto                     cbegin()     const { return parts_by_name_.cbegin(); }
    auto                     cend()       const { return parts_by_name_.cend(); }
    void                     print()      const;
    std::string              string()     const;
    MultipartFormData&       setBoundary  (std::string);
    std::vector<Part>& operator[](std::string_view name);
    const std::vector<Part>& operator[](std::string_view name) const;


private:
    std::string boundary_;
    std::unordered_map <std::string, std::vector<Part>> parts_by_name_;
};