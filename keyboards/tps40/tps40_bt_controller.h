#pragma once

#include "report.h"

enum BtCommState {
    STATE_INITIAL = 0,

    STATE_INITIALIZING,

    STATE_IDLE,

    STATE_PAIRING,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTING,

    STATE_ERROR
};

bool start_discovering(int slot);
bool start_connection(int slot);
bool start_disconnection(void);

bool send_basic_keycodes(report_keyboard_t *report);
bool send_mouse_keycodes(report_mouse_t *report);
bool send_consumer_keycodes(uint16_t usage);