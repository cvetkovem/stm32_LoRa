#ifndef __SX1257_H__
#define __SX1257_H__

#define 	SX1257_REG_MODE	            0x00	// 0x00 Operating modes of the SX1257
#define	    SX1257_REG_FRF_RX_MSB	    0x01	// 0xCB RX carrier frequency MSB
#define	    SX1257_REG_FRF_RX_MID	    0x02	// 0x55 RX carrier frequency intermediate bits
#define	    SX1257_REG_FRF_RX_LSB	    0x03	// 0x55 RX carrier frequency LSB
#define	    SX1257_REG_FRF_TX_MSB	    0x04	// 0x00 TX carrier frequency MSB
#define	    SX1257_REG_FRF_TX_MID  	    0x05	// 0xCB TX carrier frequency intermediate bits
#define	    SX1257_REG_FRF_TX_LSB	    0x06	// 0x55 TX carrier frequency LSB
#define	    SX1257_REG_VERSION	        0x07	// 0x01 Semtech ID relating to the silicon revision
#define	    SX1257_REG_TX_GAIN	        0x08	// 0x2E TX DAC and mixer gain setting
#define	    SX1257_REG_TX_BW	        0x0A	// 0x30 TX FE PLL and analog filter bandwidths
#define	    SX1257_REG_TX_DAC_BW	    0x0B	// 0x02 TX DAC bandwidth
#define	    SX1257_REG_RX_ANA_GAIN	    0x0C	// 0x3F RX FE LNA and baseband amplifier gain
#define	    SX1257_REG_RX_BW	        0x0D	// 0xFD RX FE ADC and analog filter bandwidths
#define	    SX1257_REG_RX_PLL_BW	    0x0E	// 0x06 RX FE PLL bandwidth
#define	    SX1257_REG_DIO_MAPPING	    0x0F	// 0x00 Mapping of DIO pins
#define	    SX1257_REG_CLK_SELECT	    0x10	// 0x02 Sampling clock congfiguration
#define	    SX1257_REG_MODE_STATUS	    0x11	// 0x00 SX1257 mode status
#define	    SX1257_REG_LOW_BAT_THRES	0x1A	// 0x02 Low battery threshold

/* SX1257 definitions */
#define SX1257_XTAL_FREQ                32000000
#define SX1257_FREQ_STEP                61.03515625

typedef enum
{
    SX1257_TX_DAC_GAIN_9dB = 0,   // maximum gain 9 dB
    SX1257_TX_DAC_GAIN_6dB,       // maximum gain 6 dB
    SX1257_TX_DAC_GAIN_3dB,       // maximum gain 3 dB (default)
    SX1257_TX_DAC_GAIN_0dB,        // maximum gain (0 dB full scale)
}SX1257_TxDacGain_t;

typedef enum
{
    SX1257_TX_PLL_BW_75KHZ = 0,
    SX1257_TX_PLL_BW_150KHZ,
    SX1257_TX_PLL_BW_225KHZ,
    SX1257_TX_PLL_BW_300KHZ,
}SX1257_TxPllBw_t;

typedef enum
{
    SX1257_RX_LNA_GAIN_NOT_USED_FIRST = 0,
    SX1257_RX_LNA_GAIN_0dB,               // highest gain power 0 dB
    SX1257_RX_LNA_GAIN_6dB,               // highest gain power 6 dB
    SX1257_RX_LNA_GAIN_12dB,              // highest gain power 12 dB
    SX1257_RX_LNA_GAIN_24dB,              // highest gain power 24 dB
    SX1257_RX_LNA_GAIN_36dB,              // highest gain power 36 dB
    SX1257_RX_LNA_GAIN_48dB,              // highest gain power 48 dB
    SX1257_RX_LNA_GAIN_NOT_USED_END,
}SX1257_RxLnaGain_t;

typedef enum
{
    SX1257_LNA_Z_IN_50Ohm = 0,
    SX1257_LNA_Z_IN_200Ohm,                 // (default)
}SX1257_LnaZin_t;

typedef enum
{
    SX1257_RX_BASE_BAND_BW_750KHz = 0,
    SX1257_RX_BASE_BAND_BW_500KHz,          // (default)
    SX1257_RX_BASE_BAND_BW_375KHz,
    SX1257_RX_BASE_BAND_BW_250KHz,
}SX1257_RxBasebandBw_t;

typedef enum
{
    SX1257_RX_PLL_BW_75KHz = 0,
    SX1257_RX_PLL_BW_150KHz,
    SX1257_RX_PLL_BW_225KHz,
    SX1257_RX_PLL_BW_300KHz,
}SX1257_RxPllBw_t;

/* Radio parameters */
typedef struct
{
    uint8_t StandbyEnable;
    uint8_t RxEnable;
    uint8_t TxEnable;
    uint8_t PADriverEnable;

    uint32_t RxFreq;
    uint32_t TxFreq;

    /* Tx settings */
    SX1257_TxDacGain_t TxDacGain;
    uint8_t TXMixerGain;            // 4bit; Gain ~= -38dB + 2*TxMixerGain
    SX1257_TxPllBw_t TxPllBw;
    uint8_t TxAnaBw;                // 5bit; 17.5 / (2*(41 - TxAnaBw)) MHz
    uint8_t TxDacBw;                // 3bit; 0-5;  Number of taps of TX FIR-DAC, programmable: N = 24 + 8*TxDacBw, where N_max = 64

    /* Rx settings */
    SX1257_RxLnaGain_t RxLnaGain;
    uint8_t RxBasebandGain;         // 4bit; RX Baseband amplifier gain, programmable: Gain = Gain + 2*RxBasebandGain
    SX1257_LnaZin_t LnaZin;
    uint8_t RxAdcBw;                // 3bit; RX ADC BW, programmable: 010 = 100 kHz < RxAdcBw < 200 kHz; 101 = 200 kHz < RxAdcBw < 400 kHz; 111 = 400kHz < RxAdcBw
    uint8_t RxAdcTrim;              // 3bit; 32 MHz reference crystal: RxAdcTrim = 110; 36 MHz reference crystal: RxAdcTrim = 101
    SX1257_RxBasebandBw_t RxBasebandBw;
    SX1257_RxPllBw_t RxPllBw;
}SX1257_RadioSettings_t;

/* Radio hardware and global parameters */
typedef struct SX1257_s
{
    Gpio_t        Reset;
    Gpio_t        DIO0;
    Gpio_t        DIO1;
    Gpio_t        DIO2;
    Gpio_t        DIO3;
    Spi_t         Spi;
    SX1257_RadioSettings_t Settings;
}SX1257_t;

//extern SX1276_t SX1276;

/* Initializes the radio */
void SX1257Init(void);

/* Resets the SX1257 */
void SX1257Reset(void);

void SX1257SetRxConfig(uint32_t RxFreq, SX1257_RxLnaGain_t RxLnaGain,
                       uint8_t RxBasebandGain, SX1257_LnaZin_t LnaZin,
                       uint8_t RxAdcBw, uint8_t RxAdcTrim,
                       SX1257_RxBasebandBw_t RxBasebandBw, SX1257_RxPllBw_t RxPllBw);

void SX1257SetTxConfig(uint32_t TxFreq, SX1257_TxDacGain_t TxDacGain,
                       uint8_t TXMixerGain, SX1257_TxPllBw_t TxPllBw,
                       uint8_t TxAnaBw, uint8_t TxDacBw);

void SX1257EnableRX(uint8_t enable);

void SX1257EnableTX(uint8_t enable);

/* Sets the radio in sleep mode */
void SX1257SetSleep(void);

/* Sets the radio in standby mode */
void SX1257SetStby(void);

void SX1257SetLoopBack(uint8_t digital, uint8_t rf);

uint8_t SX1257GetPllLockRx(void);
uint8_t SX1257GetPllLockTx(void);

/* Writes the radio register at the specified address */
void SX1257Write(uint8_t addr, uint8_t data);

/* Reads the radio register at the specified address */
uint8_t SX1257Read(uint8_t addr);

/* Writes multiple radio registers starting at address */
void SX1257WriteBuffer(uint8_t addr, uint8_t *buffer, uint8_t size);

/* Reads multiple radio registers starting at address */
void SX1257ReadBuffer(uint8_t addr, uint8_t *buffer, uint8_t size);

#endif // __SX1257_H__
