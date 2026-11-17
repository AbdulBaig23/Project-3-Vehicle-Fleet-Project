#ifndef SERVICE_DATE_H
#define SERVICE_DATE_H

#include <ctime>
#include <stdexcept>
#include <string>

namespace fleet_detail {

    // Gregorian calendar days, independent of local time zones and daylight saving
    inline int serviceDay(const std::string& date)
    {
        if (date.size() != 10 || date[4] != '-' || date[7] != '-')
            throw std::invalid_argument("Expected a valid YYYY-MM-DD date.");
        int parts[] = { 0, 0, 0 };
        int part = 0;
        for (std::size_t i = 0; i < date.size(); ++i) {
            if (i == 4 || i == 7) { ++part; continue; }
            if (date[i] < '0' || date[i] > '9')
                throw std::invalid_argument("Expected a valid YYYY-MM-DD date.");
            parts[part] = parts[part] * 10 + date[i] - '0';
        }
        int year = parts[0];
        const int month = parts[1], day = parts[2];
        const bool leap = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
        const int lengths[] = { 31, leap ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (year < 1 || month < 1 || month > 12 || day < 1 || day > lengths[month - 1])
            throw std::invalid_argument("Expected a valid YYYY-MM-DD date.");
        --year;
        int result = 365 * year + year / 4 - year / 100 + year / 400 + day;
        for (int i = 0; i < month - 1; ++i) result += lengths[i];
        return result;
    }

    inline std::string todayUtc()
    {
        const std::time_t now = std::time(nullptr);
        const std::tm* utc = std::gmtime(&now);
        char date[11];
        if (!utc || std::strftime(date, sizeof(date), "%Y-%m-%d", utc) == 0)
            throw std::runtime_error("Cannot determine the current UTC date.");
        return date;
    }

} // namespace fleet_detail
#endif
