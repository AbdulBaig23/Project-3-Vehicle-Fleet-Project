#include "Vehicle.h"

#include <stdexcept>
#include <utility>

Vehicle::Vehicle(std::string vin,
                 std::string make,
                 std::string model,
                 int year,
                 int mileage,
                 VehicleStatus status)
    : vin(std::move(vin)), make(std::move(make)), model(std::move(model)),
      year(year), mileage(mileage), status(status)
{
    if (mileage < 0) {
        throw std::invalid_argument("Initial mileage must not be negative.");
    }
}

const std::string& Vehicle::getVin() const { return vin; }
const std::string& Vehicle::getMake() const { return make; }
const std::string& Vehicle::getModel() const { return model; }
int Vehicle::getYear() const { return year; }
int Vehicle::getMileage() const { return mileage; }
VehicleStatus Vehicle::getStatus() const { return status; }

void Vehicle::updateMileage(int newMileage)
{
    if (newMileage < mileage) {
        throw std::invalid_argument("New mileage must be at least the current mileage.");
    }
    mileage = newMileage;
}

void Vehicle::setStatus(VehicleStatus newStatus) { status = newStatus; }

void Vehicle::addMaintenanceSchedule(const MaintenanceSchedule& schedule)
{
    if (schedule.getLastServiceMileage() > mileage) { // corner case for if the last service mileage is greater than the current mileage of the vehicle
        throw std::invalid_argument(
            "Last service mileage cannot be greater than vehicle mileage.");
    }

    for (const MaintenanceSchedule& existing : maintenanceSchedules) { // checks if the service name already exists in the maintenance schedule
        if (existing.getServiceName() == schedule.getServiceName()) {
            throw std::invalid_argument(
                "A schedule for this service already exists.");
        }
    }

    maintenanceSchedules.push_back(schedule);
}

const std::vector<MaintenanceSchedule>& Vehicle::getMaintenanceSchedules() const
{
    return maintenanceSchedules;
}

const std::vector<MaintenanceRecord>& Vehicle::getMaintenanceHistory() const
{
    return maintenanceHistory;
}

void Vehicle::logServiceEvent(const MaintenanceRecord& record)
{
    const int serviceMileage = record.getMileageAtService();

    if (serviceMileage > mileage) {
        throw std::invalid_argument(
            "Service mileage cannot be greater than current vehicle mileage.");
    }

    MaintenanceSchedule* matchingSchedule = nullptr;

    for (MaintenanceSchedule& schedule : maintenanceSchedules) {
        if (schedule.getServiceName() == record.getServiceType()) {
            if (serviceMileage < schedule.getLastServiceMileage()) {
                throw std::invalid_argument(
                    "Service mileage precedes the last recorded service.");
            }
            matchingSchedule = &schedule;
            break;
        }
    }

    maintenanceHistory.push_back(record);

    if (matchingSchedule != nullptr) {
        matchingSchedule->recordService(serviceMileage);
    }
}

bool Vehicle::isServiceRequired() const
{
    for (const MaintenanceSchedule& schedule : maintenanceSchedules) {
        if (schedule.isOverdue(mileage)) {
            return true;
        }
    }

    return false;
}
