#include "stm32f4xx.h"
#include "gpio.h"

#define NULL ((void *)0)

static uint16_t gpio_ports[8];
static GpioIrqHandler *GpioIrq[16];

void GpioInit(Gpio_t *obj, PinModes mode,  PinConfigs config, PinTypes type, uint8_t value)
{
	register uint32_t gpio_reg;

	/* Enable port */
	/* use bit banding for enabling GPIO RCC
	 * RCC->AHB1ENR = 0x40023830, bit banding address 0x42000000+(32*0x23830)+(bit_number*4)=0x42470600+(bit_number*4)
	 */
	*((uint32_t *)(0x42470600 + ((uint32_t)(obj->portIndex))*4)) = 0x01;

	/* Init GPIO pin, use bit banding */
	/* MODER offset 0x00 */
	gpio_reg = 0x42400000 + (32*0x400*((uint32_t)(obj->portIndex))) + ((uint32_t)(obj->pinIndex))*2*4;
	*((uint32_t *)(gpio_reg)) = mode & 0x01;
    *((uint32_t *)(gpio_reg + 4)) = (mode >> 1) & 0x01;

    /* OTYPER offset 0x04 */
    gpio_reg = 0x42400000 + (32*(0x400*((uint32_t)obj->portIndex))) + (32*0x04) + (((uint32_t)(obj->pinIndex))*4);
    *((uint32_t *)(gpio_reg)) = config & 0x01;

	/* OSPEEDR offset 0x08 */ /* 11 - High speed */
	gpio_reg = 0x42400000 + (32*(0x400*((uint32_t)obj->portIndex))) + (32*0x08) + (((uint32_t)(obj->pinIndex))*2*4);
    *((uint32_t *)(gpio_reg)) = 0x01;
    *((uint32_t *)(gpio_reg + 4 )) = 0x01;

    /* PUPDR offset 0x0C */
    gpio_reg = 0x42400000 + (32*(0x400*((uint32_t)obj->portIndex))) + (32*0x0C) + (((uint32_t)(obj->pinIndex))*2*4);
    *((uint32_t *)(gpio_reg)) = type & 0x01;
    *((uint32_t *)(gpio_reg + 4 )) = (type >> 1) & 0x01;

	/* mark pin as configured */
	gpio_ports[obj->portIndex] |= (uint16_t)(1 << (obj->pinIndex));

    /* Sets initial output value */
    if(mode == PIN_OUTPUT) {
        GpioWrite(obj, value);
    }
}

void GpioDeInit(Gpio_t *obj)
{
    register uint32_t gpio_reg;

    GpioRemoveInterrupt(obj);

    /* MODER offset 0x00 */
	gpio_reg = 0x42400000 + (32*(0x400*((uint32_t)obj->portIndex))) + (((uint32_t)(obj->pinIndex))*2*4);
    *((uint32_t *)(gpio_reg)) = 0x00;
    *((uint32_t *)(gpio_reg + 4)) = 0x00;

    /* OTYPER offset 0x04 */
    gpio_reg = 0x42400000 + (32*(0x400*((uint32_t)obj->portIndex))) + (32*0x04) + (((uint32_t)(obj->pinIndex))*4);
    *((uint32_t *)(gpio_reg)) = 0x00;

	/* OSPEEDR offset 0x08 */ /* 00 - Very low speed */
	gpio_reg = 0x42400000 + (32*(0x400*((uint32_t)obj->portIndex))) + (32*0x08) + (((uint32_t)(obj->pinIndex))*2*4);
    *((uint32_t *)(gpio_reg)) = 0x00;
    *((uint32_t *)(gpio_reg + 4)) = 0x00;

    /* PUPDR offset 0x0C */
    gpio_reg = 0x42400000 + (32*(0x400*((uint32_t)obj->portIndex))) + (32*0x0C) + (((uint32_t)(obj->pinIndex))*2*4);
    *((uint32_t *)(gpio_reg)) = 0x00;
    *((uint32_t *)(gpio_reg + 4)) = 0x00;

    gpio_reg = (AHB1PERIPH_BASE + 0x400*(obj->portIndex));
    ((GPIO_TypeDef *)gpio_reg)->BSRRH = 1 << (obj->pinIndex);

	/* Set alternate pin function, use bit banding offset 0x20 */
	gpio_reg = 0x42400400 + (32*0x400*((uint32_t)(obj->portIndex))) + ((uint32_t)(obj->pinIndex))*4*4;
	*((uint32_t *)(gpio_reg)) = 0x00;
	*((uint32_t *)(gpio_reg+4)) = 0x00;
	*((uint32_t *)(gpio_reg+8)) = 0x00;
	*((uint32_t *)(gpio_reg+12)) = 0x00;

    /* Mark pin as deconfigured */
	gpio_ports[obj->portIndex] &= ~((uint16_t)(1 << (obj->pinIndex)));

	/* if all pins in port are deconfigured, disable port */
	if (gpio_ports[obj->portIndex] == 0)
	{
		/* use bit banding */
		*((uint32_t *)(0x42470600 + ((uint32_t)(obj->portIndex))*4)) = 0x00;
	}
}

void GpioSetInterrupt(Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler)
{
    if(irqHandler == NULL) {
        return;
    }

    GpioIrq[obj->pinIndex & 0x0F] = irqHandler;

    /* Enable SYSCFG clock */
    /* RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; */
    /* bit banding address 0x42000000+(32*0x23844)+(14*4)=0x424708B8 */
    *((uint32_t *)(0x424708B8)) = 0x01;

    /* Connect EXTI Line to GPIO pin */
    /* SYSCFG_EXTILineConfig */
    SYSCFG->EXTICR[(obj->pinIndex >> 2)] |= (obj->portIndex) << 4*( obj->pinIndex - (obj->pinIndex >> 2)*4 );

    /* Configure EXTI line */
    /* EXTI->IMR */
    /* bit banding address 0x42000000+(32*0x13C00)+(bit_number*4)= 0x42278000 + (bit_number*4)*/
    *((uint32_t *)(0x42278000 + 4*(obj->pinIndex))) = 0x01;

    if(irqMode == NO_IRQ) {
        /* ERROR */
        while(1);
    }

    /* GPIO IRQ on a rising, falling or both edges */
    /* bit banding address 0x42000000+(32*0x13C08)+(bit_number*4)= 0x42278100 + (bit_number*4) */
    /* EXTI->RTSR offset 0x08 */
    *((uint32_t *)(0x42278100 + 4*(obj->pinIndex))) = irqMode & 0x01;
    /* EXTI->FTSR offset 0x0C */
    *((uint32_t *)(0x42278100 + (32*0x04) + 4*(obj->pinIndex))) = (irqMode >> 1) & 0x01;

    if(obj->pinIndex < 5) {
        NVIC_EnableIRQ(EXTI0_IRQn + obj->pinIndex);
        NVIC_SetPriority(EXTI0_IRQn + obj->pinIndex, irqPriority);
    } else if(obj->pinIndex < 10) {
        NVIC_EnableIRQ(EXTI9_5_IRQn);
        NVIC_SetPriority(EXTI9_5_IRQn, irqPriority);
    } else {
        NVIC_EnableIRQ(EXTI15_10_IRQn);
        NVIC_SetPriority(EXTI15_10_IRQn, irqPriority);
    }
}

void GpioRemoveInterrupt(Gpio_t *obj)
{
    uint8_t i;

    GpioIrq[obj->pinIndex & 0x0F] = NULL;

    /* Configure EXTI line */
    /* EXTI->IMR */
    /* bit banding address 0x42000000+(32*0x13C00)+(bit_number*4)= 0x42278000 + (bit_number*4)*/
    *((uint32_t *)(0x42278000 + 4*(obj->pinIndex))) = 0x00;

    SYSCFG->EXTICR[(obj->pinIndex >> 2)] &= ~( (0x07) << 4*( obj->pinIndex - (obj->pinIndex >> 2)*4 ) );

    /* GPIO IRQ on a rising, falling or both edges */
    /* bit banding address 0x42000000+(32*0x13C08)+(bit_number*4)= 0x42278100 + (bit_number*4) */
    /* EXTI->RTSR */
    *((uint32_t *)(0x42278100 + 4*(obj->pinIndex))) = 0x00;
    /* EXTI->FTSR */
    *((uint32_t *)(0x42278100 + (32*0x04) + 4*(obj->pinIndex))) = 0x00;

    if(obj->pinIndex < 5) {
        NVIC_DisableIRQ(EXTI0_IRQn + obj->pinIndex);
    } else if(obj->pinIndex < 10) {
        if(!GpioIrq[5] && !GpioIrq[6] && !GpioIrq[7] && !GpioIrq[8] && !GpioIrq[9]) {
            NVIC_DisableIRQ(EXTI9_5_IRQn);
        }
    } else {
        if(!GpioIrq[10] && !GpioIrq[11] && !GpioIrq[12] && !GpioIrq[13] && !GpioIrq[14] && !GpioIrq[15]) {
            NVIC_DisableIRQ(EXTI15_10_IRQn);
        }
    }

    for(i=0; i<15; i++) {
        if(GpioIrq[i]) {
            return;
        }
    }

    /* Disable SYSCFG clock */
    /* RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; */
    /* bit banding address 0x42000000+(32*0x23844)+(14*4)=0x424708B8 */
    *((uint32_t *)(0x424708B8)) = 0x00;
}

__INLINE void GpioWrite(Gpio_t *obj, uint8_t value)
{
    register uint32_t gpio_reg;

    gpio_reg = (AHB1PERIPH_BASE + 0x400*(obj->portIndex));

    if( value == 0 ) {
        ((GPIO_TypeDef *)gpio_reg)->BSRRH = 1 << (obj->pinIndex);
    } else {
        ((GPIO_TypeDef *)gpio_reg)->BSRRL = 1 << (obj->pinIndex);
    }
}

__INLINE uint8_t GpioRead(Gpio_t *obj)
{
    register uint32_t gpio_reg;

    gpio_reg = (AHB1PERIPH_BASE + 0x400*(obj->portIndex));

    return !!( (((GPIO_TypeDef *)gpio_reg)->IDR) & (1 << obj->pinIndex) );
}


void EXTI0_IRQHandler(void)
{
    if(EXTI->PR & (1<<0)) {
        if(GpioIrq[0] != NULL) {
            GpioIrq[0]( );
        }
        /* EXTI->PR |= EXTI_PR_PR0; */
        *((uint32_t *)(0x42278280 + 4*0)) = 0x01;
    }
}

void EXTI1_IRQHandler(void)
{
    if(EXTI->PR & (1<<1)) {
        if(GpioIrq[1] != NULL) {
            GpioIrq[1]( );
        }
        /* EXTI->PR |= EXTI_PR_PR1; */
        *((uint32_t *)(0x42278280 + 4*1)) = 0x01;
    }
}

void EXTI2_IRQHandler(void)
{
    if(EXTI->PR & (1<<2)) {
        if(GpioIrq[2] != NULL) {
            GpioIrq[2]( );
        }
        /* EXTI->PR |= EXTI_PR_PR2; */
        *((uint32_t *)(0x42278280 + 4*2)) = 0x01;
    }
}

void EXTI3_IRQHandler(void)
{
    if(EXTI->PR & (1<<3)) {
        if(GpioIrq[3] != NULL) {
            GpioIrq[3]( );
        }
        /* EXTI->PR |= EXTI_PR_PR3; */
        *((uint32_t *)(0x42278280 + 4*3)) = 0x01;
    }
}

void EXTI4_IRQHandler(void)
{
    if(EXTI->PR & (1<<4)) {
        if(GpioIrq[4] != NULL) {
            GpioIrq[4]( );
        }
        /* EXTI->PR |= EXTI_PR_PR4; */
        *((uint32_t *)(0x42278280 + 4*4)) = 0x01;
    }
}

void EXTI9_5_IRQHandler( void )
{
    uint8_t i;

    for(i=5; i < 10; i++) {
        if(EXTI->PR & (1<<i)) {
            if(GpioIrq[i] != NULL) {
                GpioIrq[i]( );
            }
            /* EXTI->PR |= EXTI_PR_PRi; */
            *((uint32_t *)(0x42278280 + 4*i)) = 0x01;
        }
    }
}

void EXTI15_10_IRQHandler( void )
{
    uint8_t i;

    for(i=10; i < 16; i++) {
        if(EXTI->PR & (1<<i)) {
            if(GpioIrq[i] != NULL) {
                GpioIrq[i]( );
            }
            /* EXTI->PR |= EXTI_PR_PRi; */
            *((uint32_t *)(0x42278280 + 4*i)) = 0x01;
        }
    }
}

