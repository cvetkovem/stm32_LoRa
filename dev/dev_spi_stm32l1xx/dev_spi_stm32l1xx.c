#include "stm32l1xx.h"
#include "gpio.h"
#include "spi.h"

void SpiInit(Spi_t *obj)
{
    /* Gpio settings */
    GpioInit(&obj->Mosi, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Miso, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Sclk, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Nss,  PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP,   1);

    /* Gpio alternate function settings */
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Miso.portIndex)))->AFR[(obj->Miso.pinIndex < 8)?0:1] |= 0x05 << 4*(obj->Miso.pinIndex - ((obj->Miso.pinIndex < 8)?0:8));
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Mosi.portIndex)))->AFR[(obj->Mosi.pinIndex < 8)?0:1] |= 0x05 << 4*(obj->Mosi.pinIndex - ((obj->Mosi.pinIndex < 8)?0:8));
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Sclk.portIndex)))->AFR[(obj->Sclk.pinIndex < 8)?0:1] |= 0x05 << 4*(obj->Sclk.pinIndex - ((obj->Sclk.pinIndex < 8)?0:8));
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Nss.portIndex )))->AFR[(obj->Nss.pinIndex  < 8)?0:1] |= 0x05 << 4*(obj->Nss.pinIndex  - ((obj->Nss.pinIndex  < 8)?0:8));

    /* Enable bus clock */
    if(obj->Spi == (SPI_TypeDef *)SPI1_BASE) {
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    } else if(obj->Spi == (SPI_TypeDef *)SPI2_BASE){
        RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    } else {
        RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    }

    /** TODO START **/
    ((SPI_TypeDef *)obj->Spi)->CR1 &= ~SPI_CR1_CPOL; //Полярность тактового сигнала 0
    ((SPI_TypeDef *)obj->Spi)->CR1 &= ~SPI_CR1_CPHA; //Фаза тактового сигнала 0
    ((SPI_TypeDef *)obj->Spi)->CR1 &= ~SPI_CR1_DFF; //8 бит данных
    ((SPI_TypeDef *)obj->Spi)->CR1 &= ~SPI_CR1_LSBFIRST; //MSB передается первым

    if (obj->slave_on) {
        ((SPI_TypeDef *)obj->Spi)->CR1 &= ~SPI_CR1_SSM; //Аппаратное управление входом NSS
        ((SPI_TypeDef *)obj->Spi)->CR1 &= ~SPI_CR1_MSTR; //Режим Master
    } else {
        ((SPI_TypeDef *)obj->Spi)->CR1 |= 0x20; //Baud rate = Fpclk/32
        ((SPI_TypeDef *)obj->Spi)->CR1 |= SPI_CR1_SSM; //Программный режим NSS
        ((SPI_TypeDef *)obj->Spi)->CR1 |= SPI_CR1_SSI; //Аналогично состоянию, когда на входе NSS высокий уровень
        ((SPI_TypeDef *)obj->Spi)->CR2 |= SPI_CR2_SSOE; //Вывод NSS - выход управления slave select
        ((SPI_TypeDef *)obj->Spi)->CR1 |= SPI_CR1_MSTR; //Режим Master
    }

    ((SPI_TypeDef *)obj->Spi)->CR1 |= SPI_CR1_SPE; //Включаем SPI
    /** TODO END **/
}

void SpiDeInit(Spi_t *obj)
{

}

uint16_t SpiInOut(Spi_t *obj, uint16_t outData)
{
    while(!(((SPI_TypeDef *)(obj->Spi))->SR & SPI_SR_TXE));
    ((SPI_TypeDef *)(obj->Spi))->DR = outData;
    while(!(((SPI_TypeDef *)(obj->Spi))->SR & SPI_SR_RXNE));
    return ((SPI_TypeDef *)(obj->Spi))->DR;
}


