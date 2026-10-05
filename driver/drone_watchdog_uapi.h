#ifndef DRONE_WATCHDOG_UAPI_H
#define DRONE_WATCHDOG_UAPI_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define DRONE_WDT_IOC_MAGIC 'D'
#define DRONE_WDT_GET_STATUS    _IOR(DRONE_WDT_IOC_MAGIC, 6, struct drone_wdt_status)

struct drone_wdt_status {
    __u32 timeout_sec;
    __u32 running;
    __u32 expired;
    __u64 kick_count;
    __u64 timeout_count;
};

#endif
