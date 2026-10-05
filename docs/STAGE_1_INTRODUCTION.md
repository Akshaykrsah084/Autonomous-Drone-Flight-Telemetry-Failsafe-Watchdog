# Stage 1 – Project Introduction

## Project Title
Autonomous Drone Flight Telemetry & Failsafe Watchdog

## Problem Statement
Autonomous drone software needs continuous monitoring of flight telemetry and a separate mechanism to detect when the monitoring software itself becomes unresponsive. Without these protections, abnormal flight data or an application stall may go undetected.

## Objective
Build a Linux-based C++ safety-monitoring application that processes simulated drone attitude and altitude telemetry, determines the operational safety state, and periodically sends a heartbeat to a small Linux kernel watchdog driver.

## Scope
- Simulated telemetry only; no real drone hardware.
- Monitor roll, pitch, yaw and altitude.
- Generate changing raw telemetry automatically and use simple threshold-based safety evaluation.
- Use NORMAL, WARNING and FAILSAFE states.
- Implement a kernel-space watchdog module with a configurable timeout.
- Demonstrate watchdog timeout when heartbeats stop.

## Expected Outcome
A demonstrable Linux project showing C++, system programming, kernel/device-driver interaction, computer-architecture concepts around hardware supervision, testing and version-controlled documentation.

## Out of Scope
Real flight control, GPS navigation, computer vision, AI/ML, ROS, PID control, real sensors and real flight hardware.
