#pragma once

#include "telemetry.h"

#include <string>

enum class SafetyLevel {
    Normal,
    Warning,
    Failsafe
};

struct SafetyAssessment {
    SafetyLevel level{SafetyLevel::Normal};
    bool attitude_warning{false};
    bool attitude_critical{false};
    bool altitude_warning{false};
    bool altitude_critical{false};
    std::string reason;
};

class SafetyMonitor {
public:
    SafetyAssessment evaluate(const Telemetry& telemetry) const;
};

std::string toString(SafetyLevel level);

