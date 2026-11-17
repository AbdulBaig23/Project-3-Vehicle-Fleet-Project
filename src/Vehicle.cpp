#include "Vehicle.h"
#include "ServiceDate.h"

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

const std::string& Vehicle::getLicensePlate() const { return licensePlate; }
void Vehicle::setLicensePlate(std::string plate) { licensePlate = std::move(plate); }
std::string Vehicle::getVehicleType() const { return "Vehicle"; }
void Vehicle::setMake(std::string value) { make = std::move(value); }
void Vehicle::setModel(std::string value) { model = std::move(value); }
void Vehicle::setYear(int value) { year = value; }

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

    if (matchingSchedule != nullptr) {
        // Validate and allocate before either the schedule or history changes
        MaintenanceSchedule updated = *matchingSchedule;
        updated.recordService(serviceMileage, record.getServiceDate());
        maintenanceHistory.push_back(record);
        std::swap(*matchingSchedule, updated);
    }
    else {
        maintenanceHistory.push_back(record);
    }
}

bool Vehicle::isServiceRequired() const
{
    return isServiceRequired(fleet_detail::todayUtc());
}

bool Vehicle::isServiceRequired(const std::string& currentDate) const
{
    fleet_detail::serviceDay(currentDate);
    bool due = false;
    for (const MaintenanceSchedule& schedule : maintenanceSchedules) {
        if (schedule.isOverdue(mileage, currentDate)) {
            due = true;
        }
    }

    return due;
}
