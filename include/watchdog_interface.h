#pragma once

#include <cstdint>
#include <string>

#include "../driver/drone_watchdog_uapi.h"

class WatchdogClient {
public:
    explicit WatchdogClient(const std::string& device = "/dev/watchdog0");
    ~WatchdogClient();

    WatchdogClient(const WatchdogClient&) = delete;
    WatchdogClient& operator=(const WatchdogClient&) = delete;

    bool openDevice();
    bool setTimeout(std::uint32_t seconds);
    bool start();
    bool kick();
    bool stop();
    bool getStatus(drone_wdt_status& status) const;

private:
    int fd_{-1};
    std::string device_;
};
