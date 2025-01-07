/**
  *********************************************************************
  * @file    MainTask.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 ����ģ��
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
/*******************************************************************************
 * ������ͷ�ļ�
 ******************************************************************************/

/* STemWINͷ�ļ� */
#include "GUI.h"
#include "DIALOG.h"
#include "BUTTON.h"
#include "DROPDOWN.h"

/*�Լ�д��ͷ�ļ�*/
#include "MainTask.h"
#include "DSO.h"
#include "bsp_dac.h"
#include "bsp_adc.h"

/*��׼ͷ�ļ�*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
/*********************************************************************
*
*       Dialog resource
*
* This table contAins the info required to create the dialog.
* It has been created manually, but could also be created by a GUI-builder.
*/
#define GUI_ID_FrameGraph           (GUI_ID_USER + 0x01)

#define ID_RightText_channel        (GUI_ID_USER + 0x10)
#define ID_RightText_tbase          (GUI_ID_USER + 0x11)

#define ID_BUTTON_MODE_SWITCH       (GUI_ID_USER + 0x20)
#define ID_BUTTON_STOP_WAVE         (GUI_ID_USER + 0x21)


static GUI_RECT GraphRect = { 10, 20, 260, 220 };       //  ������ʾ������λ��(x0,y0,x1,y1)


// �ұߵ��ı�����  ***************
const char* _aright_text_title[] =                  // �ұ��ı�����⣬����ģ�8�У�
{
    "CHANNEL", "TIMEBASE", "V BASE", "X POS", "Y POS", "TR LEVEL", "MODE", "TR MODE",
};

char RightTextInfo[][10] =                             // �ұ��ı������ݣ�����(8��10��)
{
    "CHA", "100ms", "1V", "128ms", "2.06V", "103mv", "DC", "CHA-UP",
};

const char* _atimebase[] =
{
    "25us", "50us", "100us", "250us", "500us", "1ms", "2.5ms", "5ms", "10ms", "25ms", "50ms"
};


static TEXTSTRUCT RightText[RIGHTTEXT_MAXNUMS] = { 0 };                  /* �ں����ʼ�� */

/* ���ұ��ı���ı��� */
static GUI_POINT _TextBkBig[] = {
    {0                                      , 0},																				//(0,0)
    {RIGHTTEXT_XSIZE - RIGHTTEXT_CSIZE - 1  , 0},																				//(45,0)
    {RIGHTTEXT_XSIZE - 1                    , RIGHTTEXT_CSIZE - 1},											//(49,3)
    {RIGHTTEXT_XSIZE - 1                    , RIGHTTEXT_YSIZE - 1},											//(49,22)
    {RIGHTTEXT_CSIZE - 1                    , RIGHTTEXT_YSIZE - 1},											//(3,22)
    {0                                      , RIGHTTEXT_YSIZE - RIGHTTEXT_CSIZE - 1},		//(0,18)

};

static GUI_POINT _TextBkSmall[] = {
    {RIGHTTEXT_SPACE                        , RIGHTTEXT_HALF_YSIZE - 1},								//(1,10)
    {RIGHTTEXT_XSIZE - RIGHTTEXT_SPACE - 1  , RIGHTTEXT_HALF_YSIZE - 1},								//(48,10)
    {RIGHTTEXT_XSIZE - RIGHTTEXT_SPACE - 1  , RIGHTTEXT_YSIZE - RIGHTTEXT_SPACE - 1},		//(48,21)
    {RIGHTTEXT_SPACE + RIGHTTEXT_CSIZE - 1  , RIGHTTEXT_YSIZE - RIGHTTEXT_SPACE - 1},		//(4,21)
    {RIGHTTEXT_SPACE                        , RIGHTTEXT_YSIZE - RIGHTTEXT_CSIZE - RIGHTTEXT_SPACE - 1}//(1,17)
};

// �ұߵ��ı�����  -------------------


// �·����ı�����  *******************

static TEXTSTRUCT BottomText[BOTTOMTEXT_MAXNUMS] = { 0 };           /* �ں����ʼ�� */

char BottomTextInfo[][15] =		//��3��15�У�
{
    "Vpp: 1.65V",
    "F: 10Hz",
    "Min: 100mv",
};
// �·����ı�����  --------------------

// �Ϸ��ı�����  **********************

static TEXTSTRUCT UpText[UPTEXT_MAXNUMS] = { 0 };                   /* �ں����ʼ�� */
char UpTextInfo[][12] =	//��1��12�У�
{
    "200kSa/s",
};
// �Ϸ��ı�����  ----------------------

// �Ϸ�����Ԥ�������� *******************
short GraphPreBuffer[GRAPHPRE_XSIZE - 2] = { 0 }; //[127 -2] = 125

static GRAPHPREWIN_STRUCT GraphPreWin =                  /*����Ԥ����ʼ��*/
{
    -1,	//Handle = -1
    20,	//TriggerPos
    50,	//StartPos
    60,	//CoverLength
    {
        .x0 = 70,				//��ʼ��ַx0 = 70
        .y0 = 2,				//��ʼ��ַy0 = 2
        .xsize = GRAPHPRE_XSIZE,	//xsize = 127
        .ysize = GRAPHPRE_YSIZE,	//ysize = 16
    }
};
// �Ϸ�����Ԥ�������� --------------------

// DAC�ı�����  **********************
static char DACTextInfo [10] = {"1000Hz"};  
static TEXTSTRUCT DACText = 
{
    0, NULL, DACTextInfo, 
    {
        .x0 = 260,
        .y0 = 220,
        .xsize = 60,
        .ysize = 20,
    },
    0,
    0,
    0 
};        
// DAC�ı�����  ----------------------

// ��ʱ��handle
static WM_HTIMER hTimer = -1;


/*******************************************************************
*
*       static code
*
********************************************************************
*/


/*********************************************************************
*
*       _cbDACWin
*/
static void _cbDACWin(WM_MESSAGE* pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	GUI_RECT RECT;
	WM_GetWindowRectEx(hWin, &RECT);	//���ش��ڵ���Ļ����
	
	switch(pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetColor(GUI_RED);
			GUI_FillRoundedRect(2, 2, 56, 18, 2);
		
			GUI_SetFont(GUI_FONT_13B_ASCII);
			GUI_SetBkColor(GUI_RED);
			GUI_SetColor(GUI_WHITE);
			sprintf(DACText.sinfo, "%dHz", _DACgrade[DACParams.DACFreqGrade]);//sprintf();������д��DACText.sinfo
			GUI_DispStringHCenterAt(DACText.sinfo, 30, 3);//��ָ��λ����ʾˮƽ���е��ַ���
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

/*********************************************************************
*
*       _cbGraphPreWin
*/
static void _cbGraphPreWin(WM_MESSAGE* pMsg)
{
		WM_HWIN hWin = pMsg->hWin;
    GUI_RECT RECT;
    WM_GetWindowRectEx(hWin, &RECT);	//���ش��ڵ���Ļ���꣬����洢��RECT��
	
		switch(pMsg->MsgId)
		{
			case WM_PAINT:
				GraphPreWin.TriggerPos = (I32)DSOShowParams.TriggerPos * 127 / WAVE_BUFF_SIZE;//Ԥ����������λ��
				GraphPreWin.CoverLength = 250 * 127 / WAVE_BUFF_SIZE;//Ԥ�����ĳ���
				GraphPreWin.StartPos = (I32)DSOShowParams.ShowStartPos * 127 / WAVE_BUFF_SIZE;//Ԥ������ʼ��ʾ��λ��
			
				GUI_SetBkColor(0x040005);
				GUI_Clear();
			
				GUI_SetColor(GUI_WHITE);
				GUI_DrawRoundedRect(0, 0, GRAPHPRE_XSIZE - 1, GRAPHPRE_YSIZE - 1, 2);//(0, 0, 127-1, 16-1, 2)
			
				GUI_SetColor(GUI_BLUE);
				GUI_FillRect(GraphPreWin.StartPos, 1, GraphPreWin.StartPos + GraphPreWin.CoverLength, GraphPreWin.size.ysize -2);
			
				//��ʱ�����׾����ô�
				GUI_SetPenSize(2);
				GUI_SetColor(GUI_YELLOW);
				GUI_DrawLine(GraphPreWin.TriggerPos - 2, 1, GraphPreWin.TriggerPos + 2, 1);
				GUI_DrawLine(GraphPreWin.TriggerPos, 1, GraphPreWin.TriggerPos, GraphPreWin.size.ysize - 2);
				GUI_DrawLine(GraphPreWin.TriggerPos - 2, GraphPreWin.size.ysize - 3, GraphPreWin.TriggerPos + 2, GraphPreWin.size.ysize - 3);
				break;
			default:
				WM_DefaultProc(pMsg);
		}
}

/*********************************************************************
*
*       _cbUpText
*/
static void _cbUpText(WM_MESSAGE* pMsg)
{
	int i, up_textx = 0;	//iû�г�ʼ��,up_textx=0
	WM_HWIN hWin = pMsg->hWin;
	GUI_RECT RECT;
	WM_GetWindowRectEx(hWin, &RECT);	//���ش��ڵ���Ļ���꣬�洢��RECT��
	
	switch(pMsg->MsgId)
	{
		case WM_PAINT:
			for(i = 0; i < UPTEXT_MAXNUMS; i++)
			{
				if(hWin == UpText[i].Handle)
				{
					up_textx = i;//up_textx = 0֮����˳�forѭ��
				}
			}
			
			switch(DSOParams.TimeBaseGrade)
			{
				case 0://Sa/s  Sa: sample ����
					sprintf(UpTextInfo[sps], "%dMSa/s", 1);//sprintf();������д��UpTextInfo[sps]
					break;
				case SPSMAXGRADE:
					sprintf(UpTextInfo[sps], "%dSa/s", _tgrade[SPSMAXGRADE].SPS);
					break;
				default:
					sprintf(UpTextInfo[sps], "%dkSa/s", _tgrade[DSOParams.TimeBaseGrade].SPS / 1000);
					break;
			}
			
			GUI_SetFont(&GUI_Font13B_ASCII);
			switch(up_textx)//up_textx = 0
			{
				case sps://sps = 0
					GUI_SetColor(GUI_YELLOW);
					GUI_FillRoundedRect(0, 0, UPTEXT_R_XSIZE, UPTEXT_YSIZE, 2);//(0, 0, 60, 16, 2)
				
					GUI_SetBkColor(GUI_YELLOW);
					GUI_SetColor(GUI_BLACK);
					GUI_DispStringHCenterAt(UpText[up_textx].sinfo, UPTEXT_R_XSIZE/2 + 1, 0);//UpText[up_textx].sinfo = UpTextInfo[up_textx];
					break;
				
				default:
					break;
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}


/*********************************************************************
*
*       _cbBottomText
*/
static void _cbBottomText(WM_MESSAGE* pMsg)
{
	int i, bottom_textx = 0;
	WM_HWIN hWin = pMsg->hWin;
	GUI_RECT RECT;
	WM_GetWindowRectEx(hWin, &RECT);//���ش��ڵ���Ļ���꣬�洢��RECT��
	
	switch(pMsg->MsgId)
	{
		case WM_PAINT:
			for(i = 0; i < BOTTOMTEXT_MAXNUMS; i++)
			{
				if(hWin == BottomText[i].Handle)//�ж�Ҫ���ĵ�����������TEXT�ı��ؼ��е���һ��
				{
					bottom_textx = i;
				}
			}
			
			if(WaveParams.PPValue < 1241)
			{
				sprintf(BottomTextInfo[vpp], "Vpp: %dmV", (I16)((float)WaveParams.PPValue / 1.241));
			}
			else
			{
				sprintf(BottomTextInfo[vpp], "Vpp: %.2fV", (float)WaveParams.PPValue / 1.241 / 1000);
			}
			
			if(WaveParams.MinValue < 1241)
			{
				sprintf(BottomTextInfo[minvalue], "Min: %dmV", (I16)((float)WaveParams.MinValue / 1.241));
			}
			else
			{
				sprintf(BottomTextInfo[minvalue], "Min: %.2fV", (float)WaveParams.MinValue / 1.241 / 1000);
			}
			
			if(WaveParams.Freq > 1000)
			{
				sprintf(BottomTextInfo[freq], "F: %.2lfkHz", WaveParams.Freq / 1000);
			}
			else
			{
				sprintf(BottomTextInfo[freq], "F: %.1lfHz", WaveParams.Freq);
			}
			
			GUI_SetFont(&GUI_Font13B_ASCII);
			switch(bottom_textx)//bottom_textx���ܵ�ֵ��0, 1, 2
			{
				case vpp:
				case minvalue:
					GUI_SetColor(GUI_YELLOW);
					GUI_FillRoundedRect(0, 0, BOTTOMTEXT_XSIZE, BOTTOMTEXT_YSIZE, 2);
				
					GUI_SetBkColor(GUI_YELLOW);
					GUI_SetColor(GUI_BLACK);
					GUI_DispStringAt(BottomText[bottom_textx].sinfo, 5, 0);
					break;
				case freq:
					GUI_SetColor(GUI_GREEN);
					GUI_FillRoundedRect(0, 0, BOTTOMTEXT_XSIZE, BOTTOMTEXT_YSIZE, 2);
				
					GUI_SetBkColor(GUI_GREEN);
					GUI_SetColor(GUI_BLACK);
					GUI_DispStringAt(BottomText[bottom_textx].sinfo, 5, 0);
					break;
				default:
					break;
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}
/*********************************************************************
*
*       _cbRightText
*/
static void _cbRightText(WM_MESSAGE* pMsg)
{
	char stitle[10];
	int i, right_textx = 0;
	I16 thistimebase;
	WM_HWIN hwin = pMsg->hWin;
	
	int TimerId;
	switch(pMsg->MsgId){
		case WM_PAINT:
			for(i = 0; i < 8; i++)
			{
				if(hwin == RightText[i].Handle)//�ж�ѡ�е�����һ���ı��ؼ�
				{
					right_textx = i;//right_textx = 0, 1, 2, 3, 4, 5, 6, 7
					break;
				}
			}
			TEXT_GetText(hwin, stitle, 10);		//���ص�ǰ�ı����ݣ����ı��ؼ����ݸ��Ƶ�stitle
			
			thistimebase = _tgrade[DSOParams.TimeBaseGrade].TIMEBASE;//��ȡ��ǰʱ����ֵ
			
			switch(right_textx){	//����ʾ�ַ������и�ֵ
				case tbase:	//tbase = 1
					if(DSOParams.TimeBaseGrade < 5)
					{
						sprintf(RightTextInfo[tbase], "%.1fus", (float)thistimebase * 5 / DSOShowParams.XExpan);
						sprintf(RightTextInfo[xpos], "%dus", DSOParams.XPos * 5);
					}
					else
					{
						sprintf(RightTextInfo[tbase], "%.1fms", (float)thistimebase * 5 / 1000 / DSOShowParams.XExpan);
						sprintf(RightTextInfo[xpos], "%.2fms", (float)DSOParams.XPos * 5 / 1000);
					}
					break;
					
				case vbase: //vbase = 2
					sprintf(RightTextInfo[vbase], "%dmV", _vgrade[DSOParams.VoltageBaseGrade]);
					break;
				
				case ypos: //ypos = 4
					sprintf(RightTextInfo[ypos], "%d", DSOParams.YPos);
					break;
				
				case trlevel: //trlevel = 5
					if(DSOParams.TriggerLevel > 1241)
						sprintf(RightTextInfo[trlevel], "%.2fV", ((float)DSOParams.TriggerLevel / 1.2412 / 1000));
					else
						sprintf(RightTextInfo[trlevel], "%dmV", (I16)(DSOParams.TriggerLevel / 1.2412));
					break;
					
				case mode: //mode = 6
					sprintf(RightTextInfo[mode], "%d->1", DSOShowParams.XExpan);
					break;
				
				default:
					break;
			}
			//ˢ��
			switch(right_textx){
				case channel://ʹ�ö��ر�ǩ
				case vbase:
				case ypos:
				case mode:
					GUI_SetColor(GUI_YELLOW);
					GUI_FillPolygon(_TextBkBig, GUI_COUNTOF(_TextBkBig), 0, 0);//���ƾ�����ɫ���Ķ����
				
					GUI_SetFont(&GUI_Font8_ASCII);
					GUI_SetBkColor(GUI_YELLOW);
					GUI_SetColor(GUI_BLACK);
					GUI_DispStringHCenterAt(RightText[right_textx].stitle, RightText[right_textx].Text.xsize/2, 2);
					break;
				
				case tbase:
				case xpos:
				case trlevel:
				case trmode:
					GUI_SetColor(GUI_GREEN);
					GUI_FillPolygon(_TextBkBig, GUI_COUNTOF(_TextBkBig), 0, 0);
				
					GUI_SetFont(&GUI_Font8_ASCII);
					GUI_SetBkColor(GUI_GREEN);
					GUI_SetColor(GUI_BLACK);
					GUI_DispStringHCenterAt(stitle, RightText[right_textx].Text.xsize/2, 2);
					break;
				default:
					break;
			}
			
			if(RightText[right_textx].TimerFlag == 2)//�жϱ�����ɫΪ��ɫ���Ǻ�ɫ
			{
				GUI_SetColor(GUI_WHITE);
				GUI_FillPolygon(_TextBkSmall, GUI_COUNTOF(_TextBkSmall), 0, 0);
				GUI_SetBkColor(GUI_WHITE);
				GUI_SetColor(GUI_BLACK);
			}
			else
			{
				GUI_SetColor(GUI_BLACK);
				GUI_FillPolygon(_TextBkSmall, GUI_COUNTOF(_TextBkSmall), 0, 0);
				GUI_SetBkColor(GUI_BLACK);
				GUI_SetColor(GUI_WHITE);
			}
			GUI_DispStringHCenterAt(RightText[right_textx].sinfo, RightText[right_textx].Text.xsize/2, 2+RightText[right_textx].Text.ysize/2);
			break;
			
		case WM_TIMER:
			TimerId = WM_GetTimerId(hTimer);
			if(RightText[TimerId].TimerFlag)
			{
				RightText[TimerId].TimerFlag = (RightText[TimerId].TimerFlag == 2) ? 1 : 2;
			}
			WM_InvalidateWindow(RightText[TimerId].Handle);//ʹ������Ч
			WM_RestartTimer(pMsg->Data.v, 1000);
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

//���Ʋ���������
static void Draw_GraphBk(void)
{
    // 250 * 200

    int x = GraphRect.x0;
    int y = GraphRect.y0;
    int xsize = GraphRect.x1 - GraphRect.x0;
    int ysize = GraphRect.y1 - GraphRect.y0;

    //һ����ȡ�һ���߶ȡ�һ���С����
    GRID Grid = { xsize / 10, ysize / 8 , 5 }; //25,25,5

    int i, j, k;

    //GUI_SetBkColor(GUI_GRAY);
    GUI_SetBkColor(GUI_BLACK);
    GUI_ClearRect(x, y, x + xsize, y + ysize);

    GUI_SetColor(0x111111);

    //ˮƽ�ĵ�
    for (k = 1; k < 8; k++)
    {
        for (j = 0; j < 10; j++)
        {
            for (i = 0; i < 5; i++)
            {
                GUI_DrawPoint(x + j * Grid.width + i * Grid.pointspace, y + k * Grid.height);
            }
        }
    }

    //��ֱ�ĵ�
    for (k = 0; k < 8; k++)
    {
        for (j = 1; j < 10; j++)
        {
            for (i = 0; i < 5; i++)
            {
                GUI_DrawPoint(x + j * Grid.width, y + k * Grid.height + i * Grid.pointspace);
            }
        }
    }

    GUI_SetColor(0x333333);
    GUI_DrawRectEx(&GraphRect);

    GUI_SetColor(0x666666);
    GUI_DrawLine(x, y + ysize / 2, x + xsize, y + ysize / 2);
    GUI_DrawLine(x + xsize / 2, y, x + xsize / 2, y + ysize);

    for (j = 0, k = 4; j < 10; j++)
    {
        for (i = (!j) ? 1 : 0; i < 1; i++)
        {
            GUI_DrawLine(
                x + j * Grid.width + i * Grid.pointspace,
                y + k * Grid.height - 2,
                x + j * Grid.width + i * Grid.pointspace,
                y + k * Grid.height + 2
            );
        }

    }

    for (j = 5, k = 0; k < 8; k++)
    {
        for (i = (!k) ? 1 : 0; i < 1; i++)
        {
            GUI_DrawLine(
                x + j * Grid.width - 2,
                y + k * Grid.height + i * Grid.pointspace,
                x + j * Grid.width + 2,
                y + k * Grid.height + i * Grid.pointspace
            );
        }
    }


}

static void Draw_Graph(void)
{
    GUI_SetColor(GUI_YELLOW);
    GUI_DrawGraph((signed short*)GraphShowBuffer, GUI_COUNTOF(GraphShowBuffer), GraphRect.x0, GraphRect.y1);
}

/*********************************************************************
*
*       _cbBkWindow
*/
static void _cbBkWindow(WM_MESSAGE* pMsg){
	int NCode, Id;
	WM_HWIN hButton;
	
	switch(pMsg->MsgId){
		case WM_PAINT:					//���ڱ�Ϊ��Ч��Ӧ�ػ�ʱ�����͵�����
			GUI_SetBkColor(GUI_GRAY);
			GUI_Clear();
			Draw_GraphBk();
			Draw_Graph();
		
			GUI_SetBkColor(GUI_BLACK);
			GUI_SetColor(GUI_WHITE);
			break;
		
		case WM_KEY:						//����ĳ�������͵���ǰ��������Ĵ���
			switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key){
				case GUI_KEY_ESCAPE:
					break;
				case GUI_KEY_ENTER:
					break;
			}
			break;
		
		case WM_NOTIFY_PARENT:	//��֪�����ڣ����Ӵ����з�����ĳЩ�ı�
			Id = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(NCode){
				case WM_NOTIFICATION_RELEASED:	//��������Ŀؼ����ͷ�ʱ�������ʹ�֪ͨ��Ϣ
					switch(Id){
						case ID_BUTTON_MODE_SWITCH:
							hButton = pMsg->hWinSrc;
							if(DSOShowParams.ShowMode == SHOW_FFT)
							{
								BUTTON_SetText(hButton, "->FFT");
								DSOShowParams.ShowMode = SHOW_WAVE;
							}
							else
							{
								BUTTON_SetText(hButton, "->WAVE");
								DSOShowParams.ShowMode = SHOW_FFT;
							}
							break;
							
						case ID_BUTTON_STOP_WAVE:
							hButton = pMsg->hWinSrc;
							if(DSOParams.StopFlag == DSO_RUN)
							{
								BUTTON_SetText(hButton, "->T'D");
								DSOParams.StopFlag = DSO_STOP;
							}
							else
							{
								BUTTON_SetText(hButton, "->STOP");
								DSOParams.StopFlag = DSO_RUN;
							}
							break;
							
						default:
							break;
					}
				break;
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void CreateAllWigets(void)
{
	WM_HWIN hWin;
	int i;
	
	for(i = 0; i < RIGHTTEXT_MAXNUMS; i++)
	{
		hWin = TEXT_CreateEx(RightText[i].Text.x0, RightText[i].Text.y0, RIGHTTEXT_XSIZE, RIGHTTEXT_YSIZE, WM_HBKWIN, WM_CF_SHOW, TEXT_CF_LEFT, 0, _aright_text_title[i]);
		WM_SetCallback(hWin, _cbRightText);
		RightText[i].Handle = hWin;
	}
	
	for(i = 0; i < BOTTOMTEXT_MAXNUMS; i++)
	{
		hWin = TEXT_CreateEx(BottomText[i].Text.x0, BottomText[i].Text.y0, BOTTOMTEXT_XSIZE, BOTTOMTEXT_YSIZE, WM_HBKWIN, WM_CF_SHOW, TEXT_CF_LEFT, 0, NULL);
		WM_SetCallback(hWin, _cbBottomText);
		BottomText[i].Handle = hWin;
	}
	
	for(i = 0; i < UPTEXT_MAXNUMS; i++)
	{
		hWin = TEXT_CreateEx(UpText[i].Text.x0, UpText[i].Text.y0, UpText[i].Text.xsize, UPTEXT_YSIZE, WM_HBKWIN, WM_CF_SHOW, TEXT_CF_LEFT, 0, NULL);
		WM_SetCallback(hWin, _cbUpText);
		UpText[i].Handle = hWin;
	}
	
	// ���ı���ؼ����洰�ڿؼ�����Ϊ�ò������ڿؼ��Ĺ���
	hWin = TEXT_CreateEx(GraphPreWin.size.x0, GraphPreWin.size.y0, GraphPreWin.size.xsize, GraphPreWin.size.ysize, WM_HBKWIN, WM_CF_SHOW, TEXT_CF_LEFT, 0, NULL);
	WM_SetCallback(hWin, _cbGraphPreWin);
	GraphPreWin.Handle = hWin;
	
	hWin = TEXT_CreateEx(260, 220, 60, 20, WM_HBKWIN, WM_CF_SHOW, TEXT_CF_LEFT, 0, NULL);
	WM_SetCallback(hWin, _cbDACWin);
	DACText.Handle = hWin;
	
	hWin = BUTTON_CreateEx(262, 2, 56, 18, WM_HBKWIN, WM_CF_SHOW, TEXT_CF_LEFT, ID_BUTTON_MODE_SWITCH);
	BUTTON_SetText(hWin, "->FFT");
	
	hWin = BUTTON_CreateEx(10, 2, 58, 16, WM_HBKWIN, WM_CF_SHOW, TEXT_CF_LEFT, ID_BUTTON_STOP_WAVE);
	BUTTON_SetText(hWin, "->STOP");
}

static void MY_Init(void)
{
	int i;
	
	//�ұ߰˸���ʾ�ĳ�ʼ��
	for(i = 0; i < RIGHTTEXT_MAXNUMS; i++)
	{
		RightText[i].CornerSize = RIGHTTEXT_CSIZE;
		RightText[i].Handle = -1;
		RightText[i].sinfo = RightTextInfo[i];//�ı���Ҫ��ʾ�����ݣ����
		RightText[i].stitle = _aright_text_title[i];//�ı���ı��⣬�̶�����
		RightText[i].Space = RIGHTTEXT_SPACE;
		RightText[i].Text.x0 = 265;
		RightText[i].Text.y0 = 21 + (RIGHTTEXT_YSIZE + 2)*i;
		RightText[i].Text.xsize = RIGHTTEXT_XSIZE;
		RightText[i].Text.ysize = RIGHTTEXT_YSIZE;
		RightText[i].TimerFlag = 0;
	}
	
	//����������ʾ�ĳ�ʼ��
	for(i = 0; i < BOTTOMTEXT_MAXNUMS; i++)
	{
		BottomText[i].CornerSize = 0;
		BottomText[i].Handle = -1;
		BottomText[i].sinfo = BottomTextInfo[i];
		BottomText[i].stitle = NULL;
		BottomText[i].Space = BOTTOMTEXT_SPACE;
		BottomText[i].Text.x0 = 10 + (BOTTOMTEXT_XSIZE + 2)*i;
		BottomText[i].Text.y0 = 222;
		BottomText[i].Text.xsize = BOTTOMTEXT_XSIZE;
		BottomText[i].Text.ysize = BOTTOMTEXT_YSIZE;
	}
	
	//�����ı�(Sa/s)��ʼ��
	for(i = 0; i < UPTEXT_MAXNUMS; i++)
	{
		UpText[i].Text.x0 = 259 - UPTEXT_R_XSIZE;
		UpText[i].Text.xsize = UPTEXT_R_XSIZE;
		
		UpText[i].CornerSize = 0;
		UpText[i].Handle = -1;
		UpText[i].sinfo = UpTextInfo[i];
		UpText[i].stitle = NULL;
		UpText[i].Space = UPTEXT_SPACE;
		UpText[i].Text.y0 = 2;
		UpText[i].Text.ysize = UPTEXT_YSIZE;
	}
}


/*  ����:  �����ݴ�WaveBuff������GraphShowBuffer
*   ������ ��
*   ����ֵ�� ��
*/
static void CopyToShowBuffer(void)
{
	int i, j, k;
	int sp = DSOShowParams.XExpan;
	
	if(DSOShowParams.ShowMode == SHOW_WAVE)
	{
		for(i = 0, j = DSOShowParams.ShowStartPos; i < SHOW_BUFF_SIZE; i += sp, j++)
		{
			for(k = 0; (k < sp) && ((i + k) < SHOW_BUFF_SIZE); k++)
			{
				GraphShowBuffer[i + k] = -((WaveBuffer[j] + DSOParams.YPos) / (NUMS_PER_mV/25*_vgrade[DSOParams.VoltageBaseGrade]));//��ǰADC��ֵ��Ӧ���ٸ�����,4096��3300mv��25���أ�500mv
			}
		}
	}
	else
	{
		for (i = 0, j = 0; i < SHOW_BUFF_SIZE; i += sp, j++)
		{
			for(k = 0; (k < sp) && ((i + k) < SHOW_BUFF_SIZE); k++)
			{
       GraphShowBuffer[i + k] = - ((MagArray[j]) / 25); 
			}
		}
	}
}

/**
  * @brief GUI������
  * @note ��
  * @param ��
  * @retval ��
  */
void MainTask(void)
{
	MY_Init();				//��ʼ���ṹ��
	
	WM_SetCreateFlags(WM_CF_MEMDEV);//����ʹ���ڴ��豸��־����bkwindowҲʹ��
  GUI_Init();//��ʼ��STemWin
	WM_SetCallback(WM_HBKWIN, &_cbBkWindow);
	
	CreateAllWigets();	//�������пؼ�
	
	SetDACFreq(_DACgrade[DAC_DEFAULT_GRADE]);
	DACParams.DACFreqGrade = DAC_DEFAULT_GRADE;
	WM_InvalidateWindow(DACText.Handle);
	
	SetADCSampleRate(_tgrade[SPS_DEFAULT_GRADE].SPS);
	DSOParams.TimeBaseGrade = SPS_DEFAULT_GRADE;
	WM_InvalidateWindow(UpText[sps].Handle);
	WM_InvalidateWindow(RightText[tbase].Handle);
	
	hTimer = WM_CreateTimer(RightText[channel].Handle, channel, 1000, 0);	//������ʱ��
	RightText[channel].TimerFlag = 2;
  
  while(1)
  {
		if(DSOParams.StopFlag == DSO_RUN)
		{
			CopyDataToWaveBuff();//�������ݵ�WaveBuffer[]
			FFT_GetFreq(_tgrade[DSOParams.TimeBaseGrade].SPS);    
		}
		
		CalShowStartPos();//�����WaveBuffer[]���ĸ�λ�ÿ�ʼ����250����
		CopyToShowBuffer();//�����ݴ�WaveBuffer[]������GraphShowBuffer[]
		
		WM_InvalidateRect(WM_HBKWIN, &GraphRect);	
		WM_InvalidateWindow(GraphPreWin.Handle);
		WM_InvalidateWindow(BottomText[vpp].Handle);
		
    GUI_Delay(50);
  }
}

I16 GetTextHandle(I8 Position, I8 Index)
{
    switch (Position)
    {
    case R:
        return RightText[Index].Handle;

    case B:
        return BottomText[Index].Handle;
    
    case U:
        return UpText[Index].Handle;
    
    default:
        break;
    }
    return -1;
}

/* ���ĵ�ǰѡ�е��Ҳ��ı��� */ 
void PickActiveWin(I8 Index, I8 LastIndex)
{
    /* �ر���һ�������� */

    WM_DeleteTimer(hTimer);
    RightText[LastIndex].TimerFlag = 0;
    WM_InvalidateWindow(RightText[LastIndex].Handle);

    /* �����µļ����� */
    hTimer = WM_CreateTimer(RightText[Index].Handle, Index, 1000, 0);
    RightText[Index].TimerFlag = 2;
    WM_InvalidateWindow(RightText[Index].Handle);
}

/* �������º�Ļص����� 
*  ���� 1Ϊ���ӡ����ơ�����
*/
void _cbKey(I8 Index, I8 Direction)
{
    I16 lasttimebase, thistimebase;
    switch (Index)
    {
    case tbase:
        if(Direction)
        {
            if (DSOParams.TimeBaseGrade < SPSMAXGRADE)
            {
                lasttimebase = _tgrade[DSOParams.TimeBaseGrade].TIMEBASE;
                DSOParams.TimeBaseGrade++;
                thistimebase = _tgrade[DSOParams.TimeBaseGrade].TIMEBASE;
                
                SetADCSampleRate(_tgrade[DSOParams.TimeBaseGrade].SPS);
                
                DSOShowParams.XBufPos = (DSOShowParams.XBufPos * ((float)lasttimebase / (float)thistimebase));
            }
        }
        else
        {
            if (DSOParams.TimeBaseGrade > 0)
            {
                lasttimebase = _tgrade[DSOParams.TimeBaseGrade].TIMEBASE;
                DSOParams.TimeBaseGrade--;
                thistimebase = _tgrade[DSOParams.TimeBaseGrade].TIMEBASE;

                SetADCSampleRate(_tgrade[DSOParams.TimeBaseGrade].SPS);
                DSOShowParams.XBufPos = (DSOShowParams.XBufPos * (lasttimebase / thistimebase));                            
            }
        }    
        WM_InvalidateWindow(UpText[sps].Handle);
        WM_InvalidateWindow(RightText[tbase].Handle);
        WM_InvalidateWindow(RightText[xpos].Handle);    
        break;
    
    case xpos:
        if(Direction)
        {
            thistimebase = _tgrade[DSOParams.TimeBaseGrade].TIMEBASE;
            DSOParams.XPos += (thistimebase / POINTS_ONE_MOVE);
            DSOShowParams.XBufPos += POINTS_ONE_MOVE;           
        }
        else
        {
            thistimebase = _tgrade[DSOParams.TimeBaseGrade].TIMEBASE;
            DSOParams.XPos -= (thistimebase / POINTS_ONE_MOVE);
            DSOShowParams.XBufPos -= POINTS_ONE_MOVE;
        }
        WM_InvalidateWindow(RightText[xpos].Handle);
        break;
				
    case trlevel:
        if(Direction)
        {
            DSOParams.TriggerLevel += 10;
        }
        else
        {
            DSOParams.TriggerLevel -= 10;
        }
        WM_InvalidateWindow(RightText[trlevel].Handle);
        break;
				
    case ypos:
        if(Direction)
        {
            DSOParams.YPos += 100;
        }
        else
        {
            DSOParams.YPos -= 100;
        }
        WM_InvalidateWindow(RightText[ypos].Handle);
        break;
    
    case vbase:
        if(Direction)
        {
            if (++DSOParams.VoltageBaseGrade > VOLTAGEMAXGRADE)
                DSOParams.VoltageBaseGrade--;
        }
        else
        {
            if (--DSOParams.VoltageBaseGrade < 0)
                DSOParams.VoltageBaseGrade++;
        }
        WM_InvalidateWindow(RightText[vbase].Handle);
        break;

    case mode:
        if(Direction)
        {
            if (++DSOShowParams.XExpan > 10) // >10������̻ص�10���������������˼
                DSOShowParams.XExpan--;
        }
        else
        {
            if (--DSOShowParams.XExpan < 1)	// <1������̻ص�1��
                DSOShowParams.XExpan++;
        }
        WM_InvalidateWindow(RightText[mode].Handle);
        WM_InvalidateWindow(RightText[tbase].Handle);
        break;

    case dac:
        if(Direction)
        {
            if(DACParams.DACFreqGrade++ == DACMAXGRADE)
                DACParams.DACFreqGrade--;
            SetDACFreq(_DACgrade[DACParams.DACFreqGrade]);
        }
        else
        {
            if(DACParams.DACFreqGrade-- == 0)
                DACParams.DACFreqGrade++;
            SetDACFreq(_DACgrade[DACParams.DACFreqGrade]);
        }
        WM_InvalidateWindow(DACText.Handle);      
        break;
				
    default:
        break;
    }
}
