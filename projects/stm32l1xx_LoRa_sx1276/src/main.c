#include "conf_board.h"

Gpio_t LED, BUTTON;


Spi_t spi1, spi2;
uint8_t temp;

void spi_exchange(uint8_t send_data)
{
    GpioWrite(&(spi2.Nss), 0);

    ((SPI_TypeDef *)(spi1.Spi))->DR = send_data; //Пишем в буфер передатчика SPI2
    ((SPI_TypeDef *)(spi2.Spi))->DR = 0x00; //Пишем в буфер передатчика SPI1. После этого стартует обмен данными
    while(!(((SPI_TypeDef *)(spi2.Spi))->SR & SPI_SR_RXNE)); //Ожидаем окончания приема данных модулем SPI1 (RXNE =1 - приемный буфер содержит данные)
    temp = ((SPI_TypeDef *)(spi2.Spi))->DR;//Считываем данные из приемного буфера SPI1. При этой операции происходит очистка буфера и сброс флага RXNE

    GpioWrite(&(spi2.Nss), 1);
}

volatile uint8_t tmp = 0, tmp2 = 0;
void TIM6_IRQHandler(void)
{
    TIM6->SR &= ~TIM_SR_UIF;

    tmp = GpioRead(&BUTTON);
    GpioWrite(&LED, tmp);
    //tmp = !(tmp & 0x01);
    //GPIOB->ODR ^= GPIO_OTYPER_ODR_6;

    spi_exchange(tmp2);
    tmp2++;
}

/*
volatile uint8_t ld = 0;
void led_on_off(void) {
    GpioWrite(&LED, ld);
    ld = !(ld & 0x01);
}
*/

int main()
{
    cpuInit();
/*
    NVIC_SetPriority(TIM6_IRQn, 1);
    NVIC_EnableIRQ(TIM6_IRQn);
*/
    LED.pinIndex = 6;
    LED.portIndex = 1; //B
    GpioInit(&LED, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    BUTTON.pinIndex = 0;
    BUTTON.portIndex = 0; //A
    GpioInit(&BUTTON, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /* Slave */
    spi1.slave_on = 1; spi1.Spi = SPI1;
    spi1.Mosi.pinIndex = 12; spi1.Mosi.portIndex = 0;
    spi1.Miso.pinIndex = 11; spi1.Miso.portIndex = 0;
    spi1.Sclk.pinIndex = 5;  spi1.Sclk.portIndex = 0;
    spi1.Nss.pinIndex = 4;   spi1.Nss.portIndex = 0;
    spi1.bits = SPI_8_BIT;
    spi1.cpha = 0;
    spi1.cpol = 0;
    spi1.f_pclk = SPI_F_PCLK_32;
    spi1.msb_lsb = SPI_MSB;

    /* Master */
    spi2.slave_on = 0; spi2.Spi = SPI2;
    spi2.Mosi.pinIndex = 15; spi2.Mosi.portIndex = 1;
    spi2.Miso.pinIndex = 14; spi2.Miso.portIndex = 1;
    spi2.Sclk.pinIndex = 13; spi2.Sclk.portIndex = 1;
    spi2.Nss.pinIndex = 12;  spi2.Nss.portIndex = 1;
    spi2.bits = SPI_8_BIT;
    spi2.cpha = 0;
    spi2.cpol = 0;
    spi2.f_pclk = SPI_F_PCLK_32;
    spi2.msb_lsb = SPI_MSB;

    SpiInit(&spi1);
    SpiInit(&spi2);

    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 0x7A12;
    TIM6->ARR = 0x03FF;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;

/*
    GpioSetInterrupt(&BUTTON, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, led_on_off);
    GpioRemoveInterrupt(&BUTTON);
    GpioSetInterrupt(&BUTTON, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, led_on_off);
*/

  while(1);

  return 0;
}
