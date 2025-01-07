#include "bsp_key.h"
#include "bsp_usart.h"

/*static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = KEY1_INT_EXTI_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = KEY2_INT_EXTI_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
	
	NVIC_Configuration();
	//KEY1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	//KEY2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13);
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
}*/
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
	GPIO_Init(KEY1_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
	GPIO_Init(KEY2_GPIO_PORT,&GPIO_InitStructure);
}

static KEY_TypeDef Key[KEY_NUMS]={
		{ KEY_OFF, KEY_OFF, 0, 0, 0, KEY_1CLICK},
		{ KEY_OFF, KEY_OFF, 0, 0, 0, KEY_1CLICK}
};
/*
 * ��������Key_Scan
 * ����  ������Ƿ��а�������
 * ����  ��GPIOx��gpio��port
 *		   GPIO_Pin��gpio��pin
 * ���  ��KEY_OFF��KEY_ON��KEY_HOLD��KEY_IDLE��KEY_ERROR
 */
 
uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	KEY_TypeDef *KeyTemp;
	uint8_t ReturnTemp;

	//��鰴�µ�����һ����ť
	switch ((uint32_t)GPIOx)
	{
		case ((uint32_t)KEY1_GPIO_PORT):
					switch (GPIO_Pin)
					{
							case KEY1_GPIO_PIN:
								KeyTemp = &Key[0];
								break;

							//port��pin��ƥ��
							default:
								printf("error: GPIO port pin not match\r\n");
								return KEY_IDLE;
					}
					
					break;

		case ((uint32_t)KEY2_GPIO_PORT):
					switch (GPIO_Pin)
					{
							case KEY2_GPIO_PIN:
								KeyTemp = &Key[1];
								break;

							//port��pin��ƥ��
							default:
								printf("error: GPIO port pin not match\r\n");
								return KEY_IDLE;
					}
					
					break;

		default:
			printf("error: key do not exist\r\n");
			return KEY_IDLE;
	}

	/* ��ⰴ�¡��ɿ������� */
	KeyTemp->KeyPhysic = GPIO_ReadInputDataBit(GPIOx, GPIO_Pin);

	switch (KeyTemp->KeyLogic)
	{
	
	case KEY_ON:
		switch (KeyTemp->KeyPhysic)
		{
		
		//��1��1���н��رռ������㣬���Կ��������ۼ�ֱ���л����߼�����״̬
		case KEY_ON:
			KeyTemp->KeyOFFCounts = 0;
			KeyTemp->KeyONCounts++;
			KeyTemp->MulClickCounts = 0;
			
			if(KeyTemp->MulClickFlag == KEY_2CLICK)//ready for 2 clicks, but still only 1 click
			{
				if ((KeyTemp->KeyONCounts) >= HOLD_COUNTS)
				{
					KeyTemp->KeyONCounts = 0;
					KeyTemp->KeyLogic = KEY_HOLD;
					return KEY_HOLD;
				}
			}
			return KEY_IDLE;
			
		//��1��0���жԹرռ����ۼ�ֱ���л����߼��ر�״̬
		case KEY_OFF:
			KeyTemp->KeyOFFCounts++;
		
			if (KeyTemp->KeyOFFCounts >= SHAKES_COUNTS)
			{
				KeyTemp->KeyLogic = KEY_OFF;
				KeyTemp->KeyOFFCounts = 0;
				
				return KEY_OFF;
			}
			return KEY_IDLE;

		default:
			break;
		}

	case KEY_OFF:
		switch (KeyTemp->KeyPhysic)
		{
		
		//��0��1���жԿ��������ۼ�ֱ���л����߼�����״̬
		case KEY_ON:
			(KeyTemp->KeyONCounts)++;
			if (KeyTemp->KeyONCounts >= SHAKES_COUNTS)
			{
				KeyTemp->KeyLogic = KEY_ON;
				KeyTemp->KeyONCounts = 0;

				if(KeyTemp->MulClickFlag == KEY_1CLICK)
				{
					KeyTemp->MulClickFlag = KEY_2CLICK;	//Ԥ��˫��״̬
					return KEY_IDLE;
				}
				else
				{
					if(KeyTemp->MulClickFlag != (KEY_MAX_MULCLICK + 1))
					{
						KeyTemp->MulClickFlag++;
						KeyTemp->MulClickCounts = 0;
					}
				}
			}
			return KEY_IDLE;
			
		//��0��0���н�������������,�Զ������
		case KEY_OFF:
			(KeyTemp->KeyONCounts) = 0;
			if(KeyTemp->MulClickFlag != KEY_1CLICK)
			{
				if(KeyTemp->MulClickCounts++ > MULTIPLE_CLICK_COUNTS)	//������������ʱ�䣬�رն��״̬
				{
					ReturnTemp = KeyTemp->MulClickFlag - 1;
					KeyTemp->MulClickCounts = 0;
					KeyTemp->MulClickFlag = KEY_1CLICK;
					return ReturnTemp;
				}
			}
			
			return KEY_IDLE;
		default:
			break;
		}

	case KEY_HOLD:
		switch (KeyTemp->KeyPhysic)
		{
		
		//��2��1���Թرռ�������
		case KEY_ON:
			KeyTemp->KeyOFFCounts = 0;
			KeyTemp->MulClickFlag = 0;
			KeyTemp->MulClickCounts = 0;
			return KEY_HOLD;
		//��2��0���Թرռ����ۼ�ֱ���л����߼��ر�״̬
		case KEY_OFF:
			(KeyTemp->KeyOFFCounts)++;
			if (KeyTemp->KeyOFFCounts >= SHAKES_COUNTS)
			{
				KeyTemp->KeyLogic = KEY_OFF;
				KeyTemp->KeyOFFCounts = 0;
				return KEY_OFF;
			}
			return KEY_IDLE;

		default:
			break;
		}

	default:
		break;
	}
	
	//һ�㲻�ᵽ����
	return KEY_ERROR;
}
