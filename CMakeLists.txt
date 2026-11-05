cmake_minimum_required(VERSION 3.16)

project(Project3
    	DESCRIPTION "Project 3: Vehicle Fleet Maintenance Management System"
    	LANGUAGES CXX)

add_library(maintenance_schedule src/MaintenanceSchedule.cpp)
target_include_directories(maintenance_schedule PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(maintenance_schedule PUBLIC cxx_std_11)
set_target_properties(maintenance_schedule PROPERTIES CXX_EXTENSIONS OFF)

include(CTest)
if(BUILD_TESTING)
    add_executable(maintenance_schedule_tests tests/MaintenanceScheduleTests.cpp)
    target_link_libraries(maintenance_schedule_tests PRIVATE maintenance_schedule)
    add_test(NAME MaintenanceScheduleTests COMMAND maintenance_schedule_tests)
endif()
