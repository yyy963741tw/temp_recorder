#include "Display.h"
#include "../Inc/comm.h"
//#include "../I2C_Slave/I2C_Slave_Lite.h"
#include "../DelayUtility/DelayUtility.h"

#define DisplayPAGE					5
#define DisplayColumn				4
#define MAXDisplayColumn  			DisplayPAGE*DisplayColumn
#define delayLoop2 					5

#define LCDIFSPI_CLK(x)				SPI_SCK_LCD(x)
#define LCDIFSPI_CLKTRIS(dir)		SPI_SCK_LCD_TRIS(dir)

//LCM write read dir
#define LCDIFSPI_DAT(x)				SPI_DI_LCD(x)
#define LCDIFSPI_DATRead			SPI_DI_LCD_R
#define LCDIFSPI_DATTRIS(dir)		SPI_DI_LCD_TRIS(dir)

#define LCDIFSPI_CS(x)				SPI_CS_LCD(x)
#define LCDIFSPI_CSTRIS(dir)		SPI_CS_LCD_TRIS(dir)

//extern unsigned char rx_data[SI2C_REG_LENGTH];

//unsigned char	LCDM_str[20];

char    DspBuffer[MAXDisplayColumn][16];
char 	ScrollIndex;
char 	ScrollIndexBase;
char	InReflashing;
char 	Strbuffer[16];
int		f_LCM_En = 0;
//void ShowDisplay();
//void DelaynSec(unsigned int uiNanosecond);

void DisplayEnable(int en){
	f_LCM_En = en;
}

void SPIInitiailze()
{
	LCDIFSPI_CLKTRIS(0);
	LCDIFSPI_DATTRIS(0);
	LCDIFSPI_CSTRIS(0);
    LCDIFSPI_CLK(0);
	LCDIFSPI_DAT(0);
	LCDIFSPI_CS(0);
//	f_LCM_En = 1;
}

void DelaynSec(unsigned int uiNanosecond)
{
	unsigned int j;
	for(j=0;j<uiNanosecond;j++);
}
void SPISendData(unsigned char rw,unsigned char rs,unsigned char buff)
{
	unsigned char title=0xF8;
	unsigned char Hibyte,Lobyte;
	unsigned char tmp;
	int count;
	if(rw==1) title=title|0x04;
	if(rs==1) title=title|0x02;
	Hibyte=buff&0xf0;
	Lobyte=(buff&0x0f)<<4;
	LCDIFSPI_CS(1);
	DelaynSec(1);
	for(count=0;count<8;count++)
	{
		LCDIFSPI_CLK(0);
		tmp=title;
		tmp=tmp<<count;
		DelaynSec(1);		//	maybe no use
		if((tmp&0x80)==0x80)
			LCDIFSPI_DAT(1);
		else
			LCDIFSPI_DAT(0);
		LCDIFSPI_CLK(1);
		DelaynSec(1);		
	}
	for(count=0;count<8;count++)
	{
		LCDIFSPI_CLK(0);
		tmp=Hibyte;
		tmp=tmp<<count;
		if((tmp&0x80)==0x80)
			LCDIFSPI_DAT(1);
		else
			LCDIFSPI_DAT(0);
		LCDIFSPI_CLK(1);
		DelaynSec(1);		
	}
	for(count=0;count<8;count++)
	{
		LCDIFSPI_CLK(0);
		tmp=Lobyte;
		tmp=tmp<<count;
		if((tmp&0x80)==0x80)
			LCDIFSPI_DAT(1);
		else
			LCDIFSPI_DAT(0);
		LCDIFSPI_CLK(1);
		DelaynSec(1);		
	}
	LCDIFSPI_CS(0);

}
unsigned char SPIReceiveData(unsigned char rw,unsigned char rs)
{
	unsigned char title=0xF8;
	unsigned char Hibyte,Lobyte;
	unsigned char tmp;
	int count;
	if(rw==1) title=title|0x04;
	if(rs==1) title=title|0x02;
	LCDIFSPI_CS(1);
	DelaynSec(1);
	for(count=0;count<8;count++)
	{
		LCDIFSPI_CLK(0);
		tmp=title;
		tmp=tmp<<count;
		if((tmp&0x80)==0x80)
			LCDIFSPI_DAT(1);
		else
			LCDIFSPI_DAT(0);
		LCDIFSPI_CLK(1);
		DelaynSec(1);		
	}
	LCDIFSPI_DATTRIS(1);
	tmp=0;
	for(count=0;count<8;count++)
	{	
		LCDIFSPI_CLK(1);
		DelaynSec(1);	
		if (LCDIFSPI_DATRead== 1){tmp = tmp | 0x01;}
		if (count < 7) tmp=tmp<<1;
		LCDIFSPI_CLK(0);
		DelaynSec(1);
	}
	Hibyte=tmp;
	tmp=0;
	for(count=0;count<8;count++)
	{	
		LCDIFSPI_CLK(1);
		DelaynSec(1);	
		if (LCDIFSPI_DATRead== 1){tmp = tmp | 0x01;}
		if (count < 7) tmp=tmp<<1;
		LCDIFSPI_CLK(0);
		DelaynSec(1);
	}
	Lobyte=tmp;
	LCDIFSPI_DATTRIS(0);
	LCDIFSPI_CS(0);
	return Lobyte|Hibyte;
}
void LCMBusyChk()
{
	unsigned char Busy;
	do{
		Busy=SPIReceiveData(1,0);
	}while((Busy&0x80)==0x80);
}
void LCMSendCommand(unsigned char CMD)
{
	LCMBusyChk();
	SPISendData(0,0,CMD);
}
void LCMSendData(unsigned char DAT)
{
	LCMBusyChk();
	SPISendData(0,1,DAT);
}
void LCMInit(void)
{
	int i;
//	useLCDM_debug = 0;
//	if(!LCDM_VADJ)	return;

	SPIInitiailze();
	for(i=0;i<MAXDisplayColumn;i++) memset(DspBuffer[i],0x20,16);
	InReflashing=0;
	ScrollIndexBase=0;
	ScrollIndex=0;
	Delay(100);
	LCMSendCommand(0x30);//function set 8 bit I/F
	DelayuSec(100);
	LCMSendCommand(0x34);//function set extended instruction
	DelayuSec(100);
	LCMSendCommand(0x36);//Function set graphich display ON
	DelayuSec(100);
	LCMSendCommand(0x80);//Set Graphic Display RAM Address
	DelayuSec(100);
	LCMSendCommand(0x80);//Set Graphic Display RAM Address
	DelayuSec(100);
	LCMSendCommand(0x30);//function set basic instruction	
	DelayuSec(1000);
	LCMSendCommand(0x0C);//Display control DSP ON,Cur OFF,Chr OFF
	DelayuSec(100);
	LCMSendCommand(0x01);//Display Clear
	Delay(10);
	LCMSendCommand(0x06);//Entry Mode Set,Cursor Move to right
	Delay(100);
}
//int Lprintf(char line,char *string,...)
//{
//
//  int j=0;
//  va_list ap;
//  int r;
////  if(!LCDM_VADJ)	return;
//  memset(Strbuffer,0x20,16);
//  va_start (ap, string);
//  r=vsprintf (Strbuffer,string, ap);
//  LCMSendCommand(0x30);
//  DelayuSec(3);
//  DisplayOff();
//  if(line<3) SetDDAddr(((line-1)%2)*0x10+0x80);
//  else       SetDDAddr(((line-1)%2)*0x10+0x88);
//  for(j=0;j<16;j++)
//  {
//	LCMSendData(Strbuffer[j]);
//	DelayuSec(1);
//  }
//  DisplayOn();
//  va_end (ap);
//  DelayuSec(5);
//}

void Lprint(char line,char *string){
	int j=0;
	char	tmp[20];
	if(!f_LCM_En)	return;
	memset(tmp,0x20,16);
//	for(j=0;j<16;j++)
//		tmp[j] = ' ';
	memcpy(tmp,string,strlen(string));
	LCMSendCommand(0x30);
	DelayuSec(3);
	DisplayOff();
	if(line<3) SetDDAddr((((line-1)%2)*0x10+0x80));
	else       SetDDAddr((((line-1)%2)*0x10+0x88));
	for(j=0;j<16;j++)
	{
	LCMSendData(tmp[j]);
	DelayuSec(1);
	}
	DisplayOn();
}

void ShowDisplay()
{
	unsigned char j;
	char i;
	if(!f_LCM_En)	return;
	LCMSendCommand(0x30);
  	DelayuSec(1);
	DisplayOff();
	for(i=1;i<5;i++)
	{
  		if(i<3) SetDDAddr((((i-1)%2)*0x10+0x80));
  		else    SetDDAddr((((i-1)%2)*0x10+0x88));
		DelayuSec(1);
  		for(j=0;j<16;j++)
  		{
			LCMSendData(DspBuffer[ScrollIndexBase+ScrollIndex][j]);
			DelayuSec(1);
  		} 
		ScrollIndex=(ScrollIndex+1)%4;
	}
	DisplayOn();
}
void ClearDisplay()
{
	char i,j;
	if(!f_LCM_En)	return;
	LCMSendCommand(0x30);
  	DelayuSec(1);
	for(i=1;i<=4;i++)
	{
  		if(i<3) SetDDAddr((((i-1)%2)*0x10+0x80));
  		else    SetDDAddr((((i-1)%2)*0x10+0x88));
		DelayuSec(1);
  		for(j=0;j<16;j++)
  		{
			LCMSendData(0x20);
			DelayuSec(1);
  		} 
	}
}

void SetDisplayPage(char p)
{
	if(!f_LCM_En)	return;
	ScrollIndexBase=(p-1)*4;
	ScrollIndex=0;
	ClearDisplay();
}
void ScrollDspPush(char *string,...)
{
	int i;
  	int j=0;
  	va_list ap;

  	int r;

	if(!f_LCM_En)	return;
	memset(Strbuffer,0x20,16);
  	va_start (ap,string);
	r=vsprintf (Strbuffer,string, ap);
	memcpy(DspBuffer[ScrollIndexBase+ScrollIndex],Strbuffer,16);
	ScrollIndex=ScrollIndexBase+((ScrollIndex+1)%4);
  	va_end (ap);
	LCMSendCommand(0x30);
  	DelayuSec(1);
	DisplayOff();
	for(i=1;i<=4;i++)
	{
		
  		if(i<3) SetDDAddr((((i-1)%2)*0x10+0x80));
  		else    SetDDAddr((((i-1)%2)*0x10+0x88));
		DelayuSec(1);
  		for(j=0;j<16;j++)
  		{
			LCMSendData(DspBuffer[ScrollIndexBase+ScrollIndex][j]);
			DelayuSec(1);
  		} 
		ScrollIndex=(ScrollIndex+1)%4;
	}
	DisplayOn();
}
void MemPrintf(unsigned char line,char *string,...)
{
  va_list ap;
  unsigned int i;
  int r;
  if(!f_LCM_En)	return;
  memset(Strbuffer,0x20,16);
  va_start (ap, string);
  r=vsprintf (Strbuffer,string, ap);	
  va_end (ap);
  for(i=0;i<16;i++) DspBuffer[line][i]=Strbuffer[i];

}
void MemRefresh(char page)
{
	if(!f_LCM_En)	return;
	if(InReflashing) return;
	InReflashing=1;
	ScrollIndexBase=(page-1)*4;
	ScrollIndex=0;	
	ShowDisplay();
	InReflashing=0;
}
void ShowLogo()
{
	if(!f_LCM_En)	return;
	Lprint(1,"  WONDER-SYSTEM ");
	Lprint(2,"TEL:886426568018");
	Lprint(3,"  WS-ECP5U2v1   ");
//#ifdef VBL_INV
//	Lprint(4,"==== V2.1.8 ====");
//#else
//	Lprint(4,"==== V8.0.0 ====");
//#endif

//	Delay(500);
}
