#pragma once

enum BatteryState {
    LEVEL_LOW = 0,
    LEVEL_MID,
    LEVEL_HIGH,
    CHARGING
};

void battery_state_updated(enum BatteryState state);

void start_battery_monitoring(void);
