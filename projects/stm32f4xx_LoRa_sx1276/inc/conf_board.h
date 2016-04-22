#ifndef __CONF_BOARD_H_INCLUDED__
#define __CONF_BOARD_H_INCLUDED__

#include <stdlib.h>
#include "stm32f4xx.h"
#include "dev_cpu.h"
#include "gpio.h"
#include "spi.h"
#include "dev_hw_timer.h"
#include "timer.h"
#include "sx1276.h"


/** Board LEDs pin **/
#define BOARD_LED_BLUE_port         3 //3 - PORTD
#define BOARD_LED_BLUE_pin          15
#define BOARD_LED_GREEN_port        3 //3 - PORTD
#define BOARD_LED_GREEN_pin         12

/** Board BUTTON pin **/
#define BOARD_BUTTON_port           0 //0 - PORTA
#define BOARD_BUTTON_pin            0

#endif /* __CONF_BOARD_H_INCLUDED__ */
