# UML / Component Relationships

The project is intentionally modeled with a small number of components.

## Class Relationships

```text
TelemetryGenerator
        |
        v
    Telemetry
        |
        v
  SafetyMonitor
        |
        v
SafetyAssessment
        |
        v
 FailsafeManager
        |
        v
 SafetyLevel

WatchdogClient ---> /dev/watchdogN
```

## Sequence

```text
TelemetryGenerator -> Main: generate raw telemetry
Main -> SafetyMonitor: evaluate(telemetry)
SafetyMonitor -> Main: SafetyAssessment
Main -> FailsafeManager: transition(assessment)
FailsafeManager -> Main: NORMAL/WARNING/FAILSAFE
Main -> WatchdogClient: kick()
WatchdogClient -> /dev/watchdogN: WDIOC_KEEPALIVE
/dev/watchdogN -> Linux watchdog driver: heartbeat
```

## State Flow

```text
        +---------+
        | NORMAL  |
        +----+----+
             |
       warning values
             v
       +-----------+
       |  WARNING  |
       +-----+-----+
             |
       critical values
             v
       +-----------+
       | FAILSAFE  |
       +-----------+
```

A critical condition moves directly to `FAILSAFE`, so `FAILSAFE` takes priority over `WARNING`.
