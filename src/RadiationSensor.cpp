#include "RadiationSensor.h"
#include <chrono>

RadiationSensor::RadiationSensor()
    : Sensor("RADS", "μSv/h", std::chrono::milliseconds(2000))
{}

double RadiationSensor::generateValue() {
    return dist_(rng_);
}

AlarmState RadiationSensor::classifyAlarm(double v) {
    if (v >= 10.0) return AlarmState::CRITICAL;
    if (v >=  5.0) return AlarmState::WARNING;
    return AlarmState::NORMAL;
}
