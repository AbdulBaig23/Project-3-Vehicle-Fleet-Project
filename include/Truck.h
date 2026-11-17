#ifndef TRUCK_H
#define TRUCK_H

#include "Vehicle.h"

#include <string>

class Truck : public Vehicle
{
public:
    Truck(std::string vin,
        std::string make,
        std::string model,
        int year,
        int mileage,
        VehicleStatus status = VehicleStatus::AVAILABLE,
        std::string licensePlate = "",
        std::string onboardingDate = "");

    std::string getVehicleType() const override;
};

#endif // TRUCK_H
