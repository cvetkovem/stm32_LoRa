#include "conf_board.h"

#define RF_FREQUENCY                    868000000 // Hz

void boardInit(void);
void RadioSetRX(void);
void RadioSetTX(void);
void RX_or_TX(void);

Gpio_t LED_BLUE, LED_GREEN, BUTTON;
extern SX1257_t SX1257;

int main()
{
    volatile uint8_t tmp = 0;

    boardInit();
    SX1257Init();

//////////
/*
    SX1257SetLoopBack(0, 1);

    RadioSetRX();
    RadioSetTX();

    SX1257SetStby();

    while(!SX1257GetPllLockRx());
    while(!SX1257GetPllLockTx());

    GpioWrite(&LED_GREEN, 1);
    GpioWrite(&LED_BLUE, 1);
*/
//////////

    RX_or_TX();

    //tmp = SX1257Read(SX1257_REG_VERSION);

    while(1) {
        __WFI();
    }

    return 0;
}

void RadioSetRX(void)
{
    SX1257SetRxConfig(RF_FREQUENCY,
                      SX1257_RX_LNA_GAIN_0dB,
                      0x0C,                             // RxBasebandGain
                      SX1257_LNA_Z_IN_200Ohm,
                      0x07,                             // RxAdcBw (111 = 400kHz < RxAdcBw)
                      0x06,                             // RxAdcTrim (32MHz)
                      SX1257_RX_BASE_BAND_BW_500KHz,
                      SX1257_RX_PLL_BW_75KHz);

    SX1257EnableRX(1);
}

void RadioSetTX()
{
    SX1257SetTxConfig(RF_FREQUENCY,
                      SX1257_TX_DAC_GAIN_3dB,
                      0x0E,                             // TXMixerGain
                      SX1257_TX_PLL_BW_150KHZ,
                      0x00,                             // TxAnaBw (209KHz)
                      0x05);                            // TxDacBw

    SX1257EnableTX(1);
}

volatile uint8_t rx_tx = 0;
void RX_or_TX(void)
{
    rx_tx = !rx_tx;

    SX1257SetSleep();
    cpuDelay_ms(10);

    if(rx_tx) {
        RadioSetRX();

        GpioWrite(&LED_GREEN, 1); // RX enable
        GpioWrite(&LED_BLUE, 0);  // TX enable

        SX1257SetStby();
        while(!SX1257GetPllLockRx()) {
            cpuDelay_ms(1);
        }
    } else {
        RadioSetTX();

        GpioWrite(&LED_GREEN, 0); // RX enable
        GpioWrite(&LED_BLUE, 1);  // TX enable

        SX1257SetStby();
        while(!SX1257GetPllLockTx()) {
            cpuDelay_ms(1);
        }
    }
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
    GpioSetInterrupt(&BUTTON, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, RX_or_TX);

    /* SX1257 SPI settings */
    SX1257.Spi.slave_on = 0; // Master
    SX1257.Spi.Spi = (uint32_t *)SX1257_SPI;
    SX1257.Spi.Mosi.pinIndex  = SX1257_SPI_MOSI_pin;
    SX1257.Spi.Mosi.portIndex = SX1257_SPI_MOSI_port;
    SX1257.Spi.Miso.pinIndex  = SX1257_SPI_MISO_pin;
    SX1257.Spi.Miso.portIndex = SX1257_SPI_MISO_port;
    SX1257.Spi.Sclk.pinIndex  = SX1257_SPI_SCLK_pin;
    SX1257.Spi.Sclk.portIndex = SX1257_SPI_SCLK_port;
    SX1257.Spi.Nss.pinIndex   = SX1257_SPI_NSS_pin;
    SX1257.Spi.Nss.portIndex  = SX1257_SPI_NSS_port;
    SX1257.Spi.bits = SPI_8_BIT;
    SX1257.Spi.cpha = 0;
    SX1257.Spi.cpol = 0;
    SX1257.Spi.f_pclk = SPI_F_PCLK_128;
    SX1257.Spi.msb_lsb = SPI_MSB;

    SpiInit(&(SX1257.Spi));

    /* Set other LoRa pins */
    SX1257.Reset.pinIndex  = SX1257_RESET_pin;
    SX1257.Reset.portIndex = SX1257_RESET_port;
}
