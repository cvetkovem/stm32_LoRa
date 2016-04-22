#include "conf_board.h"

Gpio_t LED_BLUE, LED_GREEN, BUTTON;

uint8_t tmp = 1;
void send_on_off(void)
{
    tmp = !tmp;
    GpioWrite(&LED_BLUE, tmp);
}

int main(void)
{
    cpuInit();

    /* LEDs settings */
    LED_BLUE.pinIndex = BOARD_LED_BLUE_pin;
    LED_BLUE.portIndex = BOARD_LED_BLUE_port;
    GpioInit(&LED_BLUE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    LED_GREEN.pinIndex = BOARD_LED_GREEN_pin;
    LED_GREEN.portIndex = BOARD_LED_GREEN_port;
    GpioInit(&LED_GREEN, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /* Button settings */
    BUTTON.pinIndex = BOARD_BUTTON_pin;
    BUTTON.portIndex = BOARD_BUTTON_port;
    GpioInit(&BUTTON, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioSetInterrupt(&BUTTON, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, send_on_off);

    //GpioRemoveInterrupt(&BUTTON);

    GpioWrite(&LED_BLUE, 1);
    GpioWrite(&LED_GREEN, 1);

    //GpioDeInit(&LED_GREEN);

    uint8_t i = 0;

    while(1)
    {
        i = !i;
        GpioWrite(&LED_GREEN, i);
        cpuDelay_ms(5000);
    }
}
