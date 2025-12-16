#ifndef __PowerManager_H_
#define __PowerManager_H_
#include "../Inc/comm.h"
#include "../Inc/adc.h"
//#include "../1014A/POWR1014A.h"
//#include "../LTC2309/LTC2309.h"
#include "../EEPROM/EEPROM.h"
#include "../DACx3004/DACx3004.h"
//#include "../FB2635/fb2635.h"
//#include "../DAC7574/DAC7574.h"
//#include "../DAC5578/DAC5578.h"
//#include "../DAC7678/DAC7678.h"
//#include "../POWER_1014A/POWER_1014A.h"
#include "../DelayUtility/DelayUtility.h"

#define	SAMPLE_TIMES			32

//	OXP debouncing counter
#define V12_OCP_DEBOUNCING_CNT	5
#define OXP_DEBOUNCING_CNT		1
#define UVP_DEBOUNCING_CNT		1
//	max retry count for voltage scan, 1 DAC step for 1 retry
#define VCH_SCAN_RETRY_MAX		300

//	wait stable when set voltage 0 to max //38ms
#define V_STABLE_MS				7000  //h to l 6.70s
#define Measure_STABLE_CNT		100
//	chanel wait power on
#define CH_POWER_ON_MS			100

#define VDD_SET_MID				124	//VBL_MID_DA, 13.5v
#define VBL_SET_MID				1547	//VBL_MID_DA, 29v

#define VDD_MIN					30	//VDD, 2A
#define VDD_MAX					240
#define VBL_MIN					100  //VBL, 8.5A
#define VBL_MAX					480
//#define AVDD_MIN				25  //AVDD, 1A
//#define AVDD_MAX				150
//#define VGH_MIN				50  //VGH, 0.2A
//#define VGH_MAX				220
//#define VGL_MIN				50  //VGL, 0.2A
//#define VGL_MAX				150

//#define VDD_IMAX				3000	// 3000mA
//#define VDD_IMAX				3000	// 3000mA
//#define VDD_IMAX				3000	// 3000mA
//#define VDD_IMAX				3000	// 3000mA
//#define VDD_IMAX				3000	// 3000mA

//	voltage set for test pattern 0
//#define	TEST_VDD0				48
//#define	TEST_VDD1				48
#define	TEST_VDD0				200
#define	TEST_VBL0				200
//#define	TEST_VBL1				150
//#define	TEST_AVDD			56
//#define	TEST_VGH			185
//#define	TEST_VGL			120
//#define	TEST_C0VDD				1000
#define	TEST_C0VDD				100
#define	TEST_C0VBL				100
////#define	TEST_C0AVDD			100
////#define	TEST_C0VGH			100
////#define	TEST_C0VGL			200
//#define	TEST_C1VDD				1000
//#define	TEST_C1VBL				1000
////#define	TEST_C1AVDD			100
////#define	TEST_C1VGH			100
////#define	TEST_C1VGL			200
//#define	TEST_C2VDD				1000
//#define	TEST_C2VBL				1000
//#define	TEST_CEXVDD				1000
//#define	TEST_CEXVBL				1000


//	voltage set for test pattern 1
//#define	TEST1_VDD0				25
//#define	TEST1_VDD1				25
#define	TEST1_VDD0				30
#define	TEST1_VBL0				100
//#define	TEST1_VBL1				100
//#define	TEST1_AVDD			50
//#define	TEST1_VGH			50
//#define	TEST1_VGL			50
//#define	TEST1_C0VDD				100
#define	TEST1_C0VDD				1000
#define	TEST1_C0VBL				1000
////#define	TEST1_C0AVDD		100
////#define	TEST1_C0VGH			100
////#define	TEST1_C0VGL			100
//#define	TEST1_C1VDD				100
//#define	TEST1_C1VBL				100
////#define	TEST1_C1AVDD		100
////#define	TEST1_C1VGH			100
////#define	TEST1_C1VGL			100
//#define	TEST1_C2VDD				100
//#define	TEST1_C2VBL				100
//#define	TEST1_CEXVDD			100
//#define	TEST1_CEXVBL			100


//	voltage set for test pattern 2
//#define	TEST2_VDD0				60
//#define	TEST2_VDD1				60
#define	TEST2_VDD0				240
#define	TEST2_VBL0				480
//#define	TEST2_VBL1				240
//#define	TEST2_AVDD			150
//#define	TEST2_VGH			220
//#define	TEST2_VGL			180
//#define	TEST2_C0VDD				2000
#define	TEST2_C0VDD				2100
#define	TEST2_C0VBL				16000
////#define	TEST2_C0AVDD		1000
////#define	TEST2_C0VGH			200
////#define	TEST2_C0VGL			200
//#define	TEST2_C1VDD				2000
//#define	TEST2_C1VBL				8500
////#define	TEST2_C1AVDD		1000
////#define	TEST2_C1VGH			200
////#define	TEST2_C1VGL			200
//#define	TEST2_C2VDD				2000
//#define	TEST2_C2VBL				8500
//#define	TEST2_CEXVDD			2000
//#define	TEST2_CEXVBL			16000

#define V_RESOLUTION			100

//	current cab. for INA199
//#define	INA199_DAC_TRANS_GAPE	10		//>10

//	OCP/current calculate parameters
//#define	CURRENT_MAX						200		//(mA)
//#define CURRENT_SAMPLE_COUNT			8
////	ensurance range :
////	0~4		mA -> +-8%
////	4~200	mA -> +-3%
////	200~250	mA -> +-5%
//#define	CURRENT_MAX_ERROR				300		//	3%
//#define	CURRENT_MAX_SCAN				5		//	if still CAB fail in 5 times, we think there is a hardware crash.
//
//#define	VSHUNT_GAIN						20
//#define	RSHUNT							1
////#define	V2I_COND						(VSHUNT_GAIN/RSHUNT)
//#define	V2I_COND						20	//	(VSHUNT_GAIN/RSHUNT)
//
//#define	CURRENT_SAMPLE_STEP				(CURRENT_MAX/(CURRENT_SAMPLE_COUNT+1)+1)
//
////	OCP parameters
//#define	OCP_MAX_I						CURRENT_MAX
//#define	OCP_C2VSET_FACTOR(x)			(VA_SET_MAX*x/OCP_MAX_I)

//#define	CCH_ALL_CNT				10		//	VDD,AVDD,VGH,VGL,VBL * 2 ports = 10
//#define	CCH_PORTS				1
//#define	CCH_SOURCE_EACH_PORT	8

//	Power parameters
//	CVDD
#define CVDD_I_MAX				2100 //16000	//	mA
#define CVDD_P_MAX				30				//	W
#define CVDD_SHUNT_R			4				//	mR
#define CVDD_SHUNT_OPA			200				//	200 for INA199; 100 for LMP8603
#define	CxVDD_BASE_LIMIT		190				//	mV resistor 20:1 ~=0.1571v
//	CVBL
#define CVBL_I_MAX				16000 //16000	//	mA
#define CVBL_P_MAX				400				//	W
#define CVBL_SHUNT_R			1				//	mR
#define CVBL_SHUNT_OPA			200				//	200 for INA199; 100 for LMP8603
#define	CxVBL_BASE_LIMIT		80				//	mV resistor 20:1 ~=0.1571v/2=0.0786
//	CVDD
//#define CVDD_I_MAX				2000	//	mA
//#define CVDD_P_MAX				12		//	W
//#define CVDD_SHUNT_R			5		//	mR
//#define CVDD_SHUNT_OPA			200		//	200 for INA199; 100 for LMP8603

////	CVBLEX
//#define CVBLEX_I_MAX			16000	//	mA, original value is 13000, relax to 16000.
//#define CVBLEX_P_MAX			350		//	W
//#define CVBLEX_SHUNT_R			1		//	mR
//#define CVBLEX_SHUNT_OPA		200		//	200 for INA199; 100 for LMP8603
////	CAVDD
//#define CAVDD_I_MAX				1000	//	mA
//#define CAVDD_P_MAX				15		//	W
//#define CAVDD_SHUNT_R			10		//	mR
//#define CAVDD_SHUNT_OPA			200		//	200 for INA199; 100 for LMP8603
////	CVGH
//#define CVGH_I_MAX				200	//	mA
//#define CVGH_P_MAX				5		//	W
//#define CVGH_SHUNT_R			10		//	mR
//#define CVGH_SHUNT_OPA			200		//	200 for INA199; 100 for LMP8603
////	CVGL
//#define CVGL_I_MAX				200	//	mA
//#define CVGL_P_MAX				3		//	W
//#define CVGL_SHUNT_R			200		//	mR
//#define CVGL_SHUNT_OPA			100		//	200 for INA199; 100 for LMP8603

//typedef enum {
//	C0VDD_IDX = 0,
//	C0VBL_IDX,
//	//------	AVDD, VGH, VGL are dumming	-----------
//	C0AVDD_IDX,
//	C0VGH_IDX,
//	C0VGL_IDX,
//	//------	AVDD, VGH, VGL are dumming	-----------
//	C1VDD_IDX,
//	C1VBL_IDX,
//	//------	AVDD, VGH, VGL are dumming	-----------
//	C1AVDD_IDX,
//	C1VGH_IDX,
//	C1VGL_IDX,
//	//------	AVDD, VGH, VGL are dumming	-----------
//	CCH_TOTAL
//}CCH_IDX;
//
//typedef enum {
//	VDD0_IDX = 0,
//	VBL0_IDX,
//	VDD1_IDX,
//	VBL1_IDX,
//	//------	AVDD, VGH, VGL are dumming	-----------
//	AVDD_IDX,
//	VGH_IDX,
//	VGL_IDX,
//	//------	AVDD, VGH, VGL are dumming	-----------
//	VCH_TOTAL
//}VCH_IDX;

typedef enum {
	C0VDD_IDX = 0,
	C0VBL_IDX,
	CCH_TOTAL
}CCH_IDX;

typedef enum {
	VDD_IDX = 0,
	VBL_IDX,
	VCH_TOTAL
}VCH_IDX;

typedef enum {
	PVDD_IDX = 0,
	PVBL_IDX,
//	PAVDD_IDX,
//	PVGH_IDX,
//	PVGL_IDX,
	PCH_TOTAL
}PCH_IDX;

typedef struct {
	DAC_HandleTypeDef			*OXP_DAC_INST_ST;
	DACx3004					*OXP_DAC_INST_63004;
}OXP_DAC_INST;

typedef struct {
	VCH_IDX						V_idx;
	char						Name[7];
	unsigned short				V_SET;			//	v set value in 0.1v
	unsigned short				V_Meas;			//	measured voltage
//	unsigned char				V_SET_DAC_PORT;
//	DAC7678						*V_SET_DAC_INST;
	DACx3004					*V_SET_DAC_INST;
	unsigned char				V_SET_DAC_PORT;
//	unsigned char				V_SET_DAC_SINK_Range;
//	unsigned char				V_SET_DAC_REF_GAIN;
//	unsigned char				V_SET_DAC_Mode;
//	unsigned char				V_SET_DAC_DA_DEFAULT;
//	LTC2309						*V_READ_ADC_INST;
	ADC_HandleTypeDef			*V_READ_ADC_INST;
	unsigned char				V_READ_ADC_PORT;
	unsigned int				V_READ_ADC_CH;
	DAC_HandleTypeDef			*OVP_DAC_INST_ST;
	DACx3004					*OVP_DAC_INST_63004;
	unsigned char				OVP_DAC_PORT;
//	unsigned char				OVP_DAC_SINK_Range;
//	unsigned char				OVP_DAC_REF_GAIN;
//	unsigned char				OVP_DAC_Mode;
//	unsigned char				OVP_DAC_DA_DEFAULT;

	unsigned short				OVP_V;			//	OVP value in 0.1v

//	unsigned char				UVP_DAC;
//	unsigned char				UVP_SADR;
	unsigned short				UVP_V;			//	UVP value in 0.1v
	EEPROM						*EPM_INST;
	unsigned short				EEPROM_adr;
	unsigned short				V_cab;			//	cab value to get real voltage
	unsigned int				V_cab_div;		//	the HW dividor ratio
	unsigned short				V_min;
	unsigned short				V_max;
	unsigned short				V_set_mid;
	unsigned short				V_set_init;
	unsigned int				OVP_flag;
	unsigned int				OVP_debouncing;
	unsigned int				UVP_flag;
	unsigned int				UVP_debouncing;
}VCH;

typedef struct {
	CCH_IDX						I_idx;
	unsigned short				I_Meas;
	unsigned char				I_READ_ADC_PORT;
	unsigned int				I_READ_ADC_CH;
//	POWER_1014A					*I_READ_ADC_INST;
//	LTC2309						*I_READ_ADC_INST;
	ADC_HandleTypeDef			*I_READ_ADC_INST;
	unsigned char				OCP_DAC_PORT;
//	unsigned char				OCP_DAC_SINK_Range;
//	unsigned char				OCP_DAC_REF_GAIN;
//	unsigned char				OCP_DAC_Mode;
//	unsigned char				OCP_DAC_DA_DEFAULT;
	DAC_HandleTypeDef			*OCP_DAC_INST_ST;
	DACx3004					*OCP_DAC_INST_63004;
	unsigned short				OCP_MA;				//	OCP value in mA
	unsigned short				UCP_MA;
	unsigned short				I_BASE_LIMIT;		//	mA
	unsigned short				I_MAX;				//	mA
	unsigned short				P_MAX;				//	W
	unsigned short				SHUNT_R;			//	shunt resistor mR
	unsigned short				SHUNT_OPA;			//	shunt op ratio; INA199=200, LMP8602=100
	EEPROM						*EPM_INST;
	unsigned short				EEPROM_adr;
	unsigned short				I_cab;				//	cab value
	unsigned short				HC_SHUNT;			//	use only for high current error
	VCH							*vch;				//	point to the voltage source
	unsigned int				OCP_flag;
	unsigned int				OCP_debouncing;
	unsigned int				UCP_flag;
//	unsigned int				P_EN;				//	power enable setting from config
//	unsigned int				P_STAT;				//	power status (0=off)
	//	power on/off delay
//	unsigned int				PON_Delay;
//	unsigned int				POFF_Delay;
//	unsigned short				V_min;
//	unsigned short				V_max;
//	unsigned short				V_set_init;
//	unsigned short				Wait_ms;		//	wait time for rise/fall when doing CAB
}CCH;

typedef struct{
	PCH_IDX						idx;
	unsigned int				POW_EN;
	unsigned int				POW_STAT;
	unsigned short				PON_Delay;
	unsigned short				POFF_Delay;
}POWER_CHANNEL;

//	voltage div before input ADC
#define	VDD_CAB_DIV				11000	//	mV resistor 10:1
#define	VBL_CAB_DIV				16000	//	mV resistor 15:1 20:1
//#define	VDD1_CAB_DIV		2000	//	resistor 1:1
//#define	VBL1_CAB_DIV		16000	//	resistor 15:1
//#define	AVDD_CAB_DIV		8000	//	resistor 2.1:0.3
//#define	VGH_CAB_DIV			8000	//	resistor 2.1:0.3
//#define	VGL_CAB_DIV			8000	//	resistor 2.1:0.3

//	DAC port associate with WSP-B610Bv5
//	VSET
//#define	VDD_SET_PORT			DACx3004_DAC_2 //DAC7678_DAC_E
//#define	VDD1_SET_PORT		DAC7678_DAC_C
//#define	VBL_SET_PORT			DACx3004_DAC_3 //DAC7678_DAC_G
//#define	VBL1_SET_PORT		DAC7678_DAC_D
//#define	AVDD_SET_PORT		DAC7678_DAC_E
//#define	VGH_SET_PORT		DAC7678_DAC_F
//#define	VGL_SET_PORT		DAC7678_DAC_G
#define	VDD_SET_Mode			DACx3004_IOut //DAC7678_DAC_E
#define	VDD_SET_SINK_RANGE		I_0_TO_N240uA //DAC7678_DAC_E
#define	VDD_SET_REF_GAIN		Gain_1_5X_IN_REF //DAC7678_DAC_E
#define	VDD_SET_DA_DEFAULT		1
#define	VDD_SET_PORT_I			DACx3004_DAC_0 //DAC7678_DAC_G
#define	VDD_SET_PORT_V			NULL //DAC7678_DAC_G

#define	VBL_SET_Mode			DACx3004_VOut //DAC7678_DAC_E
#define	VBL_SET_SINK_RANGE		I_0_TO_N240uA //DAC7678_DAC_E
#define	VBL_SET_REF_GAIN		Gain_1_5X_IN_REF //DAC7678_DAC_E
#define	VBL_SET_DA_DEFAULT		1
#define	VBL_SET_PORT_I			NULL //DAC7678_DAC_G
#define	VBL_SET_PORT_V			DACx3004_DAC_1 //DAC7678_DAC_G
//	OVP
//#define	VDD_OVP_PORT			DAC_CHANNEL_1 //DAC7678_DAC_D
//#define	VDD1_OVP_PORT		DAC5578_DAC_D
//63004
#define	VDD_OVP_Mode			DACx3004_VOut //DAC7678_DAC_E
#define	VDD_OVP_SINK_RANGE		I_0_TO_N240uA //DAC7678_DAC_E
#define	VDD_OVP_REF_GAIN		Gain_3_0X_IN_REF //DAC7678_DAC_E
#define	VDD_OVP_DA_DEFAULT		1
#define	VDD_OVP_PORT_I			NULL //DAC7678_DAC_G
#define	VDD_OVP_PORT_V			DACx3004_DAC_3 //DAC7678_DAC_G
//stm32
#define	VBL_OVP_PORT			DAC_CHANNEL_1 //DAC7678_DAC_H
//#define	VBL1_OVP_PORT		DAC5578_DAC_B
//#define	AVDD_OVP_PORT		DAC5578_DAC_E
//#define	VGH_OVP_PORT		DAC5578_DAC_A
//#define	VGL_OVP_PORT		DAC5578_DAC_F

//	OCP
//#define	VDD0_OCP_PORT			DAC5578_DAC_E
//63004
#define	VDD0_OCP_Mode			DACx3004_VOut //DAC7678_DAC_E
#define	VDD0_OCP_SINK_RANGE		I_0_TO_N240uA //DAC7678_DAC_E
#define	VDD0_OCP_REF_GAIN		Gain_3_0X_IN_REF //DAC7678_DAC_E
#define	VDD0_OCP_DA_DEFAULT		1
#define	VDD0_OCP_PORT_I			NULL //DAC7678_DAC_G
#define	VDD0_OCP_PORT_V			DACx3004_DAC_2 //DAC7678_DAC_G
//stm32
#define	VBL0_OCP_PORT			DAC_CHANNEL_2
//#define	VDD1_OCP_PORT			DAC5578_DAC_A
//#define	VBL1_OCP_PORT			DAC5578_DAC_C
//#define	VDD2_OCP_PORT			DAC5578_DAC_F
//#define	VBL2_OCP_PORT			DAC5578_DAC_D
//#define	VDDEX_OCP_PORT		DAC5578_DAC_H
//#define	VBLEX_OCP_PORT			DAC5578_DAC_H
//#define	AVDD_OCP_PORT		DAC5578_DAC_A
//#define	VGH_OCP_PORT		DAC5578_DAC_E
//#define	VGL_OCP_PORT		DAC5578_DAC_G

//#ifdef LTC2309
////	ADC port for measure
////	Voltage
//#define VDD0_V_READ_PORT		LTC2309_CH0
//#define VBL0_V_READ_PORT		LTC2309_CH1
//#define VDD1_V_READ_PORT		LTC2309_CH2
//#define VBL1_V_READ_PORT		LTC2309_CH3
////#define AVDD_V_READ_PORT		LTC2309_CH4
////#define VGH_V_READ_PORT		LTC2309_CH5
////#define VGL_V_READ_PORT		LTC2309_CH6
//
////	Current
//#define C0VDD_I_READ_PORT		LTC2309_CH7
//#define C1VDD_I_READ_PORT		LTC2309_CH5
//#define C0VBL_I_READ_PORT		LTC2309_CH4
//#define C1VBL_I_READ_PORT		LTC2309_CH6
//#else
//	Voltage
//#define VDD_V_READ_PORT			ADC1_CH8

//	Voltage
#define VDD_V_READ_PORT			ADC1_CH3
#define VBL_V_READ_PORT			ADC1_CH1
#define VDD_V_READ_CH			ADC_CHANNEL_3
#define VBL_V_READ_CH			ADC_CHANNEL_1
//	Current
//#define CEXVBL_I_READ_PORT		ADC1_CH0
//#define C2VBL_I_READ_PORT		ADC1_CH1
//#define C1VBL_I_READ_PORT		ADC1_CH2
#define C0VDD_I_READ_PORT		ADC1_CH2
#define C0VBL_I_READ_PORT		ADC1_CH0

#define C0VDD_I_READ_CH			ADC_CHANNEL_2
#define C0VBL_I_READ_CH			ADC_CHANNEL_0
//#define CEXVDD_I_READ_PORT	ADC1_CH5
//#define C2VDD_I_READ_PORT		ADC1_CH6
//#define C1VDD_I_READ_PORT		ADC1_CH7
//#define C0VDD_I_READ_PORT		ADC1_CH17

////	Voltage
//#define VDD0_V_READ_PORT		ADC1_CH0
//#define VBL0_V_READ_PORT		ADC1_CH1
//#define VDD1_V_READ_PORT		ADC1_CH2
//#define VBL1_V_READ_PORT		ADC1_CH3
////#define AVDD_V_READ_PORT	LTC2309_CH4
////#define VGH_V_READ_PORT		LTC2309_CH5
////#define VGL_V_READ_PORT		LTC2309_CH6
//
////	Current
//#define C0VDD_I_READ_PORT		ADC1_CH7
//#define C1VDD_I_READ_PORT		ADC1_CH5
//#define C0VBL_I_READ_PORT		ADC1_CH4
//#define C1VBL_I_READ_PORT		ADC1_CH6
//#endif

//#define C0AVDD_I_READ_PORT	A1014_VMON06
//#define C1AVDD_I_READ_PORT	A1014_VMON05
//#define C0VGH_I_READ_PORT		A1014_VMON04
//#define C1VGH_I_READ_PORT		A1014_VMON03
//#define C0VGL_I_READ_PORT		A1014_VMON02
//#define C1VGL_I_READ_PORT		A1014_VMON01

//#define	CCH0V1_CAB_IDX		0

//#define	IBL_CAB_DUTY		75
//#define	IBL_CAB_FACTOR		33
//#define	IBL_MAX				200
//#define	IBL_MIN				5

//unsigned char	PowerScanChannel(unsigned char ch);
//unsigned char	PowerScanAllChannel();
//unsigned char	SetScanChannelVoltage_100mv(unsigned char ch, unsigned char v);
//unsigned char	SetVBL_10mv(unsigned char ch, unsigned short v);

#endif		//__PowerManager_H_
