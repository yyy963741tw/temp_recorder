//	Hardware related define
#ifndef __COMM_H_
#define __COMM_H_
#include "main.h"
#include <string.h>
#include <stdio.h>

//#include <pic24xxx.h>
//#include <p24FJ128GA006.h>
//#include "I2C_Slave/I2C_Slave_Lite.h"
//#include "DelayUtility/DelayUtility.h"
//#include "ControlReg.h"
//#include "stm32g0xx_hal_gpio.h"
#define NOP()				asm("nop")
#define nop					asm("nop")

#define GPIODIR(GPIOType, pin, dir) {GPIOType->MODER&=~(3<<pin*2);  GPIOType->MODER |= dir<<pin*2;}

//	IO Pins

//	board address (input)
//#define ADX_ADR						0x44 //0100010x w:x=0 R:x=1
#define ADX8						AD0_GPIO_Port->IDR & AD0_Pin//?1:0
#define ADX4						AD1_GPIO_Port->IDR & AD1_Pin
#define ADX2						AD2_GPIO_Port->IDR & AD2_Pin//?1:0
#define ADX1						AD3_GPIO_Port->IDR & AD3_Pin//?1:0

//#define ADX1						GPIOA->IDR & GPIO_PIN_5?1:0
//#define ADX2						GPIOA->IDR & GPIO_PIN_5?1:0
//#define ADX4						GPIOA->IDR & GPIO_PIN_5?1:0
////#define ADX8					_RB8
//#define ADY1						GPIOA->IDR & GPIO_PIN_5?1:0
//#define ADY2						GPIOA->IDR & GPIO_PIN_5?1:0
//#define ADY4						GPIOA->IDR & GPIO_PIN_5?1:0
////#define ADY8					_RB12
//
//#define ADX1_TRIS(dir)			GPIODIR(GPIOA, 5, !dir)
//#define ADX2_TRIS(dir)			GPIODIR(GPIOA, 5, !dir)
//#define ADX4_TRIS(dir)			GPIODIR(GPIOA, 5, !dir)
////#define ADX8_TRIS				_TRISB8
//#define ADY1_TRIS(dir)			GPIODIR(GPIOA, 5, !dir)
//#define ADY2_TRIS(dir)			GPIODIR(GPIOA, 5, !dir)
//#define ADY4_TRIS(dir)			GPIODIR(GPIOA, 5, !dir)
////#define ADY8_TRIS				_TRISB12

//	LCM related
//#define SPI_SCK_LCD(x)				HAL_GPIO_WritePin(SPI_SCK_LCD_GPIO_Port, SPI_SCK_LCD_Pin, x)
//#define SPI_CS_LCD(x)				HAL_GPIO_WritePin(SPI_CS_LCD_GPIO_Port, SPI_CS_LCD_Pin, x)
//#define SPI_DI_LCD(x)				HAL_GPIO_WritePin(LCD_SI_GPIO_Port, LCD_SI_Pin, x)
//
//#define SPI_SCK_LCD_TRIS(dir)		GPIODIR(SPI_SCK_LCD_GPIO_Port, 5, !dir)
//#define SPI_CS_LCD_TRIS(dir)		GPIODIR(SPI_CS_LCD_GPIO_Port, 0, !dir)
//#define SPI_DI_LCD_TRIS(dir)		GPIODIR(LCD_SI_GPIO_Port, 1, !dir)

//#define SPI_SCK_LCD_R				SPI_SCK_LCD_GPIO_Port->IDR & SPI_SCK_LCD_Pin//?1:0
//#define SPI_CS_LCD_R				SPI_CS_LCD_GPIO_Port->IDR & SPI_CS_LCD_Pin//?1:0
//#define SPI_DI_LCD_R				LCD_SI_GPIO_Port->IDR & LCD_SI_Pin//?1:0

#define VADJ						V_ADJ_A_GPIO_Port->IDR & V_ADJ_A_Pin //?1:0
#define VADJ_TRIS(dir)				GPIODIR(V_ADJ_A_GPIO_Port, 6, !dir)

//	LED
#define LED1(x)						HAL_GPIO_WritePin(M_LED0_GPIO_Port, M_LED0_Pin, !x)
#define LED2(x)						HAL_GPIO_WritePin(M_LED1_GPIO_Port, M_LED1_Pin, !x)
#define LED3(x)						HAL_GPIO_WritePin(M_LED2_GPIO_Port, M_LED2_Pin, !x)
#define LED4(x)						HAL_GPIO_WritePin(M_LED3_GPIO_Port, M_LED3_Pin, !x)

#define LED1Toggle					HAL_GPIO_TogglePin(M_LED0_GPIO_Port, M_LED0_Pin);
#define LED2Toggle					HAL_GPIO_TogglePin(M_LED1_GPIO_Port, M_LED1_Pin);
#define LED3Toggle					HAL_GPIO_TogglePin(M_LED2_GPIO_Port, M_LED2_Pin);
#define LED4Toggle					HAL_GPIO_TogglePin(M_LED3_GPIO_Port, M_LED3_Pin);


//DEBUG
//#define DEBUG_F(x)					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, x)//x?(GPIOB->BSRR = GPIO_PIN_1):(GPIOB->BRR = GPIO_PIN_1)
//#define DEBUG_F_RO					GPIOB->ODR & GPIO_PIN_1
//#define DEBUG_F_Toggle				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);

//	power enable (7VM)
//#define	POWER_EN(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, x)

//	power enable (fpga)
//#define	VDD_VOUT_EN(x)				HAL_GPIO_WritePin(VDD_OUT_EN_GPIO_Port, VDD_OUT_EN_Pin, x)
//#define	VBL_VOUT_EN(x)				HAL_GPIO_WritePin(VBL_OUT_EN_GPIO_Port, VBL_OUT_EN_Pin, x)

//	power enable (LM5145)
#define	VDD_EN(x)					HAL_GPIO_WritePin(VDD_PEN_GPIO_Port, VDD_PEN_Pin, x)
#define	VBL_EN(x)					HAL_GPIO_WritePin(VBL_PEN_GPIO_Port, VBL_PEN_Pin, x)

//	power enable (12V)
#define	V12_EN(x)					HAL_GPIO_WritePin(EXT12_PEN_GPIO_Port, EXT12_PEN_Pin, x)
#define	V12_EN_TRIS(dir)			GPIODIR(EXT12_PEN_GPIO_Port, 5, !dir)
#define V12_EN_R					EXT12_PEN_GPIO_Port->IDR & EXT12_PEN_Pin//?1:0
//	fault flag (LM5145)
#define VBL_PGOOD					PGOOD_OC_GPIO_Port->IDR & PGOOD_OC_Pin //?1:0

//	fault flag (LTC7003)
#define VBL_FL						VOUT12_FL_GPIO_Port->IDR & VOUT12_FL_Pin //?1:0

//	fault flag (12V)
#define V12_FL						VOUT12_FL_GPIO_Port->IDR & VOUT12_FL_Pin //?1:0
//#define	PD_EN(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, x)
//#define	EXP_EN(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, x)
//#define	PC14_EN(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, x)
//#define	PC15_EN(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, x)

//#define OCP_IO_R					HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)
//#define OVP_IO_R					HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5)

//#define OCP_TRIG_PIN				OCP_MI_IO_Pin
//#define OVP_TRIG_PIN				OVP_MV_IO_Pin
#define TB_TRIG_PIN					EXT_INT_Pin
//	power turn on/off pin (replaced by SPI command in this version)
//#define START_VDD					_LATE4
//#define START_VBL					_LATE2
//#define START_AVDD					_LATE0
//#define START_VGH					_LATF0
//#define START_VGL					_LATD6

//	ACH off (disable channel) (replaced by SPI command in this version)
//#define ACHOFF0					_LATE1
//#define ACHOFF1					_LATE3

//	FPGA (XO1200) (merge into ECP5 in this version)
//#define FPGA_XO2_SPI_CS			_LATC12
//#define FPGA_XO2_RESET			_LATF1
//#define FPGA_XO2_SP0			_LATB0
//#define FPGA_XO2_SP1			_LATD7
//#define FPGA_XO2_SP2			_LATD5
//#define FPGA_XO2_SP3			_LATD1

#define LTC298X_RESET(x)			HAL_GPIO_WritePin(FPGA_RST_GPIO_Port, FPGA_RST_Pin, x)
//#define	FPGA_S_EN				_LATD4		//	signal enable (no use in this version, use command)
#define	LTC298X_CS1(x)				HAL_GPIO_WritePin(LTC2983_CS1_GPIO_Port, LTC2983_CS1_Pin, x)		//	there are 2 ECP5 in this case
#define	LTC298X_CS2(x)				HAL_GPIO_WritePin(LTC2983_CS2_GPIO_Port, LTC2983_CS2_Pin, x)		//	there are 2 ECP5 in this case
#define	LTC298X_CS3(x)				HAL_GPIO_WritePin(LTC2983_CS3_GPIO_Port, LTC2983_CS3_Pin, x)		//	there are 2 ECP5 in this case
//#define	LTC298X_CS4(x)				HAL_GPIO_WritePin(LTC2983_CS4_GPIO_Port, LTC2983_CS4_Pin, x)		//	there are 2 ECP5 in this case
//#define	LTC298X_SDO4(x)				HAL_GPIO_WritePin(LTC2983_SDO4_GPIO_Port, LTC2983_SDO4_Pin, x)		//	there are 2 ECP5 in this case


//	FPGA (ECP5)
#define FPGA_RESET(x)				HAL_GPIO_WritePin(FPGA_RST_GPIO_Port, FPGA_RST_Pin, x)
//#define	FPGA_S_EN				_LATD4		//	signal enable (no use in this version, use command)
#define	FPGA_SPI_CS0(x)				HAL_GPIO_WritePin(FPGA_SPI_CS_GPIO_Port, FPGA_SPI_CS_Pin, x)		//	there are 2 ECP5 in this case
//#define	FPGA_SPI_CS1(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//
//#define FPGA_SP0_0(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//#define FPGA_SP0_1(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//#define FPGA_SP0_2(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//#define FPGA_SP1_0(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//#define FPGA_SP1_1(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//#define FPGA_SP1_2(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//
//#define TRIS_FPGA_SP0_0(dir)		GPIODIR(GPIOA, 5, !dir)
//#define TRIS_FPGA_SP0_1(dir)		GPIODIR(GPIOA, 5, !dir)
//#define TRIS_FPGA_SP0_2(dir)		GPIODIR(GPIOA, 5, !dir)
//#define TRIS_FPGA_SP1_0(dir)		GPIODIR(GPIOA, 5, !dir)
//#define TRIS_FPGA_SP1_1(dir)		GPIODIR(GPIOA, 5, !dir)
//#define TRIS_FPGA_SP1_2(dir)		GPIODIR(GPIOA, 5, !dir)
//
//#define R_FPGA_SP0_0				GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_FPGA_SP0_1				GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_FPGA_SP0_2				GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_FPGA_SP1_0				GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_FPGA_SP1_1				GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_FPGA_SP1_2				GPIOA->IDR & GPIO_PIN_5?1:0
//
////	panel SPI&IIC control by GPIO
//#define	PANEL_RST0(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)		//	panel reset pin (PICO-RSTx)
//#define	PANEL_RST1(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//
//#define	IO_SPI_CS0(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)		//	(PICO-SPI-CS0)
//#define	IO_SPI_CS1(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)		//	(PICO-SPI-CS1)
//#define IO_SCL_SPI_SCK0(x)			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)		//	(PICO-SCL_SPI_SLK0)
//#define IO_SCL_SPI_SCK1(x)			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)		//	(PICO-SCL_SPI_SLK1)
//#define IO_SDA_SPI_MISO0(x)			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)		//	(PICO-SDA_SPI_MISO0)
//#define IO_SDA_SPI_MISO1(x)			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)		//	(PICO-SDA_SPI_MISO1)
//#define IO_SPI_MOSI0(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)		//	(PICO-SPI-MOSI0)
//#define IO_SPI_MOSI1(x)				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)		//	(PICO-SPI-MOSI1)
//
//#define	TRIS_IO_SPI_CS0(dir)		GPIODIR(GPIOA, 5, !dir)
//#define	TRIS_IO_SPI_CS1(dir)		GPIODIR(GPIOA, 5, !dir)
//#define TRIS_IO_SCL_SPI_SCK0(dir)	GPIODIR(GPIOA, 5, !dir)
//#define TRIS_IO_SCL_SPI_SCK1(dir)	GPIODIR(GPIOA, 5, !dir)
//#define TRIS_IO_SDA_SPI_MISO0(dir)	GPIODIR(GPIOA, 5, !dir)
//#define TRIS_IO_SDA_SPI_MISO1(dir)	GPIODIR(GPIOA, 5, !dir)
//#define TRIS_IO_SPI_MOSI0(dir)		GPIODIR(GPIOA, 5, !dir)
//#define TRIS_IO_SPI_MOSI1(dir)		GPIODIR(GPIOA, 5, !dir)
//
//#define	R_IO_SPI_CS0				GPIOA->IDR & GPIO_PIN_5?1:0
//#define	R_IO_SPI_CS1				GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_IO_SCL_SPI_SCK0			GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_IO_SCL_SPI_SCK1			GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_IO_SDA_SPI_MISO0			GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_IO_SDA_SPI_MISO1			GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_IO_SPI_MOSI0				GPIOA->IDR & GPIO_PIN_5?1:0
//#define R_IO_SPI_MOSI1				GPIOA->IDR & GPIO_PIN_5?1:0

////	IO IIC for PLL (CY22150)
//#define	TRIS_PLL_SDA0(dir)			GPIODIR(GPIOA, 5, !dir)
//#define	TRIS_PLL_SCL0(dir)			GPIODIR(GPIOA, 5, !dir)
//#define	TRIS_PLL_SDA1(dir)			GPIODIR(GPIOA, 5, !dir)
//#define	TRIS_PLL_SCL1(dir)			GPIODIR(GPIOA, 5, !dir)
//
//#define	PLL_SDA0(x)					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//#define	PLL_SCL0(x)					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//#define	PLL_SDA1(x)					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//#define	PLL_SCL1(x)					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//
//#define	R_PLL_SDA0					GPIOA->IDR & GPIO_PIN_5?1:0
//#define	R_PLL_SDA1					GPIOA->IDR & GPIO_PIN_5?1:0

////	UART1 GPIO for switch port
//#define U1_GPIO(x)					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, x)
//
////	UART2 GPIO for RS485 direction control
#define U_RXD(x)					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, x)
//	Operating modes
typedef enum{
	MODE_NORMAL=0,
	MODE_TEST,
	MODE_VCAB,
	MODE_VSCAN,
	MODE_ICAB,
	MODE_XVP_TEST,
	MODE_BADDR_TEST,
	MODE_SET_I2CS_ADDR_TEST,
	MODE_SHOW_INFO,
//	MODE_ADC_TEST,
//	MODE_VBL_ERROR,
//	MODE_DEBUG,
//	MODE_BIST,
	MODE_TOTAL
} MODE;

//	quick control macros
//#define CV_ON(x)				x = 1
//#define CV_OFF(x)				x = 0

////	FPGA reset control
//#define FPGA_RST_ON(x)			x = 0
//#define FPGA_RST_OFF(x)			x = 1

//	LED control
//#define LED_ON(x)				x = 0
//#define LED_OFF(x)				x = 1

//	slave I2C control ram address


// #define ENTER_CRITICAL()	//INTDisableInterrupts()
// #define EXIT_CRITICAL()		//INTEnableInterrupts()

#endif
