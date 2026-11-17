#include "Truck.h"
#include "ServiceDate.h"

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

Truck::Truck(std::string vin,
    std::string make,
    std::string model,
    int year,
    int mileage,
    VehicleStatus status,
    std::string licensePlate,
    std::string onboardingDate)
    : Vehicle(std::move(vin),
        std::move(make),
        std::move(model),
        year,
        mileage,
        status)
{
    setLicensePlate(std::move(licensePlate));
    const std::string date = onboardingDate.empty()
        ? fleet_detail::todayUtc() : onboardingDate;
    // Initial service baselines start at registration, as in Sedan.
    // These are fleet defaults; use manufacturer schedules for actual vehicles.
    addMaintenanceSchedule(
        MaintenanceSchedule("Oil and Filter Change",
            oilAndFilterIntervalMiles, getMileage(), 180, date));

    addMaintenanceSchedule(
        MaintenanceSchedule("Tire Rotation",
            tireRotationIntervalMiles, getMileage(), 180, date));

    addMaintenanceSchedule(
        MaintenanceSchedule("Brake Inspection",
            brakeInspectionIntervalMiles, getMileage(), 365, date));

    addMaintenanceSchedule(
        MaintenanceSchedule("Tire Replacement",
            tireReplacementIntervalMiles, getMileage(), 1825, date));

    addMaintenanceSchedule(
        MaintenanceSchedule("Coolant Service",
            coolantServiceIntervalMiles, getMileage(), 730, date));

    addMaintenanceSchedule(
        MaintenanceSchedule("Engine Air Filter Replacement",
            airFilterIntervalMiles, getMileage(), 365, date));
}

std::string Truck::getVehicleType() const { return "Truck"; }
