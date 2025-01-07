#include <stdio.h>
#include "DSO.h"

long WaveBuffer[WAVE_BUFF_SIZE];									//示波器采样深度

short GraphShowBuffer[SHOW_BUFF_SIZE];                   // 显示在屏幕上用的数组

const DSO_MATCH_2PARAMS _tgrade[SPSMAXGRADE + 1] =
{
	//[采样率，时基] 时基：示波器水平每格所代表的时间，时基变小，波形放大
    //5us为单位1
    {1000000, 5}, //0  ,50us
    {500000, 10},
    {200000, 20},
    {100000, 50},
    {50000, 100},	//起始档位
    {20000, 200}, 
    {10000, 500},
    {5000, 1000},
    {2000, 2000},
    {1000, 5000},
    {500, 10000}
};

const I16 _vgrade[VOLTAGEMAXGRADE + 1] = { 1, 2, 5, 10, 20, 50, 100, 200, 500};

const I32 _DACgrade[DACMAXGRADE + 1] = {625, 1250, 2500, 12500, 62500 ,187500};


DSOSHOWPARAMS DSOShowParams =
{
    0, 0, 0, 0, 1, 0, 3
};


DSOPARAMS DSOParams =
{
    SAMPLE_DEPTH, DSO_RUN, 1, 869, 0, 0, SPS_DEFAULT_GRADE, VOLTAGE_DEFAULT_GRADE,
};

DACPARAMS DACParams =
{
    DAC_DEFAULT_GRADE, 0,
};

/*  函数:  触发位置计算函数，如果有多个触发点选最靠近中间的
*   参数： 触发模式
*          DSO参数结构体指针
*   返回值： 在WaveBuffer中的触发位置
*/
I16 Trigger(char trmode, DSOPARAMS* DSOParams)
{
	int i;
	int trpos, tempi, temptr;
	switch(trmode)
	{
		case 0:
        for (i = 0, trpos = 0; i < GUI_COUNTOF(WaveBuffer) - 1; i++)
        {
            if (WaveBuffer[i] < DSOParams->TriggerLevel && WaveBuffer[i + 1] > DSOParams->TriggerLevel)//达到触发电平
            {
                if (!trpos)
                {
                    trpos = i;
                }
                else
                {
                    tempi = ((i - MID_POS_WAVEBUF) > 0) ? (i - MID_POS_WAVEBUF) : (MID_POS_WAVEBUF - i);//后一个触发位置与中间点位置的距离
                    temptr = ((trpos - MID_POS_WAVEBUF) > 0) ? (trpos - MID_POS_WAVEBUF) : (MID_POS_WAVEBUF - trpos);//前一个触发位置与中间点位置的距离
                    if (temptr < tempi)//找出最靠近中间位置的触发点
                        return trpos;
                    else
                        trpos = i;
                }
            }
        }
        return trpos;

    default:
        break;
	}
	return trpos;
}


/*  函数:  计算从WaveBuffer的哪里开始复制250个点
*   参数： 无
*   返回值： 无
*/
void CalShowStartPos(void)
{
	DSOShowParams.TriggerPos = Trigger(0, &DSOParams);		//找出WaveBuffer中的触发位置
	int maxmovepoints;
	I16 xBufPos = DSOShowParams.XBufPos;
	
	//一个小格子是一个单位，即5个点 和 当前时基/5
	I16 unit_small_grid, unit_tbase;
	
	//左边空间不够了
	if((DSOShowParams.TriggerPos + xBufPos) < LEFT_LIMIT)
	{
		if(DSOShowParams.TriggerPos < LEFT_LIMIT)			//触发位置 < 125
		{
			DSOShowParams.ShowStartPos = DSOShowParams.TriggerPos;		//开始显示的位置 = 触发位置
		}
		else
		{
			maxmovepoints = LEFT_LIMIT - DSOShowParams.TriggerPos;
			
			unit_small_grid = (maxmovepoints / POINTS_ONE_MOVE);
			unit_tbase = _tgrade[DSOParams.TimeBaseGrade].TIMEBASE / POINTS_ONE_MOVE;
			
			DSOShowParams.XBufPos = unit_small_grid * POINTS_ONE_MOVE;	//XBufPos = maxmovepoints
			DSOParams.XPos = unit_tbase * unit_small_grid;
			
			DSOShowParams.ShowStartPos = DSOShowParams.TriggerPos + DSOShowParams.XBufPos - LEFT_LIMIT;//ShowStartPos = 0
		}
	}
	//右边空间不够了
	else if((DSOShowParams.TriggerPos + xBufPos) > RIGHT_LIMIT)
	{
		if(DSOShowParams.TriggerPos > RIGHT_LIMIT)
		{
			DSOShowParams.ShowStartPos = DSOShowParams.TriggerPos - SHOW_BUFF_SIZE;		//触发位置做结尾
		}
		else
		{
			maxmovepoints = RIGHT_LIMIT - DSOShowParams.TriggerPos;            //减小Xpos直到正好显示出WaveBuffer的最后一个点

			unit_small_grid = (maxmovepoints / POINTS_ONE_MOVE);
			unit_tbase = _tgrade[DSOParams.TimeBaseGrade].TIMEBASE / POINTS_ONE_MOVE;

			DSOShowParams.XBufPos = unit_small_grid * POINTS_ONE_MOVE;		//XBufPos = maxmovepoints
			DSOParams.XPos = unit_tbase * unit_small_grid;

			DSOShowParams.ShowStartPos = DSOShowParams.TriggerPos + DSOShowParams.XBufPos - LEFT_LIMIT;
		}
	}
	else
	{
			DSOShowParams.ShowStartPos = DSOShowParams.TriggerPos + DSOShowParams.XBufPos - LEFT_LIMIT; //没有溢出，正常计算
	}
}

