#pragma once

#include "report.h"

enum BtCommState {
    STATE_INITIAL = 0,

    STATE_PREPARING,

    STATE_IDLE,

    STATE_PAIRING,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTING,

    STATE_IDLE_SLEEPING,
    STATE_CONNECTED_SLEEPING,

    STATE_DEEPSLEEP_BY_USER,

    STATE_ERROR
};

void start_preparation(void);
bool start_discovering(int slot);
bool start_connection(int slot);
bool start_disconnection(void);
bool reconnect_last_slot(void);

bool enable_auto_sleep(void);
bool deepsleep(void);
void enter_deepsleep(void);

bool send_basic_keycodes(report_keyboard_t *report);
bool send_mouse_keycodes(report_mouse_t *report);
bool send_consumer_keycodes(uint16_t usage);
