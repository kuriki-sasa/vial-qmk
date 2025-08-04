#pragma once

enum BatteryLevel {
    LEVEL_LOW = 0,
    LEVEL_MID,
    LEVEL_HIGH
};

void battery_state_updated(enum BatteryLevel level, bool charging);

void start_battery_monitoring(void);
enum BatteryLevel get_current_battery_level(void);
bool is_charging(void);