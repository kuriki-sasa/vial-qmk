#include "tps40_led.h"

#include "quantum.h"
#include "printf.h"

static const pin_t tps40_leds[LED_NUM]  = LED_PINS;
static enum LedState tps40_led_states[LED_NUM] = { 0 };
static thread_t* blink_slow_thread;
static thread_t* blink_mid_thread;
static thread_t* blink_fast_thread;

void initialize_led(void) {
    setPinOutput(tps40_leds[0]);
    setPinOutput(tps40_leds[1]);
    setPinOutput(tps40_leds[2]);
}

static THD_WORKING_AREA(waLedBlinkSlowThread, 128);
static THD_FUNCTION(LedBlinkSlowThread, arg) {
    chRegSetThreadName("led_blink_slow");

    while (true) {
        if (chThdShouldTerminateX()) {
            break;
        }
        for (int num = 0; num < LED_NUM; num++) {
            if (tps40_led_states[num] == BLINK_SLOW) {
                writePin(tps40_leds[num], false);
            }
        }
        chThdSleepMilliseconds(BLINK_SLOW_INTERVAL_MSEC);
        if (chThdShouldTerminateX()) {
            break;
        }
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
        if (chThdShouldTerminateX()) {
            break;
        }
        for (int num = 0; num < LED_NUM; num++) {
            if (tps40_led_states[num] == BLINK_MID) {
                writePin(tps40_leds[num], false);
            }
        }
        chThdSleepMilliseconds(BLINK_MID_INTERVAL_MSEC);
        if (chThdShouldTerminateX()) {
            break;
        }
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
        if (chThdShouldTerminateX()) {
            break;
        }
        for (int num = 0; num < LED_NUM; num++) {
            if (tps40_led_states[num] == BLINK_FAST) {
                writePin(tps40_leds[num], false);
            }
        }
        chThdSleepMilliseconds(BLINK_FAST_INTERVAL_MSEC);
        if (chThdShouldTerminateX()) {
            break;
        }
        for (int num = 0; num < LED_NUM; num++) {
            if (tps40_led_states[num] == BLINK_FAST) {
                writePin(tps40_leds[num], true);
            }
        }
        chThdSleepMilliseconds(BLINK_FAST_INTERVAL_MSEC);
    }
}

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

void set_led_state(uint8_t num, enum LedState state) {
    enum LedState old_state = tps40_led_states[num];
    if (old_state == state) {
        return;
    }

    uint8_t old_state_count;
    switch (state) {
        case BLINK_SLOW:
            old_state_count = count_blink_slow();
            break;
        case BLINK_MID:
            old_state_count = count_blink_mid();
            break;
        case BLINK_FAST:
            old_state_count = count_blink_fast();
            break;
        default:
            old_state_count = 0;
            break;
    }

    tps40_led_states[num] = state;

    if (count_blink_slow() == 0 && blink_slow_thread) {
        chThdTerminate(blink_slow_thread);
        chThdWait(blink_slow_thread);
        blink_slow_thread = NULL;
    }
    if (count_blink_mid() == 0 && blink_mid_thread) {
        chThdTerminate(blink_mid_thread);
        chThdWait(blink_mid_thread);
        blink_mid_thread = NULL;
    }
    if (count_blink_fast() == 0 && blink_fast_thread) {
        chThdTerminate(blink_fast_thread);
        chThdWait(blink_fast_thread);
        blink_fast_thread = NULL;
    }

    switch (state) {
        case ON:
            writePin(tps40_leds[num], false);
            break;
        case OFF:
            writePin(tps40_leds[num], true);
            break;
        case BLINK_SLOW:
            if (old_state_count == 0) {
                blink_slow_thread = chThdCreateStatic(waLedBlinkSlowThread, sizeof(waLedBlinkSlowThread), LED_BLINK_SLOW_THREAD_PRIORITY, LedBlinkSlowThread, NULL);
            }
            break;
        case BLINK_MID:
            if (old_state_count == 0) {
                blink_mid_thread = chThdCreateStatic(waLedBlinkMidThread, sizeof(waLedBlinkMidThread), LED_BLINK_MID_THREAD_PRIORITY, LedBlinkMidThread, NULL);
            }
            break;
        case BLINK_FAST:
            if (old_state_count == 0) {
                blink_fast_thread = chThdCreateStatic(waLedBlinkFastThread, sizeof(waLedBlinkFastThread), LED_BLINK_FAST_THREAD_PRIORITY, LedBlinkFastThread, NULL);
            }
            break;
    }

}
