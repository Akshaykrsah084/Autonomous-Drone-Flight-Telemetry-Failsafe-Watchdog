#include "safety_monitor.h"
#include "project_config.h"

#include <sstream>

SafetyAssessment SafetyMonitor::evaluate(const Telemetry& t) const
{
    SafetyAssessment a;
    a.attitude_critical = (t.roll_deg > config::kCriticalRollDeg) ||
                          (t.pitch_deg > config::kCriticalPitchDeg);
    a.attitude_warning = (t.roll_deg > config::kWarningRollDeg) ||
                          (t.pitch_deg > config::kWarningPitchDeg);
    a.altitude_critical = (t.altitude_m <= config::kCriticalAltitudeM);
    a.altitude_warning = (t.altitude_m <= config::kWarningAltitudeM);

    std::ostringstream reason;

    if (a.attitude_critical || a.altitude_critical) {
        a.level = SafetyLevel::Failsafe;
        if (a.attitude_critical)
            reason << "critical attitude; ";
        if (a.altitude_critical)
            reason << "critical altitude; ";
    } else if (a.attitude_warning || a.altitude_warning) {
        a.level = SafetyLevel::Warning;
        if (a.attitude_warning)
            reason << "attitude warning; ";
        if (a.altitude_warning)
            reason << "altitude warning; ";
    } else {
        a.level = SafetyLevel::Normal;
        reason << "all monitored telemetry within project limits;";
    }

    a.reason = reason.str();
    return a;
}

std::string toString(SafetyLevel level)
{
    switch (level) {
    case SafetyLevel::Normal: return "NORMAL";
    case SafetyLevel::Warning: return "WARNING";
    case SafetyLevel::Failsafe: return "FAILSAFE";
    }
    return "UNKNOWN";
}
