#include <stdio.h>
#include "DSO.h"

long WaveBuffer[WAVE_BUFF_SIZE];									//ʾ�����������

short GraphShowBuffer[SHOW_BUFF_SIZE];                   // ��ʾ����Ļ���õ�����

const DSO_MATCH_2PARAMS _tgrade[SPSMAXGRADE + 1] =
{
	//[�����ʣ�ʱ��] ʱ����ʾ����ˮƽÿ���������ʱ�䣬ʱ����С�����ηŴ�
    //5usΪ��λ1
    {1000000, 5}, //0  ,50us
    {500000, 10},
    {200000, 20},
    {100000, 50},
    {50000, 100},	//��ʼ��λ
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

/*  ����:  ����λ�ü��㺯��������ж��������ѡ����м��
*   ������ ����ģʽ
*          DSO�����ṹ��ָ��
*   ����ֵ�� ��WaveBuffer�еĴ���λ��
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
            if (WaveBuffer[i] < DSOParams->TriggerLevel && WaveBuffer[i + 1] > DSOParams->TriggerLevel)//�ﵽ������ƽ
            {
                if (!trpos)
                {
                    trpos = i;
                }
                else
                {
                    tempi = ((i - MID_POS_WAVEBUF) > 0) ? (i - MID_POS_WAVEBUF) : (MID_POS_WAVEBUF - i);//��һ������λ�����м��λ�õľ���
                    temptr = ((trpos - MID_POS_WAVEBUF) > 0) ? (trpos - MID_POS_WAVEBUF) : (MID_POS_WAVEBUF - trpos);//ǰһ������λ�����м��λ�õľ���
                    if (temptr < tempi)//�ҳ�����м�λ�õĴ�����
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


/*  ����:  �����WaveBuffer�����￪ʼ����250����
*   ������ ��
*   ����ֵ�� ��
*/
void CalShowStartPos(void)
{
	DSOShowParams.TriggerPos = Trigger(0, &DSOParams);		//�ҳ�WaveBuffer�еĴ���λ��
	int maxmovepoints;
	I16 xBufPos = DSOShowParams.XBufPos;
	
	//һ��С������һ����λ����5���� �� ��ǰʱ��/5
	I16 unit_small_grid, unit_tbase;
	
	//��߿ռ䲻����
	if((DSOShowParams.TriggerPos + xBufPos) < LEFT_LIMIT)
	{
		if(DSOShowParams.TriggerPos < LEFT_LIMIT)			//����λ�� < 125
		{
			DSOShowParams.ShowStartPos = DSOShowParams.TriggerPos;		//��ʼ��ʾ��λ�� = ����λ��
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
	//�ұ߿ռ䲻����
	else if((DSOShowParams.TriggerPos + xBufPos) > RIGHT_LIMIT)
	{
		if(DSOShowParams.TriggerPos > RIGHT_LIMIT)
		{
			DSOShowParams.ShowStartPos = DSOShowParams.TriggerPos - SHOW_BUFF_SIZE;		//����λ������β
		}
		else
		{
			maxmovepoints = RIGHT_LIMIT - DSOShowParams.TriggerPos;            //��СXposֱ��������ʾ��WaveBuffer�����һ����

			unit_small_grid = (maxmovepoints / POINTS_ONE_MOVE);
			unit_tbase = _tgrade[DSOParams.TimeBaseGrade].TIMEBASE / POINTS_ONE_MOVE;

			DSOShowParams.XBufPos = unit_small_grid * POINTS_ONE_MOVE;		//XBufPos = maxmovepoints
			DSOParams.XPos = unit_tbase * unit_small_grid;

			DSOShowParams.ShowStartPos = DSOShowParams.TriggerPos + DSOShowParams.XBufPos - LEFT_LIMIT;
		}
	}
	else
	{
			DSOShowParams.ShowStartPos = DSOShowParams.TriggerPos + DSOShowParams.XBufPos - LEFT_LIMIT; //û���������������
	}
}

