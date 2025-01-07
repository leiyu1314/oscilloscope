#ifndef __MAINTASK_H
#define __MAINTASK_H

#include "DSO.h"

typedef struct {                //  �ı���ͨ�ýṹ��
    int Handle;
    const char* stitle;			//�̶�����
    char* sinfo;	//Ҫ��ʾ������
    WIGET Text;			//λ�ã��ߴ�
    int CornerSize;			//���ǳߴ�
    int Space;		//�ո�
    I8 TimerFlag;
}TEXTSTRUCT;

typedef struct {                // ����Ԥ��С���ṹ��
    int Handle;
    int TriggerPos;
    int StartPos;
    int CoverLength;
    WIGET size;
}GRAPHPREWIN_STRUCT;

typedef struct {                //  ʾ������ʾ�ı��ṹ��
    char TimeBase[10];
    char VoltageBase[10];
    char XPos[10];
    char YPos[10];
    char TriggerLevel[10];
}DSOALLSTRING;

//  �ұߵ�8���ı������ʾ��ȫ������
#define     RIGHTTEXT_XSIZE             50
#define     RIGHTTEXT_YSIZE             23
#define     RIGHTTEXT_HALF_YSIZE        (int)(RIGHTTEXT_YSIZE / 2) //Ϊ11
#define     RIGHTTEXT_CSIZE             4
#define     RIGHTTEXT_SPACE             1
#define     RIGHTTEXT_MAXNUMS           8


// ���·�3���ı�����ʾ��
#define     BOTTOMTEXT_XSIZE             82
#define     BOTTOMTEXT_YSIZE             16
#define     BOTTOMTEXT_SPACE             2
#define     BOTTOMTEXT_MAXNUMS           3


// �Ϸ��ı���
#define     UPTEXT_L_XSIZE             28
#define     UPTEXT_R_XSIZE             60
#define     UPTEXT_YSIZE               16
#define     UPTEXT_SPACE               2
#define     UPTEXT_MAXNUMS             1


// �Ϸ�����Ԥ����
#define     GRAPHPRE_XSIZE               127
#define     GRAPHPRE_YSIZE               16
#define     GRAPHPRE_MAXNUMS             1


enum { channel, tbase, vbase, xpos, ypos, trlevel, mode, trmode, dac };

enum { vpp, freq, minvalue, bottomTBD};//vpp = 0; freq = 1; minvalue = 2; bottomTBD = 3

enum { sps };

enum { R, B, U};	//R:right; B:bottom; U:up  �жϾ��

void MainTask(void);

I16 GetTextHandle(I8 Position, I8 Index);
void PickActiveWin(I8 Index, I8 LastIndex);
void _cbKey(I8 Index, I8 Direction);

#endif /* __MAINTASK_H */
