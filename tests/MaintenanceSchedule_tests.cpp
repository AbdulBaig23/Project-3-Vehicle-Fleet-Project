#include <gtest/gtest.h>
#include "MaintenanceSchedule.h"

#include <limits>
#include <stdexcept>

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

TEST_F(MaintenanceScheduleTest, RejectsBlankServiceNames) {
    EXPECT_THROW(MaintenanceSchedule("", 5000, 0), std::invalid_argument);
    EXPECT_THROW(MaintenanceSchedule(" \t\n\r\f\v", 5000, 0), std::invalid_argument);
}

TEST_F(MaintenanceScheduleTest, RejectsNonpositiveServiceIntervals) {
    EXPECT_THROW(MaintenanceSchedule("Oil Change", 0, 0), std::invalid_argument);
    EXPECT_THROW(MaintenanceSchedule("Oil Change", -1, 0), std::invalid_argument);
}

TEST_F(MaintenanceScheduleTest, RejectsNegativeLastServiceMileage) {
    EXPECT_THROW(MaintenanceSchedule("Oil Change", 5000, -1), std::invalid_argument);
}

TEST_F(MaintenanceScheduleTest, IsNotDueAtLastServiceMileage) {
    const MaintenanceSchedule schedule("Oil Change", 5000, 10000);

    EXPECT_FALSE(schedule.isOverdue(10000));
}

TEST_F(MaintenanceScheduleTest, IsNotDueImmediatelyBeforeInterval) {
    const MaintenanceSchedule schedule("Oil Change", 5000, 10000);

    EXPECT_FALSE(schedule.isOverdue(14999));
    EXPECT_TRUE(schedule.isOverdue(15000));
}

TEST_F(MaintenanceScheduleTest, RejectsNegativeCurrentMileage) {
    const MaintenanceSchedule schedule("Oil Change", 5000, 0);

    EXPECT_THROW(schedule.isOverdue(-1), std::invalid_argument);
}

TEST_F(MaintenanceScheduleTest, RejectsCurrentMileageBeforeLastService) {
    const MaintenanceSchedule schedule("Oil Change", 5000, 10000);

    EXPECT_THROW(schedule.isOverdue(9999), std::invalid_argument);
}

TEST_F(MaintenanceScheduleTest, RejectsNegativeServiceMileageWithoutChangingState) {
    MaintenanceSchedule schedule("Oil Change", 5000, 10000);

    EXPECT_THROW(schedule.recordService(-1), std::invalid_argument);
    EXPECT_EQ(schedule.getLastServiceMileage(), 10000);
    EXPECT_TRUE(schedule.isOverdue(15000));
}

TEST_F(MaintenanceScheduleTest, RejectsServiceRollbackWithoutChangingState) {
    MaintenanceSchedule schedule("Oil Change", 5000, 10000);

    EXPECT_THROW(schedule.recordService(9999), std::invalid_argument);
    EXPECT_EQ(schedule.getLastServiceMileage(), 10000);
    EXPECT_TRUE(schedule.isOverdue(15000));
}

TEST_F(MaintenanceScheduleTest, EarlyServiceResetsTheNextDueMileage) {
    MaintenanceSchedule schedule("Oil Change", 5000, 10000);

    schedule.recordService(12000);

    EXPECT_EQ(schedule.getLastServiceMileage(), 12000);
    EXPECT_FALSE(schedule.isOverdue(15000));
    EXPECT_FALSE(schedule.isOverdue(16999));
    EXPECT_TRUE(schedule.isOverdue(17000));
}

TEST_F(MaintenanceScheduleTest, AllowsRepeatedServiceAtTheSameMileage) {
    MaintenanceSchedule schedule("Oil Change", 5000, 10000);

    EXPECT_NO_THROW(schedule.recordService(10000));
    EXPECT_NO_THROW(schedule.recordService(10000));
    EXPECT_EQ(schedule.getLastServiceMileage(), 10000);
    EXPECT_FALSE(schedule.isOverdue(10000));
    EXPECT_TRUE(schedule.isOverdue(15000));
}

TEST_F(MaintenanceScheduleTest, ServiceDoesNotResetAnUnrelatedSchedule) {
    MaintenanceSchedule oil("Oil Change", 5000, 10000);
    MaintenanceSchedule tires("Tire Rotation", 7500, 10000);

    oil.recordService(17500);

    EXPECT_FALSE(oil.isOverdue(17500));
    EXPECT_EQ(tires.getLastServiceMileage(), 10000);
    EXPECT_TRUE(tires.isOverdue(17500));
}

TEST_F(MaintenanceScheduleTest, SupportsZeroMileageAndSmallestPositiveInterval) {
    const MaintenanceSchedule schedule("Inspection", 1, 0);

    EXPECT_FALSE(schedule.isOverdue(0));
    EXPECT_TRUE(schedule.isOverdue(1));
}

TEST_F(MaintenanceScheduleTest, DoesNotOverflowNearMaximumMileage) {
    const int maxMileage = std::numeric_limits<int>::max();
    const MaintenanceSchedule schedule("Inspection", 5000, maxMileage - 100);

    // The next due mileage exceeds int's range; current mileage is not due.
    EXPECT_FALSE(schedule.isOverdue(maxMileage));
}

TEST_F(MaintenanceScheduleTest, SupportsMaximumRepresentableInterval) {
    const int maxMileage = std::numeric_limits<int>::max();
    const MaintenanceSchedule schedule("Inspection", maxMileage, 0);

    EXPECT_FALSE(schedule.isOverdue(maxMileage - 1));
    EXPECT_TRUE(schedule.isOverdue(maxMileage));
}

TEST_F(MaintenanceScheduleTest, SupportsServiceAtMaximumMileage) {
    const int maxMileage = std::numeric_limits<int>::max();
    MaintenanceSchedule schedule("Inspection", 100, maxMileage - 100);

    EXPECT_TRUE(schedule.isOverdue(maxMileage));
    schedule.recordService(maxMileage);
    EXPECT_EQ(schedule.getLastServiceMileage(), maxMileage);
    EXPECT_FALSE(schedule.isOverdue(maxMileage));
}
