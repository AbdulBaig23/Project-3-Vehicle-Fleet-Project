cmake_minimum_required(VERSION 3.16)

project(Project3
        VERSION 1.0.0
        DESCRIPTION "Project 3: Vehicle Fleet Maintenance Management System"
        LANGUAGES CXX
)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY "https://github.com/google/googletest"
  GIT_TAG        v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TEST_CASES
    tests/Vehicle_tests.cpp
    tests/MaintenanceSchedule_tests.cpp
    tests/MaintenanceRecord_tests.cpp
    tests/Sedan_tests.cpp
    tests/FleetManager_tests.cpp
    tests/main_tests.cpp
)

set(FLEET_SOURCES
    src/MaintenanceSchedule.cpp
    src/MaintenanceRecord.cpp
    src/Vehicle.cpp
    src/Sedan.cpp
    src/FleetManager.cpp
    src/Van.cpp
    src/Truck.cpp
)

add_executable(Vehicle_Manager_Test ${TEST_CASES} ${FLEET_SOURCES})

target_link_libraries(Vehicle_Manager_Test PRIVATE gtest_main)

target_include_directories(Vehicle_Manager_Test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(Vehicle_Manager_Test PRIVATE cxx_std_14)

add_executable(Vehicle_Manager src/main.cpp ${FLEET_SOURCES})
target_include_directories(Vehicle_Manager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(Vehicle_Manager PRIVATE cxx_std_14)

enable_testing()
include(GoogleTest)
gtest_discover_tests(Vehicle_Manager_Test)
