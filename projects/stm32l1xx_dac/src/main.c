#include "conf_board.h"

void boardInit(void);
void buttonFunc(void);

Gpio_t LED_BLUE, LED_GREEN, BUTTON;

/*
void DMA_Configuration()
{
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sinus;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 360;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel2, ENABLE);
}

void DAC_Configuration()
{
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_1, ENABLE);
}
*/


void TIM2_init(void)
{
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;             // тактирование таймера

        TIM2->PSC = 0X00;                             // предделитель
        TIM2->ARR = 255;                                // переполнение две секунды

        TIM2->DIER |= TIM_DIER_UIE;             // прерывание по переполнению
        TIM2->DIER |= TIM_DIER_UDE;             // влк. запуск ѕƒѕ

        TIM2->CR2 |= TIM_CR2_MMS_1;     // ÷јѕ будет запускатьс€ по переполнению
        TIM2->CR1 |= TIM_CR1_CEN;               // запуск счета
}


int main(void) {
    /*
    uint8_t  sin[16] = {127, 176, 218, 245, 255, 245, 218, 176, 127, 79, 37, 10, 0,10, 37, 79};
    uint8_t chirpUP[22] = {0, 26, 51, 77, 102, 128, 153, 179, 204, 230, 255,
                            255, 230, 204, 179, 153, 128, 102, 77, 51 ,26, 0};

    uint8_t chirpUpNew[32] = {0, 8, 16, 25, 33, 41, 49, 58,
                            66, 74, 82, 90, 99, 107, 115, 123,
                            132, 140, 148, 156, 165, 173, 181, 189,
                            197, 206, 214, 222, 230, 239, 247, 255};


    uint8_t chirpUpNew[32] = {85, 87, 90, 92, 95, 97, 100, 102,
                             105, 107, 110, 112, 115, 117, 120, 122,
                             125, 127, 130, 132, 135, 137, 140, 142,
                             145, 147, 150, 152, 155, 157, 160, 162};

*/
    uint8_t chirpUpNew[32] = {115, 117, 119, 122, 124, 126, 128, 130,
                             133, 135, 137, 139, 141, 144, 146, 148,
                             150, 152, 154, 157, 159, 161, 163, 165,
                             168, 170, 172, 174, 176, 179, 181, 183};

    int i;

/*     for (i = 0; i < 32; i++) {
        chirpUpNew[i] = 183;
    }
*/
    boardInit();

    //TIM2_init();

    // DMA
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;                // Enable DMA
    DMA1_Channel2->CPAR |= (uint32_t) &DAC->DHR8R2;  // DMA address
    DMA1_Channel2->CMAR |= (uint32_t) &chirpUpNew;      // Memory address
    DMA1_Channel2->CCR |= DMA_CCR2_CIRC;             // циклическа€ передача
    DMA1_Channel2->CCR |= DMA_CCR2_DIR;              // MEM -> DAC
    DMA1_Channel2->CNDTR = 32;                       // Numbers of values
    DMA1_Channel2->CCR &= ~DMA_CCR2_PINC;            // The address of the periphery is not incremented
    DMA1_Channel2->CCR |= DMA_CCR2_MINC;             // The address of the memory is incremented

    DMA1_Channel2->CCR |= DMA_CCR1_MEM2MEM;

    DMA1_Channel2->CCR  &= ~DMA_CCR2_MSIZE;          //размерность данных 8 бит
    //DMA1_Channel2->CCR  |=  DMA_CCR2_MSIZE_0;      //размерность данных 16 бит
    //DMA1_Channel2->CCR  |=  DMA_CCR2_MSIZE_1;      //размерность данных 32 бита
    DMA1_Channel2->CCR  &= ~DMA_CCR2_PSIZE;          //размерность данных 8 бит
    //DMA1_Channel4->CCR  |=  DMA_CCR4_PSIZE_0;      //размерность данных 16 бит
    //DMA1_Channel4->CCR  |=  DMA_CCR4_PSIZE_1;      //размерность данных 32 бита

    DMA1_Channel2->CCR |= DMA_CCR2_PL;               // ѕриоритет - очень высокий (Very High)
    DMA1_Channel2->CCR |= DMA_CCR2_EN;               // –азрешаем работу канала 2 DMA

    dacInit(dac_channel2);

    while(1) {
      //__WFI();
      /*
      for (i = 0; i < 11; i++) {
        DAC->DHR8R2 = chirpUP[i];
        //DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG2;
      }
      */
    }

    return 0;
}

void boardInit(void) {
    cpuInit();

    /* LEDs settings */
    //LED_BLUE.pinIndex = BOARD_LED_BLUE_pin;
    //LED_BLUE.portIndex = BOARD_LED_BLUE_port;
    //GpioInit(&LED_BLUE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    //LED_GREEN.pinIndex = BOARD_LED_GREEN_pin;
    //LED_GREEN.portIndex = BOARD_LED_GREEN_port;
    //GpioInit(&LED_GREEN, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /* Button settings */
    //BUTTON.pinIndex = BOARD_BUTTON_pin;
    //BUTTON.portIndex = BOARD_BUTTON_port;
    //GpioInit(&BUTTON, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    //GpioSetInterrupt(&BUTTON, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, buttonFunc);
}

void buttonFunc(void) {

}

void TimerIrqHandler(void) {

}
