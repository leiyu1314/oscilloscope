/**
  ******************************************************************************
  * @file    bsp_adc.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��ȡоƬ�¶�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue[ADC_DataSize];

/**
  * @brief  ADC GPIO ��ʼ��
  * @param  ��
  * @retval ��
  */
static void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// �� ADC IO�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	// ���� ADC IO ����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// ��ʼ�� ADC IO
	GPIO_Init(GPIOC, &GPIO_InitStructure);				
}

/**
  * @brief  TIM3 ��ʼ��
  * @param  ��
  * @retval ��
  */

static void TIM3_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /* ʹ��TIM3ʱ�ӣ�TIM3CLK Ϊ72M */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* TIM3������ʱ������ */
    TIM_TimeBaseStructure.TIM_Period = (12 - 1);              //��ʱ���� 72 /12 =6
    TIM_TimeBaseStructure.TIM_Prescaler = (6-1);     //Ԥ��Ƶ 72M / 6 = 1M
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;              //ʱ�ӷ�Ƶϵ��
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* ����TIM3����Դ */
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

    /* ʹ��TIM3 */
    TIM_Cmd(TIM3, ENABLE);
}


/**
  * @brief  ����ADC����ģʽ
  * @param  ��
  * @retval ��
  */
void ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	//��DMAʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	//��ADCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	//��λDMA������
	DMA_DeInit(DMA1_Channel1);
	
	//����DMA��ʼ���ṹ��
	//��������С���������Ĵ�С���ڴ洢���Ĵ�С
	DMA_InitStructure.DMA_BufferSize = ADC_DataSize;
	
	//����Դ��������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	//��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	//�洢����ַ��ʵ����һ���ڲ�SRAM�ı���
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
	
	//�洢�����ݴ�СΪ���֣��������ֽ�
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	
	//�洢����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	
	//ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	
	//�������ַ��ADC���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));
	
	//�������ݴ�СΪ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	
	//����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	
	// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	//��ʼ��DMA
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	
	//ʹ��DMAͨ��
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	//ADCģʽ����
	//����ת��ģʽ����TIM3����
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	//ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//ʹ��TIM3��TRGO��Ϊ����Դ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	//ֻʹ��һ��ADC�����ڵ�ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	//ת��ͨ��1��
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	//��ֹɨ��ģʽ����ͨ������Ҫ����ͨ������Ҫ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	// ��ʼ��ADC
	ADC_Init(ADC1,&ADC_InitStructure);
	//����ADCʱ��ΪCLK2��6��Ƶ����12MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div2);
	// ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ1.5��ʱ������
	ADC_RegularChannelConfig(ADC1,ADC_Channel_15,1,ADC_SampleTime_1Cycles5);
	// ʹ��ADC DMA ����
	ADC_DMACmd(ADC1,ENABLE);
	// ����ADC ������ʼת��
	ADC_Cmd(ADC1,ENABLE);
	// ��ʼ��ADC У׼�Ĵ���  
	ADC_ResetCalibration(ADC1);
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	// ADC��ʼУ׼
	ADC_StartCalibration(ADC1);
	// �ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
}

/**
  * @brief  ����adc�����ʣ�����Ĳ�������ʵ�Ƕ�ʱ��������Ƶ��
  * @param  ��
  * @retval ��
  */
void SetADCSampleRate(uint32_t SampleRate)
{
	TIM_PrescalerConfig(TIM3, 6000000 / SampleRate - 1, TIM_PSCReloadMode_Immediate);
}

/**
  * @brief  ADC��ʼ��
  * @param  ��
  * @retval ��
  */
void ADCx_Init(void)
{
	ADC_GPIO_Config();
	ADC_Mode_Config();
  TIM3_Config();
	SetADCSampleRate(20000);
}

