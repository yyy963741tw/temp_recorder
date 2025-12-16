#ifndef _tftlcd_H
#define _tftlcd_H

#include "../system/system.h"
#include "font.h"


//����LCD��������������  �ɸ����Լ����ϵĲ��������ͺ���ѡ�����������
//#define TFTLCD_HX8357D 

//#define TFTLCD_HX8352C

//#define TFTLCD_ILI9341

//#define TFTLCD_R61509V

//#define TFTLCD_R61509VN

//#define TFTLCD_R61509V3

//#define TFTLCD_ILI9488

//#define TFTLCD_ST7793

//#define TFTLCD_ILI9325

//#define TFTLCD_ILI9327

//#define TFTLCD_NT35510

#define TFTLCD_RGB4342	//4.3����,480*272�ֱ���
//#define TFTLCD_RGB7084	//7����,800*480�ֱ���
//#define TFTLCD_RGB7016	//7����,1024*600�ֱ���
//#define TFTLCD_RGB7018	//7����,1280*800�ֱ���
//#define TFTLCD_RGB8016	//8����,1024*600�ֱ���
//#define TFTLCD_RGB1018	//10.1����,1280*800�ֱ���


#define TFTLCD_DIR	1	//0������  1������  Ĭ������

//LCD��Ҫ������
typedef struct  
{		 	 
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;				//LCD ID
	u8  dir;			//���������������ƣ�0��������1��������	
}_tftlcd_data; 	  

//LCD����
extern _tftlcd_data tftlcd_data;	//����LCD��Ҫ����
//LCD�Ļ�����ɫ�ͱ���ɫ	   
extern u32  FRONT_COLOR;    
extern u32  BACK_COLOR; 

//LCD MPU��������
#define LCD_REGION_NUMBER		MPU_REGION_NUMBER0		//LCDʹ��region0
#define LCD_ADDRESS_START		(0X60000000)			//LCD�����׵�ַ
#define LCD_REGION_SIZE			MPU_REGION_SIZE_256MB   //LCD����С


//LCD��ַ�ṹ��
typedef struct
{
	vu16 LCD_CMD;
	vu16 LCD_DATA;
}TFTLCD_TypeDef;

//ʹ��NOR/SRAM�� Bank1.sector4,��ַλHADDR[27,26]=11 A18��Ϊ�������������� 
//ע������ʱSTM32�ڲ�������һλ����!  			    
#define TFTLCD_BASE        ((u32)(0x60000000 | 0x0007FFFE))
#define TFTLCD             ((TFTLCD_TypeDef *) TFTLCD_BASE)
	 

//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)


void LCD_WriteCmd(u16 cmd);
void LCD_WriteData(u16 data);
void LCD_WriteCmdData(u16 cmd,u16 data);
void LCD_WriteData_Color(u16 color);

void TFTLCD_Init(void *init(void));									//��ʼ��
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);	//���ô���
void LCD_Display_Dir(u8 dir);								//������Ļ��ʾ����
void LCD_Clear(u32 Color);	 								//����
void LCD_DrawPoint(u16 x,u16 y);							//����
void LCD_DrawFRONT_COLOR(u16 x,u16 y,u32 color);			//���ٻ���
u32  LCD_ReadPoint(u16 x,u16 y); 							//���� 
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);					//��Բ
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);			//����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		//������
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color);		//��䵥ɫ
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);//��ָ�����������ָ����ɫ��	
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);		//��ʾһ���ַ�
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  		//��ʾһ������
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);	//��ʾ ����
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);//��ʾһ���ַ���,12/16����
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn);

void LCD_ShowHZ_Debug(u16 x, u16 y, const unsigned char *font_matrix);
void LCD_ShowPicture(u16 x, u16 y, u16 wide, u16 high,u8 *pic);

#endif
