/**
  ******************************************************************************
  * @file    bsp_chipid.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   stm32 chipid
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

//��ȡCPU��ID������ÿ��оƬ����Ψһ�� 96_bit unique ID         

#include "bsp_chipid.h"

uint32_t ChipUniqueID[3];

void Get_ChipID(void)
{
	ChipUniqueID[0] = *(__IO u32 *)(0x1FFFF7F0);	//���ֽ�
	ChipUniqueID[1] = *(__IO u32 *)(0x1FFFF7FC);	
	ChipUniqueID[2] = *(__IO u32 *)(0x1FFFF7E8);	//���ֽ�
}
