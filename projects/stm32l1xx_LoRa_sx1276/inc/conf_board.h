#ifndef __CONF_BOARD_H_INCLUDED__
#define __CONF_BOARD_H_INCLUDED__

#include <stdlib.h>
#include "stm32l1xx.h"
#include "dev_cpu.h"
#include "gpio.h"
#include "spi.h"
#include "dev_hw_timer.h"
#include "timer.h"
#include "sx1276.h"


/** LoRa SPI settings **/
#define LORA_SPI_MOSI_port          1
#define LORA_SPI_MOSI_pin           15

#define LORA_SPI_MISO_port          1
#define LORA_SPI_MISO_pin           14

#define LORA_SPI_SCLK_port          1
#define LORA_SPI_SCLK_pin           13

#define LORA_SPI_NSS_port           1
#define LORA_SPI_NSS_pin            12

#define LORA_SPI                    SPI2

/** LoRa DIOx pins **/
#define LORA_DIO0_port              1
#define LORA_DIO0_pin               11

#define LORA_DIO1_port              1
#define LORA_DIO1_pin               10

#define LORA_DIO2_port              1
#define LORA_DIO2_pin               2

/** LoRa TXEN, RXEN pins **/
#define LORA_ANT_SWITCH_LF_port     0
#define LORA_ANT_SWITCH_LF_pin      5
// TX: LF=0, HF=1
// RX: LF=1, HF=0
#define LORA_ANT_SWITCH_HF_port     0
#define LORA_ANT_SWITCH_HF_pin      4

/** LoRa NRESET pin **/
#define LORA_NRESET_port            0
#define LORA_NRESET_pin             3

/** Board LEDs pin **/
#define BOARD_LED_BLUE_port         1 //1 - PORTB
#define BOARD_LED_BLUE_pin          6
#define BOARD_LED_GREEN_port        1 //1 - PORTB
#define BOARD_LED_GREEN_pin         7

/** Board BUTTON pin **/
#define BOARD_BUTTON_port           0 //0 - PORTA
#define BOARD_BUTTON_pin            0

#endif /* __CONF_BOARD_H_INCLUDED__ */
