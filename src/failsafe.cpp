#include "failsafe.h"

SafetyLevel FailsafeManager::transition(const SafetyAssessment& assessment)
{
    current_ = assessment.level;
    return current_;
}

SafetyLevel FailsafeManager::state() const noexcept
{
    return current_;
}

void FailsafeManager::reset()
{
    current_ = SafetyLevel::Normal;
}
