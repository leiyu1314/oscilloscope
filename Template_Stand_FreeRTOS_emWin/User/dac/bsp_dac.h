#ifndef __DAC_H
#define	__DAC_H


#include "stm32f10x.h"

//DAC DHR12RD�Ĵ�����12λ���Ҷ��롢˫ͨ��
#define DAC_DHR12RD_ADDRESS      (DAC_BASE+0x20) 


void DAC_Mode_Init(void);
uint32_t SetDACFreq(uint32_t);

#endif /* __DAC_H */
