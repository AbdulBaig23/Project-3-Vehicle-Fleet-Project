#ifndef SEDAN_H
#define SEDAN_H

#include "Vehicle.h"

#include <string>

class Sedan : public Vehicle
{
public:
    Sedan(std::string vin,
          std::string make,
          std::string model,
          int year,
          int mileage,
          VehicleStatus status = VehicleStatus::AVAILABLE);
};

#endif // SEDAN_H