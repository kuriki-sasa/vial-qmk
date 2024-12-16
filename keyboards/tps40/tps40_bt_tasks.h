#pragma once

#include "coroutine.h"

typedef struct task_args {
    int current_slot;
    const uint8_t* received_command;
} TaskArgs;

int initialize_task(coroutine_t coroutine);
int start_discovering_task(coroutine_t coroutine);
int start_connection_task(coroutine_t coroutine);
int start_disconnection_task(coroutine_t coroutine);