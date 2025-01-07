/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* ������Ӳ��bspͷ�ļ� */
//#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_timbasic.h"
#include <string.h>
//#include "bsp_chipid.h"
#include "bsp_key.h"
#include "bsp_xpt2046_lcd.h"
#include "bsp_ili9341_lcd_emwin.h"
//#include "bsp_fsmc_sram.h"
#include "bsp_adc.h"
#include "bsp_dac.h"
//#include "bsp_AdvanceTim.h"

#include "stm32f10x.h"

/*STemWinͷ�ļ�*/
#include "GUI.h"
//#include "DIALOG.h"
#include "MainTask.h"
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;

static TaskHandle_t Touch_Task_Handle = NULL;
static TaskHandle_t Key_Task_Handle = NULL;
static TaskHandle_t GUI_Task_Handle = NULL;
static TaskHandle_t CPU_Task_Handle = NULL;
/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void Touch_Task(void* pvParameters);/*Touch_Task����ʵ��*/
static void Key_Task(void* pvParameters);/*Key_Task����ʵ��*/
static void GUI_Task(void* pvParameters);/* GUI_Task����ʵ�� */
static void CPU_Task(void* pvParameyers);/*CPU_Task����ʵ��*/

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

  /* ������Ӳ����ʼ�� */
  BSP_Init();
	
  printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS-�̼���ʵ��\n\n");
	
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )256,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
  /* ����Touch_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Touch_Task, /* ������ں��� */
                        (const char*    )"Touch_Task",/* �������� */
                        (uint16_t       )128,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )4,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Touch_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����Touch_Task����ɹ�!\r\n");
  
  /* ����Key_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Key_Task, /* ������ں��� */
                        (const char*    )"Key_Task",/* �������� */
                        (uint16_t       )128,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )10,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Key_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����Key_Task����ɹ�!\r\n");
	
	/*����GUI_Task����*/
	xReturn = xTaskCreate((TaskFunction_t)GUI_Task,/* ������ں��� */
											 (const char*      )"GUI_Task",/* �������� */
											 (uint16_t         )512,      /* ����ջ��С */
											 (void*            )NULL,      /* ������ں������� */
											 (UBaseType_t      )3,         /* ��������ȼ� */
											 (TaskHandle_t     )&GUI_Task_Handle);/* ������ƿ�ָ�� */
	if(pdPASS == xReturn)
		printf("����GUI_Task����ɹ���\r\n");
	
	/*����CPU_Task����*/
	xReturn = xTaskCreate((TaskFunction_t)CPU_Task,/* ������ں��� */
											 (const char*      )"CPU_Task",/* �������� */
											 (uint16_t         )256,      /* ����ջ��С */
											 (void*            )NULL,      /* ������ں������� */
											 (UBaseType_t      )5,         /* ��������ȼ� */
											 (TaskHandle_t     )&CPU_Task_Handle);/* ������ƿ�ָ�� */
	if(pdPASS == xReturn)
		printf("����CPU_Task����ɹ���\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



/**********************************************************************
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Touch_Task(void* parameter)
{	
	while(1)
	{
		GUI_TOUCH_Exec();	//��������ʱɨ��
		vTaskDelay(10);
	}
}

static void Key_Task(void* parameter)
{
	static int8_t Index, LastIndex;
	while(1)
	{ 
		switch(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN))
		{
			case KEY_ON:
				_cbKey(Index,1);
				break;
			
			case KEY_HOLD:
				_cbKey(Index,1);
				break;
			
			case KEY_2CLICK	:
				LastIndex = Index;
				if(Index++ == trmode)
					Index = channel;
				PickActiveWin(Index,LastIndex);
				break;
				
			case KEY_3CLICK:
				_cbKey(dac,1);
				break;
			
			case KEY_ERROR:
				printf("error\r\n");
				break;
			
			default:
				break;
		}
		
		switch(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN))
		{
			case KEY_ON:
				_cbKey(Index, 0);
				break;
			
			case KEY_HOLD:
				_cbKey(Index, 0);
				break;
			
			case KEY_2CLICK:
				LastIndex = Index;
				if(Index-- == channel)
					Index = trmode;
				PickActiveWin(Index, LastIndex);
				break;
				
			case KEY_3CLICK:
				_cbKey(dac, 0);
				break;
			
			case KEY_ERROR:
				printf("error\n");
				break;
			
			default:
				break;
		}
		vTaskDelay(10);
	}
}

/**
  * @brief GUI��������
  * @note ��
  * @param ��
  * @retval ��
  */
static void GUI_Task(void* parameter)
{

	while(1)
	{
		MainTask();
	}
}

/**
  * @brief GUI��������
  * @note ��
  * @param ��
  * @retval ��
  */
static void CPU_Task(void* parameter)
{
	uint8_t CPU_RunInfo[400];	//������������ʱ����Ϣ
	
	while(1)
	{
		memset(CPU_RunInfo, 0, 400); //��Ϣ����������

        vTaskList((char *)&CPU_RunInfo); //��ȡ��������ʱ����Ϣ

        printf("---------------------------------------------\r\n");
        printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
        printf("%s", CPU_RunInfo);
        printf("---------------------------------------------\r\n");

        memset(CPU_RunInfo, 0, 400); //��Ϣ����������

        vTaskGetRunTimeStats((char *)&CPU_RunInfo);

        printf("������       ���м���         ������\r\n");
        printf("%s", CPU_RunInfo);
        printf("---------------------------------------------\r\n\n");
        vTaskDelay(20000); /* ��ʱ20000��tick */
	}
}
/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/* CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı���������
   * ����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ�
   */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	//LED_GPIO_Config();

	/* ���ڳ�ʼ��	*/
	USART_Config();
  
  /* ������ʱ����ʼ��	*/
	BASIC_TIM_Init();
  
	/* ������ʼ��	*/
  Key_GPIO_Config();
  
  /* ��������ʼ�� */
	XPT2046_Init();
  
  /* ��ǰ��ʼ��LCD��������GUI_Init����LCD_X_Config֮ǰȷ��LCD����оƬID: lcdid */
  ILI9341_Init();
	
	/* ADC��ʼ�� */
	ADCx_Init();
	
	/* DAC��ʼ�� */
	DAC_Mode_Init();
	
	/*���PWM*/
	//ADVANCE_TIM_Init();
}

/********************************END OF FILE****************************/
