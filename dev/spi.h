/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Implements the generic SPI driver

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __SPI_H__
#define __SPI_H__

/*!
 * Data frame format
 */
typedef enum
{
    SPI_8_BIT = 0,
    SPI_16_BIT
}Spi_bits;

/*!
 * Frame format
 */
typedef enum
{
    SPI_MSB = 0,
    SPI_LSB
}Spi_MSBLSB;

/*!
 * Baud rate control
 */
typedef enum
{
    SPI_F_PCLK_2 = 0,
    SPI_F_PCLK_4,
    SPI_F_PCLK_8,
    SPI_F_PCLK_16,
    SPI_F_PCLK_32,
    SPI_F_PCLK_64,
    SPI_F_PCLK_128,
    SPI_F_PCLK_256
}Spi_F_PCLK;

typedef struct Spi_t
{
    uint32_t *Spi; //SPI1, SPI2 or SPI3
    Gpio_t Mosi;
    Gpio_t Miso;
    Gpio_t Sclk;
    Gpio_t Nss;

    uint8_t slave_on;
    Spi_bits bits;
    Spi_MSBLSB msb_lsb;
    uint8_t cpol;
    uint8_t cpha;
    Spi_F_PCLK f_pclk;
} Spi_t;

/*!
 * \brief Initializes the SPI object and MCU peripheral
 *
 * \param [IN] obj  SPI object
 */
void SpiInit(Spi_t *obj);

/*!
 * \brief De-initializes the SPI object and MCU peripheral
 *
 * \param [IN] obj SPI object
 */
void SpiDeInit(Spi_t *obj);

/*!
 * \brief Sends outData and receives inData
 *
 * \param [IN] obj     SPI object
 * \param [IN] outData Byte to be sent
 * \retval inData      Received byte.
 */
uint16_t SpiInOut(Spi_t *obj, uint16_t outData);

#endif  // __SPI_H__
