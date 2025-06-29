#include "tps40_led.h"

#include "quantum.h"
#include "printf.h"

static const pin_t tps40_leds[LED_NUM]  = LED_PINS;
static enum LedState tps40_led_states[LED_NUM] = { 0 };

uint8_t count_blink_slow(void) {
    uint8_t count = 0;
    for (int num = 0; num < LED_NUM; num++) {
        if (tps40_led_states[num] == BLINK_SLOW) {
            count++;
        }
    }
    return count;
}

uint8_t count_blink_mid(void) {
    uint8_t count = 0;
    for (int num = 0; num < LED_NUM; num++) {
        if (tps40_led_states[num] == BLINK_MID) {
            count++;
        }
    }
    return count;
}

uint8_t count_blink_fast(void) {
    uint8_t count = 0;
    for (int num = 0; num < LED_NUM; num++) {
        if (tps40_led_states[num] == BLINK_FAST) {
            count++;
        }
    }
    return count;
}

static THD_WORKING_AREA(waLedBlinkSlowThread, 128);
static THD_FUNCTION(LedBlinkSlowThread, arg) {
    chRegSetThreadName("led_blink_slow");

    while (true) {
        if (count_blink_slow() == 0) {
            chThdSleepMilliseconds(BLINK_THREAD_INTERVAL_MSEC);
            continue;
        }

        for (int num = 0; num < LED_NUM; num++) {
            if (tps40_led_states[num] == BLINK_SLOW) {
                writePin(tps40_leds[num], false);
            }
        }
        chThdSleepMilliseconds(BLINK_SLOW_INTERVAL_MSEC);

        for (int num = 0; num < LED_NUM; num++) {
            if (tps40_led_states[num] == BLINK_SLOW) {
                writePin(tps40_leds[num], true);
            }
        }
        chThdSleepMilliseconds(BLINK_SLOW_INTERVAL_MSEC);
    }
}

static THD_WORKING_AREA(waLedBlinkMidThread, 128);
static THD_FUNCTION(LedBlinkMidThread, arg) {
    chRegSetThreadName("led_blink_mid");

    while (true) {
        if (count_blink_mid() == 0) {
            chThdSleepMilliseconds(BLINK_THREAD_INTERVAL_MSEC);
            continue;
        }

        for (int num = 0; num < LED_NUM; num++) {
            if (tps40_led_states[num] == BLINK_MID) {
                writePin(tps40_leds[num], false);
            }
        }
        chThdSleepMilliseconds(BLINK_MID_INTERVAL_MSEC);

        for (int num = 0; num < LED_NUM; num++) {
            if (tps40_led_states[num] == BLINK_MID) {
                writePin(tps40_leds[num], true);
            }
        }
        chThdSleepMilliseconds(BLINK_MID_INTERVAL_MSEC);
    }
}

static THD_WORKING_AREA(waLedBlinkFastThread, 128);
static THD_FUNCTION(LedBlinkFastThread, arg) {
    chRegSetThreadName("led_blink_fast");

    while (true) {
        if (count_blink_fast() == 0) {
            chThdSleepMilliseconds(BLINK_THREAD_INTERVAL_MSEC);
            continue;
        }

        for (int num = 0; num < LED_NUM; num++) {
            if (tps40_led_states[num] == BLINK_FAST) {
                writePin(tps40_leds[num], false);
            }
        }
        chThdSleepMilliseconds(BLINK_FAST_INTERVAL_MSEC);

        for (int num = 0; num < LED_NUM; num++) {
            if (tps40_led_states[num] == BLINK_FAST) {
                writePin(tps40_leds[num], true);
            }
        }
        chThdSleepMilliseconds(BLINK_FAST_INTERVAL_MSEC);
    }
}

void initialize_led(void) {
    setPinOutput(tps40_leds[0]);
    setPinOutput(tps40_leds[1]);
    setPinOutput(tps40_leds[2]);

    chThdCreateStatic(waLedBlinkSlowThread, sizeof(waLedBlinkSlowThread), LED_BLINK_SLOW_THREAD_PRIORITY, LedBlinkSlowThread, NULL);
    chThdCreateStatic(waLedBlinkMidThread, sizeof(waLedBlinkMidThread), LED_BLINK_MID_THREAD_PRIORITY, LedBlinkMidThread, NULL);
    chThdCreateStatic(waLedBlinkFastThread, sizeof(waLedBlinkFastThread), LED_BLINK_FAST_THREAD_PRIORITY, LedBlinkFastThread, NULL);
}

void set_led_state(uint8_t num, enum LedState state) {
    tps40_led_states[num] = state;
    switch (state) {
        case ON:
            writePin(tps40_leds[num], false);
            break;
        case OFF:
            writePin(tps40_leds[num], true);
            break;
        default:
            break;
    }
}
