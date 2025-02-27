#ifndef __MAINTASK_H
#define __MAINTASK_H

#include "DSO.h"

typedef struct {                //  文本框通用结构体
    int Handle;
    const char* stitle;			//固定标题
    char* sinfo;	//要显示的内容
    WIGET Text;			//位置，尺寸
    int CornerSize;			//倒角尺寸
    int Space;		//空格
    I8 TimerFlag;
}TEXTSTRUCT;

typedef struct {                // 波形预览小窗结构体
    int Handle;
    int TriggerPos;
    int StartPos;
    int CoverLength;
    WIGET size;
}GRAPHPREWIN_STRUCT;

typedef struct {                //  示波器显示文本结构体
    char TimeBase[10];
    char VoltageBase[10];
    char XPos[10];
    char YPos[10];
    char TriggerLevel[10];
}DSOALLSTRING;

//  右边的8个文本框的显示用全部东西
#define     RIGHTTEXT_XSIZE             50
#define     RIGHTTEXT_YSIZE             23
#define     RIGHTTEXT_HALF_YSIZE        (int)(RIGHTTEXT_YSIZE / 2) //为11
#define     RIGHTTEXT_CSIZE             4
#define     RIGHTTEXT_SPACE             1
#define     RIGHTTEXT_MAXNUMS           8


// 正下方3个文本框显示用
#define     BOTTOMTEXT_XSIZE             82
#define     BOTTOMTEXT_YSIZE             16
#define     BOTTOMTEXT_SPACE             2
#define     BOTTOMTEXT_MAXNUMS           3


// 上方文本框
#define     UPTEXT_L_XSIZE             28
#define     UPTEXT_R_XSIZE             60
#define     UPTEXT_YSIZE               16
#define     UPTEXT_SPACE               2
#define     UPTEXT_MAXNUMS             1


// 上方波形预览条
#define     GRAPHPRE_XSIZE               127
#define     GRAPHPRE_YSIZE               16
#define     GRAPHPRE_MAXNUMS             1


enum { channel, tbase, vbase, xpos, ypos, trlevel, mode, trmode, dac };

enum { vpp, freq, minvalue, bottomTBD};//vpp = 0; freq = 1; minvalue = 2; bottomTBD = 3

enum { sps };

enum { R, B, U};	//R:right; B:bottom; U:up  判断句柄

void MainTask(void);

I16 GetTextHandle(I8 Position, I8 Index);
void PickActiveWin(I8 Index, I8 LastIndex);
void _cbKey(I8 Index, I8 Direction);

#endif /* __MAINTASK_H */
