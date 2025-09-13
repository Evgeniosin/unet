#include "Components/Headers/HTTPDate.h"

usub::server::component::HTTPDate::HTTPDate() {};

usub::server::component::HTTPDate::HTTPDate(const HTTPDate &other) {
    this->format = other.format;
    this->date_time = other.date_time;
};
usub::server::component::HTTPDate::HTTPDate(usub::server::component::HTTPDate &&other) noexcept {
    this->format = other.format;
    this->date_time = std::move(other.date_time);
};
usub::server::component::HTTPDate::HTTPDate(const std::string &date) {
    parse(date);
};
usub::server::component::HTTPDate::HTTPDate(const std::time_t &time) {
    this->date_time = *std::gmtime(&time);
};
usub::server::component::HTTPDate::HTTPDate(const std::tm &time) {
    this->date_time = time;
};
usub::server::component::HTTPDate::HTTPDate(const std::tm &&time) {
    this->date_time = std::move(time);
};

usub::server::component::HTTPDate &usub::server::component::HTTPDate::operator=(const usub::server::component::HTTPDate &other) {
    this->format = other.format;
    this->date_time = other.date_time;
    return *this;
}
usub::server::component::HTTPDate &usub::server::component::HTTPDate::operator=(usub::server::component::HTTPDate &&other) noexcept {
    this->format = other.format;
    this->date_time = std::move(other.date_time);
    return *this;
}
usub::server::component::HTTPDate &usub::server::component::HTTPDate::operator=(const std::time_t &time) {
    this->date_time = *std::gmtime(&time);
    return *this;
}
usub::server::component::HTTPDate &usub::server::component::HTTPDate::operator=(const std::string &date) {
    parse(date);
    return *this;
}
usub::server::component::HTTPDate &usub::server::component::HTTPDate::operator=(const std::string_view &date) {
    parse(date);
    return *this;
}
usub::server::component::HTTPDate &usub::server::component::HTTPDate::operator=(const std::tm &time) {
    this->date_time = time;
    return *this;
}
usub::server::component::HTTPDate &usub::server::component::HTTPDate::operator=(const std::tm &&time) {
    this->date_time = std::move(time);
    return *this;
}

bool usub::server::component::HTTPDate::operator==(const usub::server::component::HTTPDate &other) const {
    return this->date_time.tm_year == other.date_time.tm_year &&
           this->date_time.tm_mon == other.date_time.tm_mon &&
           this->date_time.tm_mday == other.date_time.tm_mday &&
           this->date_time.tm_hour == other.date_time.tm_hour &&
           this->date_time.tm_min == other.date_time.tm_min &&
           this->date_time.tm_sec == other.date_time.tm_sec;
}
bool usub::server::component::HTTPDate::operator!=(const usub::server::component::HTTPDate &other) const {
    return this->date_time.tm_year != other.date_time.tm_year &&
           this->date_time.tm_mon != other.date_time.tm_mon &&
           this->date_time.tm_mday != other.date_time.tm_mday &&
           this->date_time.tm_hour != other.date_time.tm_hour &&
           this->date_time.tm_min != other.date_time.tm_min &&
           this->date_time.tm_sec != other.date_time.tm_sec;
}
bool usub::server::component::HTTPDate::operator<(const usub::server::component::HTTPDate &other) const {
    return this->date_time.tm_year < other.date_time.tm_year &&
           this->date_time.tm_mon < other.date_time.tm_mon &&
           this->date_time.tm_mday < other.date_time.tm_mday &&
           this->date_time.tm_hour < other.date_time.tm_hour &&
           this->date_time.tm_min < other.date_time.tm_min &&
           this->date_time.tm_sec < other.date_time.tm_sec;
}
bool usub::server::component::HTTPDate::operator>(const usub::server::component::HTTPDate &other) const {
    return this->date_time.tm_year > other.date_time.tm_year &&
           this->date_time.tm_mon > other.date_time.tm_mon &&
           this->date_time.tm_mday > other.date_time.tm_mday &&
           this->date_time.tm_hour > other.date_time.tm_hour &&
           this->date_time.tm_min > other.date_time.tm_min &&
           this->date_time.tm_sec > other.date_time.tm_sec;
}
bool usub::server::component::HTTPDate::operator<=(const usub::server::component::HTTPDate &other) const {
    return this->date_time.tm_year <= other.date_time.tm_year &&
           this->date_time.tm_mon <= other.date_time.tm_mon &&
           this->date_time.tm_mday <= other.date_time.tm_mday &&
           this->date_time.tm_hour <= other.date_time.tm_hour &&
           this->date_time.tm_min <= other.date_time.tm_min &&
           this->date_time.tm_sec <= other.date_time.tm_sec;
}
bool usub::server::component::HTTPDate::operator>=(const usub::server::component::HTTPDate &other) const {
    return this->date_time.tm_year >= other.date_time.tm_year &&
           this->date_time.tm_mon >= other.date_time.tm_mon &&
           this->date_time.tm_mday >= other.date_time.tm_mday &&
           this->date_time.tm_hour >= other.date_time.tm_hour &&
           this->date_time.tm_min >= other.date_time.tm_min &&
           this->date_time.tm_sec >= other.date_time.tm_sec;
}

bool usub::server::component::HTTPDate::parse(std::string_view date) {
    // Sun, 06 Nov 1994 08:49:37 GMT
    if (date.length() == 29 && date[3] == ',' && date[4] == ' ' &&
        date[7] == ' ' && date[11] == ' ' && date[16] == ' ' &&
        date[19] == ':' && date[22] == ':' && date[25] == ' ') [[likely]] {
        this->format = RFC5322;
        if (strptime(date.data(), "%a, %d %b %Y %H:%M:%S %Z", &this->date_time) == nullptr) [[unlikely]] {// TODO: make our own fast parser
            this->format = UNKNOWN;
            return false;
        }
    }
    // Sun Nov  6 08:49:37 1994
    else if (date.length() == 24 && date[3] == ' ' && date[7] == ' ' && date[10] == ' ' &&
             date[13] == ':' && date[16] == ':' && date[19] == ' ') [[unlikely]] {
        this->format = ASCTIME;
        if (strptime(date.data(), "%a %b %d %H:%M:%S %Y", &this->date_time) == nullptr) [[unlikely]] {// TODO: make our own fast parser
            this->format = UNKNOWN;
            return false;
        }
    }
    // Sunday, 06-Nov-94 08:49:37 GMT
    else if (date[date.size() - 4] == ' ' && date[date.size() - 7] == ':' &&
             date[date.size() - 10] == ':' && date[date.size() - 13] == ' ' &&
             date[date.size() - 16] == '-' && date[date.size() - 20] == '-' &&
             date[date.size() - 23] == ' ' && date[date.size() - 24] == ',') {
        this->format = RFC850;
        if (strptime(date.data(), "%A, %d-%b-%y %H:%M:%S %Z", &this->date_time) == nullptr) [[unlikely]] {// TODO: make our own fast parser
            this->format = UNKNOWN;
            return false;
        }
    } else [[unlikely]] {
        this->format = UNKNOWN;
        return false;
    }
    return true;
}

bool usub::server::component::HTTPDate::empty() const {
    return this->date_time.tm_year == 0;
}

usub::server::component::HTTPDate &usub::server::component::HTTPDate::set_format(FORMAT format) {
    this->format = format;
    return *this;
}

const std::tm &usub::server::component::HTTPDate::time() const {
    return this->date_time;
}
std::tm &usub::server::component::HTTPDate::time() {
    return this->date_time;
}

const std::string usub::server::component::HTTPDate::string() const {
    std::string date(30, '\0');
    switch (this->format) {
        case RFC5322:
            strftime(date.data(), date.size(), "%a, %d %b %Y %H:%M:%S %Z", &this->date_time);
            break;
        case ASCTIME:
            strftime(date.data(), date.size(), "%a %b %d %H:%M:%S %Y", &this->date_time);
            break;
        case RFC850:
            strftime(date.data(), date.size(), "%A, %d-%b-%y %H:%M:%S %Z", &this->date_time);
            break;
        case UNKNOWN:
            date = "";
            break;
    }
    return date;
}