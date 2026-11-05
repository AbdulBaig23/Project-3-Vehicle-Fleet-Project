#include "MaintenanceSchedule.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {
int checks = 0;

void expect(bool condition, const char* message)
{
    ++checks;
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename Action>
void expectInvalidArgument(Action action, const char* message)
{
    bool caught = false;
    try {
        action();
    } catch (const std::invalid_argument&) {
        caught = true;
    }
    expect(caught, message);
}

void runTests()
{
    MaintenanceSchedule oil("Oil change", 5000, 10000);
    expect(oil.getServiceName() == "Oil change", "Service name mismatch");
    expect(oil.getIntervalMiles() == 5000, "Interval mismatch");
    expect(oil.getLastServiceMileage() == 10000, "Initial mileage mismatch");
    expect(!oil.isOverdue(10000), "Just serviced must not be due");
    expect(!oil.isOverdue(14999), "Below threshold must not be due");
    expect(oil.isOverdue(15000), "Exact threshold must be due");
    expect(oil.isOverdue(15001), "Above threshold must be due");

    MaintenanceSchedule tires("Tire rotation", 7500, 10000);
    oil.recordService(15001);
    expect(oil.getLastServiceMileage() == 15001, "Service did not update baseline");
    expect(!oil.isOverdue(15001), "Recorded service must clear due condition");
    expect(!oil.isOverdue(20000), "Next service triggered too early");
    expect(oil.isOverdue(20001), "Next threshold must use updated baseline");
    expect(tires.getLastServiceMileage() == 10000, "Unrelated schedule changed");
    oil.recordService(15001);
    expect(!oil.isOverdue(15001), "Same-mileage service must be allowed");
    oil.recordService(16000);
    expect(oil.getLastServiceMileage() == 16000, "Early service must be allowed");

    expectInvalidArgument([] { MaintenanceSchedule s("", 5000, 0); }, "Empty name accepted");
    expectInvalidArgument([] { MaintenanceSchedule s(" \t\r\n", 5000, 0); }, "Blank name accepted");
    expectInvalidArgument([] { MaintenanceSchedule s("Oil", 0, 0); }, "Zero interval accepted");
    expectInvalidArgument([] { MaintenanceSchedule s("Oil", -1, 0); }, "Negative interval accepted");
    expectInvalidArgument([] { MaintenanceSchedule s("Oil", 5000, -1); }, "Negative baseline accepted");
    expectInvalidArgument([&] { oil.isOverdue(-1); }, "Negative current mileage accepted");
    expectInvalidArgument([&] { oil.isOverdue(15999); }, "Mileage before service accepted");
    expectInvalidArgument([&] { oil.recordService(-1); }, "Negative service mileage accepted");
    expectInvalidArgument([&] { oil.recordService(15999); }, "Service rollback accepted");
    expect(oil.getLastServiceMileage() == 16000, "Rejected service changed state");

    MaintenanceSchedule newVehicle("Inspection", 1, 0);
    expect(!newVehicle.isOverdue(0), "Zero mileage must be valid");
    expect(newVehicle.isOverdue(1), "Minimum interval boundary failed");
    const int maxMileage = std::numeric_limits<int>::max();
    MaintenanceSchedule nearLimit("Inspection", 5000, maxMileage - 100);
    expect(!nearLimit.isOverdue(maxMileage), "Overflow caused a false alert");
    MaintenanceSchedule fullRange("Inspection", maxMileage, 0);
    expect(fullRange.isOverdue(maxMileage), "Maximum representable interval failed");
    MaintenanceSchedule exactLimit("Inspection", 100, maxMileage - 100);
    expect(exactLimit.isOverdue(maxMileage), "Maximum mileage boundary failed");
    exactLimit.recordService(maxMileage);
    expect(!exactLimit.isOverdue(maxMileage), "Service at maximum mileage failed");
}
} // namespace

int main()
{
    try {
        runTests();
        std::cout << "PASS: " << checks << " maintenance schedule checks\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
