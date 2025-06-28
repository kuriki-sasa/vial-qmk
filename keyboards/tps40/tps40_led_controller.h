#pragma once

enum LedMode {
    CONNECTION_AND_BATTERY = 0,
    LAYER
};

enum BatteryIndicatorState {
    BAT_IND_LOW = 0,
    BAT_IND_MID,
    BAT_IND_HIGH,
    BAT_IND_CHARGING
};

enum BluetoothIndicatorState {
    BT_IND_IDLE = 0,
    BT_IND_PAIRING,
    BT_IND_CONNECTING,
    BT_IND_CONNECTED
};

enum LayerIndicatorState {
    LAYER_IND_QWERTY = 0,
    LAYER_IND_LOWER,
    LAYER_IND_RAISE,
    LAYER_IND_ADJUST
};

void set_led_mode(enum LedMode mode);
void update_led_by_current_mode(void);

void toggle_led_mode(void);

void set_bluetooth_indicator(enum BluetoothIndicatorState state);
void set_caps_indicator(bool is_on);
void set_layer_indicator(enum LayerIndicatorState state);
void set_battery_indicator(enum BatteryIndicatorState state);
