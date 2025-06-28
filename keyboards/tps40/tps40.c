#include "tps40.h"

#include "outputselect.h"

#include "tps40_led.h"
#include "tps40_led_controller.h"
#include "tps40_battery_monitoring.h"
#include "tps40_usb_monitoring.h"
#include "tps40_usart.h"
#include "tps40_bt_communication.h"
#include "tps40_bt_controller.h"

enum Tps40Keycodes {
    DEEP_SLEEP = QK_KB_0,
    PAIRING_1,
    PAIRING_2,
    PAIRING_3,
    BT_1,
    BT_2,
    BT_3,
    DISCONNECT_BT,
    TOGGLE_LED_MODE,
};

void keyboard_pre_init_kb(void) {
    initialize_led();
    set_led_mode(CONNECTION_AND_BATTERY);
}

void keyboard_post_init_kb(void) {
    start_battery_monitoring();
    start_usb_monitoring();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Handling custom keycodes
    switch (keycode) {
        case DEEP_SLEEP:
            if (record->event.pressed) {
                enter_deepsleep();
            }
            break;
        case PAIRING_1:
            if (record->event.pressed) {
                start_discovering(1);
            }
            break;
        case PAIRING_2:
            if (record->event.pressed) {
                start_discovering(2);
            }
            break;
        case PAIRING_3:
            if (record->event.pressed) {
                start_discovering(3);
            }
            break;
        case BT_1:
            if (record->event.pressed) {
                start_connection(1);
            }
            break;
        case BT_2:
            if (record->event.pressed) {
                start_connection(2);
            }
            break;
        case BT_3:
            if (record->event.pressed) {
                start_connection(3);
            }
            break;
        case DISCONNECT_BT:
            if (record->event.pressed) {
                start_disconnection();
            }
            break;
        case TOGGLE_LED_MODE:
            if (record->event.pressed) {
                toggle_led_mode();
            }
            break;
    }

    return true;
}
