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
    set_output(OUTPUT_BLUETOOTH);
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
//#ifdef CONSOLE_ENABLE
    // uprintf("command1: %s aaaa\n", readCommand1);
    // uprintf("command2: %s aaaa\n", readCommand2);
    // uprintf("command3: %s aaaa\n", readCommand3);
    // uprintf("APB2EN = %s\n", int2bin(CRM->APB2EN));
    // uprintf("APB1EN = %s\n", int2bin(CRM->APB1EN));
    // uprintf("baudr = %lu\n", tps40_read_baudr());
    //tps40_usart_send_break_frame();
    // uint8_t command[] = "AT\r\n";
    // size_t commandLength = strlen((char*)command);
    // tps40_usart_send(command, commandLength);
    // while (timeout <= UART_MATRIX_RESPONSE_TIMEOUT) {
    //     size_t length = read_from_uart();
    //     if (length <= 0) {
    //         timeout++;
    //         continue;
    //     }
    //     timeout = 0;
    // }
    // uprintf("timeout: %lu aaaaaaaaa\n", timeout);
    // uprintf("result: %d aaaaaaaaa\n", lastIndex);
    // for (int i = 0; i < lastIndex; i++) {
    //     uprintf("%02x ", buffer[i]);
    // }
    // print("\n");
    if (record->event.pressed && keycode == KC_A) {
        //run_initial_sequence();
        // print_debug_log();
        // while (timeout <= UART_MATRIX_RESPONSE_TIMEOUT) {
        //     size_t length = read_from_uart();
        //     if (length <= 0) {
        //         timeout++;
        //         continue;
        //     }
        //     timeout = 0;
        // }
        // uprintf("timeout: %lu aaaaaaaaa\n", timeout);
        // uprintf("result: %d aaaaaaaaa\n", lastIndex);
        // for (int i = 0; i < lastIndex; i++) {
        //     uprintf("%02x ", buffer[i]);
        // }
        // print("\n");
        //start_discovering(1);
        set_led_state(0, BLINK_SLOW);
        set_led_state(1, BLINK_SLOW);
        set_led_state(2, BLINK_SLOW);
        deepsleep();
        //soft_reset_keyboard();
    } else if (record->event.pressed && keycode == KC_S) {
        print("=== start connection ====\n");
        enable_auto_sleep();
//        set_led_state(0, BLINK_FAST);
//        set_led_state(1, BLINK_MID);
//        set_led_state(2, BLINK_MID);
    } else if (record->event.pressed && keycode == KC_D) {
        print("=== start disconn ====\n");
        start_connection(1);
    } else if (record->event.pressed && keycode == KC_F) {
        start_connection(2);
    } else if (record->event.pressed && keycode == KC_G) {
        set_led_state(0, BLINK_SLOW);
        set_led_state(1, BLINK_MID);
        set_led_state(2, BLINK_FAST);
    }

//    uprintf("KL: kc: 0x%04X, mod: 0x%02X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u\n", keycode, get_mods(), record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
//#endif
  return true;
}
