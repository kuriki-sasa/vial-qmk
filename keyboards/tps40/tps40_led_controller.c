#include "tps40_led_controller.h"

#include "tps40_led.h"

static enum LedMode led_mode;
static enum LedState connection_and_battery_states[LED_NUM] = { 0 };
static enum LedState layer_states[LED_NUM] = { 0 };

void set_led_mode(enum LedMode mode) {
    if (led_mode != mode) {
        led_mode = mode;
    }
    update_led_by_current_mode();
}

void update_led_by_current_mode(void) {
    switch (led_mode) {
        case CONNECTION_AND_BATTERY:
            for (int led = 0; led < LED_NUM; led++) {
                set_led_state(led, connection_and_battery_states[led]);
            }
            break;
        case LAYER:
            for (int led = 0; led < LED_NUM; led++) {
                set_led_state(led, layer_states[led]);
            }
            break;
    }
}

void toggle_led_mode(void) {
    enum LedMode newMode = led_mode + 1;
    if (newMode > LAYER) {
        newMode = 0;
    }
    set_led_mode(newMode);
}

void set_bluetooth_indicator(enum BluetoothIndicatorState state) {
    switch (state) {
        case BT_IND_IDLE:
            connection_and_battery_states[2] = OFF;
            break;
        case BT_IND_PAIRING:
            connection_and_battery_states[2] = BLINK_FAST;
            break;
        case BT_IND_CONNECTING:
            connection_and_battery_states[2] = BLINK_MID;
            break;
        case BT_IND_CONNECTED:
            connection_and_battery_states[2] = ON;
            break;
    }
    update_led_by_current_mode();
}

void set_caps_indicator(bool is_on) {
    connection_and_battery_states[1] = is_on ? ON : OFF;
    update_led_by_current_mode();
}

void set_layer_indicator(enum LayerIndicatorState state) {
    switch (state) {
        case LAYER_IND_QWERTY:
            layer_states[1] = OFF;
            layer_states[2] = OFF;
            break;
        case LAYER_IND_LOWER:
            layer_states[1] = ON;
            layer_states[2] = OFF;
            break;
        case LAYER_IND_RAISE:
            layer_states[1] = OFF;
            layer_states[2] = ON;
            break;
        case LAYER_IND_ADJUST:
            layer_states[1] = ON;
            layer_states[2] = ON;
            break;
    }
    update_led_by_current_mode();
}

void set_battery_indicator(enum BatteryIndicatorState state) {
    switch (state) {
        case BAT_IND_CHARGED:
            connection_and_battery_states[0] = ON;
            layer_states[0] = ON;
            break;
        case BAT_IND_CHARGING:
            connection_and_battery_states[0] = BLINK_SLOW;
            layer_states[0] = BLINK_SLOW;
            break;
        case BAT_IND_HIGH:
            connection_and_battery_states[0] = ON;
            layer_states[0] = ON;
            break;
        case BAT_IND_MID:
            connection_and_battery_states[0] = ON;
            layer_states[0] = ON;
            break;
        case BAT_IND_LOW:
            connection_and_battery_states[0] = BLINK_MID;
            layer_states[0] = BLINK_MID;
            break;
    }
    update_led_by_current_mode();
}