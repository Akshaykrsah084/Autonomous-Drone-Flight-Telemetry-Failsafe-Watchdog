#pragma once

#include "safety_monitor.h"

class FailsafeManager {
public:
    SafetyLevel transition(const SafetyAssessment& assessment);
    SafetyLevel state() const noexcept;
    void reset();

private:
    SafetyLevel current_{SafetyLevel::Normal};
};

