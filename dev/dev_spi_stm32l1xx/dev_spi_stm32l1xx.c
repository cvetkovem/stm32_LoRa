#include "stm32l1xx.h"
#include "gpio.h"
#include "spi.h"

void SpiInit(Spi_t *obj)
{
    register uint32_t gpio_reg;

    SPI_TypeDef *spi = (SPI_TypeDef *)(obj->Spi);

    /* Gpio settings */
    GpioInit(&obj->Mosi, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Miso, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Sclk, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);

    /* Gpio alternate function settings */
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Miso.portIndex)))->AFR[(obj->Miso.pinIndex < 8)?0:1] |= 0x05 << 4*(obj->Miso.pinIndex - ((obj->Miso.pinIndex < 8)?0:8));
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Mosi.portIndex)))->AFR[(obj->Mosi.pinIndex < 8)?0:1] |= 0x05 << 4*(obj->Mosi.pinIndex - ((obj->Mosi.pinIndex < 8)?0:8));
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Sclk.portIndex)))->AFR[(obj->Sclk.pinIndex < 8)?0:1] |= 0x05 << 4*(obj->Sclk.pinIndex - ((obj->Sclk.pinIndex < 8)?0:8));

    if(obj->slave_on) {
        GpioInit(&obj->Nss,  PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
        ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Nss.portIndex )))->AFR[(obj->Nss.pinIndex  < 8)?0:1] |= 0x05 << 4*(obj->Nss.pinIndex  - ((obj->Nss.pinIndex  < 8)?0:8));
    } else {
        GpioInit(&obj->Nss,  PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
    }

    /* Enable bus clock */
    if(spi == SPI1) {
        /* RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; */
        *((uint32_t *)(0x42000000+(32*0x23820)+4*12)) = 0x01;
        /* bit banding address for SPI1 */
        gpio_reg = 0x42000000+(32*0x13000);
    } else if(spi == SPI2){
        /* RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; */
        *((uint32_t *)(0x42000000+(32*0x23824)+4*14)) = 0x01;
        /* bit banding address for SPI2 */
        gpio_reg = 0x42000000+(32*0x3800);
    } else {
        /* RCC->APB1ENR |= RCC_APB1ENR_SPI3EN; */
        *((uint32_t *)(0x42000000+(32*0x23824)+4*15)) = 0x01;
        /* bit banding address for SPI3 */
        gpio_reg = 0x42000000+(32*0x3C00);
    }

    /* Settings SPI->CR1 */
    *((uint32_t *)(gpio_reg)) = (obj->cpha) & 0x01;
    *((uint32_t *)(gpio_reg + 4)) = (obj->cpol) & 0x01;
    *((uint32_t *)(gpio_reg + 8)) = !(obj->slave_on);

    *((uint32_t *)(gpio_reg + 12)) = (obj->f_pclk) & 0x01;
    *((uint32_t *)(gpio_reg + 16)) = ((obj->f_pclk) >> 1) & 0x01;
    *((uint32_t *)(gpio_reg + 20)) = ((obj->f_pclk) >> 2) & 0x01;

    *((uint32_t *)(gpio_reg + 44)) = (obj->bits) & 0x01;
    *((uint32_t *)(gpio_reg + 28)) = (obj->msb_lsb) & 0x01;

    if (obj->slave_on) {
        /* Software slave management disabled (SSM = 0) */
        *((uint32_t *)(gpio_reg + 36)) = 0x00;
    } else {
        /* Software slave management enabled (SSM = 1) */
        *((uint32_t *)(gpio_reg + 36)) = 0x01;
        /* SSI = 1 */
        *((uint32_t *)(gpio_reg + 32)) = 0x01;

        /* SPI->CR2 SSOE */
        *((uint32_t *)(gpio_reg + 32*0x04 + 8)) = 0x01;
    }

    /* SPI Enable */
    /* SPI->CR1 set SPE */
    *((uint32_t *)(gpio_reg + 24)) = 0x01;
}

void SpiDeInit(Spi_t *obj)
{
    register uint32_t gpio_reg;

    SPI_TypeDef *spi = (SPI_TypeDef *)(obj->Spi);

    /* Disable SPI */
    if(spi == SPI1) {
        /* bit banding address for SPI1 */
        gpio_reg = 0x42000000+(32*0x13000);
        *((uint32_t *)(gpio_reg + 24)) = 0x00;
        /* RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN; */
        *((uint32_t *)(0x42000000+(32*0x23820)+4*12)) = 0x00;
    } else if(spi == SPI2){
        /* bit banding address for SPI2 */
        gpio_reg = 0x42000000+(32*0x3800);
        *((uint32_t *)(gpio_reg + 24)) = 0x00;
        /* RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN; */
        *((uint32_t *)(0x42000000+(32*0x23824)+4*14)) = 0x00;
    } else {
        /* bit banding address for SPI3 */
        gpio_reg = 0x42000000+(32*0x3C00);
        *((uint32_t *)(gpio_reg + 24)) = 0x00;
        /* RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN; */
        *((uint32_t *)(0x42000000+(32*0x23824)+4*15)) = 0x00;
    }

    GpioDeInit(&obj->Mosi);
    GpioDeInit(&obj->Miso);
    GpioDeInit(&obj->Sclk);
    GpioDeInit(&obj->Nss);
}

uint16_t SpiInOut(Spi_t *obj, uint16_t outData)
{
    SPI_TypeDef *spi = (SPI_TypeDef *)(obj->Spi);

    while(!(spi->SR & SPI_SR_TXE));
    spi->DR = outData;

    while(!(spi->SR & SPI_SR_RXNE));
    return spi->DR;
}


