#include "stm32l1xx.h"
#include "dac.h"

void dacInit(Dac_number_channel channel) {
  switch(channel) {
    case 0:
      RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  // Enable port A
      GPIOA->MODER |= GPIO_MODER_MODER4;  // Analog input / output line PA4
      GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4; // Disable pull-up resistors

      RCC->APB1ENR |= RCC_APB1ENR_DACEN;  // Enable DAC
      DAC->CR |= DAC_CR_EN1;              // Enable channel 1 DAC
      DAC->CR |= DAC_CR_TEN1;             // Enable conversion in channel 1 DAC
      DAC->CR |= DAC_CR_TSEL1;            // Choose the source of launch - software
    break;

    case 1:
      RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  // Enable port A
      GPIOA->MODER |= GPIO_MODER_MODER5;  // Analog input / output line PA5
      GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5; // Disable pull-up resistors

      RCC->APB1ENR |= RCC_APB1ENR_DACEN;  // Enable DAC
      DAC->CR |= DAC_CR_BOFF2;
      //DAC->CR |= DAC_CR_DMAEN2;
      DAC->CR |= DAC_CR_EN2;              // Enable channel 2 DAC
      //DAC->CR |= DAC_CR_TEN2;             // Enable conversion in channel 2 DAC
      //DAC->CR |= DAC_CR_TSEL2;            // Choose the source of launch - software
    break;

    default:
    break;
  }
}

void dacDeInit(Dac_number_channel channel) {

}

void dacStart(Dac_number_channel channel) {
}

void dacStop(Dac_number_channel channel) {

}
