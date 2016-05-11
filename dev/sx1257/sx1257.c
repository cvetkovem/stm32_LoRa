#include <stdlib.h>
#include "dev_cpu.h"
#include "gpio.h"
#include "spi.h"
#include "sx1257.h"

/* Radio hardware and global parameters */
SX1257_t SX1257;

/* Initializes the radio */
void SX1257Init(void)
{
    SX1257Reset();
    SX1257SetSleep();
}

void SX1257EnableRX(uint8_t enable)
{
    if(enable) {
        SX1257Write(SX1257_REG_MODE, (SX1257Read(SX1257_REG_MODE) | 0b00000010));
    } else {
        SX1257Write(SX1257_REG_MODE, (SX1257Read(SX1257_REG_MODE) & 0b11111101));
    }
}

void SX1257EnableTX(uint8_t enable)
{
    if(enable) {
        SX1257Write(SX1257_REG_MODE, (SX1257Read(SX1257_REG_MODE) | 0b00001100));
    } else {
        SX1257Write(SX1257_REG_MODE, (SX1257Read(SX1257_REG_MODE) & 0b11110011));
    }
}

void SX1257SetSleep(void)
{
    /* PA, TX, RX - disable, and select sleep mode */
    SX1257Write(SX1257_REG_MODE, (SX1257Read(SX1257_REG_MODE) & 0b11110000));
}

void SX1257SetStby(void)
{
    SX1257Write(SX1257_REG_MODE, (SX1257Read(SX1257_REG_MODE) | 0b00000001));
}

void SX1257SetRxConfig(uint32_t RxFreq, SX1257_RxLnaGain_t RxLnaGain,
                       uint8_t RxBasebandGain, SX1257_LnaZin_t LnaZin,
                       uint8_t RxAdcBw, uint8_t RxAdcTrim,
                       SX1257_RxBasebandBw_t RxBasebandBw, SX1257_RxPllBw_t RxPllBw)
{
    SX1257SetSleep();

    cpuDelay_ms(10);

    SX1257.Settings.RxFreq = RxFreq;
    RxFreq = (uint32_t)((double)RxFreq / (double)SX1257_FREQ_STEP);
    SX1257Write(SX1257_REG_FRF_RX_MSB, (uint8_t)(( RxFreq >> 16) & 0xFF));
    SX1257Write(SX1257_REG_FRF_RX_MID, (uint8_t)(( RxFreq >> 8) & 0xFF));
    SX1257Write(SX1257_REG_FRF_RX_LSB, (uint8_t)(RxFreq & 0xFF));

    SX1257.Settings.RxLnaGain = RxLnaGain;
    SX1257.Settings.RxBasebandGain = RxBasebandGain;
    SX1257.Settings.LnaZin = LnaZin;
    SX1257Write(SX1257_REG_RX_ANA_GAIN, ((((uint8_t)RxLnaGain) & 0x07) << 5) | ((((uint8_t)RxBasebandGain) & 0x0F) << 1) | (((uint8_t)LnaZin) & 0x01) );

    SX1257.Settings.RxAdcBw = RxAdcBw;
    SX1257.Settings.RxAdcTrim = RxAdcTrim;
    SX1257.Settings.RxBasebandBw = RxBasebandBw;
    SX1257Write(SX1257_REG_RX_BW, ((((uint8_t)RxAdcBw) & 0x07) << 5) | ((((uint8_t)RxAdcTrim) & 0x07) << 2) | (((uint8_t)RxBasebandBw) & 0x03) );

    SX1257.Settings.RxPllBw = RxPllBw;
    SX1257Write(SX1257_REG_RX_PLL_BW, ((((uint8_t)RxPllBw) & 0x03) << 1) );
}

void SX1257SetTxConfig(uint32_t TxFreq, SX1257_TxDacGain_t TxDacGain,
                       uint8_t TXMixerGain, SX1257_TxPllBw_t TxPllBw,
                       uint8_t TxAnaBw, uint8_t TxDacBw)
{
    SX1257SetSleep();

    cpuDelay_ms(10);

    SX1257.Settings.TxFreq = TxFreq;
    TxFreq = (uint32_t)((double)TxFreq / (double)SX1257_FREQ_STEP);
    SX1257Write(SX1257_REG_FRF_TX_MSB, (uint8_t)(( TxFreq >> 16) & 0xFF));
    SX1257Write(SX1257_REG_FRF_TX_MID, (uint8_t)(( TxFreq >> 8) & 0xFF));
    SX1257Write(SX1257_REG_FRF_TX_LSB, (uint8_t)(TxFreq & 0xFF));

    SX1257.Settings.TxDacGain = TxDacGain;
    SX1257.Settings.TXMixerGain = TXMixerGain;
    SX1257Write(SX1257_REG_TX_GAIN, ((((uint8_t)TxDacGain) & 0x07) << 4) | (((uint8_t)TXMixerGain) & 0x0F) );

    SX1257.Settings.TxPllBw = TxPllBw;
    SX1257.Settings.TxAnaBw = TxAnaBw;
    SX1257Write(SX1257_REG_TX_BW, ((((uint8_t)TxPllBw) & 0x03) << 5) | (((uint8_t)TxAnaBw) & 0x1F) );

    SX1257.Settings.TxDacBw = TxDacBw;
    SX1257Write(SX1257_REG_TX_DAC_BW, (((uint8_t)TxDacBw) & 0x07) );
}

void SX1257Reset(void)
{
    // Set RESET pin to 0
    GpioInit(&SX1257.Reset, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);

    // Wait 10 ms
    cpuDelay_ms(10);

    // Configure RESET as input
    GpioInit(&SX1257.Reset, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    //GpioInit(&SX1257.Reset, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);

    // Wait 20 ms
    cpuDelay_ms(20);
}

void SX1257Write(uint8_t addr, uint8_t data)
{
    SX1257WriteBuffer(addr, &data, 1);
}

uint8_t SX1257Read(uint8_t addr)
{
    uint8_t data;
    SX1257ReadBuffer(addr, &data, 1);
    return data;
}

void SX1257WriteBuffer(uint8_t addr, uint8_t *buffer, uint8_t size)
{
    uint8_t i;

    //NSS = 0;
    GpioWrite(&SX1257.Spi.Nss, 0);

    SpiInOut(&SX1257.Spi, addr | 0x80);
    for(i = 0; i < size; i++) {
        SpiInOut(&SX1257.Spi, buffer[i]);
    }

    //NSS = 1;
    GpioWrite(&SX1257.Spi.Nss, 1);
}

void SX1257ReadBuffer(uint8_t addr, uint8_t *buffer, uint8_t size)
{
    uint8_t i;

    //NSS = 0;
    GpioWrite(&SX1257.Spi.Nss, 0);

    SpiInOut(&SX1257.Spi, addr & 0x7F);

    for(i = 0; i < size; i++) {
        buffer[i] = SpiInOut(&SX1257.Spi, 0);
    }

    //NSS = 1;
    GpioWrite(&SX1257.Spi.Nss, 1);
}
