#include "Sedan.h"

#include <utility>

namespace
{
    constexpr int oilAndFilterIntervalMiles = 5000;
    constexpr int tireRotationIntervalMiles = 7500;
    constexpr int brakeInspectionIntervalMiles = 10000;
    constexpr int tireReplacementIntervalMiles = 50000;
    constexpr int coolantServiceIntervalMiles = 50000;
    constexpr int airFilterIntervalMiles = 15000;
}

Sedan::Sedan(std::string vin,
             std::string make,
             std::string model,
             int year,
             int mileage,
             VehicleStatus status)
    : Vehicle(std::move(vin),
              std::move(make),
              std::move(model),
              year,
              mileage,
              status)
{
    addMaintenanceSchedule(
        MaintenanceSchedule("Oil and Filter Change",
                            oilAndFilterIntervalMiles, getMileage()));

    addMaintenanceSchedule(
        MaintenanceSchedule("Tire Rotation",
                            tireRotationIntervalMiles, getMileage()));

    addMaintenanceSchedule(
        MaintenanceSchedule("Brake Inspection",
                            brakeInspectionIntervalMiles, getMileage()));

    addMaintenanceSchedule(
        MaintenanceSchedule("Tire Replacement",
                            tireReplacementIntervalMiles, getMileage()));

    addMaintenanceSchedule(
        MaintenanceSchedule("Coolant Service",
                            coolantServiceIntervalMiles, getMileage()));

    addMaintenanceSchedule(
        MaintenanceSchedule("Engine Air Filter Replacement",
                            airFilterIntervalMiles, getMileage()));
}