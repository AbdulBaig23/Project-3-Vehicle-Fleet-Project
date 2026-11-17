#include "FleetManager.h"
#include "Sedan.h"
#include "ServiceDate.h"
#include "Truck.h"
#include "Van.h"

#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

using namespace std;

// Reads line into a string; true if input succeeds, false if input ends (Ex. EOF, Ctrl+Z)
bool readLine(const string& prompt, string& value)
{
    cout << prompt;
    return static_cast<bool>(getline(cin, value));
}

// Checks for valid int input
bool readInteger(const string& prompt, int& value)
{
    while (true) {
        string input;
        if (!readLine(prompt, input)) {
            return false;
        }

        istringstream parser(input);
        long long parsed = 0;
        char extra = '\0';
        if (parser >> parsed && !(parser >> extra)
            && parsed >= numeric_limits<int>::min()
            && parsed <= numeric_limits<int>::max()) {
            value = static_cast<int>(parsed);
            return true;
        }
        cout << "Please enter a whole number.\n";
    }
}

// Similar to readInteger(), but stores a larger integer value, used for service costs in cents
bool readInt64(const string& prompt, int64_t& value)
{
    while (true) {
        string input;
        if (!readLine(prompt, input)) {
            return false;
        }

        istringstream parser(input);
        long long parsed = 0;
        char extra = '\0';
        if (parser >> parsed && !(parser >> extra)) {
            value = static_cast<int64_t>(parsed);
            return true;
        }
        cout << "Please enter a whole number of cents.\n";
    }
}

// Converts VehicleStatus value to a readable vehicle status
const char* statusName(VehicleStatus status)
{
    switch (status) {
    case VehicleStatus::AVAILABLE: return "Available";
    case VehicleStatus::IN_SERVICE: return "In service";
    case VehicleStatus::OUT_OF_SERVICE: return "Out of service";
    }
    return "Unknown";
}

// Reads a number choice, then converts that choice to its corresponding vehicle status
bool readStatus(VehicleStatus& status)
{
    int choice = 0;
    while (true) {
        cout << "1. Available\n2. In service\n3. Out of service\n";
        if (!readInteger("Status: ", choice)) {
            return false;
        }
        if (choice >= 1 && choice <= 3) {
            status = choice == 1 ? VehicleStatus::AVAILABLE
                : choice == 2 ? VehicleStatus::IN_SERVICE
                : VehicleStatus::OUT_OF_SERVICE;
            return true;
        }
        cout << "Choose 1, 2, or 3.\n";
    }
}

// Prints a vehicle's information. Also shows if service is due
void printVehicle(const Vehicle& vehicle)
{
    cout << vehicle.getVehicleType() << " | " << vehicle.getYear() << ' '
        << vehicle.getMake() << ' ' << vehicle.getModel()
        << " | VIN: " << vehicle.getVin()
        << " | Mileage: " << vehicle.getMileage()
        << " | Status: " << statusName(vehicle.getStatus());
    if (!vehicle.getLicensePlate().empty()) {
        cout << " | Plate: " << vehicle.getLicensePlate();
    }
    if (vehicle.isServiceRequired()) {
        cout << " | SERVICE DUE";
    }
    cout << '\n';
}

// Provides a list of registered vehicles
// Tells user if the list is empty
void listVehicles(const FleetManager& fleet)
{
    const auto vehicles = fleet.listVehicles();
    if (vehicles.empty()) {
        cout << "No vehicles are registered.\n";
        return;
    }
    for (const Vehicle* vehicle : vehicles) {
        printVehicle(*vehicle);
    }
}

// Prompts for vehicle type and details, creates a vehicle of its respective type, and adds it to the fleet
// returns false if input ends
bool registerVehicle(FleetManager& fleet)
{
    int type = 0;
    while (true) {
        cout << "1. Sedan\n2. Truck\n3. Van\n";
        if (!readInteger("Vehicle type: ", type)) {
            return false;
        }
        if (type >= 1 && type <= 3) {
            break;
        }
        cout << "Choose 1, 2, or 3.\n";
    }

    string vin, make, model, plate;
    int year = 0, mileage = 0;
    if (!readLine("VIN: ", vin)
        || !readLine("Make: ", make)
        || !readLine("Model: ", model)
        || !readInteger("Year: ", year)
        || !readInteger("Mileage: ", mileage)
        || !readLine("License plate (optional): ", plate)) {
        return false;
    }

    unique_ptr<Vehicle> vehicle;
    if (type == 1) {
        vehicle = make_unique<Sedan>(vin, make, model, year, mileage);
    }
    else if (type == 2) {
        vehicle = make_unique<Truck>(vin, make, model, year, mileage);
    }
    else {
        vehicle = make_unique<Van>(vin, make, model, year, mileage);
    }
    vehicle->setLicensePlate(plate);
    fleet.addVehicle(move(vehicle));
    cout << "Vehicle registered.\n";
    return true;
}

// Prints vehicle summary, maintenance schedule, and due status
// Also prints service history and costs
void showVehicleDetails(const Vehicle& vehicle)
{
    printVehicle(vehicle);
    const string today = fleet_detail::todayUtc();

    cout << "\nMaintenance schedule:\n";
    for (const MaintenanceSchedule& schedule : vehicle.getMaintenanceSchedules()) {
        cout << "  " << schedule.getServiceName()
            << " | every " << schedule.getIntervalMiles() << " miles";
        if (schedule.getIntervalDays() > 0) {
            cout << " / " << schedule.getIntervalDays() << " days";
        }
        cout << " | last service at " << schedule.getLastServiceMileage()
            << " miles";
        if (!schedule.getLastServiceDate().empty()) {
            cout << " on " << schedule.getLastServiceDate();
        }
        if (schedule.isOverdue(vehicle.getMileage(), today)) {
            cout << " | DUE";
        }
        cout << '\n';
    }

    const auto& history = vehicle.getMaintenanceHistory();
    cout << "\nCompleted service history:\n";
    if (history.empty()) {
        cout << "  No service records.\n";
        return;
    }
    for (const MaintenanceRecord& record : history) {
        cout << "  " << record.getRecordId() << " | "
            << record.getServiceType() << " | " << record.getServiceDate()
            << " | " << record.getMileageAtService() << " miles | $"
            << fixed << setprecision(2)
            << static_cast<double>(record.getCostCents()) / 100.0 << '\n';
    }
}

// Searches for a vehicle via VIN number, and displays its details
void viewVehicle(const FleetManager& fleet)
{
    string vin;
    if (!readLine("VIN: ", vin)) {
        return;
    }
    const Vehicle* vehicle = fleet.findVehicle(vin);
    if (!vehicle) {
        cout << "No vehicle is registered with that VIN.\n";
        return;
    }
    showVehicleDetails(*vehicle);
}

// Searches for a vehicle via VIN number, and allows user to update mileage/status
void updateVehicle(FleetManager& fleet)
{
    string vin;
    if (!readLine("VIN: ", vin)) {
        return;
    }
    const Vehicle* vehicle = fleet.findVehicle(vin);
    if (!vehicle) {
        cout << "No vehicle is registered with that VIN.\n";
        return;
    }

    int choice = 0;
    cout << "1. Update mileage\n2. Update status\n";
    if (!readInteger("Update option: ", choice)) {
        return;
    }
    if (choice == 1) {
        int mileage = 0;
        if (readInteger("New mileage: ", mileage)) {
            fleet.updateMileage(vin, mileage);
            cout << "Mileage updated.\n";
        }
    }
    else if (choice == 2) {
        VehicleStatus status;
        if (readStatus(status)) {
            fleet.setVehicleStatus(vin, status);
            cout << "Status updated.\n";
        }
    }
    else {
        cout << "Invalid update option.\n";
    }
}

// Creates a service log that is saved to the fleet manager
void logService(FleetManager& fleet)
{
    string vin, recordId, serviceType, date;
    int mileage = 0;
    int64_t costCents = 0;
    if (!readLine("VIN: ", vin)
        || !readLine("Record ID: ", recordId)
        || !readLine("Service type (match a schedule name to update it): ", serviceType)
        || !readLine("Service date (YYYY-MM-DD): ", date)
        || !readInteger("Mileage at service: ", mileage)
        || !readInt64("Cost in cents: ", costCents)) {
        return;
    }
    const MaintenanceRecord record(recordId, serviceType, date, mileage, costCents);
    fleet.logServiceEvent(vin, record);
    cout << "Service record saved.\n";
}

// Displays all vehicles that are due service
void listVehiclesNeedingService(const FleetManager& fleet)
{
    const auto vehicles = fleet.getVehiclesNeedingService();
    if (vehicles.empty()) {
        cout << "No vehicles currently need service.\n";
        return;
    }
    for (const Vehicle* vehicle : vehicles) {
        printVehicle(*vehicle);
    }
}

// Searches for a vehicle via VIN number, and removes it from the fleet
void removeVehicle(FleetManager& fleet)
{
    string vin;
    if (!readLine("VIN to remove: ", vin)) {
        return;
    }
    if (fleet.removeVehicle(vin)) {
        cout << "Vehicle removed.\n";
    }
    else {
        cout << "No vehicle is registered with that VIN.\n";
    }
}

// Main menu UI
void printMenu()
{
    cout << "\n------ Vehicle Fleet Maintenance Manager ------\n"
        << "1. List vehicles\n"
        << "2. Register vehicle\n"
        << "3. View vehicle details and service history\n"
        << "4. Update vehicle information\n"
        << "5. Log completed service\n"
        << "6. List vehicles needing service\n"
        << "7. Remove vehicle\n"
        << "8. Exit\n";
}

int main()
{
    FleetManager fleet;
    cout << "Vehicle Fleet Maintenance Manager\n"
        << "Note: fleet data is kept in memory and is cleared when the program exits.\n";

    while (true) {
        printMenu();
        int choice = 0;
        if (!readInteger("Choose an option: ", choice)) {
            cout << "\nExiting.\n";
            return 0;
        }

        try {
            switch (choice) {
            case 1: listVehicles(fleet); break;
            case 2:
                if (!registerVehicle(fleet)) {
                    cout << "\nExiting.\n";
                    return 0;
                }
                break;
            case 3: viewVehicle(fleet); break;
            case 4: updateVehicle(fleet); break;
            case 5: logService(fleet); break;
            case 6: listVehiclesNeedingService(fleet); break;
            case 7: removeVehicle(fleet); break;
            case 8:
                cout << "Goodbye.\n";
                return 0;
            default:
                cout << "Choose an option from 1 through 8.\n";
                break;
            }
        }
        catch (const exception& error) {
            cout << "Operation failed: " << error.what() << '\n';
        }
    }
}
