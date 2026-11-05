// THIS IS A PLACEHOLDER FOR TESTS, DON'T USE THIS ONCE THE ACTUAL CPP FILES ARE MADE

#include "Vehicle.h"

Vehicle::Vehicle(std::string vin, std::string make, std::string model, int year, int mileage, VehicleStatus status)
    : vin(std::move(vin)), make(std::move(make)), model(std::move(model)), year(year), mileage(mileage), status(status) {}

const std::string& Vehicle::getVin() const { return vin; }
const std::string& Vehicle::getMake() const { return make; }
const std::string& Vehicle::getModel() const { return model; }
int Vehicle::getYear() const { return year; }
int Vehicle::getMileage() const { return mileage; }
VehicleStatus Vehicle::getStatus() const { return status; }

void Vehicle::updateMileage(int newMileage) { mileage = newMileage; }
void Vehicle::setStatus(VehicleStatus newStatus) { status = newStatus; }