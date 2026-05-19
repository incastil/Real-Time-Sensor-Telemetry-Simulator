#include "TelemetryQueue.h"
#include "TelemetryProcessor.h"
#include "Logger.h"
#include "TemperatureSensor.h"
#include "PressureSensor.h"
#include "VoltageSensor.h"
#include "HeartbeatSensor.h"
#include "RadiationSensor.h"

#include <iostream>
#include <csignal>
#include <atomic>
#include <vector>
#include <memory>

// Global flag for clean SIGINT shutdown
static std::atomic<bool> g_running{true};

void handleSignal(int) {
    g_running = false;
}

int main(int argc, char* argv[]) {
    // Optional: duration argument in seconds (default = run until Ctrl+C)
    int duration_sec = 0;
    if (argc > 1) {
        try { duration_sec = std::stoi(argv[1]); } catch (...) {}
    }

    std::signal(SIGINT,  handleSignal);
    std::signal(SIGTERM, handleSignal);

    // Setup logger
    Logger logger("telemetry.log");

    // Setup shared queue
    TelemetryQueue queue;

    // Setup processor (reads from queue → terminal + log)
    TelemetryProcessor processor(logger);
    processor.start(queue);

    // Create and start all sensors
    std::vector<std::unique_ptr<Sensor>> sensors;
    sensors.push_back(std::make_unique<TemperatureSensor>());
    sensors.push_back(std::make_unique<PressureSensor>());
    sensors.push_back(std::make_unique<VoltageSensor>());
    sensors.push_back(std::make_unique<HeartbeatSensor>());
    sensors.push_back(std::make_unique<RadiationSensor>());

    for (auto& sensor : sensors) {
        sensor->start(queue);
    }

    logger.logMessage("All sensors started");
    std::cout << "\033[2m[Press Ctrl+C to stop]\033[0m\n\n";

    // Run until signal or duration expires
    if (duration_sec > 0) {
        auto end = std::chrono::steady_clock::now() + std::chrono::seconds(duration_sec);
        while (g_running && std::chrono::steady_clock::now() < end) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } else {
        while (g_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    // Graceful shutdown
    std::cout << "\n\033[2mShutting down...\033[0m\n";
    logger.logMessage("Shutdown signal received");

    for (auto& sensor : sensors) {
        sensor->stop();
    }

    queue.shutdown();
    processor.stop();

    std::cout << "\033[2mTelemetry log saved to: telemetry.log\033[0m\n";
    return 0;
}
