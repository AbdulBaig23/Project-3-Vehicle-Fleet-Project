#ifndef MAINTENANCE_RECORD_H
#define MAINTENANCE_RECORD_H

#include <cstdint>
#include <string>

class MaintenanceRecord
{
public:
    // Stores the odometer reading in whole miles and the cost in cents.
    // Throws std::invalid_argument for a blank ID or service type, an invalid
    // YYYY-MM-DD date, or a negative mileage or cost.
    MaintenanceRecord(const std::string& recordId,
                      const std::string& serviceType,
                      const std::string& serviceDate,
                      int mileageAtService,
                      std::int64_t costCents);

    const std::string& getRecordId() const;
    const std::string& getServiceType() const;
    const std::string& getServiceDate() const;
    int getMileageAtService() const;
    std::int64_t getCostCents() const;

private:
    std::string recordId;
    std::string serviceType;
    std::string serviceDate;
    int mileageAtService;
    std::int64_t costCents;
};

#endif // MAINTENANCE_RECORD_H
