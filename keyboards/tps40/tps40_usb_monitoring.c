#include "tps40_usb_monitoring.h"

#include "quantum.h"

#define REENABLE_USB_EVENT_TIME_MSEC TIME_MS2I(50)

static void enable_vbus_state_event(void);
static void disable_vbus_state_event(void);
static void set_reenable_event_timer(void);
static void reenable_event_timer_callback(virtual_timer_t *vtp, void *p);
static void vbus_pin_callback(void *arg);

bool current_connection_state;

static void enable_vbus_state_event(void) {
    palEnableLineEventI(VBUS_PIN, PAL_EVENT_MODE_BOTH_EDGES);
    palSetLineCallbackI(VBUS_PIN, vbus_pin_callback, NULL);
}

static void disable_vbus_state_event(void) {
    palDisableLineEventI(VBUS_PIN);
}

static virtual_timer_t reenable_event_timer;
static void set_reenable_event_timer(void) {
    chVTResetI(&reenable_event_timer);
    chVTDoSetI(&reenable_event_timer, REENABLE_USB_EVENT_TIME_MSEC, reenable_event_timer_callback, NULL);
}

static void reenable_event_timer_callback(virtual_timer_t *vtp, void *p) {
  chSysLockFromISR();
  enable_vbus_state_event();
  chSysUnlockFromISR();
}

static void vbus_pin_callback(void *arg) {
    current_connection_state = readPin(VBUS_PIN) == true;
    usb_connection_state_updated(current_connection_state);

    chSysLockFromISR();
    disable_vbus_state_event();
    set_reenable_event_timer();
    chSysUnlockFromISR();
}

void start_usb_monitoring(void) {
    palSetLineMode(VBUS_PIN, PAL_MODE_INPUT);
    enable_vbus_state_event();

    current_connection_state = readPin(VBUS_PIN) == true;
    usb_connection_state_updated(current_connection_state);
}

bool is_connected(void) {
    return current_connection_state;
}
