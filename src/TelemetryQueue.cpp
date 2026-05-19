#include "TelemetryQueue.h"

void TelemetryQueue::push(SensorReading reading) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(reading));
    }
    cv_.notify_one();
}

bool TelemetryQueue::pop(SensorReading& out, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    bool got_item = cv_.wait_for(lock, timeout, [this] {
        return !queue_.empty() || shutdown_;
    });

    if (!got_item || (queue_.empty() && shutdown_)) {
        return false;
    }

    out = std::move(queue_.front());
    queue_.pop();
    return true;
}

void TelemetryQueue::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdown_ = true;
    }
    cv_.notify_all();
}

bool TelemetryQueue::isShutdown() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return shutdown_;
}
