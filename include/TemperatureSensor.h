#pragma once
#include "Sensor.h"

// Simulates spacecraft thermal sensor
// Normal: 20–75 °C | Warning: 75–95 °C | Critical: ≥ 95 °C
class TemperatureSensor : public Sensor {
public:
    TemperatureSensor();

protected:
    double     generateValue()         override;
    AlarmState classifyAlarm(double v) override;

private:
    std::normal_distribution<> dist_{55.0, 18.0};
};
