// Describes what every vehicle has and what ever vehicle should be able to do. This is an abstract class that will be inherited by the specific vehicle types (Car, Truck, Motorcycle, etc.)

#ifndef VEHICLE_H //lines prevent header from being included multiple times in a single compilation unit, which can cause errors due to redefinition of classes, functions, or variables.
#define VEHICLE_H 

#include <string> // inldues standard library for stirng type
enum class VehicleStatus // gives possible status values for a vehicle stored under a class called vehicle status
{
    AVAILABLE,
    IN_SERVICE,
    OVERDUE,
    OUT_OF_SERVICE
};

class Vehicle
{
public:
    // Throws std::invalid_argument if the initial mileage is negative.
    Vehicle(std::string vin,
            std::string make,
            std::string model,
            int year,
            int mileage,
            VehicleStatus status = VehicleStatus::AVAILABLE); // can edit avalible to set default to a different option
    virtual ~Vehicle() = default; // tells the compiler to provide the usual destructor. virtual is included because this class has child classes which will get cleaned up correctly

    const std::string& getVin() const; // gets vin
    const std::string& getMake() const; // gets make
    const std::string& getModel() const; // gets model
    int getYear() const; // gets year
    int getMileage() const; // gets mileage
    VehicleStatus getStatus() const; // gets status

    // Sets the absolute odometer reading in miles. Equal readings are allowed.
    // Throws std::invalid_argument for a negative or lower reading and keeps the old value.
    void updateMileage(int newMileage);
    void setStatus(VehicleStatus status); // sets status

    virtual bool isServiceRequired() const = 0; // pure virtual fucntion so you cant make a plain vehicle, it has to be sedan truck etc.

private:
    std::string vin; // attributes stored in the vehicle
    std::string make;
    std::string model;
    int year;
    int mileage;
    VehicleStatus status;
};

#endif
