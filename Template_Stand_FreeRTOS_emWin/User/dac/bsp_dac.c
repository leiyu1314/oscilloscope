/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   dac 应用bsp / DMA 模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 指南者 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "bsp_dac.h"

//正弦波单个周期的点数
#define POINT_NUM 32

/* 波形数据 ---------------------------------------------------------*/
const uint16_t Sine12bit[POINT_NUM] = {
	2048	, 2460	, 2856	, 3218	, 3532	, 3786	, 3969	, 4072	,
	4093	, 4031	, 3887	, 3668	, 3382	, 3042	,	2661	, 2255	, 
	1841	, 1435	, 1054	, 714		, 428		, 209		, 65		, 3			,
	24		, 127		, 310		, 564		, 878		, 1240	, 1636	, 2048
};

uint32_t DualSine12bit[POINT_NUM];


/**
  * @brief  使能DAC的时钟，初始化GPIO
  * @param  无
  * @retval 无
  */
static void DAC_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef  DAC_InitStructure;
	
	/* 使能GPIOA 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* 使能DAC时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	/*DAC的GPIO配置，模拟输入*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 配置DAC通道1 */
	//使用TIM2作为触发源
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
	//不使用波形发生器
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	//不使用DAC输出缓冲
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	/* 配置DAC通道2 */
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	
	/* 使能通道1 由PA4输出 */
	DAC_Cmd(DAC_Channel_1, ENABLE);
	/* 使能通道2 由PA5输出 */
	DAC_Cmd(DAC_Channel_2, ENABLE);
	
	/* 使能DAC的DMA请求 */
	DAC_DMACmd(DAC_Channel_2, ENABLE);
}

/**
  * @brief  配置TIM
  * @param  无
  * @retval 无
  */
static void DAC_TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/* 使能TIM2时钟，TIM2CLK为72M */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	/* TIM2通用定时器配置，DAC频率为2500Hz (72MHz/(150*6*32) = 2500Hz)  */
	TIM_TimeBaseStructure.TIM_Prescaler = (150 - 1);	//预分频，72M / 150 
	TIM_TimeBaseStructure.TIM_Period = (6 - 1);	//定时周期 6 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//时钟分频系数
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* 配置TIM2触发源 */
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	
	/* 使能TIM2 */
	TIM_Cmd(TIM2, ENABLE);
}


/**
  * @brief  配置DMA
  * @param  无
  * @retval 无
  */
static void DAC_DMA_Config(void)
{
	DMA_InitTypeDef	DMA_InitStructure;
	
	/* 使能DMA2时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	
	/* 配置DMA2 */
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_ADDRESS;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DualSine12bit;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = POINT_NUM;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA2_Channel4, &DMA_InitStructure);
	
	/* 使能DMA2-4通道*/
	DMA_Cmd(DMA2_Channel4, ENABLE);
}


/**
  * @brief  DAC初始化函数
  * @param  无
  * @retval 无
  */
void DAC_Mode_Init(void)
{
    uint32_t Idx = 0;

    DAC_Config();
    DAC_TIM_Config();

    /* 填充正弦波形数据，双通道右对齐*/
    for (Idx = 0; Idx < POINT_NUM; Idx++)
    {
        DualSine12bit[Idx] = (Sine12bit[Idx] << 16) + (Sine12bit[Idx]);
    }

    DAC_DMA_Config();
}

/**
* @brief  更改DAC频率
* @param  无
* @retval 无
* @定时器触发一次，生成一个点，一个周期内，产生32个点，Fdac = 72MHz/((PSC+1)*6*32)
*/
uint32_t SetDACFreq(uint32_t Freq)
{
	TIM_PrescalerConfig(TIM2, (375000 / Freq) - 1, TIM_PSCReloadMode_Immediate);
	return Freq;
}





