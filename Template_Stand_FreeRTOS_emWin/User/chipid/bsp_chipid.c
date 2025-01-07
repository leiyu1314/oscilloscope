/**
  ******************************************************************************
  * @file    bsp_chipid.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   stm32 chipid
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

//获取CPU的ID函数，每个芯片都有唯一的 96_bit unique ID         

#include "bsp_chipid.h"

uint32_t ChipUniqueID[3];

void Get_ChipID(void)
{
	ChipUniqueID[0] = *(__IO u32 *)(0x1FFFF7F0);	//高字节
	ChipUniqueID[1] = *(__IO u32 *)(0x1FFFF7FC);	
	ChipUniqueID[2] = *(__IO u32 *)(0x1FFFF7E8);	//低字节
}
