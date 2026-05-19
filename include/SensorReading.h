#pragma once

#include <string>
#include <chrono>

enum class AlarmState {
    NORMAL,
    WARNING,
    CRITICAL,
    FAULT
};

inline std::string alarmStateToString(AlarmState state) {
    switch (state) {
        case AlarmState::NORMAL:   return "NORMAL";
        case AlarmState::WARNING:  return "WARNING";
        case AlarmState::CRITICAL: return "CRITICAL";
        case AlarmState::FAULT:    return "FAULT";
    }
    return "UNKNOWN";
}

struct SensorReading {
    std::string           timestamp;
    std::string           sensor_name;
    double                value;
    std::string           unit;
    AlarmState            alarm_state;
    bool                  is_fault{false};
};
