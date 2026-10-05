#include "failsafe.h"
#include "project_config.h"
#include "safety_monitor.h"
#include "telemetry.h"
#include "watchdog_interface.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

namespace {
struct Options {
    std::string scenario = "auto";
    unsigned cycles = 15;
    std::string device = "/dev/watchdog0";
};

void printUsage(const char* program)
{
    std::cout << "Usage: " << program << " [options]\n"
              << "  --scenario auto|normal|warning|failsafe|watchdog\n"
              << "  --cycles N\n"
              << "  --device PATH\n";
}

bool parseOptions(int argc, char** argv, Options& options)
{
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--scenario" && i + 1 < argc) {
            options.scenario = argv[++i];
        } else if (arg == "--cycles" && i + 1 < argc) {
            options.cycles = static_cast<unsigned>(std::strtoul(argv[++i], nullptr, 10));
            if (options.cycles == 0)
                options.cycles = 1;
        } else if (arg == "--device" && i + 1 < argc) {
            options.device = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return false;
        } else {
            std::cerr << "Unknown argument: " << arg << '\n';
            printUsage(argv[0]);
            return false;
        }
    }

    if (options.scenario != "auto" && options.scenario != "normal" &&
        options.scenario != "warning" && options.scenario != "failsafe" &&
        options.scenario != "watchdog") {
        std::cerr << "Invalid scenario: " << options.scenario << '\n';
        return false;
    }
    return true;
}

Telemetry scenarioTelemetry(TelemetryGenerator& generator,
                            const std::string& scenario,
                            std::uint64_t sequence)
{
    if (scenario == "auto")
        return generator.automatic(sequence);
    if (scenario == "warning" && sequence == 4)
        return generator.warning(sequence);
    if (scenario == "failsafe" && (sequence == 4 || sequence == 5))
        return generator.critical(sequence);
    return generator.normal(sequence);
}
}

int main(int argc, char** argv)
{
    Options options;
    if (!parseOptions(argc, argv, options))
        return 1;

    std::cout << "Autonomous Drone Flight Telemetry & Failsafe Watchdog\n";
    if (options.scenario == "auto")
        std::cout << "Mode: automatic raw telemetry\n";
    else
        std::cout << "Test scenario: " << options.scenario << "\n";
    std::cout << "Limits: roll/pitch warning >20 deg, critical >45 deg; "
                 "altitude warning <=20 m, critical <=5 m\n";

    WatchdogClient watchdog(options.device);
    if (!watchdog.openDevice()) {
        std::cerr << "Unable to open " << options.device
                  << ". Load the driver first.\n";
        return 2;
    }
    if (!watchdog.setTimeout(config::kWatchdogTimeoutSec) || !watchdog.start()) {
        std::cerr << "Unable to configure/start watchdog.\n";
        return 3;
    }

    TelemetryGenerator generator;
    SafetyMonitor monitor;
    FailsafeManager failsafe;

    for (unsigned i = 1; i <= options.cycles; ++i) {
        const Telemetry telemetry = scenarioTelemetry(generator, options.scenario, i);
        const SafetyAssessment assessment = monitor.evaluate(telemetry);
        const SafetyLevel state = failsafe.transition(assessment);

        std::cout << formatTelemetry(telemetry) << '\n';
        std::cout << "Safety state = " << toString(state)
                  << " (" << assessment.reason << ")\n";

        const bool simulate_heartbeat_loss =
            (options.scenario == "watchdog" && i == 4);

        if (!simulate_heartbeat_loss) {
            if (!watchdog.kick()) {
                std::cerr << "Heartbeat failed; watchdog is no longer running.\n";
                break;
            }
            std::cout << "Heartbeat sent to kernel watchdog\n";
        } else {
            std::cout << "Simulating monitoring application stall: heartbeat intentionally stopped\n";
            std::this_thread::sleep_for(
                std::chrono::seconds(config::kWatchdogTimeoutSec + 1));

            drone_wdt_status status{};
            if (watchdog.getStatus(status)) {
                std::cout << "Watchdog status: running=" << status.running
                          << ", expired=" << status.expired
                          << ", timeouts=" << status.timeout_count << '\n';
            }
            break;
        }

        if (state == SafetyLevel::Failsafe)
            std::cout << "FAILSAFE ACTION: safety response requested\n";

        std::this_thread::sleep_for(
            std::chrono::milliseconds(config::kTelemetryPeriodMs));
    }

    drone_wdt_status status{};
    if (watchdog.getStatus(status)) {
        std::cout << "Final watchdog status: kicks=" << status.kick_count
                  << ", timeouts=" << status.timeout_count
                  << ", running=" << status.running << '\n';
    }
    watchdog.stop();

    std::cout << "Demo complete\n";
    return 0;
}
