#ifndef __DEV_DAC_H_INCLUDED__
#define __DEV_DAC_H_INCLUDED__

typedef enum
{
    dac_channel1 = 0,
    dac_channel2
}Dac_number_channel;

void dacInit(Dac_number_channel channel);
void dacDeInit(Dac_number_channel channel);

void dacStart(Dac_number_channel channel);
void dacStop(Dac_number_channel channel);

#endif /* __DEV_DAC_H_INCLUDED__ */
