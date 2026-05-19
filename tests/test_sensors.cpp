#include <gtest/gtest.h>
#include "SensorReading.h"
#include "TelemetryQueue.h"
#include "TemperatureSensor.h"
#include "PressureSensor.h"
#include "VoltageSensor.h"
#include "HeartbeatSensor.h"
#include "RadiationSensor.h"

// ─── AlarmState string conversion ────────────────────────────────────────────

TEST(AlarmState, StringConversion) {
    EXPECT_EQ(alarmStateToString(AlarmState::NORMAL),   "NORMAL");
    EXPECT_EQ(alarmStateToString(AlarmState::WARNING),  "WARNING");
    EXPECT_EQ(alarmStateToString(AlarmState::CRITICAL), "CRITICAL");
    EXPECT_EQ(alarmStateToString(AlarmState::FAULT),    "FAULT");
}

// ─── TelemetryQueue ───────────────────────────────────────────────────────────

TEST(TelemetryQueue, PushAndPop) {
    TelemetryQueue q;
    SensorReading r;
    r.sensor_name = "TEST";
    r.value       = 42.0;
    q.push(r);

    SensorReading out;
    EXPECT_TRUE(q.pop(out, std::chrono::milliseconds(100)));
    EXPECT_EQ(out.sensor_name, "TEST");
    EXPECT_DOUBLE_EQ(out.value, 42.0);
}

TEST(TelemetryQueue, PopTimesOutWhenEmpty) {
    TelemetryQueue q;
    SensorReading out;
    EXPECT_FALSE(q.pop(out, std::chrono::milliseconds(50)));
}

TEST(TelemetryQueue, ShutdownUnblocksWaiters) {
    TelemetryQueue q;
    q.shutdown();
    EXPECT_TRUE(q.isShutdown());

    SensorReading out;
    EXPECT_FALSE(q.pop(out, std::chrono::milliseconds(50)));
}

TEST(TelemetryQueue, MultipleReadings) {
    TelemetryQueue q;
    for (int i = 0; i < 5; ++i) {
        SensorReading r;
        r.value = static_cast<double>(i);
        q.push(r);
    }
    for (int i = 0; i < 5; ++i) {
        SensorReading out;
        ASSERT_TRUE(q.pop(out, std::chrono::milliseconds(100)));
        EXPECT_DOUBLE_EQ(out.value, static_cast<double>(i));
    }
}

// ─── Temperature alarm thresholds ────────────────────────────────────────────
// We test classifyAlarm indirectly via a thin wrapper; the protected method
// is tested by subclassing in the test.

class TemperatureAlarmTest : public TemperatureSensor {
public:
    AlarmState testAlarm(double v) { return classifyAlarm(v); }
};

TEST(TemperatureSensor, AlarmBoundaries) {
    TemperatureAlarmTest s;
    EXPECT_EQ(s.testAlarm(50.0),  AlarmState::NORMAL);
    EXPECT_EQ(s.testAlarm(75.0),  AlarmState::WARNING);
    EXPECT_EQ(s.testAlarm(94.9),  AlarmState::WARNING);
    EXPECT_EQ(s.testAlarm(95.0),  AlarmState::CRITICAL);
    EXPECT_EQ(s.testAlarm(120.0), AlarmState::CRITICAL);
}

// ─── Pressure alarm thresholds ───────────────────────────────────────────────

class PressureAlarmTest : public PressureSensor {
public:
    AlarmState testAlarm(double v) { return classifyAlarm(v); }
};

TEST(PressureSensor, AlarmBoundaries) {
    PressureAlarmTest s;
    EXPECT_EQ(s.testAlarm(14.7),  AlarmState::NORMAL);
    EXPECT_EQ(s.testAlarm(13.0),  AlarmState::WARNING);
    EXPECT_EQ(s.testAlarm(16.0),  AlarmState::WARNING);
    EXPECT_EQ(s.testAlarm(11.0),  AlarmState::CRITICAL);
    EXPECT_EQ(s.testAlarm(18.0),  AlarmState::CRITICAL);
}

// ─── Voltage alarm thresholds ────────────────────────────────────────────────

class VoltageAlarmTest : public VoltageSensor {
public:
    AlarmState testAlarm(double v) { return classifyAlarm(v); }
};

TEST(VoltageSensor, AlarmBoundaries) {
    VoltageAlarmTest s;
    EXPECT_EQ(s.testAlarm(29.5),  AlarmState::NORMAL);
    EXPECT_EQ(s.testAlarm(26.0),  AlarmState::WARNING);
    EXPECT_EQ(s.testAlarm(34.0),  AlarmState::WARNING);
    EXPECT_EQ(s.testAlarm(24.0),  AlarmState::CRITICAL);
    EXPECT_EQ(s.testAlarm(36.0),  AlarmState::CRITICAL);
}

// ─── Heartbeat alarm thresholds ──────────────────────────────────────────────

class HeartbeatAlarmTest : public HeartbeatSensor {
public:
    AlarmState testAlarm(double v) { return classifyAlarm(v); }
};

TEST(HeartbeatSensor, AlarmBoundaries) {
    HeartbeatAlarmTest s;
    EXPECT_EQ(s.testAlarm(72.0),  AlarmState::NORMAL);
    EXPECT_EQ(s.testAlarm(55.0),  AlarmState::WARNING);
    EXPECT_EQ(s.testAlarm(105.0), AlarmState::WARNING);
    EXPECT_EQ(s.testAlarm(40.0),  AlarmState::CRITICAL);
    EXPECT_EQ(s.testAlarm(120.0), AlarmState::CRITICAL);
}

// ─── Radiation alarm thresholds ──────────────────────────────────────────────

class RadiationAlarmTest : public RadiationSensor {
public:
    AlarmState testAlarm(double v) { return classifyAlarm(v); }
};

TEST(RadiationSensor, AlarmBoundaries) {
    RadiationAlarmTest s;
    EXPECT_EQ(s.testAlarm(2.0),   AlarmState::NORMAL);
    EXPECT_EQ(s.testAlarm(7.0),   AlarmState::WARNING);
    EXPECT_EQ(s.testAlarm(10.0),  AlarmState::CRITICAL);
    EXPECT_EQ(s.testAlarm(50.0),  AlarmState::CRITICAL);
}

// ─── Sensor produces readings into queue ─────────────────────────────────────

TEST(SensorIntegration, TemperatureProducesReadings) {
    TelemetryQueue q;
    TemperatureSensor sensor;
    sensor.start(q);

    SensorReading reading;
    bool got = q.pop(reading, std::chrono::milliseconds(1000));
    sensor.stop();

    EXPECT_TRUE(got);
    EXPECT_EQ(reading.sensor_name, "TEMP");
    EXPECT_EQ(reading.unit, "°C");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
