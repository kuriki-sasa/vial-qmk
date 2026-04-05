#pragma once

/* VBUS detection pin for master/slave determination */
#define USB_VBUS_PIN GP26

/* serial driver setting */
#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_DRIVER SIOD0
#define SERIAL_USART_TX_PIN GP0
#define SERIAL_USART_RX_PIN GP1
