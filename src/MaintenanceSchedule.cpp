// THIS IS A PLACEHOLDER FOR TESTS, DON'T USE THIS ONCE THE ACTUAL CPP FILES ARE MADE

#include "MaintenanceSchedule.h"

MaintenanceSchedule::MaintenanceSchedule(const std::string& serviceName, int intervalMiles, int lastServiceMileage)
    : serviceName(serviceName), intervalMiles(intervalMiles), lastServiceMileage(lastServiceMileage) {}

const std::string& MaintenanceSchedule::getServiceName() const { return serviceName; }
int MaintenanceSchedule::getIntervalMiles() const { return intervalMiles; }
int MaintenanceSchedule::getLastServiceMileage() const { return lastServiceMileage; }

bool MaintenanceSchedule::isOverdue(int currentMileage) const {
    return (currentMileage - lastServiceMileage) >= intervalMiles;
}

void MaintenanceSchedule::recordService(int currentMileage) {
    lastServiceMileage = currentMileage;
}