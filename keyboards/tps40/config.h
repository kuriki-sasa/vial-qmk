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
#define TPS40_BOARD_NAME               "TPS-40-CE"
#define TPS40_IDLETIMES_SEC                    600
#define TPS40_VIDPID                    "1EA71F46"
#define TPS40_WAKEUP_DELAY_MSEC                100
#define TPS40_WAKEUP_DELAY_INPUT_MSEC           30
#define TPS40_BT_COMMAND_INTERVAL_MSEC         100

/*
 * Deep sleep settings
 */
#define TPS40_WAKEUP_KEY_OUTPUT_PIN             B1
#define TPS40_WAKEUP_KEY_OBSERVE_PIN           A10

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

/*
 * LED blink interval
 */
#define BLINK_THREAD_INTERVAL_MSEC 1000
#define BLINK_SLOW_INTERVAL_MSEC   1000
#define BLINK_MID_INTERVAL_MSEC     600
#define BLINK_FAST_INTERVAL_MSEC    200

/*
 * Thread priority management
 */
#define LED_BLINK_SLOW_THREAD_PRIORITY     NORMALPRIO + 1
#define LED_BLINK_MID_THREAD_PRIORITY      NORMALPRIO + 2
#define LED_BLINK_FAST_THREAD_PRIORITY     NORMALPRIO + 3
#define BATTERY_MONITORING_THREAD_PRIORITY NORMALPRIO + 16
#define COMMUNICATION_THREAD_PRIORITY      NORMALPRIO + 32

#define PWC_CTRL_VRSEL                   (0x1U << 0)
#define PWC_CTRL_LPSEL                   (0x1U << 1)