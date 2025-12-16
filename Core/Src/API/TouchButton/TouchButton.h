#ifndef __TOUCH_H_2023_
#define __TOUCH_H_2023_
#include "../Inc/comm.h"
#include "../I2C_Master/I2C_Master.h"
//	I2C slave address
#define	TOUCH_DEVICE_ADR		0x90		//	fixed, cant change

//	Touch LEDs
#define	TLED_FN					0x0001
#define	TLED_GRAY				0x0002
#define	TLED_AUTO				0x0004
#define	TLED_DOWN				0x0008
#define	TLED_UP					0x0010
#define	TLED_ONOFF				0x0020

#define	TLED_F					0x0100
#define	TLED_E					0x0200
#define	TLED_D					0x0400
#define	TLED_C					0x0800
//	***	change because of orignal code makes an error mapping
#define	TLED_B					0x2000
#define	TLED_A					0x1000

//	Touch buttons
typedef enum {
	TB_FN_IDX = 0,
	TB_GRAY_IDX,
	TB_AUTO_IDX,
	TB_DOWN_IDX,
	TB_UP_IDX,
	TB_ONOFF_IDX,
	TB_TOTAL
}TOUCH_BUTTON_IDX;

// #define	TBUTTON_FN				0
// #define	TBUTTON_GRAY			1
// #define	TBUTTON_AUTO			2
// #define	TBUTTON_UP				3
// #define	TBUTTON_DOWN			4
// #define	TBUTTON_ONOFF			5

//	TouchButton instance
typedef struct {
	I2CM_IDX			I2C_idx;	//	I2C port to use
	unsigned char		sadr;		//	slaver address in 8-bit
	unsigned int		page_size;	//	page size
//	unsigned int		Wait_ms;	//	wait time for program data
	unsigned char		exsw_pressed[TB_TOTAL];
	unsigned char		ExSW[TB_TOTAL];
	unsigned short		touch_LED;
	unsigned int		tcnt;
	int					INT_trig;
}TOUCH_BUTTON;

int TOUCH_Init(TOUCH_BUTTON *inst, I2CM_IDX I2C_idx, unsigned char sadr);
int TOUCH_Read_1B(TOUCH_BUTTON *inst, unsigned char *data);
void TOUCH_ClearButton(TOUCH_BUTTON *inst);
int TOUCH_ButtonPolling(TOUCH_BUTTON *inst);
#endif		//__TOUCH_H_2023_
