#include "TouchButton.h"

int TOUCH_Init(TOUCH_BUTTON *inst, I2CM_IDX I2C_idx, unsigned char sadr){
	inst->I2C_idx = I2C_idx;
	inst->sadr = sadr;
	memset(inst->exsw_pressed,0,6);
	memset(inst->ExSW,0,6);
	inst->tcnt=0;
	return 0;
}

static int TOUCH_Write_2B(TOUCH_BUTTON *inst, unsigned char *data){
	int fail;
	fail = I2CM_Write(inst->I2C_idx, inst->sadr, data, 2);
	if(fail){
		return 1;		//	failed
	}else{
		return 0;		//	complete transfer without errors
	}
}

int TOUCH_Read_1B(TOUCH_BUTTON *inst, unsigned char *data){
	int fail;
	//	read out data
	fail = I2CM_Read(inst->I2C_idx, inst->sadr, data, 1);
	if(fail){
		return 1;		//	failed
	}else{
		return 0;		//	complete transfer without errors
	}
}

int TOUCH_Read_2B(TOUCH_BUTTON *inst, unsigned char *data){
		int fail;
	//	read out data
	fail = I2CM_Read(inst->I2C_idx, inst->sadr, data, 2);
	if(fail){
		return 1;		//	failed
	}else{
		return 0;		//	complete transfer without errors
	}
}

int TOUCH_LEDClearAll(TOUCH_BUTTON *inst){
	unsigned char	fail;
	inst->touch_LED = 0;
	fail=TOUCH_Write_2B(inst, &(inst->touch_LED));
	return fail;
}

int TOUCH_LEDSet(TOUCH_BUTTON *inst, unsigned short led, unsigned char s){
	int	fail;
	if(!s){					//	clear LED
		led = ~led;
		inst->touch_LED &= led;
	}else{					//	set LED
		inst->touch_LED |= led;
	}
	fail=TOUCH_Write_2B(inst,&(inst->touch_LED));
	return fail;
}

//void __attribute__((__interrupt__, auto_psv)) _INT4Interrupt(void){
//	unsigned char	rdata;
//	unsigned int	i;
//	unsigned char	fail,fail2;
//	unsigned char	wd[2];
//	unsigned char	str[20];
//	fail=TOUCH_Read_1B(&rdata);
//	sprintf(str,"%02X",rdata);
//	Lprint(2,str);
//	if(fail){
//		Lprint(3,"I2C R fail");
//	}else{
//		sprintf(str,"tcnt=%d",tcnt++);
//		Lprint(3,str);
//		wd[0] = ~rdata;
//		wd[1] = rdata;
//
//		fail2=TOUCH_Write_2B(wd);
//		if(fail2)	Lprint(4,"I2C W fail");
//
//	}
//
//	IFS3bits.INT4IF = 0; //Clear the INT1 interrupt flag or else
//}

void TOUCH_ClearButton(TOUCH_BUTTON *inst){
	memset(inst->ExSW, 0, TB_TOTAL);
}

int TOUCH_ButtonPolling(TOUCH_BUTTON *inst){
	unsigned char	rdata;
	unsigned int	i;
	unsigned char	fail;

	fail=TOUCH_Read_1B(inst, &rdata);
	if(fail)
		return 1;
	for(i=0;i<TB_TOTAL;i++){
		if(!(rdata&(0x01<<i))){		//	released
			if(inst->exsw_pressed[i]){
				inst->ExSW[i] = 1;
				inst->exsw_pressed[i]=0;
				break;
			}
		}else{						//	pressed
			if(inst->exsw_pressed[i] != 0x01)
				inst->exsw_pressed[i] = 1;
	//			exsw_pressed[i]++;
	//		if(sw_pressed[4] >= 20){
	//			SW[4] = 1;
	//			sw_pressed[4] = 20;
	//		}
		}
	}
	return 0;
}