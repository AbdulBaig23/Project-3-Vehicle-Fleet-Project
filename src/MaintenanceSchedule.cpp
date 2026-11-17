#include "MaintenanceSchedule.h"
#include "ServiceDate.h"

#include <stdexcept>

MaintenanceSchedule::MaintenanceSchedule(const std::string& serviceName,
    int intervalMiles,
    int lastServiceMileage)
    : serviceName(serviceName), intervalMiles(intervalMiles),
    lastServiceMileage(lastServiceMileage)
{
    if (serviceName.find_first_not_of(" \t\n\r\f\v") == std::string::npos) {
        throw std::invalid_argument("Service name must not be blank.");
    }
    if (intervalMiles <= 0) {
        throw std::invalid_argument("Service interval must be positive.");
    }
    if (lastServiceMileage < 0) {
        throw std::invalid_argument("Last service mileage must not be negative.");
    }
}

const std::string& MaintenanceSchedule::getServiceName() const
{
    return serviceName;
}

int MaintenanceSchedule::getIntervalMiles() const
{
    return intervalMiles;
}

int MaintenanceSchedule::getLastServiceMileage() const
{
    return lastServiceMileage;
}

bool MaintenanceSchedule::isOverdue(int currentMileage) const
{
    if (currentMileage < lastServiceMileage) {
        throw std::invalid_argument("Current mileage precedes the last service.");
    }
    // Subtraction avoids overflow from adding interval to the last mileage.
    return currentMileage - lastServiceMileage >= intervalMiles;
}

void MaintenanceSchedule::recordService(int currentMileage)
{
    if (intervalDays > 0) {
        throw std::invalid_argument("A service date is required for this schedule.");
    }
    if (currentMileage < lastServiceMileage) {
        throw std::invalid_argument("Service mileage precedes the last service.");
    }
    lastServiceMileage = currentMileage;
}

MaintenanceSchedule::MaintenanceSchedule(const std::string& name, int miles,
    int mileage, int days,
    const std::string& date)
    : MaintenanceSchedule(name, miles, mileage)
{
    if (days <= 0) throw std::invalid_argument("Day interval must be positive.");
    fleet_detail::serviceDay(date);
    intervalDays = days;
    lastServiceDate = date;
}

int MaintenanceSchedule::getIntervalDays() const { return intervalDays; }
const std::string& MaintenanceSchedule::getLastServiceDate() const { return lastServiceDate; }

bool MaintenanceSchedule::isOverdue(int mileage, const std::string& date) const
{
    const int currentDay = fleet_detail::serviceDay(date);
    if (intervalDays == 0) return isOverdue(mileage);
    const int elapsed = currentDay - fleet_detail::serviceDay(lastServiceDate);
    if (elapsed < 0) throw std::invalid_argument("Current date precedes the last service.");
    const bool mileageDue = isOverdue(mileage);
    return mileageDue || elapsed >= intervalDays;
}

void MaintenanceSchedule::recordService(int mileage, const std::string& date)
{
    const int day = fleet_detail::serviceDay(date);
    if (mileage < lastServiceMileage)
        throw std::invalid_argument("Service mileage precedes the last service.");
    if (intervalDays > 0 && day < fleet_detail::serviceDay(lastServiceDate))
        throw std::invalid_argument("Service date precedes the last service.");
    // Allocate before updating mileage so a failed update leaves the schedule intact
    std::string newDate(date);
    lastServiceDate.swap(newDate);
    lastServiceMileage = mileage;
}
