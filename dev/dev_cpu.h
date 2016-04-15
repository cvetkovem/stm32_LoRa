#ifndef __DEV_CPU_H_INCLUDED__
#define __DEV_CPU_H_INCLUDED__

void cpuInit();

void cpuDelay_ms(uint32_t m_sec);
void cpuDelay_us(uint32_t u_sec);

void cpuDisable_irq(void);
void cpuEnable_irq(void);

#endif /* __DEV_CPU_H_INCLUDED__ */
