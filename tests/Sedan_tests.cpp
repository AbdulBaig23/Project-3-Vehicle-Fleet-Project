#include <gtest/gtest.h>
#include "Sedan.h"

TEST(SedanTest, InitializesSchedulesAtOnboardingMileage)
{
    const Sedan sedan("VIN001", "Toyota", "Camry", 2021, 60000);
    const auto& schedules = sedan.getMaintenanceSchedules();

    ASSERT_EQ(schedules.size(), 6u);

    for (const auto& schedule : schedules) {
        EXPECT_EQ(schedule.getLastServiceMileage(), 60000);
    }

    EXPECT_FALSE(sedan.isServiceRequired());
}

TEST(SedanTest, OilChangeBecomesDueAtItsInterval)
{
    Sedan sedan("VIN002", "Honda", "Civic", 2022, 60000);

    EXPECT_FALSE(sedan.isServiceRequired());

    sedan.updateMileage(64999);
    EXPECT_FALSE(sedan.isServiceRequired());

    sedan.updateMileage(65000);
    EXPECT_TRUE(sedan.isServiceRequired());
}