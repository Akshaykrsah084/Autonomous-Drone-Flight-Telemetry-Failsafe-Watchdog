# Stage 2 – Project Requirements Document (PRD)

## Functional Requirements

| ID | Requirement |
|---|---|
| FR1 | Generate simulated roll, pitch, yaw and altitude telemetry. |
| FR2 | Format and display telemetry samples. |
| FR3 | Evaluate attitude against project-defined warning/critical thresholds. |
| FR4 | Evaluate altitude against project-defined warning/critical thresholds. |
| FR5 | Produce an overall NORMAL, WARNING or FAILSAFE assessment. |
| FR6 | Update a failsafe state machine from the assessment. |
| FR7 | Open and configure the Linux watchdog device. |
| FR8 | Send a heartbeat/keepalive periodically. |
| FR9 | Report watchdog status. |
| FR10 | Demonstrate watchdog timeout after heartbeat loss. |

## Non-Functional Requirements

- Linux-only execution for the capstone environment.
- C++17 for the application and C for the kernel driver.
- No external runtime libraries are required beyond the standard C/C++ and Linux interfaces.
- Modular source structure.
- Clear build and execution instructions.
- Reproducible test cases.
- Git-based version control.

## Deliverables
- C++ application source.
- Linux kernel module source.
- Makefiles.
- Unit tests for safety logic.
- README and six-stage documentation.
- Class, sequence and state-machine diagrams.
- Git history showing staged progress.
