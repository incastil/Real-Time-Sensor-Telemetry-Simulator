#include "TemperatureSensor.h"
#include <chrono>

TemperatureSensor::TemperatureSensor()
    : Sensor("TEMP", "°C", std::chrono::milliseconds(500))
{}

double TemperatureSensor::generateValue() {
    return dist_(rng_);
}

AlarmState TemperatureSensor::classifyAlarm(double v) {
    if (v >= 95.0) return AlarmState::CRITICAL;
    if (v >= 75.0) return AlarmState::WARNING;
    return AlarmState::NORMAL;
}
