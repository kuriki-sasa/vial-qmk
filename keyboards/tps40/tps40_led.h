#pragma once

enum LedState {
    OFF = 0,
    ON,
    BLINK_SLOW,
    BLINK_MID,
    BLINK_FAST
};

void initialize_led(void);

void set_led_state(uint8_t num, enum LedState state);
