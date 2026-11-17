#include <gtest/gtest.h>

#include "FleetManager.h"
#include "MaintenanceRecord.h"
#include "Sedan.h"
#include "Truck.h"
#include "Van.h"

#include <memory>
#include <stdexcept>

TEST(FleetManagerTest, RegistersAndFindsDifferentVehicleTypesByExactVin)
{
    FleetManager fleet;
    fleet.addVehicle(std::make_unique<Sedan>("VIN-SEDAN", "Toyota", "Camry", 2021, 10000));
    fleet.addVehicle(std::make_unique<Truck>("VIN-TRUCK", "Ford", "F-150", 2022, 20000));

    EXPECT_NE(dynamic_cast<const Sedan*>(fleet.findVehicle("VIN-SEDAN")), nullptr);
    EXPECT_NE(dynamic_cast<const Truck*>(fleet.findVehicle("VIN-TRUCK")), nullptr);
    EXPECT_EQ(fleet.findVehicle("vin-truck"), nullptr);
}

TEST(FleetManagerTest, RejectsNullAndBlankVinWithoutRegisteringVehicles)
{
    FleetManager fleet;

    EXPECT_THROW(fleet.addVehicle(nullptr), std::invalid_argument);
    EXPECT_THROW(
        fleet.addVehicle(std::make_unique<Sedan>("", "Toyota", "Camry", 2021, 10000)),
        std::invalid_argument);
    EXPECT_THROW(
        fleet.addVehicle(std::make_unique<Sedan>(" \t\r\n", "Toyota", "Camry", 2021, 10000)),
        std::invalid_argument);
    EXPECT_TRUE(fleet.listVehicles().empty());
}

TEST(FleetManagerTest, DuplicateVinLeavesTheOriginalVehicleRegistered)
{
    FleetManager fleet;
    fleet.addVehicle(std::make_unique<Sedan>("VIN-001", "Toyota", "Camry", 2021, 10000));
    const Vehicle* original = fleet.findVehicle("VIN-001");
    ASSERT_NE(original, nullptr);

    EXPECT_THROW(
        fleet.addVehicle(std::make_unique<Truck>("VIN-001", "Ford", "F-150", 2022, 20000)),
        std::invalid_argument);

    ASSERT_EQ(fleet.findVehicle("VIN-001"), original);
    EXPECT_EQ(original->getMileage(), 10000);
    EXPECT_EQ(fleet.listVehicles().size(), 1u);
}

TEST(FleetManagerTest, ListsAllVehiclesAndFiltersAvailableVehiclesInVinOrder)
{
    FleetManager fleet;
    fleet.addVehicle(std::make_unique<Sedan>("VIN-D", "Honda", "Civic", 2020, 10000));
    fleet.addVehicle(std::make_unique<Sedan>("VIN-C", "Honda", "Civic", 2020, 10000));
    fleet.addVehicle(std::make_unique<Sedan>("VIN-B", "Honda", "Civic", 2020, 10000));
    fleet.addVehicle(std::make_unique<Sedan>("VIN-A", "Honda", "Civic", 2020, 10000));
    fleet.setVehicleStatus("VIN-C", VehicleStatus::IN_SERVICE);
    fleet.setVehicleStatus("VIN-B", VehicleStatus::OUT_OF_SERVICE);

    const auto vehicles = fleet.listVehicles();
    ASSERT_EQ(vehicles.size(), 4u);
    EXPECT_EQ(vehicles[0]->getVin(), "VIN-A");
    EXPECT_EQ(vehicles[1]->getVin(), "VIN-B");
    EXPECT_EQ(vehicles[2]->getVin(), "VIN-C");
    EXPECT_EQ(vehicles[3]->getVin(), "VIN-D");

    const auto available = fleet.getAvailableVehicles();
    ASSERT_EQ(available.size(), 2u);
    EXPECT_EQ(available[0]->getVin(), "VIN-A");
    EXPECT_EQ(available[1]->getVin(), "VIN-D");
}

TEST(FleetManagerTest, RemovesOnlyTheRequestedVehicle)
{
    FleetManager fleet;
    fleet.addVehicle(std::make_unique<Sedan>("VIN-A", "Toyota", "Camry", 2021, 10000));
    fleet.addVehicle(std::make_unique<Sedan>("VIN-B", "Honda", "Civic", 2022, 20000));

    EXPECT_FALSE(fleet.removeVehicle("VIN-MISSING"));
    EXPECT_TRUE(fleet.removeVehicle("VIN-A"));
    EXPECT_FALSE(fleet.removeVehicle("VIN-A"));
    EXPECT_EQ(fleet.findVehicle("VIN-A"), nullptr);
    ASSERT_EQ(fleet.listVehicles().size(), 1u);
    EXPECT_EQ(fleet.listVehicles()[0]->getVin(), "VIN-B");
}

TEST(FleetManagerTest, RejectsUpdatesForAnUnknownVin)
{
    FleetManager fleet;
    const MaintenanceRecord record("MR-001", "Oil and Filter Change", "2025-06-30", 15000, 5000);

    EXPECT_THROW(fleet.updateMileage("VIN-MISSING", 15000), std::out_of_range);
    EXPECT_THROW(fleet.setVehicleStatus("VIN-MISSING", VehicleStatus::IN_SERVICE), std::out_of_range);
    EXPECT_THROW(fleet.logServiceEvent("VIN-MISSING", record), std::out_of_range);
}

TEST(FleetManagerTest, PreservesMileageWhenAnOdometerRollbackIsRejected)
{
    FleetManager fleet;
    fleet.addVehicle(std::make_unique<Sedan>("VIN-001", "Toyota", "Camry", 2021, 10000));
    fleet.updateMileage("VIN-001", 15000);

    EXPECT_THROW(fleet.updateMileage("VIN-001", 14999), std::invalid_argument);

    const Vehicle* vehicle = fleet.findVehicle("VIN-001");
    ASSERT_NE(vehicle, nullptr);
    EXPECT_EQ(vehicle->getMileage(), 15000);
}

TEST(FleetManagerTest, FindsVehiclesDueAtTheMileageThresholdInVinOrder)
{
    FleetManager fleet;
    fleet.addVehicle(std::make_unique<Sedan>("VIN-C", "Toyota", "Camry", 2021, 10000));
    fleet.addVehicle(std::make_unique<Sedan>("VIN-B", "Honda", "Civic", 2022, 10000));
    fleet.addVehicle(std::make_unique<Sedan>("VIN-A", "Toyota", "Camry", 2021, 10000));
    fleet.updateMileage("VIN-A", 15000);
    fleet.updateMileage("VIN-B", 14999);
    fleet.updateMileage("VIN-C", 15001);

    const auto due = fleet.getVehiclesNeedingService();
    ASSERT_EQ(due.size(), 2u);
    EXPECT_EQ(due[0]->getVin(), "VIN-A");
    EXPECT_EQ(due[1]->getVin(), "VIN-C");
}

TEST(FleetManagerTest, FindsVehiclesDueByMileageOrElapsedDaysOnTheQueryDate)
{
    FleetManager fleet;
    fleet.addVehicle(std::make_unique<Van>("VIN-B", "Honda", "Odyssey", 2021, 10000,
                                          VehicleStatus::AVAILABLE, "PLATE-B", "2025-01-01"));
    fleet.addVehicle(std::make_unique<Truck>("VIN-A", "Ford", "F-150", 2022, 10000,
                                            VehicleStatus::AVAILABLE, "PLATE-A", "2025-01-01"));
    fleet.updateMileage("VIN-B", 15000);

    const auto beforeDayThreshold = fleet.getVehiclesNeedingService("2025-06-29");
    ASSERT_EQ(beforeDayThreshold.size(), 1u);
    EXPECT_EQ(beforeDayThreshold[0]->getVin(), "VIN-B");

    const auto onDayThreshold = fleet.getVehiclesNeedingService("2025-06-30");
    ASSERT_EQ(onDayThreshold.size(), 2u);
    EXPECT_EQ(onDayThreshold[0]->getVin(), "VIN-A");
    EXPECT_EQ(onDayThreshold[1]->getVin(), "VIN-B");
}

TEST(FleetManagerTest, LogsCompletedServiceAndClearsOnlyTheServicedVehiclesDueState)
{
    FleetManager fleet;
    fleet.addVehicle(std::make_unique<Sedan>("VIN-A", "Toyota", "Camry", 2021, 10000));
    fleet.addVehicle(std::make_unique<Sedan>("VIN-B", "Honda", "Civic", 2022, 10000));
    fleet.updateMileage("VIN-A", 15000);
    fleet.updateMileage("VIN-B", 15000);
    ASSERT_EQ(fleet.getVehiclesNeedingService().size(), 2u);

    const MaintenanceRecord record("MR-001", "Oil and Filter Change", "2025-06-30", 15000, 5000);
    fleet.logServiceEvent("VIN-A", record);

    const auto due = fleet.getVehiclesNeedingService();
    ASSERT_EQ(due.size(), 1u);
    EXPECT_EQ(due[0]->getVin(), "VIN-B");
    EXPECT_TRUE(due[0]->getMaintenanceHistory().empty());

    const Vehicle* serviced = fleet.findVehicle("VIN-A");
    ASSERT_NE(serviced, nullptr);
    const auto& history = serviced->getMaintenanceHistory();
    ASSERT_EQ(history.size(), 1u);
    EXPECT_EQ(history[0].getRecordId(), "MR-001");
}

TEST(FleetManagerTest, RejectsFutureServiceWithoutChangingScheduleOrHistory)
{
    FleetManager fleet;
    fleet.addVehicle(std::make_unique<Truck>("VIN-A", "Ford", "F-150", 2022, 10000,
                                            VehicleStatus::AVAILABLE, "PLATE-A", "2025-01-01"));
    fleet.updateMileage("VIN-A", 15000);

    const MaintenanceRecord record("MR-001", "Oil and Filter Change", "9999-12-31", 15000, 5000);
    EXPECT_THROW(fleet.logServiceEvent("VIN-A", record), std::invalid_argument);

    const auto due = fleet.getVehiclesNeedingService("2025-06-29");
    ASSERT_EQ(due.size(), 1u);
    EXPECT_EQ(due[0]->getVin(), "VIN-A");
    EXPECT_TRUE(due[0]->getMaintenanceHistory().empty());
}
