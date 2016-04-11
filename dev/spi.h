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

typedef struct Spi_t
{
    uint32_t *Spi;
    Gpio_t Mosi;
    Gpio_t Miso;
    Gpio_t Sclk;
    Gpio_t Nss;

    uint8_t slave_on;
} Spi_t;

/*!
 * \brief Initializes the SPI object and MCU peripheral
 *
 * \remark When NSS pin is software controlled set the pin name to NC otherwise
 *         set the pin name to be used.
 *
 * \param [IN] obj  SPI object
 * \param [IN] spi_name  ONLY SPI1 or SPI2
 * \param [IN] slave_on  master or slave
 * \param [IN] bits 8 for sx1276
 * \param [IN] CPOL 0 for sx1276
 * \param [IN] CPHA 0 for sx1276
 * \param [IN] BR 1MHz for sx1276
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
