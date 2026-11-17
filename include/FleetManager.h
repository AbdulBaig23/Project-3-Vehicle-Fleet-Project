#ifndef FLEET_MANAGER_H
#define FLEET_MANAGER_H

#include "Vehicle.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class FleetManager
{
public:
    // Takes ownership of the vehicle. Throws std::invalid_argument for a null
    // vehicle, a blank VIN, or a duplicate VIN. VINs are matched as entered.
    void addVehicle(std::unique_ptr<Vehicle> vehicle);

    // Removes the vehicle and its in-memory maintenance history.
    // Returns false if the VIN is not registered.
    bool removeVehicle(const std::string& vin);

    // Returns nullptr for an unknown VIN. Returned pointers borrow from the
    // fleet and are valid until that vehicle is removed or the fleet is destroyed.
    const Vehicle* findVehicle(const std::string& vin) const;

    // All vehicle lists are sorted by VIN.
    std::vector<const Vehicle*> listVehicles() const;
    // Availability follows the vehicle's operational status.
    std::vector<const Vehicle*> getAvailableVehicles() const;

    // Checks mileage and date thresholds. The no-argument version uses today's UTC date.
    std::vector<const Vehicle*> getVehiclesNeedingService() const;
    std::vector<const Vehicle*> getVehiclesNeedingService(const std::string& currentDate) const;

    // These operations throw std::out_of_range if the VIN is not registered.
    void updateMileage(const std::string& vin, int newMileage);
    void setVehicleStatus(const std::string& vin, VehicleStatus status);

    // Records completed service. Throws std::out_of_range for an unknown VIN
    // or std::invalid_argument for a date after today (UTC).
    void logServiceEvent(const std::string& vin, const MaintenanceRecord& record);

private:
    Vehicle& requireVehicle(const std::string& vin);

    std::unordered_map<std::string, std::unique_ptr<Vehicle>> vehicles;
};

#endif // FLEET_MANAGER_H
