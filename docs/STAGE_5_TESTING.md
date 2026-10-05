# Stage 5 – Testing, Integration & Improvement

## Test Matrix

| Test ID | Scenario | Expected Result |
|---|---|---|
| T01 | Automatic healthy telemetry | NORMAL |
| T02 | Automatic warning-level telemetry | WARNING |
| T03 | Automatic critical telemetry | FAILSAFE |
| T04 | Altitude below warning threshold | WARNING |
| T05 | Altitude below critical threshold | FAILSAFE |
| T06 | Combined abnormal telemetry | FAILSAFE |
| T07 | Heartbeat continues | Watchdog remains running |
| T08 | Heartbeat stopped | Watchdog expires |

## Unit Testing
The `tests/test_logic.cpp` program validates normal, warning and critical safety transitions using controlled generator outputs. Automatic mode is intentionally random and is used as an integration/demo mode rather than a deterministic unit test.

## Integration Testing
The C++ application is tested against the `/dev/watchdogN` kernel device to verify start, kick, status and timeout behavior.

## Reliability / Quality Improvements
- Deterministic test telemetry.
- Bounded watchdog timeout configuration.
- Clear user/kernel interface through ioctl commands.
- No actual machine reboot; timeout recovery is simulated for safe demonstration.
- Explicit error handling when the device cannot be opened.
