#ifndef MAINTENANCE_SCHEDULE_H
#define MAINTENANCE_SCHEDULE_H

#include <string>

class MaintenanceSchedule
{
public:
    MaintenanceSchedule(const std::string& serviceName, int intervalMiles, int lastServiceMileage); // prevents copying the original variable but also prevents modification of that variable
    const std::string& getServiceName() const; // gets service name. uses const so getter wont change the schedule
    int getIntervalMiles() const; // const needed at end so code cant adjust miles and then return them
    int getLastServiceMileage() const;

    bool isOverdue(int currentMileage) const; // checks if the service is overdue based on the current mileage and the last service mileage
    void recordService(int currentMileage);

private:
    std::string serviceName; // attributes stored in the maintenance schedule
    int intervalMiles;
    int lastServiceMileage;

};


#endif // MAINTENANCE_SCHEDULE_H

