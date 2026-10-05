#include "watchdog_interface.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <linux/watchdog.h>

WatchdogClient::WatchdogClient(const std::string& device) : device_(device) {}

WatchdogClient::~WatchdogClient()
{
    if (fd_ >= 0) {
        (void)stop();
        close(fd_);
    }
}

bool WatchdogClient::openDevice()
{
    if (fd_ >= 0)
        return true;

    fd_ = open(device_.c_str(), O_RDWR | O_CLOEXEC);
    if (fd_ < 0) {
        return false;
    }
    return true;
}

bool WatchdogClient::setTimeout(std::uint32_t seconds)
{
    if (fd_ < 0)
        return false;
    int value = static_cast<int>(seconds);
    return ioctl(fd_, WDIOC_SETTIMEOUT, &value) == 0;
}

bool WatchdogClient::start()
{
    if (fd_ < 0)
        return false;
    int options = WDIOS_ENABLECARD;
    return ioctl(fd_, WDIOC_SETOPTIONS, &options) == 0;
}

bool WatchdogClient::kick()
{
    if (fd_ < 0)
        return false;
    int dummy = 0;
    return ioctl(fd_, WDIOC_KEEPALIVE, &dummy) == 0;
}

bool WatchdogClient::stop()
{
    if (fd_ < 0)
        return false;
    int options = WDIOS_DISABLECARD;
    return ioctl(fd_, WDIOC_SETOPTIONS, &options) == 0;
}

bool WatchdogClient::getStatus(drone_wdt_status& status) const
{
    if (fd_ < 0)
        return false;
    return ioctl(fd_, DRONE_WDT_GET_STATUS, &status) == 0;
}

