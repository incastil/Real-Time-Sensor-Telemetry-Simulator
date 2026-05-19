#pragma once
#include "Sensor.h"

// Simulates crew biometric heartbeat monitor
// Normal: 60–100 bpm | Warning: 50–60 or 100–110 bpm | Critical: outside 50–110 bpm
class HeartbeatSensor : public Sensor {
public:
    HeartbeatSensor();

protected:
    double     generateValue()         override;
    AlarmState classifyAlarm(double v) override;

private:
    std::normal_distribution<> dist_{72.0, 12.0};
};
