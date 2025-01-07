/**
  ******************************************************************************
  * @file    bsp_usart.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usart应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "bsp_usart.h"

static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}


void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	NVIC_Configuration();
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);
	USART_Cmd(DEBUG_USARTx, ENABLE);
	
}


void Usart_SendByte(USART_TypeDef* USARTx, uint8_t ch)
{
	USART_SendData(USARTx, ch);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

void Usart_SendArray(USART_TypeDef* USARTx, uint8_t *array, uint16_t num)
{
	uint8_t i;
	
	for(i=0; i<num; i++)
	{
		Usart_SendByte(USARTx, array[i]);
	}
	
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
}


void Usart_SendString(USART_TypeDef* USARTx, char *str)
{
	unsigned int k = 0;
	do{
		Usart_SendByte(USARTx, *(str+k));
		k++;
	}while(*(str+k) != '\0');
	
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET)
	{}
}


void Usart_SendHalfWord(USART_TypeDef* USARTx,uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	temp_h = (ch&0xff00)>>8;
	temp_l = ch&0xff;
	
	USART_SendData(USARTx,temp_h);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)==RESET);
	
	USART_SendData(USARTx,temp_l);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)==RESET);
}


//重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	USART_SendData(DEBUG_USARTx, (uint8_t)ch);
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE)==RESET);
	return(ch);
}

//重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_RXNE)==RESET);
	return (int)USART_ReceiveData(DEBUG_USARTx);
}
