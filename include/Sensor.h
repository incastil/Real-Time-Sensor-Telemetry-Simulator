#pragma once

#include "SensorReading.h"
#include "TelemetryQueue.h"
#include <string>
#include <thread>
#include <atomic>
#include <random>
#include <chrono>

class Sensor {
public:
    Sensor(std::string name,
           std::string unit,
           std::chrono::milliseconds interval,
           double fault_probability = 0.05);

    virtual ~Sensor();

    // Start sensor thread — pushes readings into queue
    void start(TelemetryQueue& queue);
    void stop();

    const std::string& name() const { return name_; }

protected:
    // Subclasses implement reading generation and alarm classification
    virtual double      generateValue()         = 0;
    virtual AlarmState  classifyAlarm(double v) = 0;

    // Shared RNG for fault injection
    std::mt19937                    rng_;
    std::uniform_real_distribution<> fault_dist_{0.0, 1.0};

private:
    std::string                    name_;
    std::string                    unit_;
    std::chrono::milliseconds      interval_;
    double                         fault_probability_;
    std::thread                    thread_;
    std::atomic<bool>              running_{false};

    void run(TelemetryQueue& queue);
    std::string currentTimestamp() const;
    bool        shouldInjectFault();
};
