#pragma once

#include "coroutine.h"

typedef struct task_args {
    int current_slot;
    const uint8_t* received_command;
} TaskArgs;

int preparation_task(coroutine_t coroutine);
int start_discovering_task(coroutine_t coroutine);
int start_connection_task(coroutine_t coroutine);
int start_disconnection_task(coroutine_t coroutine);
int start_reconnection_last_slot_task(coroutine_t coroutine);

int disable_auto_idle_task(coroutine_t coroutine);
int enable_auto_idle_task(coroutine_t coroutine);
int enter_deepsleep_task(coroutine_t coroutine);

int set_keyboard_task(coroutine_t coroutine);
int get_keyboard_task(coroutine_t coroutine);
