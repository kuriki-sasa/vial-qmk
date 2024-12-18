#include "tps40.h"

#include "outputselect.h"

#include "tps40_led.h"
#include "tps40_battery_monitoring.h"
#include "tps40_usart.h"
#include "tps40_bt_communication.h"
#include "tps40_bt_controller.h"

void keyboard_pre_init_kb(void) {
    initialize_led();
    //start_control();
    set_output(OUTPUT_BLUETOOTH);
}

void keyboard_post_init_kb(void) {
    start_battery_monitoring();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
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
        start_discovering(1);
    } else if (record->event.pressed && keycode == KC_S) {
        print("=== start connection ====\n");
        start_connection(1);
    } else if (record->event.pressed && keycode == KC_D) {
        print("=== start disconn ====\n");
        start_discovering(3);
    }
    uprintf("KL: kc: 0x%04X, mod: 0x%02X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u\n", keycode, get_mods(), record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
//#endif
  return true;
}
