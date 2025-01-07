#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define    KEY1_INT_EXTI_IRQ      EXTI0_IRQn
#define    KEY2_INT_EXTI_IRQ			EXTI15_10_IRQn

#define    KEY1_GPIO_PORT					GPIOA
#define    KEY1_GPIO_PIN					GPIO_Pin_0

#define    KEY2_GPIO_PORT					GPIOC
#define    KEY2_GPIO_PIN					GPIO_Pin_13

typedef struct{
	uint8_t KeyLogic;						//�߼�״̬���رա�����������
	uint8_t KeyPhysic;					//����״̬���رա�����
	uint8_t KeyONCounts;				//��������������ʱ�����ã����ҿ����������������Գ�������ʱ����
	uint8_t KeyOFFCounts;				//�رռ������ر�ʱ������
	
	uint8_t MulClickCounts;			//����������ж����ΰ���ʱ������û�г�ʱ
	uint8_t MulClickFlag;				//�����־��
}KEY_TypeDef;

#define    KEY_OFF								0
#define 	 KEY_ON									1
#define    KEY_HOLD								7

#define    KEY_1CLICK							KEY_ON
#define    KEY_2CLICK							2
#define    KEY_3CLICK							3
#define    KEY_4CLICK							4
#define    KEY_5CLICK							5
#define    KEY_MAX_MULCLICK				KEY_5CLICK

#define		 KEY_IDLE								8
#define    KEY_ERROR							10

#define		 HOLD_COUNTS						100
#define    SHAKES_COUNTS					8
#define		 MULTIPLE_CLICK_COUNTS	20

#define    KEY_NUMS					2

//void EXTI_Key_Config(void);
void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
void Delay_ms(uint32_t);
#endif
