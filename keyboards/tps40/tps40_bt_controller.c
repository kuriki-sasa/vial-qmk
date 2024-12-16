#include "tps40_bt_controller.h"

#include "tps40_bt_tasks.h"
#include "tps40_bt_communication.h"
#include "tps40_bt_event.h"
#include "coroutine.h"
#include "print.h"

coroutine_t current_task = NULL;
void run_task(coroutine_t task) {
    current_task = task;
    co_call(current_task);
}

int current_slot = 1;
int coroutine_stack[128];
void start_initialization(void) {
    coroutine_t task = co_create(initialize_task, 0, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
}

void start_discovering(int slot) {
    current_slot = slot;
    TaskArgs args;
    args.current_slot = slot;
    args.received_command = NULL;
    coroutine_t task = co_create(start_discovering_task, &args, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
}

void start_connection(int slot) {
    TaskArgs args;
    args.current_slot = slot;
    args.received_command = NULL;
    coroutine_t task = co_create(start_connection_task, 0, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
}

void start_disconnection(void) {
    coroutine_t task = co_create(start_disconnection_task, 0, coroutine_stack, sizeof(coroutine_stack));
    run_task(task);
}

static enum BtCommState current_state = STATE_INITIAL;

enum BtCommState initial_state(enum BtCommEvent event) {
    switch (event) {
        case MODULE_PREPARED:
            start_initialization();
            return STATE_INITIALIZING;
        default:
            return STATE_INITIAL;
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
    initial_state,
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
        return PAIRING_STARTED;
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
    TaskArgs args;
    args.current_slot = current_slot;
    args.received_command = received_command;
    co_set_addrword(current_task, &args);
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
    chThdCreateStatic(waReadThread, sizeof(waReadThread), NORMALPRIO + 32, ReadThread, NULL);
    start_communication();
}
