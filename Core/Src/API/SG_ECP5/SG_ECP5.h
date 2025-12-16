#ifndef __SG_ECP5_H_
#define __SG_ECP5_H_

#include "..\Inc\comm.h"		//	to get FCY information
#include "..\SPI_Master\SPI_Master.h"
#include "FPGA_SPI_REGISTERS.h"
extern SPIM_IDX SG_ECP5_SPI_IDX;

// #include	"xbasic_types.h"

//	define pins from HW
#define SG_ECP5_CS0(x)				FPGA_SPI_CS0(x)
//#define SG_ECP5_CS1(x)				FPGA_SPI_CS1(x)
#define SG_ECP5_RST(x)				FPGA_RESET(x)
//#define FPGA_RESET(x)				FPGA_RESET(x)
//	SG_ECP5 reset control
//#define SG_ECP5_RST_ON(x)			x = 0
//#define SG_ECP5_RST_OFF(x)			x = 1

//#define SG_ECP5_CS						ECP_SPI_CS
// #define SG_ECP5_RST						SG_ECP5_SPI_CS

//	SG_ECP5 SPI wait

//Power Data

#define Power_OCP_Time					0x0030
#define Power_OVP_Time					0x0031
#define Power_ACH_OFF					0x0033
	#define CH0_ACH_OFF					0x0001
#define IIC_START						0x0033


//	#define CH1_ACH_OFF					0x0002
//	#define CH2_ACH_OFF					0x0004
//	#define CH3_ACH_OFF					0x0008
//#define Power_Update					0x0034
	#define DEBUG_EN					0x8000
#define Power_V_Start					0x0035
//	#define Power_V0_VDD				0x0001
//	#define Power_V1_VBL				0x0002
	#define CH0_VBL_Start				0x0001
	#define CH0_VDD_Start				0x0002
//	#define CH1_V0_Start				0x0004
//	#define CH1_V1_Start				0x0008
//	#define CH2_V0_Start				0x0010
//	#define CH2_V1_Start				0x0020		//VDD
//	#define Ex_V0_Start					0x0040		//VBL

	#define Power_V_Start_Signal_EN		0x8000
#define Power_OXP_Reset					0x0036		//	clear all ocp, ovp flags

//	commands
#define SG_ECP5_CMD_OCP_TIME			0x0030
#define SG_ECP5_CMD_UPDATE				0x0034
#define SG_ECP5_CMD_GPIO				0x0070
#define SG_ECP5_CMD_READ				0x00FF
// #define SG_ECP5_CMD_READOVP			0x0030

//	write data
#define SG_ECP5_WDATA_UPDATE			0x0001

//	read data
#define SG_ECP5_RDATA_OCP				0x0001
	#define CH0_VBL_OCP					0x0001
	#define CH0_VDD_OCP					0x0002
//	#define CH1_VBL_OCP					0x0004
//	#define CH1_VDD_OCP					0x0008
//	#define CH2_VBL_OCP					0x0010
//	#define CH2_VDD_OCP					0x0020
//	#define EX_VBL_OCP					0x0040
//	#define EX_VDD_OCP					0x0080

#define SG_ECP5_RDATA_OVP				0x0002
	#define VBL_OVP						0x0001
	#define VDD_OVP						0x0002
//	#define CH1_VBL_OVP					0x0004
//	#define CH1_VDD_OVP					0x0008
//	#define CH2_VBL_OVP					0x0010
//	#define CH2_VDD_OVP					0x0020
//	#define EX_VBL_OVP					0x0040
//	#define EX_VDD_OVP					0x0080

#define SPI_RDATA_data_busy_o			0x0001
#define SPI_RDATA_data_ready			0x0002
#define SPI_RDATA_data_ack_error_o		0x0003
#define SPI_RDATA_data_ALT 		        0x0006
#define SPI_RDATA_data_TEST 		    0x0007
#define SPI_RDATA_data_TEST2 		    0x0008



#define SG_ECP5_RDATA_VERSION			0x0004
#define SG_ECP5_RDATA_ADX				0x0005
#define SG_ECP5_RDATA_CHECKSUM			0x0019

#define SG_ECP5_RDATA_LAST				0x00FF

//	other return data
#define SG_ECP5_OTHER_RETURN			0xA5A5

//Panel Data

#define SG_ECP5_Panel_Vtotal			0x0010
#define SG_ECP5_Panel_Htotal			0x0011
#define SG_ECP5_Panel_Vsync				0x0012
#define SG_ECP5_Panel_Hsync				0x0013
#define SG_ECP5_Panel_Vbp				0x0014
#define SG_ECP5_Panel_Hbp				0x0015
#define SG_ECP5_Panel_Vactive			0x0016
#define SG_ECP5_Panel_Hactive			0x0017
#define SG_ECP5_Panel_ControlSetting	0x0018

//Panel_ControlSetting
#define SG_ECP5_Attr_BGR				0x0001 //	1 BGR 0 RGB
#define SG_ECP5_Attr_H_NOT				0x0002 //	1 Active High 0 Active low
#define SG_ECP5_Attr_V_NOT				0x0004 //	1 Active High 0 Active low
#define SG_ECP5_Attr_E_NOT				0x0008 //	1 Active Low 0 Active High
#define SG_ECP5_Attr_CLK_EDGE			0x0010 //  1 posedge  0 negedge
#define SG_ECP5_Attr_SHIFT_MODE			0x0060	//	3:MSB to LSB 2 :{[5:0],[7:6]} 1: {[1:0],[7:2]} 0: org
	#define SG_ECP5_Attr_SHIFT_L2B		0x0040
	#define SG_ECP5_Attr_SHIFT_R2B		0x0020
	#define SG_ECP5_Attr_SHIFT_M2L		0x0060	//	[7:0] reverse ==> [0:7]
#define SG_ECP5_Attr_LVDS_TTL			0x0080	//	0 LVDS-out TTL-off  1 TTL-out LVDS-off
#define SG_ECP5_Attr_JEIDA_VESA			0x0100	//  1JEIDA 0 VESA
#define SG_ECP5_Attr_BIT_REVERSE		0x0200	//  1 [0:7]; 0 [7:0]
#define SG_ECP5_Attr_SINGLE				0x0400	//  1 single 0 dual
#define SG_ECP5_Attr_bit8_10			0x1800	//	3/2:6bit(now not used) 1:8bit 0:10bit
	#define SG_ECP5_Attr_FIXED_8BIT		0x0800
#define SG_ECP5_Attr_odd_even			0x2000	//	1 even/odd 0 odd/even
#define SG_ECP5_Attr_DATA_NOT			0x4000	//	1 Data Not 0 Org(NULL)"

//Pic Data
#define SG_ECP5_PIC_Update				0X0025

typedef struct {
	unsigned short OCP_Time;
	unsigned short OVP_Time;
	unsigned short ACH_OFF;
	unsigned short Update;
	unsigned short V_Start;
	unsigned long  OCP_Status;
	unsigned long  OVP_Status;
} sPower_Data;

typedef enum {
	PIC_Update_Stop	=0X0000,
	PIC_Update_En	=0X0001,
}e_PIC_Update_value;

typedef enum {
	P_Luster_R255		=0,
	P_Luster_G255       =1,
	P_Luster_B255       =2,
	P_Luster_W0         =3,
	P_Luster_W255       =4,
	P_Luster_W127       =5,
	P_Luster_W92        =6,
	P_Luster_W181       =7,

	P_Gray_Level0       =8,
	P_Gray_Level1       =9,

	P_Cyan              =10,
	P_V_Gray256         =11,
	P_V_Gray64          =12,
	P_V_Gray8           =13,
	P_H_Gray256         =14,
	P_H_Gray64          =15,
	P_H_Gray8           =16,
	P_V_BW              =17,
	P_3C                =18,
	P_B_Window          =19,
	P_8C_8G             =20,
	P_8C_256G           =21,
	P_Gray_Black_Flicker_0=22,
	P_Gray_Black_Flicker_1=23,
	P_Cyan_Flicker      =24,
	P_Check_Subpixel    =25,
	P_H_Gomi1           =26,
	P_H_Gomi2           =27,
	P_Jiugongge         =28,
	P_1_4_3C            =29,
	P_1_4_V_Gray64      =30,
	P_1_4_H_Gray64      =31,
	P_chess             =32,
	P_4c                =33,
	P_frame             =34,
	P_1_4_4c            =35,
	P_8G_8G             =36,
	P_B_windows         =37,
	P_Stress_OP         =38,
	P_Column            =39,
	P_8G_8C				=40,

	P_custom_RGB=255,


}e_PatternName;

#define SG_ECP5_PG_PIC_LSB		0x0080
#define SG_ECP5_PG_PIC_LSB		0x0080
#define SG_ECP5_PG_PIC_MSB		0x0081
#define SG_ECP5_h_number_in_LSB	0x0082
#define SG_ECP5_h_number_in_MSB	0x0083
#define SG_ECP5_v_number_in_LSB	0x0084
#define SG_ECP5_v_number_in_MSB	0x0085
#define SG_ECP5_h0_v0_LSB		0x0086
#define SG_ECP5_h0_v0_MSB		0x0087
#define SG_ECP5_h1_v1_LSB		0x0088
#define SG_ECP5_h1_v1_MSB		0x0089
#define SG_ECP5_color0_in_LSB	0x008A
#define SG_ECP5_color0_in_MSB	0x008B
#define SG_ECP5_color1_in_LSB	0x008C
#define SG_ECP5_color1_in_MSB	0x008D

//flicker_color	0x0090~0X00AE
#define SG_ECP5_flicker_color_00_LSB	0x0090
#define SG_ECP5_flicker_color_00_MSB	0x0091
#define SG_ECP5_flicker_color_01_LSB	0x0092
#define SG_ECP5_flicker_color_01_MSB	0x0093
#define SG_ECP5_flicker_color_02_LSB	0x0094
#define SG_ECP5_flicker_color_02_MSB	0x0095
#define SG_ECP5_flicker_color_03_LSB	0x0096
#define SG_ECP5_flicker_color_03_MSB	0x0097

#define	SG_ECP5_flicker_color_10_LSB	0x0098
#define	SG_ECP5_flicker_color_10_MSB	0x0099
#define	SG_ECP5_flicker_color_11_LSB	0x009A
#define	SG_ECP5_flicker_color_11_MSB	0x009B
#define	SG_ECP5_flicker_color_12_LSB	0x009C
#define	SG_ECP5_flicker_color_12_MSB	0x009D
#define	SG_ECP5_flicker_color_13_LSB	0x009E
#define	SG_ECP5_flicker_color_13_MSB	0x009F

#define	SG_ECP5_flicker_color_20_LSB	0X00A0
#define	SG_ECP5_flicker_color_20_MSB	0X00A1
#define	SG_ECP5_flicker_color_21_LSB	0X00A2
#define	SG_ECP5_flicker_color_21_MSB	0X00A3
#define	SG_ECP5_flicker_color_22_LSB	0X00A4
#define	SG_ECP5_flicker_color_22_MSB	0X00A5
#define	SG_ECP5_flicker_color_23_LSB	0X00A6
#define	SG_ECP5_flicker_color_23_MSB	0X00A7

#define SG_ECP5_flicker_color_30_LSB	0x00A8
#define SG_ECP5_flicker_color_30_MSB	0x00A9
#define SG_ECP5_flicker_color_31_LSB	0x00AA
#define SG_ECP5_flicker_color_31_MSB	0x00AB
#define SG_ECP5_flicker_color_32_LSB	0x00AC
#define SG_ECP5_flicker_color_32_MSB	0x00AD
#define SG_ECP5_flicker_color_33_LSB	0x00AE
#define SG_ECP5_flicker_color_33_MSB	0x00AF


typedef struct {
	unsigned short H;
	unsigned short HBP;
	unsigned short HFP;
	unsigned short HSW;
	unsigned short V;
	unsigned short VBP;
	unsigned short VFP;
	unsigned short VSW;
	unsigned short FPS;
	unsigned int	PPC;
	// short BPC;	//	bit per color
	// short PG_REG_Base;
	unsigned short ControlSetting;
} sPG_Timing;

typedef struct {

unsigned short color_flicker_color_00_LSB;
unsigned short color_flicker_color_00_MSB;
unsigned short color_flicker_color_01_LSB;
unsigned short color_flicker_color_01_MSB;
unsigned short color_flicker_color_02_LSB;
unsigned short color_flicker_color_02_MSB;
unsigned short color_flicker_color_03_LSB;
unsigned short color_flicker_color_03_MSB;

unsigned short color_flicker_color_10_LSB;
unsigned short color_flicker_color_10_MSB;
unsigned short color_flicker_color_11_LSB;
unsigned short color_flicker_color_11_MSB;
unsigned short color_flicker_color_12_LSB;
unsigned short color_flicker_color_12_MSB;
unsigned short color_flicker_color_13_LSB;
unsigned short color_flicker_color_13_MSB;

unsigned short color_flicker_color_20_LSB;
unsigned short color_flicker_color_20_MSB;
unsigned short color_flicker_color_21_LSB;
unsigned short color_flicker_color_21_MSB;
unsigned short color_flicker_color_22_LSB;
unsigned short color_flicker_color_22_MSB;
unsigned short color_flicker_color_23_LSB;
unsigned short color_flicker_color_23_MSB;

unsigned short color_flicker_color_30_LSB;
unsigned short color_flicker_color_30_MSB;
unsigned short color_flicker_color_31_LSB;
unsigned short color_flicker_color_31_MSB;
unsigned short color_flicker_color_32_LSB;
unsigned short color_flicker_color_32_MSB;
unsigned short color_flicker_color_33_LSB;
unsigned short color_flicker_color_33_MSB;

} sFlicker_Color;

typedef struct {

unsigned short S_PG_PIC_LSB			;
unsigned short S_PG_PIC_MSB			;
unsigned short S_h_number_in_LSB		;
unsigned short S_h_number_in_MSB		;
unsigned short S_v_number_in_LSB		;
unsigned short S_v_number_in_MSB		;
unsigned short S_h0_v0_LSB			;
unsigned short S_h0_v0_MSB			;
unsigned short S_h1_v1_LSB			;
unsigned short S_h1_v1_MSB			;
unsigned short S_color0_in_LSB		;
unsigned short S_color0_in_MSB		;
unsigned short S_color1_in_LSB		;
unsigned short S_color1_in_MSB		;

sFlicker_Color sFlicker_Color;
} sPic_Data;

int SG_ECP5_Init(SPIM_IDX idx);
void SG_ECP5_SelectCS(int cs_idx);
int SG_ECP5_Transfer(unsigned short cmd, unsigned short wd, unsigned long *rd);
void SG_ECP5_Enable(int en);
int SG_ECP5_GetVersion(unsigned long *version);
int SG_ECP5_PatternChange(unsigned char *param);
void SG_ECP5_SetV_Start(sPower_Data *inst);
int SG_ECP5_SetACH_OFF(sPower_Data *inst);
int SG_ECP5_Get_OXPStatus(sPower_Data *inst);
int SG_ECP5_OXP_Reset();
#endif	//	__SG_ECP5_H_
