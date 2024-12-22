#pragma once

#define LED_NUM 3
#define LED_PINS { C8, C7, C6 }

#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE

#define SERIAL_USART_FULL_DUPLEX   // Enable full duplex operation mode.
#define SERIAL_USART_TX_PIN A2     // USART TX pin
#define SERIAL_USART_RX_PIN A3     // USART RX pin
#define SERIAL_USART_SPEED 115200
#define SERIAL_USART_TIMEOUT 10
#define SERIAL_USART_DRIVER SD2

/*
 * Board config
 */
#define AT32F415xB
#define AT32F415RBT7_7

#define AT32_HEXTCLK              16000000U

#define BOARD_OTG_NOVBUSSENS
#define USB_ENDPOINTS_ARE_REORDERABLE
#define EARLY_INIT_PERFORM_BOOTLOADER_JUMP TRUE

/*
 * BT settings
 */
#define TPS40_WAIT_COMMAND_TIMEOUT_COUNT 2500000
#define TPS40_BOARD_NAME "TPS-40-CE"
#define TPS40_IDLETIMES 600
#define TPS40_VIDPID "1EA71F46"

/*
 * ADC settings
 */
#define ADC_RESOLUTION ADC_CFGR1_RES_12BIT

/*
 * Battery monitoring
 */
#define CHARGE_STATE_PIN A0

/*
 * usb connection monitoring
 */
#define VBUS_PIN         A9