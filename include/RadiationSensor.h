#pragma once
#include "Sensor.h"

// Simulates radiation dose-rate sensor
// Normal: < 5 μSv/h | Warning: 5–10 μSv/h | Critical: ≥ 10 μSv/h
class RadiationSensor : public Sensor {
public:
    RadiationSensor();

protected:
    double     generateValue()         override;
    AlarmState classifyAlarm(double v) override;

private:
    std::exponential_distribution<> dist_{0.4};  // mean ≈ 2.5 μSv/h
};
