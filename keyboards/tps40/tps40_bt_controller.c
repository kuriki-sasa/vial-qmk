#include "tps40_bt_controller.h"

#include <stdlib.h>

#include "quantum.h"
#include "print.h"
#include "bluetooth.h"
#include "usb_main.h"
#include "debounce.h"

#include "tps40_matrix.h"
#include "tps40_bt_tasks.h"
#include "tps40_bt_constants.h"
#include "tps40_bt_communication.h"
#include "tps40_bt_event.h"
#include "tps40_led.h"
#include "tps40_led_controller.h"
#include "coroutine.h"

int current_slot = 1;
coroutine_t current_task = NULL;
static enum BtCommState current_state = STATE_INITIAL;

void run_task(coroutine_t task) {
    current_task = task;
    co_call(current_task);
}

int coroutine_stack[128];
void start_preparation(void) {
    current_state = STATE_PREPARING;
    wait_ms(500);
    coroutine_t task = co_create(preparation_task, NULL, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
}

void wakeup_before_command(void) {
    if (current_state == STATE_IDLE_SLEEPING) {
        send_wakeup();
        wait_ms(TPS40_WAKEUP_DELAY_MILLIS);
        current_state = STATE_IDLE;
    } else if (current_state == STATE_CONNECTED_SLEEPING) {
        send_wakeup();
        wait_ms(TPS40_WAKEUP_DELAY_MILLIS);
        current_state = STATE_CONNECTED;
    }
}

void wakeup_before_input(void) {
    if (current_state == STATE_IDLE_SLEEPING) {
        send_wakeup();
        wait_ms(TPS40_WAKEUP_DELAY_INPUT_MILLIS);
        current_state = STATE_IDLE;
    } else if (current_state == STATE_CONNECTED_SLEEPING) {
        send_wakeup();
        wait_ms(TPS40_WAKEUP_DELAY_INPUT_MILLIS);
        current_state = STATE_CONNECTED;
    }
}

bool start_discovering(int slot) {
    wakeup_before_command();

    switch (current_state) {
        case STATE_INITIAL:
        case STATE_PREPARING:
        case STATE_ERROR:
            return false;
        default:
            break;
    }

    current_slot = slot;
    TaskArgs args;
    args.current_slot = slot;
    args.received_command = NULL;
    coroutine_t task = co_create(start_discovering_task, &args, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
    return true;
}

bool start_connection(int slot) {
    wakeup_before_command();

    switch (current_state) {
        case STATE_INITIAL:
        case STATE_PREPARING:
        case STATE_ERROR:
            return false;
        default:
            break;
    }

    current_slot = slot;
    TaskArgs args;
    args.current_slot = slot;
    args.received_command = NULL;
    coroutine_t task = co_create(start_connection_task, &args, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
    return true;
}

bool start_disconnection(void) {
    wakeup_before_command();

    switch (current_state) {
        case STATE_INITIAL:
        case STATE_PREPARING:
        case STATE_ERROR:
            return false;
        default:
            break;
    }

    coroutine_t task = co_create(start_disconnection_task, 0, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
    return true;
}

bool reconnect_last_slot(void) {
    coroutine_t task = co_create(start_reconnection_last_slot_task, 0, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
    return true;
}

bool enable_auto_sleep(void) {
    coroutine_t task = co_create(enable_auto_idle_task, 0, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
    return true;
}

void enter_deepsleep(void) {
    coroutine_t task = co_create(enter_deepsleep_task, NULL, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
}

bool deepsleep(void) {
    set_scan_enabled(false);
    setPinOutput(TPS40_WAKEUP_KEY_OUTPUT_PIN);
    writePinLow(TPS40_WAKEUP_KEY_OUTPUT_PIN);

    palSetLineMode(TPS40_WAKEUP_KEY_OBSERVE_PIN, PAL_MODE_INPUT_PULLUP);
    palEnableLineEvent(TPS40_WAKEUP_KEY_OBSERVE_PIN, PAL_EVENT_MODE_BOTH_EDGES);

    set_led_state(0, OFF);
    set_led_state(1, OFF);
    set_led_state(2, OFF);

    usbStop(&USB_DRIVER);
    usbDisconnectBus(&USB_DRIVER);

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    PWC->CTRL &= ~PWC_CTRL_LPSEL;
    PWC->CTRL |= PWC_CTRL_VRSEL;
    __WFI();
    __disable_irq();

    // Deel sleep復帰後、ここから処理再開

    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    PWC->CTRL |= PWC_CTRL_LPSEL;
    PWC->CTRL &= ~PWC_CTRL_VRSEL;
    palDisableLineEvent(A10);

    chSysLock();
        at32_clock_init();
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
        __enable_irq();
        wait_ms(100);
        halInit();
        chSysInit();
    chSysUnlock();

    soft_reset_keyboard();

    return true;
}

bool send_basic_keycodes(report_keyboard_t *report) {
    wakeup_before_input();

    if (current_state != STATE_CONNECTED) {
        return false;
    }

    static const uint8_t *base_command = WRITE_COMMAND(COMMAND_HID, "\0\0\0\0\0\0\0\0\0");
    uint8_t command[18];
    memcpy(command, base_command, 18);
    command[7] = 0x08; // length
    command[8] = 0x01; // type: basic keycode
    command[9] = report->mods;
    command[10] = report->keys[0];
    command[11] = report->keys[1];
    command[12] = report->keys[2];
    command[13] = report->keys[3];
    command[14] = report->keys[4];
    command[15] = report->keys[5];

    send_buffer(command, 18);
    return true;
}

bool send_mouse_keycodes(report_mouse_t *report) {
    wakeup_before_input();

    if (current_state != STATE_CONNECTED) {
        return false;
    }

    static const uint8_t *base_command = WRITE_COMMAND(COMMAND_HID, "\0\0\0\0\0\0\0\0\0");
    uint8_t command[18];
    memcpy(command, base_command, 18);
    command[7] = 0x08; // length
    command[8] = 0x05; // type: mouse
    command[9] = report->buttons;

    int16_t x = report->x;
    command[10] = x & 0xFF;
    command[11] = x >> 8 & 0xFF;
    int16_t y = report->y;
    command[12] = y & 0xFF;
    command[13] = y >> 8 & 0xFF;
    command[14] = report->v;
    command[15] = report->h;

    send_buffer(command, 18);
    return true;
}

bool send_consumer_keycodes(uint16_t usage) {
    wakeup_before_input();

    if (current_state != STATE_CONNECTED) {
        return false;
    }

    static const uint8_t *base_command = WRITE_COMMAND(COMMAND_HID, "\0\0\0\0\0\0\0\0");
    uint8_t command[17];
    memcpy(command, base_command, 17);
    command[7] = 0x07; // length
    command[8] = 0x02; // type: consumer
    command[9] = usage & 0xFF;
    command[10] = usage >> 8 & 0xFF;

    send_buffer(command, 17);
    return true;
}

enum BtCommState initial_state(enum BtCommEvent event) {
    // bluetooth_init からの start_preparation 呼び出しで状態遷移するのでイベントは全スルー
    return STATE_INITIAL;
}

enum BtCommState preparing_state(enum BtCommEvent event) {
    switch (event) {
        case PREPARATION_COMPLETED:
            return STATE_IDLE;
        default:
            return STATE_PREPARING;
    }
}

enum BtCommState idle_state(enum BtCommEvent event) {
    switch (event) {
        case PAIRING_STARTED:
            set_bluetooth_indicator(BT_IND_PAIRING);
            return STATE_PAIRING;
        case CONNECTED:
            set_bluetooth_indicator(BT_IND_CONNECTED);
            return STATE_CONNECTED;
        case CONNECTION_STARTED:
            set_bluetooth_indicator(BT_IND_CONNECTING);
            return STATE_CONNECTING;
        case ENTER_DEEP_SLEEP_BY_USER:
            return STATE_DEEPSLEEP_BY_USER;
        case MODULE_SLEPT:
        case MODULE_DEEP_SLEPT:
            return STATE_IDLE_SLEEPING;
        default:
            return STATE_IDLE;
    }
}

enum BtCommState pairing_state(enum BtCommEvent event) {
    switch (event) {
        case CONNECTED:
            set_bluetooth_indicator(BT_IND_CONNECTED);
            return STATE_CONNECTED;
        case CONNECTION_STARTED:
            set_bluetooth_indicator(BT_IND_CONNECTING);
            return STATE_CONNECTING;
        case DISCONNECTION_STARTED:
            return STATE_DISCONNECTING;
        case DISCONNECTED:
            set_bluetooth_indicator(BT_IND_IDLE);
            return STATE_IDLE;
        case ENTER_DEEP_SLEEP_BY_USER:
            return STATE_DEEPSLEEP_BY_USER;
        case MODULE_SLEPT:
        case MODULE_DEEP_SLEPT:
            return STATE_IDLE_SLEEPING;
        default:
            return STATE_PAIRING;
    }
}

enum BtCommState connecting_state(enum BtCommEvent event) {
    switch (event) {
        case PAIRING_STARTED:
            set_bluetooth_indicator(BT_IND_PAIRING);
            return STATE_PAIRING;
        case CONNECTED:
            set_bluetooth_indicator(BT_IND_CONNECTED);
            return STATE_CONNECTED;
        case DISCONNECTION_STARTED:
            return STATE_DISCONNECTING;
        case DISCONNECTED:
            set_bluetooth_indicator(BT_IND_IDLE);
            return STATE_IDLE;
        case MODULE_SLEPT:
        case MODULE_DEEP_SLEPT:
            return STATE_IDLE_SLEEPING;
        default:
            return STATE_CONNECTING;
    }
}

enum BtCommState connected_state(enum BtCommEvent event) {
    switch (event) {
        case PAIRING_STARTED:
            set_bluetooth_indicator(BT_IND_PAIRING);
            return STATE_PAIRING;
        case CONNECTION_STARTED:
            set_bluetooth_indicator(BT_IND_CONNECTING);
            return STATE_CONNECTING;
        case DISCONNECTION_STARTED:
            return STATE_DISCONNECTING;
        case DISCONNECTED:
            set_bluetooth_indicator(BT_IND_IDLE);
            return STATE_IDLE;
        case ENTER_DEEP_SLEEP_BY_USER:
            return STATE_DEEPSLEEP_BY_USER;
        case MODULE_SLEPT:
        case MODULE_DEEP_SLEPT:
            return STATE_CONNECTED_SLEEPING;
        default:
            return STATE_CONNECTED;
    }
}

enum BtCommState disconnecting_state(enum BtCommEvent event) {
    switch (event) {
        case PAIRING_STARTED:
            set_bluetooth_indicator(BT_IND_PAIRING);
            return STATE_PAIRING;
        case CONNECTION_STARTED:
            set_bluetooth_indicator(BT_IND_CONNECTING);
            return STATE_CONNECTING;
        case CONNECTED:
            set_bluetooth_indicator(BT_IND_CONNECTED);
            return STATE_CONNECTED;
        case DISCONNECTED:
            set_bluetooth_indicator(BT_IND_IDLE);
            return STATE_IDLE;
        case ENTER_DEEP_SLEEP_BY_USER:
            return STATE_DEEPSLEEP_BY_USER;
        case MODULE_SLEPT:
        case MODULE_DEEP_SLEPT:
            return STATE_IDLE_SLEEPING;
        default:
            return STATE_DISCONNECTING;
    }
}

enum BtCommState idle_sleeping_state(enum BtCommEvent event) {
    switch (event) {
        case PAIRING_STARTED:
            set_bluetooth_indicator(BT_IND_PAIRING);
            return STATE_PAIRING;
        case CONNECTION_STARTED:
            set_bluetooth_indicator(BT_IND_CONNECTING);
            return STATE_CONNECTING;
        case CONNECTED:
            set_bluetooth_indicator(BT_IND_CONNECTED);
            return STATE_CONNECTED;
        case DISCONNECTED:
            set_bluetooth_indicator(BT_IND_IDLE);
            return STATE_IDLE;
        case ENTER_DEEP_SLEEP_BY_USER:
            return STATE_DEEPSLEEP_BY_USER;
        default:
            return STATE_IDLE_SLEEPING;
    }
}

enum BtCommState connected_sleeping_state(enum BtCommEvent event) {
    switch (event) {
        case PAIRING_STARTED:
            set_bluetooth_indicator(BT_IND_PAIRING);
            return STATE_PAIRING;
        case CONNECTION_STARTED:
            set_bluetooth_indicator(BT_IND_CONNECTING);
            return STATE_CONNECTING;
        case CONNECTED:
            set_bluetooth_indicator(BT_IND_CONNECTED);
            return STATE_CONNECTED;
        case DISCONNECTED:
            set_bluetooth_indicator(BT_IND_IDLE);
            return STATE_IDLE;
        case ENTER_DEEP_SLEEP_BY_USER:
            return STATE_DEEPSLEEP_BY_USER;
        default:
                return STATE_CONNECTED_SLEEPING;
    }

}

enum BtCommState deepsleep_by_user_state(enum BtCommEvent event) {
    switch (event) {
        case MODULE_DEEP_SLEPT:
            deepsleep();
            return STATE_IDLE;
        default:
            return STATE_DEEPSLEEP_BY_USER;
    }
}

enum BtCommState (*handler[10])(enum BtCommEvent) = {
    initial_state,
    preparing_state,
    idle_state,
    pairing_state,
    connecting_state,
    connected_state,
    disconnecting_state,
    idle_sleeping_state,
    connected_sleeping_state,
    deepsleep_by_user_state
};

enum BtCommState handle_event(enum BtCommEvent event) {
    return (*handler[current_state])(event);
}

enum BtCommEvent to_event(const uint8_t* received_command) {
    if (is_expected_notification(received_command, COMMAND_EVENT, "0001")) {
        return MODULE_BOOTED;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0002")) {
        return PAIRING_STARTED;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0004")) {
        return CONNECTED;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0008")) {
        return CONNECTION_STARTED;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0010")) {
        return DISCONNECTED;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0020")) {
        return MODULE_WAKED_UP;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0040")) {
        return MODULE_SLEPT;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0080")) {
        return MODULE_DEEP_SLEPT;
    } else {
        return UNKNOWN;
    }
}

static uint8_t read_command_buffer[64] = { 0 };
enum BtCommState handle_received_command(const uint8_t* received_command) {
    // 受信コマンドをイベントに変換
    enum BtCommEvent event = to_event(received_command);
    uprintf("----------------\nreceived: %s\nevent: %d\n", received_command, event);
    if (event != UNKNOWN) {
        // イベントに変換できたので発火
        return handle_event(event);
    }
    if (!current_task) {
        // 実行中のタスクがなく、不明なコマンドを受信したので無視
        return current_state;
    }
    // 実行中のタスクを進行
    TaskArgs args;
    args.current_slot = current_slot;
    args.received_command = received_command;
    co_set_addrword(current_task, &args);
    co_resume_t result = co_call(current_task);;
    if (result == CO_INVALID) {
        event = co_get_retval(current_task, enum BtCommEvent);
        current_task = NULL;
        if (event) {
            return handle_event(event);
        }
    }
    return current_state;
}

static THD_WORKING_AREA(waReadThread, 256);
static THD_FUNCTION(ReadThread, arg) {
    chRegSetThreadName("communication");

    event_listener_t el;
    eventflags_t flags;
    chEvtRegisterMaskWithFlags(chnGetEventSource(&SERIAL_USART_DRIVER), &el, EVENT_MASK(0), CHN_INPUT_AVAILABLE);
    while (true) {
        chEvtWaitAny(EVENT_MASK(0));
    	flags = chEvtGetAndClearFlags(&el);
        if (flags & CHN_INPUT_AVAILABLE) {
            read_from_uart();
            if (read_command_from_buffer(read_command_buffer)) {
                current_state = handle_received_command(read_command_buffer);
                uprintf("  state: %d\n", current_state);
                if (current_state == STATE_ERROR) {
                    break;
                }
            }
        }
    }
}

void start_control(void) {
    chThdCreateStatic(waReadThread, sizeof(waReadThread), COMMUNICATION_THREAD_PRIORITY, ReadThread, NULL);
    start_communication();
}

// ================================
// Call by QMK
// ================================

void bluetooth_init(void) {
    start_control();
    send_wakeup();
    start_preparation();
}

void bluetooth_task(void) {}

void bluetooth_send_keyboard(report_keyboard_t *report) {
    send_basic_keycodes(report);
}

void bluetooth_send_mouse(report_mouse_t *report) {
    send_mouse_keycodes(report);
}

void bluetooth_send_consumer(uint16_t usage) {
    send_consumer_keycodes(usage);
}
