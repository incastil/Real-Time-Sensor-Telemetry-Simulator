#pragma once
#include "Sensor.h"

// Simulates cabin/module pressure sensor
// Normal: 13.5–15.5 psi | Warning: 12–13.5 or 15.5–17 psi | Critical: outside 12–17 psi
class PressureSensor : public Sensor {
public:
    PressureSensor();

protected:
    double     generateValue()         override;
    AlarmState classifyAlarm(double v) override;

private:
    std::normal_distribution<> dist_{14.7, 1.2};
};
