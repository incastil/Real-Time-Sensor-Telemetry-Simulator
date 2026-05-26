# Real-Time Sensor Telemetry Simulator

[![CI](https://github.com/incastil/Real-Time-Sensor-Telemetry-Simulator/actions/workflows/ci.yml/badge.svg)](https://github.com/incastil/Real-Time-Sensor-Telemetry-Simulator/actions/workflows/ci.yml)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/cmake-3.16+-064F8C?logo=cmake&logoColor=white)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey)](https://github.com/incastil/Real-Time-Sensor-Telemetry-Simulator)
[![License: MIT](https://img.shields.io/badge/license-MIT-green)](LICENSE)

A multithreaded C++17 application that simulates spacecraft sensors, streams live telemetry to a color-coded terminal dashboard, and logs all events to a file. Built to demonstrate systems-level engineering skills relevant to simulation and spacecraft-support work (METECS portfolio project).

---

## Features

| Feature | Details |
|---|---|
| **5 sensors** | Temperature, Pressure, Voltage, Heartbeat, Radiation |
| **Multithreaded** | One `std::thread` per sensor, independent publish rates |
| **Shared queue** | Thread-safe `TelemetryQueue` using `mutex` + `condition_variable` |
| **Fault injection** | ~5% random faults per sensor (missing/invalid data) |
| **ANSI dashboard** | Color-coded alarm states: green=NORMAL, yellow=WARNING, red=CRITICAL, magenta=FAULT |
| **File logging** | All readings appended to `telemetry.log` with timestamps |
| **Unit tests** | GoogleTest coverage for alarm thresholds and queue behavior |
| **CMake build** | Clean CMake project with static library + test target |

---

## Architecture

```mermaid
flowchart TD
    T["TemperatureSensor\nthread @500ms"]
    P["PressureSensor\nthread @750ms"]
    V["VoltageSensor\nthread @250ms"]
    H["HeartbeatSensor\nthread @1000ms"]
    R["RadiationSensor\nthread @2000ms"]

    Q["TelemetryQueue\nmutex + condition_variable"]
    PROC["TelemetryProcessor\nreader thread"]
    DASH["Terminal ANSI\nDashboard"]
    LOG["Logger\ntelemetry.log"]

    T --> Q
    P --> Q
    V --> Q
    H --> Q
    R --> Q
    Q --> PROC
    PROC --> DASH
    PROC --> LOG
```

---

## Sensors & Alarm Thresholds

| Sensor | Unit | Interval | NORMAL | WARNING | CRITICAL |
|---|---|---|---|---|---|
| TEMP (Temperature) | °C | 500 ms | < 75 | 75–95 | ≥ 95 |
| PRES (Pressure) | psi | 750 ms | 13.5–15.5 | 12–13.5 / 15.5–17 | < 12 or > 17 |
| VOLT (Voltage) | V | 250 ms | 27–33 | 25–27 / 33–35 | < 25 or > 35 |
| HRTB (Heartbeat) | bpm | 1000 ms | 60–100 | 50–60 / 100–110 | < 50 or > 110 |
| RADS (Radiation) | μSv/h | 2000 ms | < 5 | 5–10 | ≥ 10 |

---

## Build

### Prerequisites

- GCC ≥ 9 or Clang ≥ 10 (C++17 support)
- CMake ≥ 3.16
- Linux (or macOS for local dev)
- Optional: GoogleTest — auto-installed by `build_run.sh` if missing

### Quick start

```bash
# Clone and enter the project
git clone https://github.com/incastil/Real-Time-Sensor-Telemetry-Simulator.git
cd Real-Time-Sensor-Telemetry-Simulator

# Build only
./scripts/build_run.sh

# Build and run (Ctrl+C to stop)
./scripts/build_run.sh --run

# Build, test, and run for 30 seconds
./scripts/build_run.sh --test --run 30

# Clean rebuild
./scripts/build_run.sh --clean --run
```

### Manual CMake

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/telemetry_sim          # run until Ctrl+C
./build/telemetry_sim 30       # run for 30 seconds
./build/run_tests              # unit tests (if GTest installed)
```

---

## Demo

![Terminal demo](docs/demo.svg)

---

## Project Structure

```
Real-Time-Sensor-Telemetry-Simulator/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── SensorReading.h        # Data structs + AlarmState enum
│   ├── TelemetryQueue.h       # Thread-safe queue interface
│   ├── Sensor.h               # Abstract sensor base class
│   ├── TemperatureSensor.h
│   ├── PressureSensor.h
│   ├── VoltageSensor.h
│   ├── HeartbeatSensor.h
│   ├── RadiationSensor.h
│   ├── TelemetryProcessor.h   # Queue consumer + terminal display
│   └── Logger.h               # File logger
├── src/
│   ├── main.cpp
│   ├── Sensor.cpp
│   ├── TelemetryQueue.cpp
│   ├── Logger.cpp
│   ├── TelemetryProcessor.cpp
│   ├── TemperatureSensor.cpp
│   ├── PressureSensor.cpp
│   ├── VoltageSensor.cpp
│   ├── HeartbeatSensor.cpp
│   └── RadiationSensor.cpp
├── tests/
│   └── test_sensors.cpp       # GoogleTest unit tests
└── scripts/
    └── build_run.sh           # Build/run/test automation
```

---

## Systems Engineering Concepts Demonstrated

- **Concurrent producer/consumer architecture** — independent sensor threads push to a shared queue; a dedicated consumer thread drains it without polling
- **Thread synchronization and race-condition prevention** — `std::mutex` + `std::condition_variable` guard every queue operation; `std::atomic<bool>` controls clean shutdown across threads
- **Real-time telemetry processing** — readings flow from sensor threads through the queue to the terminal dashboard with sub-second latency
- **Fault-state simulation and observability** — 5% randomized fault injection per sensor models missing/invalid data; fault sentinel values are classified, displayed, and logged distinctly from alarm states
- **Structured logging and operational diagnostics** — all readings written to `telemetry.log` with millisecond timestamps; session start/end boundaries enable post-run analysis
- **Automated unit testing and modular builds** — GoogleTest covers alarm thresholds and queue behavior; CMake builds a static `sensor_lib` linked by both the main executable and the test binary

---

## Why this project

Demonstrates spacecraft-support engineering skills:
- **Concurrent systems**: independent sensor threads, race-condition-free queue
- **Real-time data processing**: producer/consumer pattern with condition variables
- **Fault simulation**: randomized fault injection for testing alarm logic
- **Observability**: structured logging for post-run analysis
- **Build hygiene**: CMake static library, separate test target, CI-ready
