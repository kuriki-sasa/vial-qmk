#include "tps40_usart.h"

#include "quantum.h"
#include <hal.h>

#include <print.h>

#define WAKEUP_FREAM_TIME_US 2000

static SerialConfig serial_config = {
    .speed = (SERIAL_USART_SPEED),
    .ctrl1 = (0x00),
    .ctrl2 = (0x00),
    .ctrl3 = (0x00)
};

static SerialDriver* serial_driver = &SERIAL_USART_DRIVER;

/**
 * @brief SERIAL Driver startup routine.
 */
void tps40_usart_driver_start(void) {
    osalSysLock();

    palSetLineMode(SERIAL_USART_TX_PIN, PAL_MODE_ALTERNATE_PUSHPULL);
    palSetLineMode(SERIAL_USART_RX_PIN, PAL_MODE_INPUT);
    sdStart(serial_driver, &serial_config);

    osalSysUnlock();
}

bool tps40_usart_send(const uint8_t* source, const size_t size) {
    // serial_driver->usart->CTRL1 |= USART_CTRL1_SBF;
    // uprintf("LINEN: %d, SBF: %d\n", (serial_driver->usart->CTRL2 & USART_CTRL2_LINEN) != 0, (serial_driver->usart->CTRL1 & USART_CTRL1_SBF) != 0);
    // while ((serial_driver->usart->CTRL1 & USART_CTRL1_SBF) != 0);
    return (size_t)chnWriteTimeout(serial_driver, source, size, TIME_MS2I(SERIAL_USART_TIMEOUT)) == size;
}

size_t tps40_usart_receive(uint8_t* destination, const size_t max_size) {
    return (size_t)chnReadTimeout(serial_driver, destination, max_size, TIME_IMMEDIATE);
}

bool tps40_wakeup_frame_send(void) {
    osalSysLock();

    palSetLineMode(SERIAL_USART_TX_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    writePin(SERIAL_USART_TX_PIN, false);
    wait_us(WAKEUP_FREAM_TIME_US);
    writePin(SERIAL_USART_TX_PIN, true);
    palSetLineMode(SERIAL_USART_TX_PIN, PAL_MODE_ALTERNATE_PUSHPULL);

    osalSysUnlock();
    return true;
}