#ifndef __CONF_BOARD_H_INCLUDED__
#define __CONF_BOARD_H_INCLUDED__

#include <stdlib.h>
#include "stm32l1xx.h"
#include "dev_cpu.h"
#include "gpio.h"
#include "dev_hw_timer.h"
#include "dac.h"

/** Timer IRQ event handler **/
void TimerIrqHandler(void);

/** DAC settings **/


/** Board LEDs pin **/
#define BOARD_LED_BLUE_port           1 //1 - PORTB
#define BOARD_LED_BLUE_pin            6
#define BOARD_LED_GREEN_port          1 //1 - PORTB
#define BOARD_LED_GREEN_pin           7

/** Board BUTTON pin **/
#define BOARD_BUTTON_port             0 //0 - PORTA
#define BOARD_BUTTON_pin              0

#endif /* __CONF_BOARD_H_INCLUDED__ */
