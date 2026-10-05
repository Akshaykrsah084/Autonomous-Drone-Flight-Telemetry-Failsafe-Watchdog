#pragma once

#include <cstdint>

namespace config {
constexpr double kWarningRollDeg = 20.0;
constexpr double kCriticalRollDeg = 45.0;
constexpr double kWarningPitchDeg = 20.0;
constexpr double kCriticalPitchDeg = 45.0;
constexpr double kWarningAltitudeM = 20.0;
constexpr double kCriticalAltitudeM = 5.0;
constexpr unsigned kWatchdogTimeoutSec = 3;
constexpr unsigned kTelemetryPeriodMs = 1000;
}

