#include "PressureSensor.h"
#include <chrono>

PressureSensor::PressureSensor()
    : Sensor("PRES", "psi", std::chrono::milliseconds(750))
{}

double PressureSensor::generateValue() {
    return dist_(rng_);
}

AlarmState PressureSensor::classifyAlarm(double v) {
    if (v < 12.0 || v > 17.0) return AlarmState::CRITICAL;
    if (v < 13.5 || v > 15.5) return AlarmState::WARNING;
    return AlarmState::NORMAL;
}
