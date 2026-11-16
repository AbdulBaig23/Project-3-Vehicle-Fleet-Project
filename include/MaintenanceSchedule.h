#ifndef MAINTENANCE_SCHEDULE_H
#define MAINTENANCE_SCHEDULE_H

#include <string>

class MaintenanceSchedule
{
public:
    // Throws std::invalid_argument for a blank name, nonpositive interval,
    // or negative last-service mileage.
    MaintenanceSchedule(const std::string& serviceName, int intervalMiles,
                        int lastServiceMileage);

    const std::string& getServiceName() const;
    int getIntervalMiles() const;
    int getLastServiceMileage() const;

    // Compatibility name: true when due OR overdue (including exact threshold).
    // Throws std::invalid_argument if mileage precedes the last service.
    bool isOverdue(int currentMileage) const;

    // Updates this schedule only. Does not create a maintenance history record.
    // Early service and repeated service at the same mileage are allowed.
    // Throws std::invalid_argument if mileage precedes the last service.
    void recordService(int currentMileage);

private:
    std::string serviceName;
    int intervalMiles;
    int lastServiceMileage;
};

#endif // MAINTENANCE_SCHEDULE_H
