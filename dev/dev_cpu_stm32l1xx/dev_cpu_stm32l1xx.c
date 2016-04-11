#include "stm32l1xx.h"
#include "dev_cpu.h"

void cpuInit()
{
    /* Enable HSI 16MHz */
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR&RCC_CR_HSIRDY));

    /* Enable LSE */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;
    RCC->CSR |= RCC_CSR_LSEON;
    while(!(RCC->CSR & RCC_CSR_LSERDY));

    /* PLL input set as HSI */
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSI;

    /* PLL 16*4/2 = 32MHz */
    RCC->CFGR |= RCC_CFGR_PLLMUL4;
    RCC->CFGR |= RCC_CFGR_PLLDIV2;

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR&RCC_CR_PLLRDY));

    /* Settings FLASH for 32MHz */
    FLASH->ACR |= FLASH_ACR_ACC64;
    FLASH->ACR |= FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_PRFTEN;

    /* USE PLL */
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Disable MSI */
    RCC->CR &= ~RCC_CR_MSION;void TIM6_IRQHandler(void);
}
