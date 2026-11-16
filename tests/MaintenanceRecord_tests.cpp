#include <gtest/gtest.h>
#include "MaintenanceRecord.h"

#include <cstdint>
#include <limits>
#include <stdexcept>

TEST(MaintenanceRecordTest, PreservesEveryFieldAndSupportsWideCosts) {
    const auto costCents = std::numeric_limits<std::int64_t>::max();
    const MaintenanceRecord record("  Work Order 0042  ", "Oil and Filter Change",
                                   "2026-11-16", 15000, costCents);

    EXPECT_EQ(record.getRecordId(), "  Work Order 0042  ");
    EXPECT_EQ(record.getServiceType(), "Oil and Filter Change");
    EXPECT_EQ(record.getServiceDate(), "2026-11-16");
    EXPECT_EQ(record.getMileageAtService(), 15000);
    EXPECT_EQ(record.getCostCents(), costCents);
}

TEST(MaintenanceRecordTest, AcceptsZeroMileageAndCost) {
    const MaintenanceRecord record("MR-001", "Inspection", "2026-11-16", 0, 0);

    EXPECT_EQ(record.getMileageAtService(), 0);
    EXPECT_EQ(record.getCostCents(), 0);
}

TEST(MaintenanceRecordTest, RejectsBlankRecordId) {
    EXPECT_THROW(MaintenanceRecord("", "Inspection", "2026-11-16", 0, 0),
                 std::invalid_argument);
    EXPECT_THROW(MaintenanceRecord(" \t\n\r\f\v", "Inspection", "2026-11-16", 0, 0),
                 std::invalid_argument);
}

TEST(MaintenanceRecordTest, RejectsBlankServiceType) {
    EXPECT_THROW(MaintenanceRecord("MR-001", "", "2026-11-16", 0, 0),
                 std::invalid_argument);
    EXPECT_THROW(MaintenanceRecord("MR-001", " \t\n\r\f\v", "2026-11-16", 0, 0),
                 std::invalid_argument);
}

TEST(MaintenanceRecordTest, RejectsMalformedServiceDates) {
    const char* malformedDates[] = {
        "", "26-11-16", "2026-1-16", "2026-11-6", "2026/11/16",
        "2026-1A-16", "2026-11-16 ", "2026-11-16T00:00:00", "10000-01-01"
    };

    for (const char* date : malformedDates) {
        SCOPED_TRACE(date);
        EXPECT_THROW(MaintenanceRecord("MR-001", "Inspection", date, 0, 0),
                     std::invalid_argument);
    }
}

TEST(MaintenanceRecordTest, RejectsNonexistentGregorianDates) {
    const char* invalidDates[] = {
        "0000-01-01", "2026-00-01", "2026-13-01", "2026-11-00",
        "2026-01-32", "2026-04-31", "2024-02-30", "2025-02-29", "1900-02-29"
    };

    for (const char* date : invalidDates) {
        SCOPED_TRACE(date);
        EXPECT_THROW(MaintenanceRecord("MR-001", "Inspection", date, 0, 0),
                     std::invalid_argument);
    }
}

TEST(MaintenanceRecordTest, AcceptsGregorianBoundaryDates) {
    const char* validDates[] = {
        "0001-01-01", "1900-02-28", "2000-02-29", "2024-02-29", "2026-04-30",
        "9999-12-31"
    };

    for (const char* date : validDates) {
        SCOPED_TRACE(date);
        const MaintenanceRecord record("MR-001", "Inspection", date, 0, 0);
        EXPECT_EQ(record.getServiceDate(), date);
    }
}

TEST(MaintenanceRecordTest, RejectsNegativeMileage) {
    EXPECT_THROW(MaintenanceRecord("MR-001", "Inspection", "2026-11-16", -1, 0),
                 std::invalid_argument);
}

TEST(MaintenanceRecordTest, RejectsNegativeCost) {
    EXPECT_THROW(MaintenanceRecord("MR-001", "Inspection", "2026-11-16", 0, -1),
                 std::invalid_argument);
}
