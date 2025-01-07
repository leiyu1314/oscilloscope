/*	 
*ADC采样数据处理，有三个数组，
*ADC原始数据数组，WaveBuff示波器缓冲数组，ShowBuff显示数组
*/
#include <math.h>
#include <stdio.h>

#include "DSO.h"
#include "stm32f10x.h"

#include "bsp_adc.h"
#include "bsp_usart.h"
#include "stm32_dsp.h"
#include "table_fft.h"

long FFTBuffer[WAVE_BUFF_SIZE / 2];	//示波器2000的采样深度
long MagArray[WAVE_BUFF_SIZE / 2];	//FFT幅值

WAVEPARAMS WaveParams = {0, 4095, 0, 0, 0};

static uint16_t ADC_DMA_Pos;
static uint16_t DMA_Pos_In_Array;

/* 返回最大频率处  */
void FFT_GetFreq(I32 SPS)
{
    s16 X1, Y1;
    u32 i;
    u32 maxtemp;
    uint16_t pos;

    uint16_t N = 1024;
		
	/*
		void cr4_fft_1024_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);
		pssOUT：表示FFT输出数组指针
		pssIN：表示要进行FFT运算的输入数组指针
		Nbin：表示点数
	
		pssOUT和pssIN都必须是32位的数据类型，其中高16位存储实部，低16位存储虚部
		对于pssIN来说，低16位存储的虚部总是为0
	*/
    cr4_fft_1024_stm32(FFTBuffer, WaveBuffer, N);

    for (i = 0; i < N / 2; i++)//由于FFT的频域结果是关于奈奎斯特频率对称的，所以只计算一半
    {
        X1 = (FFTBuffer[i] << 16) >> 16;	//取低十六位，虚部
        Y1 = (FFTBuffer[i] >> 16);				//取高十六位，实部
			
				//除以32768再乘以65536是为了符合浮点数计算规律
				float X = N * ((float)X1) / 32768;
				float Y = N * ((float)Y1) / 32768;
				float Mag = sqrt(X * X + Y * Y) / N;
				MagArray[i] = (u32)(Mag * 65536);
        //这些就是计算振幅FFTBuffer[i]=sqrt(X1*X1+Y1*Y1)*2/N
    }
    MagArray[0] /= 2; //这个是直流分量，不需要乘以2
    for (i = 2, maxtemp = MagArray[1], pos = 1; i < N / 2; i++)
    {
        maxtemp = (maxtemp > MagArray[i]) ? maxtemp : MagArray[i];
        pos = (maxtemp > MagArray[i]) ? pos : i;
    }

    WaveParams.Freq = (double)SPS / N * pos;
}

/* 拷贝数据到WaveBuffer，同时避免DMA过快覆盖数组数据 */
void CopyDataToWaveBuff(void)
{
	uint16_t i, j;
	
	
	/* 获取ADC_DMA当前位置 */
	ADC_DMA_Pos = ADC_DataSize - DMA_GetCurrDataCounter(ADC_DMA_CHANNEL);	//数组中当前位置 = 数据总数-当前DMA通道传输中剩余的数据单元数
	
	if(ADC_DMA_Pos < WAVE_BUFF_SIZE)//ADC_ConvertedValue[]中的数据无法装满WaveBuffer[]
	{
		/* 用于校准拷贝数组时的位置 */
		DMA_Pos_In_Array = WAVE_BUFF_SIZE - ADC_DMA_Pos;
		
		for(j = 0, i = ADC_DataSize - DMA_Pos_In_Array; i < ADC_DataSize; i++, j++)
		{
			WaveBuffer[j] = ADC_ConvertedValue[i]; //拷贝DMA_Pos_In_Array个数据到WaveBuffer[]
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
	
	/* 对WaveBuffer做一些处理----计算峰峰值 */
	WaveParams.MaxValue = 0;
	WaveParams.MinValue = 4096;
	WaveParams.PPValue = 0;
	for(i = 0; i < WAVE_BUFF_SIZE; i++)
	{
		WaveParams.MaxValue = (WaveBuffer[i] > WaveParams.MaxValue) ? WaveBuffer[i] : WaveParams.MaxValue;//找出WaveBuffer[]的最大值
		WaveParams.MinValue = (WaveBuffer[i] < WaveParams.MinValue) ? WaveBuffer[i] : WaveParams.MinValue;//找出WaveBuffer[]的最小值
	}
	WaveParams.PPValue = WaveParams.MaxValue - WaveParams.MinValue;//求出峰峰值
}




