#include "tps40_battery_monitoring.h"

#include "quantum.h"
#include "analog.h"
#include "print.h"

// 150kΩ, 30kΩで分圧
// Vo = (30 / 30 + 150) * Vi
// バッテリー電圧が4.2Vのとき: 0.7V
// バッテリー電圧が3.4Vのとき: 0.57

// Vref+実測 3.3Vを12bitの分解能最大値4095で割り、分解能1あたりので電圧を計算
// 3.3 / 4095 = 0.00080586

// このことから最大電圧のときのADCから読み取れる値は
// 0.7 / 0.00080586 = 868
// 実際に返ってきた値は900
// 誤差はあるが高めに出ていて安全側に振れているのでOK

// 最少電圧のときの予測計測値は
// 0.57 / 0.00080586 = 707
// 707でLow通知が出せれば安全

#define BATTERY_MID_THRESHOLD 788
#define BATTERY_LOW_THRESHOLD 707
#define BATTERY_CHECK_INTERVAL_SEC 60
#define REENABLE_BATTERY_EVENT_TIME_MSEC TIME_MS2I(50)

static void enable_charge_state_event(void);
static void disable_charge_state_event(void);
static void set_reenable_event_timer(void);
static void reenable_event_timer_callback(virtual_timer_t *vtp, void *p);
static void battery_pin_callback(void *arg);

static bool charging;
static int16_t last_battery_level;
static enum BatteryLevel current_state;

static int16_t update_battery_level(void) {
    last_battery_level = analogReadPin(C3);
    uprintf("charging: %d  level: %d\n", charging, last_battery_level);

    enum BatteryLevel new_state;
    if (last_battery_level > BATTERY_MID_THRESHOLD){
        new_state = LEVEL_HIGH;
    } else if (last_battery_level > BATTERY_LOW_THRESHOLD) {
        new_state = LEVEL_MID;
    } else {
        new_state = LEVEL_LOW;
    }
    uprintf("current_state: %d, new_state: %d\n", current_state, new_state);
    if (current_state != new_state) {
        current_state = new_state;
        battery_state_updated(current_state, charging);
    }

    return last_battery_level;
}

static bool update_charging_state(void) {
    bool new_state = readPin(CHARGE_STATE_PIN) == false;
    if (charging != new_state) {
        charging = new_state;
        battery_state_updated(current_state, charging);
    }

    return charging;
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

    chSysLockFromISR();
    disable_charge_state_event();
    set_reenable_event_timer();
    chSysUnlockFromISR();
}

static THD_WORKING_AREA(waBatteryThread, 2048);
static THD_FUNCTION(BatteryThread, arg) {
    chRegSetThreadName("battery_monitoring");

    palSetLineMode(CHARGE_STATE_PIN, PAL_MODE_INPUT_PULLUP);
    enable_charge_state_event();

    update_battery_level();
    update_charging_state();

    while (true) {
        update_battery_level();
        chThdSleepSeconds(BATTERY_CHECK_INTERVAL_SEC);
    }
}

void start_battery_monitoring(void) {
    chThdCreateStatic(waBatteryThread, sizeof(waBatteryThread), BATTERY_MONITORING_THREAD_PRIORITY, BatteryThread, NULL);
}

enum BatteryLevel get_current_battery_level(void) {
    return current_state;
}

bool is_charging(void) {
    return charging;
}