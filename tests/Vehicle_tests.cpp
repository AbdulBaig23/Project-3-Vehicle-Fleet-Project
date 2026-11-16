#include <gtest/gtest.h>
#include "Vehicle.h"

// Concrete subclass used to instantiate and test the abstract Vehicle class
class TestVehicle : public Vehicle {
public:
    TestVehicle(std::string vin, std::string make, std::string model, int year, int mileage, VehicleStatus status = VehicleStatus::AVAILABLE)
        : Vehicle(std::move(vin), std::move(make), std::move(model), year, mileage, status) {}

    // Implement the pure virtual function for testing purposes
    bool isServiceRequired() const override {
        // Example logic: service required if mileage exceeds 10,000
        return getMileage() > 10000;
    }
};

// Test fixture for shared vehicle setup (optional, but good practice)
class VehicleTest : public ::testing::Test {
protected:
    // You can set up common objects here if needed
};

// 1. Test Constructor and Getters with explicit status
TEST_F(VehicleTest, ConstructorAndGettersWithStatus) {
    TestVehicle vehicle("1HGCR2F83HA000123", "Honda", "Accord", 2017, 15000, VehicleStatus::IN_SERVICE);

    EXPECT_EQ(vehicle.getVin(), "1HGCR2F83HA000123");
    EXPECT_EQ(vehicle.getMake(), "Honda");
    EXPECT_EQ(vehicle.getModel(), "Accord");
    EXPECT_EQ(vehicle.getYear(), 2017);
    EXPECT_EQ(vehicle.getMileage(), 15000);
    EXPECT_EQ(vehicle.getStatus(), VehicleStatus::IN_SERVICE);
}

// 2. Test Constructor default parameter for VehicleStatus
TEST_F(VehicleTest, DefaultVehicleStatus) {
    TestVehicle vehicle("1234567890ABCDEFG", "Toyota", "Camry", 2021, 5000);

    // Should default to VehicleStatus::AVAILABLE as defined in the header
    EXPECT_EQ(vehicle.getStatus(), VehicleStatus::AVAILABLE);
}

// 3. Test updating mileage
TEST_F(VehicleTest, UpdateMileage) {
    TestVehicle vehicle("VIN123456789", "Ford", "F-150", 2020, 25000);

    EXPECT_EQ(vehicle.getMileage(), 25000);
    
    vehicle.updateMileage(26500);
    EXPECT_EQ(vehicle.getMileage(), 26500);
}

// 4. Test changing vehicle status
TEST_F(VehicleTest, SetStatus) {
    TestVehicle vehicle("VIN987654321", "Tesla", "Model 3", 2022, 1200);

    EXPECT_EQ(vehicle.getStatus(), VehicleStatus::AVAILABLE);

    vehicle.setStatus(VehicleStatus::OVERDUE);
    EXPECT_EQ(vehicle.getStatus(), VehicleStatus::OVERDUE);

    vehicle.setStatus(VehicleStatus::OUT_OF_SERVICE);
    EXPECT_EQ(vehicle.getStatus(), VehicleStatus::OUT_OF_SERVICE);
}

// 5. Test pure virtual function implementation via base class pointer/reference
TEST_F(VehicleTest, PureVirtualServiceRequiredBehavior) {
    TestVehicle lowMileageVehicle("VIN001", "Subaru", "Outback", 2019, 4000);
    TestVehicle highMileageVehicle("VIN002", "Subaru", "Outback", 2019, 12000);

    // Test through the concrete object directly
    EXPECT_FALSE(lowMileageVehicle.isServiceRequired());
    EXPECT_TRUE(highMileageVehicle.isServiceRequired());

    // Test through polymorphism (Base class pointer)
    Vehicle* vPtr = &highMileageVehicle;
    EXPECT_TRUE(vPtr->isServiceRequired());
}