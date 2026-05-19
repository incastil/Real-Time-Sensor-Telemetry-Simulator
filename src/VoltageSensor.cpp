#include "VoltageSensor.h"
#include <chrono>

VoltageSensor::VoltageSensor()
    : Sensor("VOLT", "V", std::chrono::milliseconds(250))
{}

double VoltageSensor::generateValue() {
    return dist_(rng_);
}

AlarmState VoltageSensor::classifyAlarm(double v) {
    if (v < 25.0 || v > 35.0) return AlarmState::CRITICAL;
    if (v < 27.0 || v > 33.0) return AlarmState::WARNING;
    return AlarmState::NORMAL;
}
