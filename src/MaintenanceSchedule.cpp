#include "MaintenanceSchedule.h"

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
    if (currentMileage < lastServiceMileage) {
        throw std::invalid_argument("Service mileage precedes the last service.");
    }
    lastServiceMileage = currentMileage;
}
