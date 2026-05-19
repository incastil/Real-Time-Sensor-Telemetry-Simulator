#include "TelemetryProcessor.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// ANSI escape codes
const char* TelemetryProcessor::RESET = "\033[0m";
const char* TelemetryProcessor::BOLD  = "\033[1m";
const char* TelemetryProcessor::DIM   = "\033[2m";

const char* TelemetryProcessor::alarmColor(AlarmState state) {
    switch (state) {
        case AlarmState::NORMAL:   return "\033[32m";   // green
        case AlarmState::WARNING:  return "\033[33m";   // yellow
        case AlarmState::CRITICAL: return "\033[31m";   // red
        case AlarmState::FAULT:    return "\033[35m";   // magenta
    }
    return "";
}

TelemetryProcessor::TelemetryProcessor(Logger& logger)
    : logger_(logger)
{}

TelemetryProcessor::~TelemetryProcessor() {
    stop();
}

void TelemetryProcessor::start(TelemetryQueue& queue) {
    running_ = true;
    thread_ = std::thread(&TelemetryProcessor::run, this, std::ref(queue));
}

void TelemetryProcessor::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void TelemetryProcessor::run(TelemetryQueue& queue) {
    // Print header
    std::cout << BOLD
              << "┌─────────────────────────────────────────────────────────────┐\n"
              << "│      REAL-TIME SENSOR TELEMETRY SIMULATOR  v1.0             │\n"
              << "│      Sensors: TEMP | PRES | VOLT | HRTB | RADS              │\n"
              << "└─────────────────────────────────────────────────────────────┘\n"
              << RESET;
    std::cout << DIM
              << std::setw(12) << "TIME"
              << " │ "
              << std::setw(4)  << "SNS"
              << " │ "
              << std::setw(12) << "VALUE"
              << " │ "
              << std::setw(8)  << "STATE"
              << "\n"
              << std::string(55, '-') << "\n"
              << RESET;

    SensorReading reading;
    while (running_ || !queue.isShutdown()) {
        if (!queue.pop(reading)) {
            continue;
        }
        displayReading(reading);
        logger_.log(reading);
    }
}

void TelemetryProcessor::displayReading(const SensorReading& reading) const {
    const char* color = alarmColor(reading.alarm_state);

    std::ostringstream value_str;
    if (reading.is_fault) {
        value_str << std::setw(8) << "FAULT" << "      ";
    } else {
        value_str << std::setw(8) << std::fixed << std::setprecision(3) << reading.value
                  << " " << std::setw(5) << std::left << reading.unit << std::right;
    }

    std::cout << color
              << std::setw(12) << reading.timestamp
              << " │ "
              << std::setw(4)  << reading.sensor_name
              << " │ "
              << value_str.str()
              << " │ "
              << BOLD << std::setw(8) << alarmStateToString(reading.alarm_state)
              << RESET << color
              << RESET << "\n";
}
