#ifndef __CONF_BOARD_H_INCLUDED__
#define __CONF_BOARD_H_INCLUDED__

#include <stdlib.h>
#include "stm32l1xx.h"
#include "dev_cpu.h"
#include "gpio.h"
#include "spi.h"
#include "sx1257.h"


/** SX1257 SPI settings **/
#define SX1257_SPI_MOSI_port          1
#define SX1257_SPI_MOSI_pin           15

#define SX1257_SPI_MISO_port          1
#define SX1257_SPI_MISO_pin           14

#define SX1257_SPI_SCLK_port          1
#define SX1257_SPI_SCLK_pin           13

#define SX1257_SPI_NSS_port           1
#define SX1257_SPI_NSS_pin            12

#define SX1257_SPI                    SPI2

/** SX1257 NRESET pin **/
#define SX1257_RESET_port             0
#define SX1257_RESET_pin              3

/** Board LEDs pin **/
#define BOARD_LED_BLUE_port           1 //1 - PORTB
#define BOARD_LED_BLUE_pin            6
#define BOARD_LED_GREEN_port          1 //1 - PORTB
#define BOARD_LED_GREEN_pin           7

/** Board BUTTON pin **/
#define BOARD_BUTTON_port             0 //0 - PORTA
#define BOARD_BUTTON_pin              0

#endif /* __CONF_BOARD_H_INCLUDED__ */
