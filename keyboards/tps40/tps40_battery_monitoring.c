#include "tps40_battery_monitoring.h"

#include "quantum.h"
#include "analog.h"
#include "print.h"

// C3ピンの最大電圧: 0.7 (バッテリー電圧4.2Vのときに計測)
// C3ピンの最少電圧: 0.55
// AT32F415は3.3V駆動なので、4.2Vと0.7Vの比率からバッテリー電圧3.3V時のC3ピン電圧を計算

// GPIOの入力上限3.3Vを12bitの分解能最大値4095で割り、係数を出す
// 3.3 / 4095 = 0.00080586

// このことから最大電圧のときのADCから読み取れる値は
// 0.7 / 0.00080586 = 868
// 実際に返ってきた値は900
// 誤差はあるが高めに出ていて安全側に振れているのでOK

// 最少電圧のときの予測計測値は
// 0.55 / 0.00080586 = 682
// 682でLow通知が出せれば安全

#define BATTERY_MID_THRESHOLD 775
#define BATTERY_LOW_THRESHOLD 682
#define BATTERY_CHECK_INTERVAL_SEC 60

static bool is_charging;
static int16_t last_battery_level;
static enum BatteryState current_state;

static void update_battery_state(void) {
    enum BatteryState old_state = current_state;
    if (is_charging) {
        current_state = CHARGING;
    } else if (last_battery_level > BATTERY_MID_THRESHOLD){
        current_state = LEVEL_HIGH;
    } else if (last_battery_level > BATTERY_LOW_THRESHOLD) {
        current_state = LEVEL_MID;
    } else {
        current_state = LEVEL_LOW;
    }
    if (old_state != current_state) {
        battery_state_updated(current_state);
    }
}

static int16_t update_battery_level(void) {
    last_battery_level = analogReadPin(C3);
    uprintf("charging: %d  level: %d\n", is_charging, last_battery_level);
    return last_battery_level;
}

static bool update_charging_state(void) {
    is_charging = readPin(A0) == false;
    return is_charging;
}

static void battery_pin_callback(void *arg) {
    update_charging_state();
    update_battery_level();
    update_battery_state();
}

static THD_WORKING_AREA(waBatteryThread, 128);
static THD_FUNCTION(BatteryThread, arg) {
    chRegSetThreadName("battery_monitoring");

    while (true) {
        update_battery_level();
        update_battery_state();
        chThdSleepSeconds(BATTERY_CHECK_INTERVAL_SEC);
    }
}

void start_battery_monitoring(void) {
    setPinInput(A0);
    palEnableLineEventI(A0, PAL_EVENT_MODE_BOTH_EDGES);
    palSetLineCallbackI(A0, &battery_pin_callback, NULL);

    update_battery_level();
    update_charging_state();
    update_battery_state();

    chThdCreateStatic(waBatteryThread, sizeof(waBatteryThread), NORMALPRIO + 8, BatteryThread, NULL);
}

