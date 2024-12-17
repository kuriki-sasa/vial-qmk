#include "tps40_usart.h"

#include "quantum.h"
#include <hal.h>

#include <print.h>

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
    palSetLineMode(SERIAL_USART_TX_PIN, PAL_MODE_ALTERNATE_PUSHPULL);
    palSetLineMode(SERIAL_USART_RX_PIN, PAL_MODE_INPUT);
    sdStart(serial_driver, &serial_config);
}

bool tps40_usart_send(const uint8_t* source, const size_t size) {
    return (size_t)chnWriteTimeout(serial_driver, source, size, TIME_MS2I(SERIAL_USART_TIMEOUT)) == size;
}

size_t tps40_usart_receive(uint8_t* destination, const size_t max_size) {
    return (size_t)chnReadTimeout(serial_driver, destination, max_size, TIME_IMMEDIATE);
}
