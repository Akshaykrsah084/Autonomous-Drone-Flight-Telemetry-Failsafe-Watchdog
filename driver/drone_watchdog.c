// Educational simulated hardware watchdog driver.
// It uses the Linux Watchdog Driver Core, while the hrtimer models the
// hardware countdown. A timeout records/logs a recovery event and does not
// reboot the host machine.

#include <linux/errno.h>
#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/watchdog.h>

#include "drone_watchdog_uapi.h"

#define DRIVER_NAME "drone_watchdog"
#define DEFAULT_TIMEOUT_SEC 3U
#define MIN_TIMEOUT_SEC 1U
#define MAX_TIMEOUT_SEC 60U

struct drone_watchdog {
    struct watchdog_device wdd;
    struct hrtimer timer;
    spinlock_t lock;
    bool expired;
    __u64 kick_count;
    __u64 timeout_count;
};

static struct drone_watchdog dw;

static void drone_wdt_expire(const char *reason)
{
    unsigned long flags;
    bool changed = false;

    spin_lock_irqsave(&dw.lock, flags);
    if (watchdog_active(&dw.wdd)) {
        dw.expired = true;
        dw.timeout_count++;
        clear_bit(WDOG_ACTIVE, &dw.wdd.status);
        changed = true;
    }
    spin_unlock_irqrestore(&dw.lock, flags);

    if (changed)
        pr_warn(DRIVER_NAME ": watchdog timeout (%s) - simulated recovery action\n", reason);
}

static enum hrtimer_restart drone_wdt_timer_cb(struct hrtimer *timer)
{
    drone_wdt_expire("heartbeat not received");
    return HRTIMER_NORESTART;
}

static int drone_wdt_start(struct watchdog_device *wdd)
{
    unsigned long flags;

    spin_lock_irqsave(&dw.lock, flags);
    dw.expired = false;
    spin_unlock_irqrestore(&dw.lock, flags);

    hrtimer_start(&dw.timer, ktime_set(wdd->timeout, 0), HRTIMER_MODE_REL);
    pr_info(DRIVER_NAME ": started with timeout=%u s\n", wdd->timeout);
    return 0;
}

static int drone_wdt_stop(struct watchdog_device *wdd)
{
    hrtimer_cancel(&dw.timer);
    pr_info(DRIVER_NAME ": stopped\n");
    return 0;
}

static int drone_wdt_ping(struct watchdog_device *wdd)
{
    unsigned long flags;

    if (!watchdog_active(wdd))
        return -EPIPE;

    spin_lock_irqsave(&dw.lock, flags);
    dw.expired = false;
    dw.kick_count++;
    spin_unlock_irqrestore(&dw.lock, flags);

    hrtimer_start(&dw.timer, ktime_set(wdd->timeout, 0), HRTIMER_MODE_REL);
    return 0;
}

static int drone_wdt_set_timeout(struct watchdog_device *wdd, unsigned int timeout)
{
    if (timeout < MIN_TIMEOUT_SEC || timeout > MAX_TIMEOUT_SEC)
        return -EINVAL;

    wdd->timeout = timeout;
    if (watchdog_active(wdd)) {
        hrtimer_cancel(&dw.timer);
        hrtimer_start(&dw.timer, ktime_set(wdd->timeout, 0), HRTIMER_MODE_REL);
    }
    return 0;
}

static unsigned int drone_wdt_status(struct watchdog_device *wdd)
{
    return watchdog_active(wdd) ? WDOG_ACTIVE : 0;
}

static long drone_wdt_ioctl(struct watchdog_device *wdd, unsigned int cmd, unsigned long arg)
{
    struct drone_wdt_status status;
    unsigned long flags;

    switch (cmd) {
    case DRONE_WDT_GET_STATUS:
        memset(&status, 0, sizeof(status));
        spin_lock_irqsave(&dw.lock, flags);
        status.timeout_sec = wdd->timeout;
        status.running = watchdog_active(wdd) ? 1U : 0U;
        status.expired = dw.expired ? 1U : 0U;
        status.kick_count = dw.kick_count;
        status.timeout_count = dw.timeout_count;
        spin_unlock_irqrestore(&dw.lock, flags);

        if (copy_to_user((void __user *)arg, &status, sizeof(status)))
            return -EFAULT;
        return 0;

    default:
        return -ENOIOCTLCMD;
    }
}

static const struct watchdog_info drone_wdt_info = {
    .options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING,
    .firmware_version = 1,
    .identity = "Drone Simulated WDT",
};

static const struct watchdog_ops drone_wdt_ops = {
    .owner = THIS_MODULE,
    .start = drone_wdt_start,
    .stop = drone_wdt_stop,
    .ping = drone_wdt_ping,
    .status = drone_wdt_status,
    .set_timeout = drone_wdt_set_timeout,
    .ioctl = drone_wdt_ioctl,
};

static int __init drone_wdt_init(void)
{
    int ret;

    memset(&dw, 0, sizeof(dw));
    spin_lock_init(&dw.lock);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(7, 0, 0)
    hrtimer_setup(&dw.timer, drone_wdt_timer_cb, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
#else
    hrtimer_init(&dw.timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    dw.timer.function = drone_wdt_timer_cb;
#endif

    dw.wdd.info = &drone_wdt_info;
    dw.wdd.ops = &drone_wdt_ops;
    dw.wdd.timeout = DEFAULT_TIMEOUT_SEC;
    dw.wdd.min_timeout = MIN_TIMEOUT_SEC;
    dw.wdd.max_timeout = MAX_TIMEOUT_SEC;
    dw.wdd.bootstatus = 0;

    watchdog_stop_on_reboot(&dw.wdd);
    watchdog_stop_on_unregister(&dw.wdd);

    ret = watchdog_register_device(&dw.wdd);
    if (ret) {
        pr_err(DRIVER_NAME ": watchdog_register_device failed: %d\n", ret);
        return ret;
    }

    pr_info(DRIVER_NAME ": loaded and registered with the Linux watchdog core\n");
    return 0;
}

static void __exit drone_wdt_exit(void)
{
    hrtimer_cancel(&dw.timer);
    watchdog_unregister_device(&dw.wdd);
    pr_info(DRIVER_NAME ": unloaded\n");
}

module_init(drone_wdt_init);
module_exit(drone_wdt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Capstone Project");
MODULE_DESCRIPTION("Simulated hardware watchdog timer driver for drone safety monitoring");
MODULE_VERSION("1.1");
