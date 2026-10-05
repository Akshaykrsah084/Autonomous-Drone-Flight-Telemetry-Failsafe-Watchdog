# Stage 3 – System Design & Architecture

## 1. High-Level Architecture

```text
+-------------------------- USER SPACE --------------------------+
|                                                                |
|  TelemetryGenerator --> SafetyMonitor --> FailsafeManager      |
|          |                     |                   |            |
|          +---- raw data -------+---- safety state  |            |
|                                                   |            |
|                                             WatchdogClient      |
|                                                   |            |
+---------------------------------------------------|------------+
                                                    v
                                             /dev/watchdogN
                                                    |
+-------------------------- KERNEL SPACE -------------------------+
|                                                    |
|      Linux Watchdog Driver Core + simulated WDT + hrtimer       |
|                                                    |
|                 heartbeat received?                          |
|                    |             |                            |
|                   yes            no                           |
|                    |             |                            |
|                 restart      watchdog timeout                 |
|                               / recovery log                   |
+---------------------------------------------------------------+
```

## 2. Main Components

- **TelemetryGenerator:** creates raw roll, pitch, yaw and altitude values. Automatic mode generates changing random values. Controlled scenarios are only for repeatable testing.
- **SafetyMonitor:** checks project-defined attitude and altitude limits and creates a `SafetyAssessment`.
- **FailsafeManager:** stores the current safety state and applies the priority `NORMAL < WARNING < FAILSAFE`.
- **WatchdogClient:** opens the watchdog device and uses standard Linux watchdog ioctls for timeout, start/stop and heartbeat.
- **Kernel watchdog driver:** registers with the Linux Watchdog Driver Core. An `hrtimer` models the hardware countdown and reports a timeout when the heartbeat is missed.

## 3. User Space / Kernel Space Boundary

```text
C++ application (user space)
        |
   open / ioctl
        |
        v
   /dev/watchdogN
        |
        v
Linux watchdog driver (kernel space)
        |
      hrtimer
```

## 4. Data Flow

```text
Raw telemetry
   -> safety evaluation
   -> safety state
   -> heartbeat to watchdog
```

The watchdog is independent of the flight-state decision. It checks **software liveness**, not whether the simulated flight values are safe.
