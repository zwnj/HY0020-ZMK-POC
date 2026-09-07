/*
 * Minimal RGB status manager for the HY0020 keyboard PoC.
 *
 * This is intentionally an external ZMK module. It consumes public ZMK events
 * and drives an upstream Zephyr LED driver; it does not patch ZMK core or add
 * a custom transport/device driver.
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include <zmk/battery.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#if IS_ENABLED(CONFIG_ZMK_SPLIT) && !IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/split/bluetooth/peripheral.h>
#define HY_STATUS_IS_SPLIT_PERIPHERAL 1
#else
#include <zmk/ble.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#define HY_STATUS_IS_SPLIT_PERIPHERAL 0
#endif

LOG_MODULE_REGISTER(hy_status, CONFIG_ZMK_LOG_LEVEL);

#define RGB_LED_NODE DT_NODELABEL(rgb_status)

#if !DT_NODE_EXISTS(RGB_LED_NODE)
#error "CONFIG_HY_STATUS requires a devicetree node labeled rgb_status"
#endif

static const struct led_dt_spec status_led = LED_DT_SPEC_GET(RGB_LED_NODE);

static uint8_t battery_level = 100;
static bool connected;
static bool pairing;
static bool blink_on;
static bool initialized_connection_state;
static bool transient_active;
static uint8_t transient_rgb[3];

static void status_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(status_work, status_work_handler);

static int set_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    const uint8_t color[3] = {red, green, blue};
    int rc = led_set_color(status_led.dev, status_led.index, ARRAY_SIZE(color), color);

    if (rc < 0) {
        LOG_WRN("Failed to set RGB status LED: %d", rc);
    }

    return rc;
}

static void schedule_status(k_timeout_t delay) {
    k_work_reschedule(&status_work, delay);
}

static void start_transient(uint8_t red, uint8_t green, uint8_t blue) {
    transient_rgb[0] = red;
    transient_rgb[1] = green;
    transient_rgb[2] = blue;
    transient_active = true;
    schedule_status(K_NO_WAIT);
}

static void update_connection_state(bool now_connected, bool now_pairing) {
    if (initialized_connection_state && connected != now_connected) {
        if (now_connected) {
            /* Short green confirmation flash. */
            start_transient(0, 48, 0);
        } else {
            /* Short amber/yellow disconnect flash. */
            start_transient(48, 24, 0);
        }
    }

    connected = now_connected;
    pairing = now_pairing;
    initialized_connection_state = true;
    schedule_status(K_NO_WAIT);
}

static void render_steady_or_blinking_state(void) {
    /* Highest priority: critical battery. */
    if (battery_level <= CONFIG_HY_STATUS_CRITICAL_BATTERY) {
        blink_on = !blink_on;
        set_rgb(blink_on ? 64 : 0, 0, 0);
        schedule_status(K_MSEC(blink_on ? 250 : 500));
        return;
    }

    /* An open host BLE profile means the keyboard is available for pairing. */
    if (pairing) {
        blink_on = !blink_on;
        set_rgb(0, 0, blink_on ? 48 : 0);
        schedule_status(K_MSEC(500));
        return;
    }

    /* Low battery remains visible without waking periodically. */
    if (battery_level <= CONFIG_HY_STATUS_LOW_BATTERY) {
        blink_on = false;
        set_rgb(24, 0, 0);
        return;
    }

    blink_on = false;
    set_rgb(0, 0, 0);
}

static void status_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    if (transient_active &&
        battery_level > CONFIG_HY_STATUS_CRITICAL_BATTERY && !pairing) {
        transient_active = false;
        set_rgb(transient_rgb[0], transient_rgb[1], transient_rgb[2]);
        schedule_status(K_MSEC(650));
        return;
    }

    render_steady_or_blinking_state();
}

static int battery_listener(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *event = as_zmk_battery_state_changed(eh);

    if (event == NULL) {
        return -ENOTSUP;
    }

    battery_level = event->state_of_charge;
    schedule_status(K_NO_WAIT);
    return 0;
}

ZMK_LISTENER(hy_status_battery, battery_listener);
ZMK_SUBSCRIPTION(hy_status_battery, zmk_battery_state_changed);

#if HY_STATUS_IS_SPLIT_PERIPHERAL
static int split_listener(const zmk_event_t *eh) {
    const struct zmk_split_peripheral_status_changed *event =
        as_zmk_split_peripheral_status_changed(eh);

    if (event == NULL) {
        return -ENOTSUP;
    }

    /* Split peripherals pair only with their central; there is no host profile. */
    update_connection_state(event->connected, false);
    return 0;
}

ZMK_LISTENER(hy_status_split, split_listener);
ZMK_SUBSCRIPTION(hy_status_split, zmk_split_peripheral_status_changed);
#else
static void refresh_host_ble_state(void) {
    update_connection_state(zmk_ble_active_profile_is_connected(),
                            zmk_ble_active_profile_is_open());
}

static int host_ble_listener(const zmk_event_t *eh) {
    ARG_UNUSED(eh);
    refresh_host_ble_state();
    return 0;
}

ZMK_LISTENER(hy_status_host_ble, host_ble_listener);
ZMK_SUBSCRIPTION(hy_status_host_ble, zmk_ble_active_profile_changed);
ZMK_SUBSCRIPTION(hy_status_host_ble, zmk_endpoint_changed);
#endif

static int hy_status_init(void) {
    if (!led_is_ready_dt(&status_led)) {
        LOG_ERR("RGB status LED device is not ready");
        return -ENODEV;
    }

    battery_level = zmk_battery_state_of_charge();

#if HY_STATUS_IS_SPLIT_PERIPHERAL
    connected = zmk_split_bt_peripheral_is_connected();
    pairing = false;
#else
    connected = zmk_ble_active_profile_is_connected();
    pairing = zmk_ble_active_profile_is_open();
#endif

    initialized_connection_state = true;
    set_rgb(0, 0, 0);
    schedule_status(K_NO_WAIT);
    return 0;
}

SYS_INIT(hy_status_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
