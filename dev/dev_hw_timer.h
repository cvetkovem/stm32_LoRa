#ifndef __DEV_HW_TIMER_H__
#define __DEV_HW_TIMER_H__

/* Timer time variable definition */
#ifndef TimerTime_t
typedef uint32_t TimerTime_t;
#endif

/*
 * Initializes the timer
 * The timer is based on TIM2 (for stm32l1xx)
 * with a 100uS time basis
 */
void TimerHwInit(void);

/* DeInitializes the timer */
void TimerHwDeInit(void);

/*
 * Return the minimum timeout the Timer is able to handle
 * minimum value for a timeout
 */
uint32_t TimerHwGetMinimumTimeout(void);

/*
 * Start the Standard Timer counter
 * \param [IN] val Timer duration (us)
 */
void TimerHwStart(uint32_t val);

/* Stop the the Standard Timer counter */
void TimerHwStop(void);

/* Return the value of the timer counter */
TimerTime_t TimerHwGetTimerValue(void);

/* Return the value of the current time in us */
TimerTime_t TimerHwGetTime(void);

/* Return the value on the timer Tick counter */
TimerTime_t TimerHwGetElapsedTime(void);

extern void TimerIrqHandler(void);

#endif // __DEV_HW_TIMER_H__
