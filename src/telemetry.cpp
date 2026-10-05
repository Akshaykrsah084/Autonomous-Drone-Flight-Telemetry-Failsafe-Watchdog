#include "telemetry.h"

#include <chrono>
#include <iomanip>
#include <sstream>

namespace {
double randomDouble(std::mt19937& rng, double min_value, double max_value)
{
    std::uniform_real_distribution<double> distribution(min_value, max_value);
    return distribution(rng);
}
}

std::string formatTelemetry(const Telemetry& telemetry)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(1)
        << "seq=" << telemetry.sequence
        << " roll=" << telemetry.roll_deg << " deg"
        << " pitch=" << telemetry.pitch_deg << " deg"
        << " yaw=" << telemetry.yaw_deg << " deg"
        << " altitude=" << telemetry.altitude_m << " m";
    return out.str();
}

TelemetryGenerator::TelemetryGenerator(std::uint32_t seed)
    : seed_(seed)
{
    if (seed_ == 0U) {
        std::random_device rd;
        const auto now = static_cast<std::uint32_t>(
            std::chrono::steady_clock::now().time_since_epoch().count());
        seed_ = rd() ^ now;
    }
    rng_.seed(seed_);
}

Telemetry TelemetryGenerator::normal(std::uint64_t sequence) const
{
    const double small_offset = static_cast<double>((sequence + seed_) % 3U);
    return Telemetry{sequence, 5.0 + small_offset, 3.0, 90.0 + small_offset, 100.0 - small_offset};
}

Telemetry TelemetryGenerator::warning(std::uint64_t sequence) const
{
    Telemetry t = normal(sequence);
    t.roll_deg = 28.0;
    t.altitude_m = 15.0;
    return t;
}

Telemetry TelemetryGenerator::critical(std::uint64_t sequence) const
{
    Telemetry t = normal(sequence);
    t.roll_deg = 55.0;
    t.pitch_deg = 10.0;
    t.altitude_m = 3.0;
    return t;
}

Telemetry TelemetryGenerator::automatic(std::uint64_t sequence)
{
    // The simulator generates raw telemetry only. It never sends a state
    // such as NORMAL/WARNING/FAILSAFE. The C++ safety monitor decides that.
    // The distribution simply makes healthy, abnormal and critical raw data
    // appear with enough variety for a short live demonstration.
    std::uniform_int_distribution<int> condition(0, 99);
    const int bucket = condition(rng_);

    Telemetry t{};
    t.sequence = sequence;
    t.yaw_deg = randomDouble(rng_, 0.0, 360.0);

    if (bucket < 72) {
        // Healthy flight data.
        t.roll_deg = randomDouble(rng_, 0.0, 15.0);
        t.pitch_deg = randomDouble(rng_, 0.0, 15.0);
        t.altitude_m = randomDouble(rng_, 30.0, 120.0);
    } else if (bucket < 92) {
        // Abnormal but non-critical data: either attitude or altitude warning.
        const bool attitude_warning = (bucket % 2) == 0;
        if (attitude_warning) {
            t.roll_deg = randomDouble(rng_, 21.0, 35.0);
            t.pitch_deg = randomDouble(rng_, 0.0, 15.0);
            t.altitude_m = randomDouble(rng_, 40.0, 120.0);
        } else {
            t.roll_deg = randomDouble(rng_, 0.0, 15.0);
            t.pitch_deg = randomDouble(rng_, 0.0, 15.0);
            t.altitude_m = randomDouble(rng_, 10.0, 19.0);
        }
    } else {
        // Critical raw data: high attitude and/or very low altitude.
        const bool critical_attitude = (bucket % 2) == 0;
        if (critical_attitude) {
            t.roll_deg = randomDouble(rng_, 46.0, 65.0);
            t.pitch_deg = randomDouble(rng_, 0.0, 20.0);
            t.altitude_m = randomDouble(rng_, 30.0, 100.0);
        } else {
            t.roll_deg = randomDouble(rng_, 0.0, 20.0);
            t.pitch_deg = randomDouble(rng_, 0.0, 20.0);
            t.altitude_m = randomDouble(rng_, 1.0, 5.0);
        }
    }

    return t;
}
