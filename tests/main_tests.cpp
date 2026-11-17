#include <gtest/gtest.h>

#define main vehicle_fleet_application_main
#include "../src/main.cpp"
#undef main

class ConsoleRedirect
{
public:
    explicit ConsoleRedirect(const string& input)
        : input_(input),
          oldCinState_(cin.rdstate()),
          oldCoutState_(cout.rdstate()),
          oldCoutFlags_(cout.flags()),
          oldCoutPrecision_(cout.precision()),
          oldCin_(cin.rdbuf(input_.rdbuf())),
          oldCout_(cout.rdbuf(output_.rdbuf()))
    {
    }

    ~ConsoleRedirect()
    {
        cin.rdbuf(oldCin_);
        cin.clear(oldCinState_);
        cout.rdbuf(oldCout_);
        cout.clear(oldCoutState_);
        cout.flags(oldCoutFlags_);
        cout.precision(oldCoutPrecision_);
    }

    string output() const
    {
        return output_.str();
    }

private:
    istringstream input_;
    ostringstream output_;
    ios::iostate oldCinState_;
    ios::iostate oldCoutState_;
    ios::fmtflags oldCoutFlags_;
    streamsize oldCoutPrecision_;
    streambuf* oldCin_;
    streambuf* oldCout_;
};

TEST(MainInputTest, ReadsAWholeLineAndReturnsFalseAtEndOfInput)
{
    ConsoleRedirect console("vehicle details\n");
    string value;

    EXPECT_TRUE(readLine("Input: ", value));
    EXPECT_EQ(value, "vehicle details");
    EXPECT_FALSE(readLine("Input: ", value));
}

TEST(MainInputTest, RetriesInvalidAndOutOfRangeIntegers)
{
    const string input = "not a number\n"
        + to_string(static_cast<long long>(numeric_limits<int>::max()) + 1)
        + "\n42\n";
    ConsoleRedirect console(input);
    int value = 0;

    ASSERT_TRUE(readInteger("Number: ", value));

    EXPECT_EQ(value, 42);
    EXPECT_NE(console.output().find("Please enter a whole number."), string::npos);
    EXPECT_NE(console.output().find("Please enter a whole number."),
              console.output().rfind("Please enter a whole number."));
}

TEST(MainInputTest, RetriesInvalidInt64Input)
{
    ConsoleRedirect console("12 cents\n1234567890123\n");
    int64_t value = 0;

    ASSERT_TRUE(readInt64("Cost: ", value));

    EXPECT_EQ(value, 1234567890123LL);
    EXPECT_NE(console.output().find("Please enter a whole number of cents."),
              string::npos);
}

TEST(MainInputTest, RetriesInvalidStatusChoice)
{
    ConsoleRedirect console("4\n3\n");
    VehicleStatus status = VehicleStatus::AVAILABLE;

    ASSERT_TRUE(readStatus(status));

    EXPECT_EQ(status, VehicleStatus::OUT_OF_SERVICE);
    EXPECT_NE(console.output().find("Choose 1, 2, or 3."), string::npos);
}

TEST(MainInputTest, ReturnsFalseWhenIntegerInputEnds)
{
    ConsoleRedirect console("");
    int value = 0;

    EXPECT_FALSE(readInteger("Number: ", value));
}

TEST(MainUiTest, RegistersEachVehicleTypeAndRetriesAnInvalidType)
{
    ConsoleRedirect console(
        "0\n1\nVIN-SEDAN\nToyota\nCamry\n2021\n10000\n\n"
        "2\nVIN-TRUCK\nFord\nF-150\n2022\n20000\nPLATE-T\n"
        "3\nVIN-VAN\nHonda\nOdyssey\n2023\n30000\nPLATE-V\n");
    FleetManager fleet;

    EXPECT_TRUE(registerVehicle(fleet));
    EXPECT_TRUE(registerVehicle(fleet));
    EXPECT_TRUE(registerVehicle(fleet));

    EXPECT_NE(dynamic_cast<const Sedan*>(fleet.findVehicle("VIN-SEDAN")), nullptr);
    EXPECT_NE(dynamic_cast<const Truck*>(fleet.findVehicle("VIN-TRUCK")), nullptr);
    EXPECT_NE(dynamic_cast<const Van*>(fleet.findVehicle("VIN-VAN")), nullptr);
    EXPECT_EQ(fleet.listVehicles().size(), 3u);
    EXPECT_NE(console.output().find("Choose 1, 2, or 3."), string::npos);
}

TEST(MainUiTest, RegistersListsAndExits)
{
    ConsoleRedirect console(
        "2\n1\nVIN-001\nToyota\nCamry\n2021\n10000\nPLATE-1\n"
        "1\n8\n");

    EXPECT_EQ(vehicle_fleet_application_main(), 0);

    const string output = console.output();
    EXPECT_NE(output.find("Vehicle registered."), string::npos);
    EXPECT_NE(output.find("Vehicle | 2021 Toyota Camry | VIN: VIN-001"),
              string::npos);
    EXPECT_NE(output.find("Goodbye."), string::npos);
}

TEST(MainUiTest, ReportsInvalidMenuChoiceAndUnknownVehicle)
{
    ConsoleRedirect console(
        "0\n3\nMISSING-VIN\n4\nMISSING-VIN\n7\nMISSING-VIN\n8\n");

    EXPECT_EQ(vehicle_fleet_application_main(), 0);

    const string output = console.output();
    EXPECT_NE(output.find("Choose an option from 1 through 8."), string::npos);
    EXPECT_NE(output.find("No vehicle is registered with that VIN."),
              string::npos);
    EXPECT_EQ(output.find("Vehicle removed."), string::npos);
    EXPECT_NE(output.find("Goodbye."), string::npos);
}

TEST(MainUiTest, ListsVehiclesThatNeedService)
{
    ConsoleRedirect console(
        "2\n1\nVIN-DUE\nToyota\nCamry\n2021\n10000\n\n"
        "4\nVIN-DUE\n1\n15000\n"
        "6\n8\n");

    EXPECT_EQ(vehicle_fleet_application_main(), 0);

    const string output = console.output();
    EXPECT_NE(output.find("Mileage updated."), string::npos);
    EXPECT_NE(output.find("Vehicle | 2021 Toyota Camry | VIN: VIN-DUE"),
              string::npos);
    EXPECT_NE(output.find("SERVICE DUE"), string::npos);
}

TEST(MainUiTest, ReportsDuplicateVinAndInvalidServiceDate)
{
    ConsoleRedirect console(
        "2\n1\nVIN-DUP\nToyota\nCamry\n2021\n10000\n\n"
        "2\n2\nVIN-DUP\nFord\nF-150\n2022\n20000\n\n"
        "5\nVIN-DUP\nMR-INVALID\nOil and Filter Change\n2025-02-30\n"
        "10000\n5000\n"
        "8\n");

    EXPECT_EQ(vehicle_fleet_application_main(), 0);

    const string output = console.output();
    EXPECT_NE(output.find("A vehicle with this VIN is already registered."),
              string::npos);
    EXPECT_NE(output.find("Service date must be a valid date in YYYY-MM-DD format."),
              string::npos);
    EXPECT_EQ(output.find("Service record saved."), string::npos);
}

TEST(MainUiTest, UpdatesLogsViewsAndRemovesVehicle)
{
    const string today = fleet_detail::todayUtc();
    ConsoleRedirect console(
        "2\n1\nVIN-002\nHonda\nAccord\n2022\n15000\nPLATE-2\n"
        "4\nVIN-002\n1\n16000\n"
        "4\nVIN-002\n2\n2\n"
        "5\nVIN-002\nMR-002\nOil and Filter Change\n" + today + "\n"
        "16000\n12550\n"
        "3\nVIN-002\n"
        "7\nVIN-002\n"
        "1\n"
        "8\n");

    EXPECT_EQ(vehicle_fleet_application_main(), 0);

    const string output = console.output();
    EXPECT_NE(output.find("Mileage updated."), string::npos);
    EXPECT_NE(output.find("Status updated."), string::npos);
    EXPECT_NE(output.find("Service record saved."), string::npos);
    EXPECT_NE(output.find("16000 miles | $125.50"), string::npos);
    EXPECT_NE(output.find("Vehicle removed."), string::npos);
    EXPECT_NE(output.find("No vehicles are registered."), string::npos);
    EXPECT_EQ(output.find("Operation failed:"), string::npos);
}
