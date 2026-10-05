#pragma once

#include <cstdint>
#include <random>
#include <string>

struct Telemetry {
    std::uint64_t sequence{};
    double roll_deg{};
    double pitch_deg{};
    double yaw_deg{};
    double altitude_m{};
};

std::string formatTelemetry(const Telemetry& telemetry);

class TelemetryGenerator {
public:
    // A seed of 0 uses a non-deterministic seed. Supplying a non-zero seed
    // makes automatic telemetry repeatable for demonstrations/tests.
    explicit TelemetryGenerator(std::uint32_t seed = 0);

    Telemetry automatic(std::uint64_t sequence);
    Telemetry normal(std::uint64_t sequence) const;
    Telemetry warning(std::uint64_t sequence) const;
    Telemetry critical(std::uint64_t sequence) const;

private:
    std::uint32_t seed_{};
    std::mt19937 rng_;
};
