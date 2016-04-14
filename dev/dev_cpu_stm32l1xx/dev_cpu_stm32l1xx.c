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
    RCC->CR &= ~RCC_CR_MSION;
}

/**********************************/
/** use TIM3 for Delay functions **/
/**********************************/

__INLINE static void cpuTimerInit_ms()
{
    /* RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; */
    *((uint32_t *)(0x42000000 + 32*0x23824 + 4)) = 0x01;

    TIM3->PSC = 0x7CFF;
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;
}

__INLINE static void cpuTimerInit_us()
{
    /* RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; */
    *((uint32_t *)(0x42000000 + 32*0x23824 + 4)) = 0x01;

    TIM3->PSC = 0x0020;
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;
}

__INLINE static void cpuStopTimer()
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
    /* RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN; */
    *((uint32_t *)(0x42000000 + 32*0x23824 + 4)) = 0x00;
}

void cpuDelay_ms(uint32_t m_sec)
{
    cpuTimerInit_ms();

	volatile uint32_t start = TIM3->CNT;
	while((TIM3->CNT-start) <= m_sec);

    cpuStopTimer();
}

void cpuDelay_us(uint32_t u_sec)
{
    cpuTimerInit_us();

	volatile uint32_t start = TIM3->CNT;
	while((TIM3->CNT-start) <= u_sec);

    cpuStopTimer();
}

