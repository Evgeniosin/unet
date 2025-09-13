#ifndef USUB_HTTPDATE_H
#define USUB_HTTPDATE_H

#include <ctime>
#include <string>

namespace usub::server::component {
    //https://httpwg.org/specs/rfc9110.html#rfc.section.5.6.7
    class HTTPDate {
    public:
        enum FORMAT {
            RFC5322,
            RFC850,
            ASCTIME,
            UNKNOWN
        };

        FORMAT format{RFC5322};

        std::tm date_time{};

    public:
        HTTPDate();
        ~HTTPDate() = default;

        HTTPDate(const HTTPDate &other);
        HTTPDate(HTTPDate &&other) noexcept;
        explicit HTTPDate(const std::string &date);
        explicit HTTPDate(const std::time_t &time);
        explicit HTTPDate(const std::tm &&time);
        explicit HTTPDate(const std::tm &time);

        HTTPDate &operator=(const HTTPDate &other);
        HTTPDate &operator=(HTTPDate &&other) noexcept;
        HTTPDate &operator=(const std::time_t &time);
        HTTPDate &operator=(const std::string &date);
        HTTPDate &operator=(const std::string_view &date);
        HTTPDate &operator=(const std::tm &time);
        HTTPDate &operator=(const std::tm &&time);

        bool operator==(const HTTPDate &other) const;
        bool operator!=(const HTTPDate &other) const;
        bool operator<(const HTTPDate &other) const;
        bool operator>(const HTTPDate &other) const;
        bool operator<=(const HTTPDate &other) const;
        bool operator>=(const HTTPDate &other) const;

        bool operator==(const std::tm &other) const;
        bool operator!=(const std::tm &other) const;
        bool operator<(const std::tm &other) const;
        bool operator>(const std::tm &other) const;
        bool operator<=(const std::tm &other) const;
        bool operator>=(const std::tm &other) const;

        bool operator==(const std::time_t &other) const;
        bool operator!=(const std::time_t &other) const;
        bool operator<(const std::time_t &other) const;
        bool operator>(const std::time_t &other) const;
        bool operator<=(const std::time_t &other) const;
        bool operator>=(const std::time_t &other) const;

        bool operator==(std::string_view other) const;
        bool operator!=(std::string_view other) const;
        bool operator<(std::string_view other) const;
        bool operator>(std::string_view other) const;
        bool operator<=(std::string_view other) const;
        bool operator>=(std::string_view other) const;

        HTTPDate &from_time(const std::time_t &time);
        HTTPDate &from_string(const std::string &date);

        bool parse(std::string_view date);
        bool empty() const;

        HTTPDate &set_format(FORMAT format);

        const std::tm &time() const;
        std::tm &time();

        const std::string string() const;
    };
}// namespace usub::server::component

#endif//USUB_HTTPDATE_H