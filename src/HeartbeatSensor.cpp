#include "HeartbeatSensor.h"
#include <chrono>

HeartbeatSensor::HeartbeatSensor()
    : Sensor("HRTB", "bpm", std::chrono::milliseconds(1000))
{}

double HeartbeatSensor::generateValue() {
    return dist_(rng_);
}

AlarmState HeartbeatSensor::classifyAlarm(double v) {
    if (v < 50.0 || v > 110.0) return AlarmState::CRITICAL;
    if (v < 60.0 || v > 100.0) return AlarmState::WARNING;
    return AlarmState::NORMAL;
}
