#pragma once

#include "TelemetryQueue.h"
#include "Logger.h"
#include <atomic>
#include <thread>

class TelemetryProcessor {
public:
    explicit TelemetryProcessor(Logger& logger);
    ~TelemetryProcessor();

    void start(TelemetryQueue& queue);
    void stop();

private:
    Logger&          logger_;
    std::thread      thread_;
    std::atomic<bool> running_{false};

    void run(TelemetryQueue& queue);
    void displayReading(const SensorReading& reading) const;

    // ANSI color helpers
    static const char* alarmColor(AlarmState state);
    static const char* RESET;
    static const char* BOLD;
    static const char* DIM;
};
