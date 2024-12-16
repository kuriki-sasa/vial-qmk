#include "tps40_bt_controller.h"

#include "tps40_bt_tasks.h"
#include "tps40_bt_communication.h"
#include "tps40_bt_event.h"
#include "coroutine.h"
#include "print.h"

coroutine_t current_task = NULL;

int coroutine_stack[128];
void start_initialization(void) {
    current_task = co_create(initialize_task, 0, coroutine_stack, sizeof(coroutine_stack));
    co_call(current_task);
}

static enum BtCommState current_state = STATE_UNSUPPORTED;

enum BtCommState unsupported_state(enum BtCommEvent event) {
    switch (event) {
        case MODULE_PREPARED:
            start_initialization();
            return STATE_INITIALIZING;
        default:
            return STATE_UNSUPPORTED;
    }
}

enum BtCommState initialize_state(enum BtCommEvent event) {
    switch (event) {
        case INITIALIZE_COMPLETED:
            return STATE_IDLE;
        default:
            return STATE_INITIALIZING;
    }
}

enum BtCommState idle_state(enum BtCommEvent event) {
    return STATE_IDLE;
}

enum BtCommState (*handler[3])(enum BtCommEvent) = {
    unsupported_state,
    initialize_state,
    idle_state
};

enum BtCommState fire_event(enum BtCommEvent event) {
    return (*handler[current_state])(event);
}

enum BtCommEvent to_event(const uint8_t* received_command) {
    if (is_expected_notification(received_command, COMMAND_EVENT, "0001")) {
        return MODULE_BOOTED;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0002")) {
        return PAIRING_EXITED;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0004")) {
        return CONNECTED;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0008")) {
        return CONNECTION_STARTED;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0010")) {
        return DISCONNECTED;
    } else if (is_expected_notification(received_command, COMMAND_EVENT, "0020")) {
        return MODULE_PREPARED;
    } else {
        return UNKNOWN;
    }
}

static uint8_t read_command_buffer[64] = { 0 };


void handle_received_command(const uint8_t* received_command) {
    // 受信コマンドをイベントに変換
    enum BtCommEvent event = to_event(received_command);
    uprintf("----------------\nreceived: %s\nevent: %d\n", received_command, event);
    if (event != UNKNOWN) {
        // イベントに変換できたので発火
        current_state = fire_event(event);
        return;
    }
    if (!current_task) {
        // 実行中のタスクがなく、不明なコマンドを受信したので無視
        return;
    }
    // 実行中のタスクを進行
    co_set_addrword(current_task, received_command);
    co_resume_t result = co_call(current_task);;
    if (result == CO_INVALID) {
        event = co_get_retval(current_task, enum BtCommEvent);
        uprintf("new event!!!!!!!: %d\n", event);
        if (event) {
            current_state = fire_event(event);
        }
        current_task = NULL;
    }
    uprintf("current_state: %d\n", current_state);
}

static THD_WORKING_AREA(waReadThread, 256);
static THD_FUNCTION(ReadThread, arg) {
    chRegSetThreadName("communication");

    event_listener_t el;
    eventflags_t flags;
    chEvtRegisterMask(chnGetEventSource(&SD2), &el, EVENT_MASK(0));
    while (true) {
        chEvtWaitAny(EVENT_MASK(0));
        osalSysLock();
    	flags = chEvtGetAndClearFlags(&el);
        if (flags & CHN_INPUT_AVAILABLE) {
            read_from_uart();
            if (read_command_from_buffer(read_command_buffer)) {
                handle_received_command(read_command_buffer);
            }
        }
        osalSysUnlock();
    }
}

void start_control(void) {
    chThdCreateStatic(waReadThread, sizeof(waReadThread), NORMALPRIO + 8, ReadThread, NULL);
    start_communication();
}
