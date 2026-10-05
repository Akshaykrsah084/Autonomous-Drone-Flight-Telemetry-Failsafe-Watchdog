# Stage 6 – Final Implementation & Presentation

## Final Deliverables
- Complete C++ application.
- Linux kernel watchdog driver.
- Unit tests.
- README.
- PRD and architecture documentation.
- UML diagrams.
- Testing report.

## 5–10 Minute Presentation Flow

1. State the problem and objective.
2. Show the architecture diagram.
3. Run the normal telemetry scenario.
4. Run the warning scenario.
5. Run the failsafe scenario.
6. Explain the heartbeat and Linux watchdog interface.
7. Run the watchdog scenario and show timeout evidence.
8. State limitations and future scope.

## Limitations
- Telemetry is simulated.
- The driver models a hardware watchdog; it does not control physical drone hardware.
- The timeout recovery is logged/simulated instead of rebooting the host.

## Future Improvements
Possible future work could include physical sensor input, a real watchdog peripheral, richer telemetry sources, and integration with a flight controller.
