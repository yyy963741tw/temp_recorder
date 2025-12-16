//#include "cpu.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
//#include <p24fj128ga006.h>
#ifndef Display_H
#define Display_H
/************************************************************
					Display-LCD Define
************************************************************/
#define LCM_CS				LATGbits.LATG14
#define LCM_WR				LATGbits.LATG12//SED_W	
#define LCM_RS				LATGbits.LATG13//A0
#define LCM_E				LATCbits.LATC15//SED_E
#define LCM_DBOut			LATE			//B8~B15
#define LCM_DBIn			PORTE
#define	DBForInput			PORTSetPinsDigitalIn(IOPORT_E, BIT_0|BIT_1|BIT_2|BIT_3|BIT_4|BIT_5|BIT_6|BIT_7)
#define DBForOutput			PORTSetPinsDigitalOut(IOPORT_E, BIT_0|BIT_1|BIT_2|BIT_3|BIT_4|BIT_5|BIT_6|BIT_7)
/************************************************************
				function define
************************************************************/
extern void SetDisplayPage(char p);
extern void ScrollDspPush(char *string,...);
extern void LCMInit(void);
extern int 	Lprintf(char line,char *string,...);
extern void LCMSendData(unsigned char DAT);
extern void LCMSendCommand(unsigned char CMD);
extern void LCMBusyChk();
extern unsigned char LCMDBInput();
extern void LCMDBOutput(unsigned char buf);
/************************************************************
				varraiable define
************************************************************/
#define BasicInstruction()     LCMSendCommand(0x30)
#define ExtendedInstruction()  LCMSendCommand(0x34)
//BasicInstruction 
#define DisplayClr()		   LCMSendCommand(0x01)
#define ReturnHome()		   LCMSendCommand(0x02)
#define DisplayOn()			   LCMSendCommand(0xC0)
#define DisplayOff()		   LCMSendCommand(0x80)
#define SetCGAddr(adr)		   LCMSendCommand(adr&0x7F)
#define SetDDAddr(adr)         LCMSendCommand(adr&0xFF)
//ExtendedInstruction
#define GraphicModeOn() 	    LCMSendCommand(0x36)
#define GraphicModeOff()	    LCMSendCommand(0x34)
#define SetVScrollModeOn()		LCMSendCommand(0x03)
#define SetVscrollModeOff()		SetCGAddModeOn()
#define SetCGAddModeOn()		LCMSendCommand(0x02)
#define SetCGAddModeOff()		SetVScrollModeOn()
#define SetScrollAddr(x)		LCMSendCommand(x&0x7F)
#define SetGraphicAddr(v,h)		LCMSendCommand(v&0xBF);LCMSendCommand(h&0X8F)

//extern int		f_LCM_En;
void LCMInit(void);
void DisplayEnable(int en);
void Lprint(char line,char *string);

#endif
