#include "tps40_led.h"

#include "quantum.h"

static const pin_t tps40_leds[LED_NUM]       = LED_PINS;
bool               tps40_led_states[LED_NUM] = { 0 };

void initialize_led(void) {
    setPinOutput(tps40_leds[0]);
    setPinOutput(tps40_leds[1]);
    setPinOutput(tps40_leds[2]);
}

void set_led_state(uint8_t num, bool is_lit) {
    tps40_led_states[num] = is_lit;
    writePin(tps40_leds[num], !is_lit);
}
