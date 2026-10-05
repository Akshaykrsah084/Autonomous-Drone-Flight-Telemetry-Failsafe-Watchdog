#include "failsafe.h"
#include "safety_monitor.h"
#include "telemetry.h"

#include <cassert>
#include <iostream>

int main()
{
    TelemetryGenerator generator;
    SafetyMonitor monitor;
    FailsafeManager manager;

    const auto normal = monitor.evaluate(generator.normal(1));
    assert(normal.level == SafetyLevel::Normal);
    assert(manager.transition(normal) == SafetyLevel::Normal);

    const auto warning = monitor.evaluate(generator.warning(2));
    assert(warning.level == SafetyLevel::Warning);
    assert(manager.transition(warning) == SafetyLevel::Warning);

    const auto critical = monitor.evaluate(generator.critical(3));
    assert(critical.level == SafetyLevel::Failsafe);
    assert(manager.transition(critical) == SafetyLevel::Failsafe);

    std::cout << "All safety-logic tests passed.\n";
    return 0;
}
