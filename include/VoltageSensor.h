#pragma once
#include "Sensor.h"

// Simulates spacecraft bus voltage sensor
// Normal: 27–33 V | Warning: 25–27 or 33–35 V | Critical: outside 25–35 V
class VoltageSensor : public Sensor {
public:
    VoltageSensor();

protected:
    double     generateValue()         override;
    AlarmState classifyAlarm(double v) override;

private:
    std::normal_distribution<> dist_{29.5, 1.8};
};
