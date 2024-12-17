#include "tps40_bt_tasks.h"

#include "print.h"

#include "tps40_bt_event.h"
#include "tps40_bt_communication.h"

int initialize_task(coroutine_t coroutine) {
    TaskArgs* args = (TaskArgs*)co_get_addrword(coroutine);
    const uint8_t* received_command = args->received_command;
    //uint8_t* received_command = (uint8_t*)co_get_addrword(coroutine);

    co_begin_rettype(coroutine, enum BtCommEvent);

    // Send initial "AT" command
    send_run_command();

    co_yield();

    if (!is_success_response(received_command)) {
        uprintf("Initial sequence: step1 failed! %s\n", received_command);
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    // Set board name
    send_write_command(COMMAND_NAME_SETTINGS, TPS40_BOARD_NAME);

    co_yield();

    if (!is_success_response(received_command)) {
        uprintf("Initial sequence: step2 failed %s\n", received_command);
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    // Check the board name is set properly
    send_read_command(COMMAND_NAME_SETTINGS);

    co_yield();

    if (!is_expected_notification(received_command, COMMAND_NAME_SETTINGS, TPS40_BOARD_NAME)) {
        uprintf("Initial sequence: step3 failed! %s\n", received_command);
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    co_yield();

    if (!is_success_response(received_command)) {
        uprintf("Initial sequence: step4 failed! %s\n", received_command);
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    // Set idle settings
    send_write_command(COMMAND_AUTOIDLE_SETTINGS, DISABLE);

    co_yield();

    if (!is_success_response(received_command)) {
        uprintf("Initial sequence: step5 failed! %s\n", received_command);
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    send_write_command(COMMAND_IDLETIMES_SETTINGS, TO_STR_HELPER(TPS40_IDLETIMES));

    co_yield();

    if (!is_success_response(received_command)) {
        uprintf("Initial sequence: step6 failed! %s\n", received_command);
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    // Set VID and PID
    send_write_command(COMMAND_VIDPID_SETTINGS, TPS40_VIDPID);

    co_yield();

    if (!is_success_response(received_command)) {
        uprintf("Initial sequence: step7 failed! %s\n", received_command);
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    // Check VID and PID are set properly
    send_read_command(COMMAND_VIDPID_SETTINGS);

    co_yield();

    if (!is_expected_notification(received_command, COMMAND_VIDPID_SETTINGS, TPS40_VIDPID)) {
        uprintf("Initial sequence: step8 failed! %s\n", received_command);
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    co_yield();

    if (!is_success_response(received_command)) {
        uprintf("Initial sequence: step9 failed! %s\n", received_command);
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    // Set event settings
    send_write_command(COMMAND_EVENT, "00FF");

    co_yield();

    if (!is_success_response(received_command)) {
        uprintf("Initial sequence: step10 failed! %s\n", received_command);
        co_exit();
    }
    send_write_command(COMMAND_AUTOEVENT_SETTINGS, ENABLE);

    print("Initial sequece: completed!\n");
    co_end_ret(INITIALIZE_COMPLETED);
}

int start_discovering_task(coroutine_t coroutine) {
    TaskArgs* args = (TaskArgs*)co_get_addrword(coroutine);
    int slot = args->current_slot;
    const uint8_t* received_command = args->received_command;

    co_begin_rettype(coroutine, enum BtCommEvent);

    switch (slot) {
        case 1:
            send_write_command(COMMAND_DISCOVER, "1");
            break;
        case 2:
            send_write_command(COMMAND_DISCOVER, "2");
            break;
        case 3:
            send_write_command(COMMAND_DISCOVER, "3");
            break;
        default:
            return false;
    }

    co_yield();

    if (!is_success_response(received_command)) {
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    co_end_ret(UNKNOWN);
}

int start_connection_task(coroutine_t coroutine) {
    TaskArgs* args = (TaskArgs*)co_get_addrword(coroutine);
    int slot = args->current_slot;
    const uint8_t* received_command = args->received_command;

    co_begin_rettype(coroutine, enum BtCommEvent);

    switch (slot) {
        case 1:
            send_write_command(COMMAND_CONNECT, "1");
            break;
        case 2:
            send_write_command(COMMAND_CONNECT, "2");
            break;
        case 3:
            send_write_command(COMMAND_CONNECT, "3");
            break;
        default:
            return false;
    }

    co_yield();

    if (!is_success_response(received_command)) {
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    co_end_ret(UNKNOWN);
}

int start_disconnection_task(coroutine_t coroutine) {
    TaskArgs* args = (TaskArgs*)co_get_addrword(coroutine);
    const uint8_t* received_command = args->received_command;

    co_begin_rettype(coroutine, enum BtCommEvent);

    send_run_command(COMMAND_DISCONNECT);

    co_yield();

    if (!is_success_response(received_command)) {
        co_exit_ret(UNEXPECTED_COMMAND_RECEIVED);
    }

    co_end_ret(DISCONNECTION_STARTED);
}
