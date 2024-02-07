#include "tps40.h"

static const pin_t tos40_leds[LED_NUM]       = LED_PINS;
bool               tps40_led_states[LED_NUM] = {0};

void keyboard_post_init_kb(void) {
    setPinOutput(tos40_leds[0]);
    setPinOutput(tos40_leds[1]);
    setPinOutput(tos40_leds[2]);
}

void tps40_set_led(uint8_t num, bool is_lit) {
    tps40_led_states[num] = is_lit;
    writePin(tos40_leds[num], !is_lit);
}
