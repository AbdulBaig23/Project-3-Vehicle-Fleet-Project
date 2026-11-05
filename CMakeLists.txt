cmake_minimum_required(VERSION 4.5)

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
)

set(VEHICLE_MANAGER
    src/MaintenanceSchedule.cpp
    src/Vehicle.cpp
)

add_executable(Vehicle_Manager ${TEST_CASES} ${VEHICLE_MANAGER})

target_link_libraries(Vehicle_Manager PRIVATE gtest_main)

target_include_directories(Vehicle_Manager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()
include(GoogleTest)
gtest_discover_tests(Vehicle_Manager)