#pragma once

enum BtCommState {
    STATE_INITIAL = 0,

    STATE_INITIALIZING,

    STATE_IDLE,

    STATE_PAIRING,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTING,
};

void start_control(void);
void start_discovering(void);
void start_connection(void);
void start_disconnection(void);