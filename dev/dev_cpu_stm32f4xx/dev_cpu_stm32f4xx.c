#include "stm32f4xx.h"
#include "dev_cpu.h"

#define CPU_PLL_M     8
#define CPU_PLL_N	  336
#define CPU_PLL_P	  2
#define CPU_PLL_Q	  7

void cpuInit()
{
    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset CFGR register */
    RCC->CFGR = 0x00000000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset PLLCFGR register */
    RCC->PLLCFGR = 0x24003010;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Disable all interrupts */
    RCC->CIR = 0x00000000;

    /* Enable HSE */
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);

	/* Wait till HSE is ready and if Time out is reached exit */
	while ((RCC->CR & RCC_CR_HSERDY) == 0);

    /* Select HSE */
    //RCC->CFGR |= RCC_CFGR_SW_HSE;

	/* Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;
	//PWR->CR |= PWR_CR_PMODE;

	/* HCLK = SYSCLK / 1*/
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	/* PCLK2 = HCLK / 2 */
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
	/* PCLK1 = HCLK / 4 */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	/* Configure the main PLL */
	RCC->PLLCFGR = CPU_PLL_M | (CPU_PLL_N << 6) | (((CPU_PLL_P >> 1) - 1) << 16) | (RCC_PLLCFGR_PLLSRC_HSE) | (CPU_PLL_Q << 24);
	/* Enable the main PLL */
	RCC->CR |= RCC_CR_PLLON;

	/* Wait till the main PLL is ready */
	while((RCC->CR & RCC_CR_PLLRDY) == 0);

	/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

	/* Select the main PLL as system clock source */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	/* Wait till the main PLL is used as system clock source */
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);

    /* Disable HSI */
	RCC->CR &= ~((uint32_t)RCC_CR_HSION);
}

/**********************************/
/** use TIM3 for Delay functions **/
/**********************************/

__INLINE static void cpuTimerInit_ms()
{
    /* RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; */
    *((uint32_t *)(0x42000000 + 32*0x23840 + 4)) = 0x01;

    /* 500 KHz */
    TIM3->PSC = 0xA40F;
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;
}

__INLINE static void cpuTimerInit_us()
{
    /* RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; */
    *((uint32_t *)(0x42000000 + 32*0x23840 + 4)) = 0x01;

    /* 1 MHz */
    TIM3->PSC = 0x0053;
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;
}

__INLINE static void cpuStopTimer()
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
    /* RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN; */
    *((uint32_t *)(0x42000000 + 32*0x23840 + 4)) = 0x00;
}

void cpuDelay_ms(uint32_t m_sec)
{
    cpuTimerInit_ms();

	volatile uint32_t start = TIM3->CNT;
	while((TIM3->CNT-start) <= m_sec * 2);

    cpuStopTimer();
}

void cpuDelay_us(uint32_t u_sec)
{
    cpuTimerInit_us();

	volatile uint32_t start = TIM3->CNT;
	while((TIM3->CNT-start) <= u_sec);

    cpuStopTimer();
}

__INLINE void cpuDisable_irq()
{
    __ASM volatile ("cpsid i");
}

__INLINE void cpuEnable_irq()
{
    __ASM volatile ("cpsie i");
}
