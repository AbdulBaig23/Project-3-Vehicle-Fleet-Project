#include "MaintenanceRecord.h"

#include <cstddef>
#include <stdexcept>

namespace {

int parseDatePart(const std::string& date, std::size_t start, std::size_t length)
{
    int value = 0;
    for (std::size_t i = start; i < start + length; ++i) {
        const char digit = date[i];
        if (digit < '0' || digit > '9') {
            return -1;
        }
        value = value * 10 + (digit - '0');
    }
    return value;
}

bool isValidServiceDate(const std::string& date)
{
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    const int year = parseDatePart(date, 0, 4);
    const int month = parseDatePart(date, 5, 2);
    const int day = parseDatePart(date, 8, 2);
    if (year < 1 || month < 1 || month > 12 || day < 1) {
        return false;
    }

    const bool leapYear = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    const int daysInMonth[] = {31, leapYear ? 29 : 28, 31, 30, 31, 30,
                               31, 31, 30, 31, 30, 31};
    return day <= daysInMonth[month - 1];
}

} // namespace

MaintenanceRecord::MaintenanceRecord(const std::string& recordId,
                                     const std::string& serviceType,
                                     const std::string& serviceDate,
                                     int mileageAtService,
                                     std::int64_t costCents)
    : recordId(recordId), serviceType(serviceType), serviceDate(serviceDate),
      mileageAtService(mileageAtService), costCents(costCents)
{
    const char* whitespace = " \t\n\r\f\v";
    if (recordId.find_first_not_of(whitespace) == std::string::npos) {
        throw std::invalid_argument("Record ID must not be blank.");
    }
    if (serviceType.find_first_not_of(whitespace) == std::string::npos) {
        throw std::invalid_argument("Service type must not be blank.");
    }
    if (!isValidServiceDate(serviceDate)) {
        throw std::invalid_argument("Service date must be a valid date in YYYY-MM-DD format.");
    }
    if (mileageAtService < 0) {
        throw std::invalid_argument("Mileage at service must not be negative.");
    }
    if (costCents < 0) {
        throw std::invalid_argument("Service cost in cents must not be negative.");
    }
}

const std::string& MaintenanceRecord::getRecordId() const
{
    return recordId;
}

const std::string& MaintenanceRecord::getServiceType() const
{
    return serviceType;
}

const std::string& MaintenanceRecord::getServiceDate() const
{
    return serviceDate;
}

int MaintenanceRecord::getMileageAtService() const
{
    return mileageAtService;
}

std::int64_t MaintenanceRecord::getCostCents() const
{
    return costCents;
}
