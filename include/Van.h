#ifndef VAN_H
#define VAN_H

#include "Vehicle.h"

#include <string>

class Van : public Vehicle
{
public:
    Van(std::string vin,
        std::string make,
        std::string model,
        int year,
        int mileage,
        VehicleStatus status = VehicleStatus::AVAILABLE,
        std::string licensePlate = "",
        std::string onboardingDate = "");

    std::string getVehicleType() const override;
};

#endif // VAN_H
