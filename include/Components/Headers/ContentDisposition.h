#ifndef CONTENT_DISPOSITION_H
#define CONTENT_DISPOSITION_H

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <filesystem>


//local headers
#include <utils/utils.h>

namespace usub::server::component {
    class FileInfo {
    public:
        FileInfo(std::string charset, std::string language, std::filesystem::path filename);
        FileInfo(std::string charset, std::string language, std::string filename);
        FileInfo(const std::string &filename);
        FileInfo() = default;
        ~FileInfo() = default;


        [[maybe_unused]] uint8_t parse(const std::string &filename);

        bool empty() const { return this->filename_.empty(); };

        void clear();
        std::string string() const;
        size_t size() const;
        friend std::ostream &operator<<(std::ostream &os, const FileInfo &file_info);
        FileInfo &operator=(const FileInfo &other);
        bool operator==(const FileInfo &other) const;

    private:
        std::string charset_;
        std::string language_;
        std::filesystem::path filename_{};
    };

    namespace header {
    class ContentDisposition {
    private:
        // Member Variables
        std::string disposition_;
        std::string name_;
        FileInfo filename_;
        FileInfo filename_star_;
        std::unordered_map<std::string, std::string> params_;

        // Utility Functions
        bool is_number(const std::string &s) const;

        // Parses a single parameter key-value pair
        void parseParam(const std::string &token);

    public:
        // Constructors and Destructor
        ContentDisposition();
        explicit ContentDisposition(const std::string &content_disposition);
        ~ContentDisposition() = default;

        // Getter Methods
        const std::string &getDisposition() const;
        const std::string &getName() const;
        const FileInfo &getFilename() const;
        const FileInfo &getFilenameStar() const;
        const std::unordered_map<std::string, std::string> &getParams() const;

        // Check if the object is empty
        bool empty() const;

        void clear();

        // Access parameter by name
        const std::string &at(const std::string &param_name) const;
        std::string &at(const std::string &param_name);

        // Overloaded subscript operator
        std::string &operator[](const std::string &param_name);

        ContentDisposition &operator=(const ContentDisposition &other);
        bool operator==(const ContentDisposition &other) const;

        // Reconstruct the Content-Disposition header as a string
        std::string string() const;

        size_t size() const;

        // Parse method

        [[maybe_unused]] uint8_t parse(const std::string &content_disposition);
    };
    } // namespace header

}// namespace usub::server::component

#endif// CONTENT_DISPOSITION_H
