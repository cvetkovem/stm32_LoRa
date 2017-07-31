#include "conf_board.h"

#define RF_FREQUENCY                    868000000 // Hz
#define TX_OUTPUT_POWER                 14        // dBm

#define LORA_BANDWIDTH                  0         // [0: 125 kHz,
                                                  //  1: 250 kHz,
                                                  //  2: 500 kHz,
                                                  //  3: Reserved]
#define LORA_SPREADING_FACTOR           7         // [SF7..SF12]
#define LORA_CODINGRATE                 1         // [1: 4/5,
                                                  //  2: 4/6,
                                                  //  3: 4/7,
                                                  //  4: 4/8]
#define LORA_PREAMBLE_LENGTH            8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT             5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON      false
#define LORA_IQ_INVERSION_ON            false
#define LORA_CRC_ON                     true

#define RX_TIMEOUT_VALUE                1000000   // uS
#define TX_TIMEOUT_VALUE                300000    // uS
#define BUFFER_SIZE                     64        // Define the payload size here

uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

/* Radio events function pointer */
static RadioEvents_t RadioEvents;
/* Function to be executed on Radio Tx Done event */
void OnTxDone(void);
/* Function to be executed on Radio Rx Done event */
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
/* Function executed on Radio Tx Timeout event */
void OnTxTimeout(void);
/* Function executed on Radio Rx Timeout event */
void OnRxTimeout(void);
/* Function executed on Radio Rx Error event */
void OnRxError(void);

void boardInit(void);
void radio_init(void);
void send_on_off(void);
void led_green_off(void);
void TIM6_IRQHandler(void);

Gpio_t LED_BLUE, LED_GREEN, BUTTON;
extern SX1276_t SX1276;

TimerEvent_t led_off_timer;

int main()
{
    uint8_t i;

    for(i = 0; i < BufferSize; i++) {
        Buffer[i] = i;
    }

    TimerInit(&led_off_timer, led_green_off);

    boardInit();
    radio_init();

    while(1) {
        __WFI();
    }

    return 0;
}

void radio_init(void) {
    // Radio initialization
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;

    SX1276Init(&RadioEvents);
    SX1276SetChannel(RF_FREQUENCY);

    SX1276SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   LORA_CRC_ON, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

/*
    SX1276SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, LORA_CRC_ON, 0, 0, LORA_IQ_INVERSION_ON, true);
*/
    //SX1276SetRx(RX_TIMEOUT_VALUE);
}

void boardInit(void) {
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

    /* LoRa SPI settings */
    SX1276.Spi.slave_on = 0; // Master
    SX1276.Spi.Spi = (uint32_t *)LORA_SPI;
    SX1276.Spi.Mosi.pinIndex  = LORA_SPI_MOSI_pin;
    SX1276.Spi.Mosi.portIndex = LORA_SPI_MOSI_port;
    SX1276.Spi.Miso.pinIndex  = LORA_SPI_MISO_pin;
    SX1276.Spi.Miso.portIndex = LORA_SPI_MISO_port;
    SX1276.Spi.Sclk.pinIndex  = LORA_SPI_SCLK_pin;
    SX1276.Spi.Sclk.portIndex = LORA_SPI_SCLK_port;
    SX1276.Spi.Nss.pinIndex   = LORA_SPI_NSS_pin;
    SX1276.Spi.Nss.portIndex  = LORA_SPI_NSS_port;
    SX1276.Spi.bits = SPI_8_BIT;
    SX1276.Spi.cpha = 0;
    SX1276.Spi.cpol = 0;
    SX1276.Spi.f_pclk = SPI_F_PCLK_128;
    SX1276.Spi.msb_lsb = SPI_MSB;

    SpiInit(&(SX1276.Spi));

    /* Set other LoRa pins */
    SX1276.Reset.pinIndex  = LORA_NRESET_pin;
    SX1276.Reset.portIndex = LORA_NRESET_port;
    SX1276.DIO0.pinIndex   = LORA_DIO0_pin;
    SX1276.DIO0.portIndex  = LORA_DIO0_port;
    SX1276.DIO1.pinIndex   = LORA_DIO1_pin;
    SX1276.DIO1.portIndex  = LORA_DIO1_port;
    SX1276.DIO2.pinIndex   = LORA_DIO2_pin;
    SX1276.DIO2.portIndex  = LORA_DIO2_port;

    /* Set TXEN, RXEN pins */
    SX1276.AntSwitchLf.pinIndex  = LORA_ANT_SWITCH_LF_pin;
    SX1276.AntSwitchLf.portIndex = LORA_ANT_SWITCH_LF_port;
    SX1276.AntSwitchHf.pinIndex  = LORA_ANT_SWITCH_HF_pin;
    SX1276.AntSwitchHf.portIndex = LORA_ANT_SWITCH_HF_port;

    SX1276IoInit();

    NVIC_SetPriority(TIM4_IRQn, 8);
    //NVIC_EnableIRQ(TIM6_IRQn);
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    TIM4->PSC = 0xA40F; //41999
    TIM4->ARR = 0x018F;//0x07CF; //1999
    TIM4->DIER |= TIM_DIER_UIE;
    TIM4->CR1 |= TIM_CR1_CEN;

    TimerHwInit();
}

void TIM4_IRQHandler(void)
{
    TIM4->SR &= ~TIM_SR_UIF;

    SX1276Send(Buffer, BufferSize);
}

volatile uint8_t send_enable = false;
void send_on_off(void) {
    if(!send_enable) {
        GpioWrite(&LED_BLUE, 1);
        NVIC_EnableIRQ(TIM4_IRQn);
    } else {
        GpioWrite(&LED_BLUE, 0);
        NVIC_DisableIRQ(TIM4_IRQn);
    }

    send_enable = !send_enable;
}

void led_green_off(void)
{
    GpioWrite(&LED_GREEN, 0);
}

void OnTxDone(void)
{
    __NOP();

    GpioWrite(&LED_GREEN, 1);

    TimerSetValue(&led_off_timer, 125000);
    TimerStart(&led_off_timer);

    SX1276SetSleep();
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    __NOP();
    //SX1276SetSleep();
}

void OnTxTimeout(void)
{
    __NOP();
    SX1276SetSleep();
}

void OnRxTimeout(void)
{
    __NOP();
    //SX1276SetSleep();
}

void OnRxError(void)
{
    __NOP();
    //SX1276SetSleep();
}
