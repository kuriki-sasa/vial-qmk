#pragma once

enum BtCommState {
    STATE_UNSUPPORTED = 0,

    STATE_INITIALIZING,

    STATE_IDLE,

    STATE_PAIRING,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTING,
};

void start_control(void);
