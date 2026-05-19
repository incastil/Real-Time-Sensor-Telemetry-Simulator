#include "Sensor.h"
#include <sstream>
#include <iomanip>
#include <ctime>

Sensor::Sensor(std::string name,
               std::string unit,
               std::chrono::milliseconds interval,
               double fault_probability)
    : rng_(std::random_device{}())
    , name_(std::move(name))
    , unit_(std::move(unit))
    , interval_(interval)
    , fault_probability_(fault_probability)
{}

Sensor::~Sensor() {
    stop();
}

void Sensor::start(TelemetryQueue& queue) {
    running_ = true;
    thread_ = std::thread(&Sensor::run, this, std::ref(queue));
}

void Sensor::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void Sensor::run(TelemetryQueue& queue) {
    while (running_) {
        SensorReading reading;
        reading.sensor_name = name_;
        reading.unit        = unit_;
        reading.timestamp   = currentTimestamp();

        if (shouldInjectFault()) {
            reading.value       = -999.0;
            reading.alarm_state = AlarmState::FAULT;
            reading.is_fault    = true;
        } else {
            reading.value       = generateValue();
            reading.alarm_state = classifyAlarm(reading.value);
            reading.is_fault    = false;
        }

        queue.push(std::move(reading));
        std::this_thread::sleep_for(interval_);
    }
}

std::string Sensor::currentTimestamp() const {
    using namespace std::chrono;
    auto now     = system_clock::now();
    auto ms      = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm_info;
    localtime_r(&t, &tm_info);

    std::ostringstream oss;
    oss << std::put_time(&tm_info, "%H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

bool Sensor::shouldInjectFault() {
    return fault_dist_(rng_) < fault_probability_;
}
