cmake_minimum_required(VERSION 3.16)
project(SensorTelemetrySimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ─── Library of shared sources ───────────────────────────────────────────────
set(SENSOR_SOURCES
    src/Sensor.cpp
    src/TelemetryQueue.cpp
    src/Logger.cpp
    src/TelemetryProcessor.cpp
    src/TemperatureSensor.cpp
    src/PressureSensor.cpp
    src/VoltageSensor.cpp
    src/HeartbeatSensor.cpp
    src/RadiationSensor.cpp
)

add_library(sensor_lib STATIC ${SENSOR_SOURCES})
target_include_directories(sensor_lib PUBLIC include)
target_link_libraries(sensor_lib PUBLIC pthread)

# ─── Main executable ─────────────────────────────────────────────────────────
add_executable(telemetry_sim src/main.cpp)
target_link_libraries(telemetry_sim PRIVATE sensor_lib)

# ─── Tests ───────────────────────────────────────────────────────────────────
enable_testing()

# Try to find GoogleTest; gracefully skip if not installed
find_package(GTest QUIET)

if(GTest_FOUND)
    add_executable(run_tests tests/test_sensors.cpp)
    target_link_libraries(run_tests PRIVATE sensor_lib GTest::GTest GTest::Main)
    add_test(NAME SensorTests COMMAND run_tests)
    message(STATUS "GoogleTest found — tests enabled")
else()
    message(WARNING "GoogleTest not found — skipping tests. Install with: sudo apt install libgtest-dev (Linux) or brew install googletest (macOS)")
endif()

# ─── Install ─────────────────────────────────────────────────────────────────
install(TARGETS telemetry_sim DESTINATION bin)
