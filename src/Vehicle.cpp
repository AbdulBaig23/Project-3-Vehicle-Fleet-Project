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
