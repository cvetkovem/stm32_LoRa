#include <stm32f4xx.h>
#include "dev_hw_timer.h"

/* Hardware Time base in us */
#define HW_TIMER_TIME_BASE    100 //us

/* Hardware Timer tick counter */
volatile TimerTime_t TimerTickCounter = 1;

/* Saved value of the Tick counter at the start of the next event */
static TimerTime_t TimerTickCounterContext = 0;

/* Value trigging the IRQ */
volatile TimerTime_t TimeoutCntValue = 0;

void TimerHwInit(void)
{
    /* TIM2 clock enable */
    /* RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; */
    *((uint32_t *)(0x42000000 + 32*0x23840)) = 0x01;

    /* --------------------------NVIC Configuration -------------------------------*/
    /* Enable the TIM2 gloabal Interrupt */
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);

    TimeoutCntValue = 0;

    /* Time base configuration */
    TIM2->PSC = 0;
    TIM2->ARR = 8399;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->SR &= ~TIM_SR_UIF;

    /* TIM2 disable counter */
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TimerHwDeInit(void)
{
    NVIC_DisableIRQ(TIM2_IRQn);
    TIM2->CR1 &= ~TIM_CR1_CEN;

    /* RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN; */
    *((uint32_t *)(0x42000000 + 32*0x23840)) = 0x00;
}

uint32_t TimerHwGetMinimumTimeout(void)
{
    return (2 * HW_TIMER_TIME_BASE);
}

void TimerHwStart(uint32_t val)
{
    TIM2->SR &= ~TIM_SR_UIF;
    TIM2->CR1 |= TIM_CR1_CEN;

    TimerTickCounterContext = TimerHwGetTimerValue();

    if(val <= HW_TIMER_TIME_BASE + 1) {
        TimeoutCntValue = TimerTickCounterContext + 1;
    } else {
        TimeoutCntValue = TimerTickCounterContext + ((val - 1) / HW_TIMER_TIME_BASE);
    }
}

void TimerHwStop(void)
{
    //TIM_ITConfig( TIM2, TIM_IT_CC1, DISABLE );
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->SR &= ~TIM_SR_UIF;
}

TimerTime_t TimerHwGetElapsedTime(void)
{
     return (((TimerHwGetTimerValue() - TimerTickCounterContext) + 1) * HW_TIMER_TIME_BASE);
}

TimerTime_t TimerHwGetTimerValue(void)
{
    TimerTime_t val = 0;

    __disable_irq();
    val = TimerTickCounter;
    __enable_irq();

    return val;
}

TimerTime_t TimerHwGetTime(void)
{
    return (TimerHwGetTimerValue() * HW_TIMER_TIME_BASE);
}

/* Increment the Hardware Timer tick counter */
__INLINE static void TimerIncrementTickCounter(void)
{
    __disable_irq();
    TimerTickCounter++;
    __enable_irq();
}

/* Timer IRQ handler */
void TIM2_IRQHandler(void)
{
    if(TIM2->SR & TIM_SR_UIF) {
        TimerIncrementTickCounter();
        TIM2->SR &= ~TIM_SR_UIF;

        if(TimerTickCounter == TimeoutCntValue) {
            TimerIrqHandler();
        }
    }
}

