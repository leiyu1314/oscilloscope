#ifndef __DSO_H
#define __DSO_H

#include "DIALOG.h"

#define     SHOW_WAVE				0					//显示波形
#define			SHOW_FFT				1					//显示FFT

#define     DSO_STOP        0
#define     DSO_RUN         1

#define			NUMS_PER_mV					((double)4096/(double)3300)

#define     SHOW_BUFF_SIZE            250           //  一个屏幕显示250个点
#define     SAMPLE_DEPTH              2048          //  存储深度1000 * 2字节（short）
#define     WAVE_BUFF_SIZE            (SAMPLE_DEPTH / 2)  //1024
#define			MID_POS_WAVEBUF						(WAVE_BUFF_SIZE / 2)	//512
#define			MID_POS_SHOWBUF						(SHOW_BUFF_SIZE / 2)	//125

#define			POINTS_ONE_MOVE						 5				//每次左右移一小格是5个点

#define     VOLTAGEMAXGRADE            8           // 电压挡位 0-8,      以1mV为单位1
#define     VOLTAGE_DEFAULT_GRADE      8

#define     SPSMAXGRADE                10           // [采样率，时基]挡位 0-10,    以5us为单位1
#define     SPS_DEFAULT_GRADE          4

#define     DACMAXGRADE                5           // DAC挡位 0-5
#define     DAC_DEFAULT_GRADE          2 

#define			LEFT_LIMIT								 MID_POS_SHOWBUF	//125
#define			RIGHT_LIMIT								 (WAVE_BUFF_SIZE - MID_POS_SHOWBUF)	//1024-125

typedef struct{I16 width, height, pointspace;}GRID;
typedef struct{I16 x0, y0, xsize, ysize;}WIGET;

/*采样点数:是一次向pc发送的数据量包含的点数，采样点数决定了每次传到pc内的数据量。
比如点数设为1000，pc内会开辟初始大小1000的buffer（buffer大小可以自己改），
每采1000点往pc传一次。程序每次从buffer读1000点进行处理。*/
/*采样频率（HZ）、采样率（SPS）
		采样率是指该通道每秒钟采样的点数，决定了采样的精度，与采样频率基本是一样的，只是单位标注不一样。
如果采样频率是1000，则代表它每秒钟采1000个点，如果采样点数为100，则每秒钟向PC机传送10次。
		如果采样频率为 1000，采样点数也设为 1000，数据的更新率是1次/每秒。
		如果采样频率为 1000，采样点数也设为 100，数据的更新率是10次/每秒。*/
typedef struct {                //  采样率和时基
    I32 SPS;
    I32 TIMEBASE;
}DSO_MATCH_2PARAMS;


typedef struct {                //  示波器物理参数
    const I16 Depth;						//存储深度
    I8 StopFlag;								//停止标志
    char TriggerMode;						//触发模式(自动模式(Auto)、正常模式(Normal)、单次模式(Single))
    I16 TriggerLevel;						//触发电平,Trigger()函数用
    I32 XPos;										//X轴移动距离
    I32 YPos;										//Y轴移动距离
    I8 TimeBaseGrade;						//时基档位
    I8 VoltageBaseGrade;				//电压档位，一个大格(高度)对应的电压的值
}DSOPARAMS;


typedef struct {                //  示波器显示参数
    I8 ShowMode;                //  选择FFT还是波形

    I16 TriggerPos;							//WaveBuffer中的触发位置
    double XBufPos;
    I16 ShowStartPos;						//触发位置对应的显示位置

    double YRatio;
    I16 YStartPos;

    I8 XExpan;				//mode
}DSOSHOWPARAMS;


typedef struct {                //  波形参数
    I16 MaxValue;
    I16 MinValue;
    I16 PPValue;

    double Freq;
    I32 Period;
}WAVEPARAMS;


typedef struct {                //  DAC参数
    I32 DACFreqGrade;
    I8 DACMode;
}DACPARAMS;


//外部声明
extern long WaveBuffer[WAVE_BUFF_SIZE];
extern long MagArray[WAVE_BUFF_SIZE/2];	//FFT幅值

extern short GraphShowBuffer[SHOW_BUFF_SIZE];

extern DSOPARAMS DSOParams;
extern DSOSHOWPARAMS DSOShowParams;
extern WAVEPARAMS WaveParams;
extern DACPARAMS DACParams;

extern const DSO_MATCH_2PARAMS _tgrade[SPSMAXGRADE + 1];
extern const I16 _vgrade[VOLTAGEMAXGRADE + 1];
extern const I32 _DACgrade[DACMAXGRADE + 1];

void CopyDataToWaveBuff(void);
void CalShowStartPos(void);

void FFT_GetFreq(I32 SPS);

#endif
