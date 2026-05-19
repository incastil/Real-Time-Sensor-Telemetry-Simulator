#pragma once

#include "SensorReading.h"
#include <fstream>
#include <mutex>
#include <string>

class Logger {
public:
    explicit Logger(const std::string& filepath);
    ~Logger();

    void log(const SensorReading& reading);
    void logMessage(const std::string& message);

private:
    std::ofstream file_;
    std::mutex    mutex_;
};
