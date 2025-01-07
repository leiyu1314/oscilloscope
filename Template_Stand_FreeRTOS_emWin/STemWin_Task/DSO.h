#ifndef __DSO_H
#define __DSO_H

#include "DIALOG.h"

#define     SHOW_WAVE				0					//��ʾ����
#define			SHOW_FFT				1					//��ʾFFT

#define     DSO_STOP        0
#define     DSO_RUN         1

#define			NUMS_PER_mV					((double)4096/(double)3300)

#define     SHOW_BUFF_SIZE            250           //  һ����Ļ��ʾ250����
#define     SAMPLE_DEPTH              2048          //  �洢���1000 * 2�ֽڣ�short��
#define     WAVE_BUFF_SIZE            (SAMPLE_DEPTH / 2)  //1024
#define			MID_POS_WAVEBUF						(WAVE_BUFF_SIZE / 2)	//512
#define			MID_POS_SHOWBUF						(SHOW_BUFF_SIZE / 2)	//125

#define			POINTS_ONE_MOVE						 5				//ÿ��������һС����5����

#define     VOLTAGEMAXGRADE            8           // ��ѹ��λ 0-8,      ��1mVΪ��λ1
#define     VOLTAGE_DEFAULT_GRADE      8

#define     SPSMAXGRADE                10           // [�����ʣ�ʱ��]��λ 0-10,    ��5usΪ��λ1
#define     SPS_DEFAULT_GRADE          4

#define     DACMAXGRADE                5           // DAC��λ 0-5
#define     DAC_DEFAULT_GRADE          2 

#define			LEFT_LIMIT								 MID_POS_SHOWBUF	//125
#define			RIGHT_LIMIT								 (WAVE_BUFF_SIZE - MID_POS_SHOWBUF)	//1024-125

typedef struct{I16 width, height, pointspace;}GRID;
typedef struct{I16 x0, y0, xsize, ysize;}WIGET;

/*��������:��һ����pc���͵������������ĵ�������������������ÿ�δ���pc�ڵ���������
���������Ϊ1000��pc�ڻῪ�ٳ�ʼ��С1000��buffer��buffer��С�����Լ��ģ���
ÿ��1000����pc��һ�Ρ�����ÿ�δ�buffer��1000����д���*/
/*����Ƶ�ʣ�HZ���������ʣ�SPS��
		��������ָ��ͨ��ÿ���Ӳ����ĵ����������˲����ľ��ȣ������Ƶ�ʻ�����һ���ģ�ֻ�ǵ�λ��ע��һ����
�������Ƶ����1000���������ÿ���Ӳ�1000���㣬�����������Ϊ100����ÿ������PC������10�Ρ�
		�������Ƶ��Ϊ 1000����������Ҳ��Ϊ 1000�����ݵĸ�������1��/ÿ�롣
		�������Ƶ��Ϊ 1000����������Ҳ��Ϊ 100�����ݵĸ�������10��/ÿ�롣*/
typedef struct {                //  �����ʺ�ʱ��
    I32 SPS;
    I32 TIMEBASE;
}DSO_MATCH_2PARAMS;


typedef struct {                //  ʾ�����������
    const I16 Depth;						//�洢���
    I8 StopFlag;								//ֹͣ��־
    char TriggerMode;						//����ģʽ(�Զ�ģʽ(Auto)������ģʽ(Normal)������ģʽ(Single))
    I16 TriggerLevel;						//������ƽ,Trigger()������
    I32 XPos;										//X���ƶ�����
    I32 YPos;										//Y���ƶ�����
    I8 TimeBaseGrade;						//ʱ����λ
    I8 VoltageBaseGrade;				//��ѹ��λ��һ�����(�߶�)��Ӧ�ĵ�ѹ��ֵ
}DSOPARAMS;


typedef struct {                //  ʾ������ʾ����
    I8 ShowMode;                //  ѡ��FFT���ǲ���

    I16 TriggerPos;							//WaveBuffer�еĴ���λ��
    double XBufPos;
    I16 ShowStartPos;						//����λ�ö�Ӧ����ʾλ��

    double YRatio;
    I16 YStartPos;

    I8 XExpan;				//mode
}DSOSHOWPARAMS;


typedef struct {                //  ���β���
    I16 MaxValue;
    I16 MinValue;
    I16 PPValue;

    double Freq;
    I32 Period;
}WAVEPARAMS;


typedef struct {                //  DAC����
    I32 DACFreqGrade;
    I8 DACMode;
}DACPARAMS;


//�ⲿ����
extern long WaveBuffer[WAVE_BUFF_SIZE];
extern long MagArray[WAVE_BUFF_SIZE/2];	//FFT��ֵ

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
