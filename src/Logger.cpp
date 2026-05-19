#include "Logger.h"
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>

Logger::Logger(const std::string& filepath)
    : file_(filepath, std::ios::app)
{
    if (!file_.is_open()) {
        throw std::runtime_error("Logger: cannot open file: " + filepath);
    }
    logMessage("=== Telemetry session started ===");
}

Logger::~Logger() {
    logMessage("=== Telemetry session ended ===");
}

void Logger::log(const SensorReading& reading) {
    std::lock_guard<std::mutex> lock(mutex_);
    file_ << reading.timestamp
          << " | " << std::setw(4) << reading.sensor_name
          << " | ";

    if (reading.is_fault) {
        file_ << std::setw(10) << "FAULT";
    } else {
        file_ << std::setw(10) << std::fixed << std::setprecision(3) << reading.value
              << " " << reading.unit;
    }

    file_ << " | " << alarmStateToString(reading.alarm_state) << "\n";
    file_.flush();
}

void Logger::logMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);

    using namespace std::chrono;
    auto now = system_clock::now();
    auto ms  = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm_info;
    localtime_r(&t, &tm_info);

    file_ << std::put_time(&tm_info, "%H:%M:%S")
          << '.' << std::setfill('0') << std::setw(3) << ms.count()
          << " | [SYS]  | " << message << "\n";
    file_.flush();
}
