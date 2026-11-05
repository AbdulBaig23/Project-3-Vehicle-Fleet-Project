#include <gtest/gtest.h>
#include "MaintenanceSchedule.h"

class MaintenanceScheduleTest : public ::testing::Test {
protected:
    // Shared setup can be placed here if needed for multiple tests
};

// 1. Test Constructor and Getters
TEST_F(MaintenanceScheduleTest, ConstructorAndGetters) {
    MaintenanceSchedule schedule("Oil Change", 5000, 10000);

    EXPECT_EQ(schedule.getServiceName(), "Oil Change");
    EXPECT_EQ(schedule.getIntervalMiles(), 5000);
    EXPECT_EQ(schedule.getLastServiceMileage(), 10000);
}

// 2. Test isOverdue when well below the threshold
TEST_F(MaintenanceScheduleTest, IsNotOverdueBelowInterval) {
    MaintenanceSchedule schedule("Tire Rotation", 7500, 20000);

    // Mileage is 23000 (3000 miles since last service, interval is 7500)
    EXPECT_FALSE(schedule.isOverdue(23000));
}

// 3. Test isOverdue at the exact interval boundary
TEST_F(MaintenanceScheduleTest, IsOverdueAtExactInterval) {
    MaintenanceSchedule schedule("Brake Inspection", 10000, 15000);

    // Exactly 10000 miles since last service (15000 + 10000 = 25000)
    EXPECT_TRUE(schedule.isOverdue(25000));
}

// 4. Test isOverdue when past the interval limit
TEST_F(MaintenanceScheduleTest, IsOverdueAboveInterval) {
    MaintenanceSchedule schedule("Air Filter Replacement", 15000, 30000);

    // 16000 miles since last service (exceeds 15000 interval)
    EXPECT_TRUE(schedule.isOverdue(46000));
}

// 5. Test recordService updates mileage and resets overdue state
TEST_F(MaintenanceScheduleTest, RecordServiceUpdatesMileage) {
    MaintenanceSchedule schedule("Oil Change", 5000, 10000);

    // Verify it is currently overdue at 16000 miles
    EXPECT_TRUE(schedule.isOverdue(16000));

    // Record the service at the current mileage
    schedule.recordService(16000);

    // Last service mileage should be updated
    EXPECT_EQ(schedule.getLastServiceMileage(), 16000);

    // It should no longer be overdue immediately after service
    EXPECT_FALSE(schedule.isOverdue(16000));
    EXPECT_FALSE(schedule.isOverdue(20000));

    // It should become overdue again once the new interval is reached (16000 + 5000 = 21000)
    EXPECT_FALSE(schedule.isOverdue(20999));
    EXPECT_TRUE(schedule.isOverdue(21000));
}