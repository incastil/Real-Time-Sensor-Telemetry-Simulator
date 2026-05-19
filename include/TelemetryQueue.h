#pragma once

#include "SensorReading.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>

class TelemetryQueue {
public:
    void push(SensorReading reading);

    // Returns false if shutdown and queue empty (timed out wait)
    bool pop(SensorReading& out, std::chrono::milliseconds timeout = std::chrono::milliseconds(500));

    void shutdown();
    bool isShutdown() const;

private:
    std::queue<SensorReading>   queue_;
    mutable std::mutex          mutex_;
    std::condition_variable     cv_;
    bool                        shutdown_{false};
};
