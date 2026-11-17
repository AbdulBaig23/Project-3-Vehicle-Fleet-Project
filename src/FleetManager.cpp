#include "FleetManager.h"
#include "ServiceDate.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

void FleetManager::addVehicle(std::unique_ptr<Vehicle> vehicle)
{
    if (!vehicle) {
        throw std::invalid_argument("Vehicle must not be null.");
    }

    const std::string vin = vehicle->getVin();
    if (vin.find_first_not_of(" \t\n\r\f\v") == std::string::npos) {
        throw std::invalid_argument("VIN must not be blank.");
    }

    if (!vehicles.emplace(vin, std::move(vehicle)).second) {
        throw std::invalid_argument("A vehicle with this VIN is already registered.");
    }
}

bool FleetManager::removeVehicle(const std::string& vin)
{
    return vehicles.erase(vin) != 0;
}

const Vehicle* FleetManager::findVehicle(const std::string& vin) const
{
    const auto entry = vehicles.find(vin);
    return entry == vehicles.end() ? nullptr : entry->second.get();
}

std::vector<const Vehicle*> FleetManager::listVehicles() const
{
    std::vector<const Vehicle*> result;
    result.reserve(vehicles.size());
    for (const auto& entry : vehicles) {
        result.push_back(entry.second.get());
    }

    std::sort(result.begin(), result.end(), [](const Vehicle* left, const Vehicle* right) {
        return left->getVin() < right->getVin();
    });
    return result;
}

std::vector<const Vehicle*> FleetManager::getAvailableVehicles() const
{
    auto result = listVehicles();
    result.erase(std::remove_if(result.begin(), result.end(), [](const Vehicle* vehicle) {
        return vehicle->getStatus() != VehicleStatus::AVAILABLE;
    }), result.end());
    return result;
}

std::vector<const Vehicle*> FleetManager::getVehiclesNeedingService() const
{
    auto result = listVehicles();
    result.erase(std::remove_if(result.begin(), result.end(), [](const Vehicle* vehicle) {
        return !vehicle->isServiceRequired();
    }), result.end());
    return result;
}

std::vector<const Vehicle*> FleetManager::getVehiclesNeedingService(
    const std::string& currentDate) const
{
    fleet_detail::serviceDay(currentDate);
    auto result = listVehicles();
    result.erase(std::remove_if(result.begin(), result.end(), [&currentDate](const Vehicle* vehicle) {
        return !vehicle->isServiceRequired(currentDate);
    }), result.end());
    return result;
}

void FleetManager::updateMileage(const std::string& vin, int newMileage)
{
    requireVehicle(vin).updateMileage(newMileage);
}

void FleetManager::setVehicleStatus(const std::string& vin, VehicleStatus status)
{
    requireVehicle(vin).setStatus(status);
}

void FleetManager::logServiceEvent(const std::string& vin, const MaintenanceRecord& record)
{
    Vehicle& vehicle = requireVehicle(vin);
    if (record.getServiceDate() > fleet_detail::todayUtc()) {
        throw std::invalid_argument("Completed service cannot have a future date.");
    }
    vehicle.logServiceEvent(record);
}

Vehicle& FleetManager::requireVehicle(const std::string& vin)
{
    const auto entry = vehicles.find(vin);
    if (entry == vehicles.end()) {
        throw std::out_of_range("No vehicle is registered with this VIN.");
    }
    return *entry->second;
}
