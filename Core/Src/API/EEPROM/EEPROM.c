#include "EEPROM.h"

int EEPROM_Init(EEPROM *epm, I2CM_IDX I2C_idx, unsigned char sadr, unsigned int page_size){
	epm->I2C_idx = I2C_idx;
	epm->sadr = sadr;
	epm->page_size = page_size;
	return 0;
}

//	write nB "MUST" within a page, or it will fail
int EEPROM_Write_nB(EEPROM *epm, unsigned short adr, unsigned char *data, unsigned int n){
	unsigned char da[EEPROM_TMP_DATA_SIZE+2];
	int fail;
	int	t16;
	//	check if within page
	t16  = adr % epm->page_size;
	t16 += n;
	if(t16 > epm->page_size)
		return 2;	//	more than a page
	da[0] = adr >> 8;
	da[1] = adr & 0x00FF;
	memcpy(da+2,data,n);
	fail = I2CM_Write(epm->I2C_idx, epm->sadr, da,n+2);
	if(fail){
		return 1;		//	fail
	}else{
		Delay(5);		//	wait for EEPROM program data
		return 0;		//	complete transfer without errors
	}
}

//	write page "MUST" match page start address, or it will fail
int EEPROM_Write_Page(EEPROM *epm, unsigned short adr, unsigned char *data){
	unsigned char da[EEPROM_TMP_DATA_SIZE+2];
	int fail;
	//	check if match page start address
	if(adr % epm->page_size)
		return 2;	//	not page start address
	da[0] = adr >> 8;
	da[1] = adr & 0x00FF;
	memcpy(da+2,data,epm->page_size);
	fail = I2CM_Write(epm->I2C_idx, epm->sadr, da, epm->page_size+2);
	if(fail){
		return 1;		//	failed
	}else{
		Delay(5);		//	wait for EEPROM program data
		return 0;		//	complete transfer without errors
	}
}

int EEPROM_Read_nB(EEPROM *epm, unsigned short adr, unsigned char *data, unsigned int n){
	unsigned char da[2];
	unsigned char fail;
	//	set address to read
	da[0] = adr >> 8;
	da[1] = adr & 0x00FF;
	fail = I2CM_Write(epm->I2C_idx, epm->sadr, da, 2);
	if(fail){
		return 1;		//	failed
	}
	//	read out data
	fail = I2CM_Read(epm->I2C_idx, epm->sadr, data, n);
	if(fail){
		return 1;		//	failed
	}else{
		return 0;		//	complete transfer without errors
	}
}
