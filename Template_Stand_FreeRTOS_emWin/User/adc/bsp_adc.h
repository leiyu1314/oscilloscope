#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define ADC_DMA_CHANNEL				DMA1_Channel1

//对于12位的ADC，3.3V的ADC值为0xfff,温度为25度时对应的电压值为1.43V即0x6EE
//#define V25 0x6ee

//斜率 每摄氏度4.3mV 对应每摄氏度0x05
//#define AVG_SLOPE 0x05

#define ADC_DataSize	2000

extern __IO uint16_t ADC_ConvertedValue[ADC_DataSize];

void SetADCSampleRate(uint32_t SampleRate);
void ADCx_Init(void);

#endif
