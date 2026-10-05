# Stage 4 – Initial Implementation & Prototype

## Prototype Modules
1. Telemetry model and generator.
2. Safety evaluator.
3. Failsafe state manager.
4. Kernel watchdog driver.
5. C++ watchdog client.
6. Logging and demonstration modes.

## Prototype Demonstrations

- `auto`: normal operating mode; raw telemetry changes automatically and the C++ safety monitor determines NORMAL/WARNING/FAILSAFE.
- `normal`: controlled healthy telemetry for repeatable testing.
- `warning`: controlled warning-level telemetry for repeatable testing.
- `failsafe`: controlled critical telemetry for repeatable testing.
- `watchdog`: intentional heartbeat stop followed by driver timeout.

## Development Evidence
Record screenshots and terminal output in Git commits at the end of each stage. Example evidence:

```text
$ ls driver
$ ls src
$ make app
$ make test
$ make driver KDIR=...
$ sudo insmod driver/drone_watchdog.ko
$ ls -l /dev/watchdogN
```
