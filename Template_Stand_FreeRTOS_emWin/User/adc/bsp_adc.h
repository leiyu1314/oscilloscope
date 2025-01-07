#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define ADC_DMA_CHANNEL				DMA1_Channel1

//����12λ��ADC��3.3V��ADCֵΪ0xfff,�¶�Ϊ25��ʱ��Ӧ�ĵ�ѹֵΪ1.43V��0x6EE
//#define V25 0x6ee

//б�� ÿ���϶�4.3mV ��Ӧÿ���϶�0x05
//#define AVG_SLOPE 0x05

#define ADC_DataSize	2000

extern __IO uint16_t ADC_ConvertedValue[ADC_DataSize];

void SetADCSampleRate(uint32_t SampleRate);
void ADCx_Init(void);

#endif
