/*	 
*ADC�������ݴ������������飬
*ADCԭʼ�������飬WaveBuffʾ�����������飬ShowBuff��ʾ����
*/
#include <math.h>
#include <stdio.h>

#include "DSO.h"
#include "stm32f10x.h"

#include "bsp_adc.h"
#include "bsp_usart.h"
#include "stm32_dsp.h"
#include "table_fft.h"

long FFTBuffer[WAVE_BUFF_SIZE / 2];	//ʾ����2000�Ĳ������
long MagArray[WAVE_BUFF_SIZE / 2];	//FFT��ֵ

WAVEPARAMS WaveParams = {0, 4095, 0, 0, 0};

static uint16_t ADC_DMA_Pos;
static uint16_t DMA_Pos_In_Array;

/* �������Ƶ�ʴ�  */
void FFT_GetFreq(I32 SPS)
{
    s16 X1, Y1;
    u32 i;
    u32 maxtemp;
    uint16_t pos;

    uint16_t N = 1024;
		
	/*
		void cr4_fft_1024_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);
		pssOUT����ʾFFT�������ָ��
		pssIN����ʾҪ����FFT�������������ָ��
		Nbin����ʾ����
	
		pssOUT��pssIN��������32λ���������ͣ����и�16λ�洢ʵ������16λ�洢�鲿
		����pssIN��˵����16λ�洢���鲿����Ϊ0
	*/
    cr4_fft_1024_stm32(FFTBuffer, WaveBuffer, N);

    for (i = 0; i < N / 2; i++)//����FFT��Ƶ�����ǹ����ο�˹��Ƶ�ʶԳƵģ�����ֻ����һ��
    {
        X1 = (FFTBuffer[i] << 16) >> 16;	//ȡ��ʮ��λ���鲿
        Y1 = (FFTBuffer[i] >> 16);				//ȡ��ʮ��λ��ʵ��
			
				//����32768�ٳ���65536��Ϊ�˷��ϸ������������
				float X = N * ((float)X1) / 32768;
				float Y = N * ((float)Y1) / 32768;
				float Mag = sqrt(X * X + Y * Y) / N;
				MagArray[i] = (u32)(Mag * 65536);
        //��Щ���Ǽ������FFTBuffer[i]=sqrt(X1*X1+Y1*Y1)*2/N
    }
    MagArray[0] /= 2; //�����ֱ������������Ҫ����2
    for (i = 2, maxtemp = MagArray[1], pos = 1; i < N / 2; i++)
    {
        maxtemp = (maxtemp > MagArray[i]) ? maxtemp : MagArray[i];
        pos = (maxtemp > MagArray[i]) ? pos : i;
    }

    WaveParams.Freq = (double)SPS / N * pos;
}

/* �������ݵ�WaveBuffer��ͬʱ����DMA���츲���������� */
void CopyDataToWaveBuff(void)
{
	uint16_t i, j;
	
	
	/* ��ȡADC_DMA��ǰλ�� */
	ADC_DMA_Pos = ADC_DataSize - DMA_GetCurrDataCounter(ADC_DMA_CHANNEL);	//�����е�ǰλ�� = ��������-��ǰDMAͨ��������ʣ������ݵ�Ԫ��
	
	if(ADC_DMA_Pos < WAVE_BUFF_SIZE)//ADC_ConvertedValue[]�е������޷�װ��WaveBuffer[]
	{
		/* ����У׼��������ʱ��λ�� */
		DMA_Pos_In_Array = WAVE_BUFF_SIZE - ADC_DMA_Pos;
		
		for(j = 0, i = ADC_DataSize - DMA_Pos_In_Array; i < ADC_DataSize; i++, j++)
		{
			WaveBuffer[j] = ADC_ConvertedValue[i]; //����DMA_Pos_In_Array�����ݵ�WaveBuffer[]
		}
		for(i = 0; i < ADC_DMA_Pos; i++, j++)
		{
			WaveBuffer[j] = ADC_ConvertedValue[i];
		}
	}
	else
	{
		for(j = 0, i = ADC_DMA_Pos - WAVE_BUFF_SIZE; i < ADC_DMA_Pos; i++, j++)
		{
			WaveBuffer[j] = ADC_ConvertedValue[i];
		}
	}
	
	/* ��WaveBuffer��һЩ����----������ֵ */
	WaveParams.MaxValue = 0;
	WaveParams.MinValue = 4096;
	WaveParams.PPValue = 0;
	for(i = 0; i < WAVE_BUFF_SIZE; i++)
	{
		WaveParams.MaxValue = (WaveBuffer[i] > WaveParams.MaxValue) ? WaveBuffer[i] : WaveParams.MaxValue;//�ҳ�WaveBuffer[]�����ֵ
		WaveParams.MinValue = (WaveBuffer[i] < WaveParams.MinValue) ? WaveBuffer[i] : WaveParams.MinValue;//�ҳ�WaveBuffer[]����Сֵ
	}
	WaveParams.PPValue = WaveParams.MaxValue - WaveParams.MinValue;//������ֵ
}




