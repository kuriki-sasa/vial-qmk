#include "tps40.h"

#include "bluetooth.h"
#include "outputselect.h"

#include "tps40_usart.h"
#include "tps40_bt_communication.h"
#include "tps40_bt_controller.h"

static const pin_t tps40_leds[LED_NUM]       = LED_PINS;
bool               tps40_led_states[LED_NUM] = {0};

void tps40_set_led(uint8_t num, bool is_lit) {
    tps40_led_states[num] = is_lit;
    writePin(tps40_leds[num], !is_lit);
}

#define UART_MATRIX_RESPONSE_TIMEOUT 500000

uint8_t readCommand1[128] = { 0 };
uint8_t readCommand2[128] = { 0 };
uint8_t readCommand3[128] = { 0 };
uint8_t readCommand4[128] = { 0 };
uint32_t timeout = 0;
uint32_t timeout2 = 0;

void keyboard_pre_init_kb(void) {
    setPinOutput(tps40_leds[0]);
    setPinOutput(tps40_leds[1]);
    setPinOutput(tps40_leds[2]);
    //start_control();
    set_output(OUTPUT_BLUETOOTH);
}

void keyboard_post_init_kb(void) {
//    read_from_uart();
//    start_communication();
//    run_initial_sequence();
//    start_control();
}

char *int2bin(int x)
{
    int i;
    static char bin[40];
    for(i=31; i>=0; i--){
         bin[31-i]= ((x>>i)&1 )+'0';
   }
   bin[32]=0; // 文字列の最後に\0を挿入
   return bin;
}

void bluetooth_init(void) {
    start_control();
}

void bluetooth_task(void) {}

void bluetooth_send_keyboard(report_keyboard_t *report) {
    send_basic_keycodes(report);
}

void bluetooth_send_mouse(report_mouse_t *report) {
    print("send mouse\n");
    send_mouse_keycodes(report);
}

void bluetooth_send_consumer(uint16_t usage) {
    print("send mouse\n");
    send_consumer_keycodes(usage);
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
        print("=== start discovering ====\n");
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
