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
#define REENABLE_BATTERY_EVENT_TIME_MSEC TIME_MS2I(50)

static void update_battery_state(void);
static bool update_charging_state(void);
static void enable_charge_state_event(void);
static void disable_charge_state_event(void);
static void set_reenable_event_timer(void);
static void reenable_event_timer_callback(virtual_timer_t *vtp, void *p);
static void battery_pin_callback(void *arg);

static bool is_charging;
static int16_t last_battery_level;
static enum BatteryState current_state;

static void update_battery_state(void) {
    enum BatteryState new_state;
    if (is_charging) {
        new_state = CHARGING;
    } else if (last_battery_level > BATTERY_MID_THRESHOLD){
        new_state = LEVEL_HIGH;
    } else if (last_battery_level > BATTERY_LOW_THRESHOLD) {
        new_state = LEVEL_MID;
    } else {
        new_state = LEVEL_LOW;
    }
    uprintf("current_state: %d, new_state: %d\n", current_state, new_state);
    if (current_state != new_state) {
        current_state = new_state;
        battery_state_updated(current_state);
    }
}

static int16_t update_battery_level(void) {
    last_battery_level = analogReadPin(C3);
    uprintf("charging: %d  level: %d\n", is_charging, last_battery_level);
    return last_battery_level;
}

static bool update_charging_state(void) {
    is_charging = readPin(CHARGE_STATE_PIN) == false;
    return is_charging;
}


static void enable_charge_state_event(void) {
    palEnableLineEventI(CHARGE_STATE_PIN, PAL_EVENT_MODE_BOTH_EDGES);
    palSetLineCallbackI(CHARGE_STATE_PIN, battery_pin_callback, NULL);
}

static void disable_charge_state_event(void) {
    palDisableLineEventI(CHARGE_STATE_PIN);
}

static virtual_timer_t reenable_event_timer;
static void set_reenable_event_timer(void) {
    chVTResetI(&reenable_event_timer);
    chVTDoSetI(&reenable_event_timer, REENABLE_BATTERY_EVENT_TIME_MSEC, reenable_event_timer_callback, NULL);
}

static void reenable_event_timer_callback(virtual_timer_t *vtp, void *p) {
  chSysLockFromISR();
  enable_charge_state_event();
  chSysUnlockFromISR();
}

static void battery_pin_callback(void *arg) {
    update_charging_state();
    update_battery_state();

    chSysLockFromISR();
    disable_charge_state_event();
    set_reenable_event_timer();
    chSysUnlockFromISR();
}

static THD_WORKING_AREA(waBatteryThread, 2048);
static THD_FUNCTION(BatteryThread, arg) {
    chRegSetThreadName("battery_monitoring");

    palSetLineMode(CHARGE_STATE_PIN, PAL_MODE_INPUT);
    enable_charge_state_event();

    update_battery_level();
    update_charging_state();
    update_battery_state();

    while (true) {
        update_battery_level();
        update_battery_state();
        chThdSleepSeconds(BATTERY_CHECK_INTERVAL_SEC);
    }
}

void start_battery_monitoring(void) {
    chThdCreateStatic(waBatteryThread, sizeof(waBatteryThread), NORMALPRIO + 8, BatteryThread, NULL);
}

