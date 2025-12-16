#ifndef __WSS_EEPROM_2023_
#define __WSS_EEPROM_2023_
//#include "../I2C/I2C.h"
//#include <i2c.h>
#include "../I2C_Master/I2C_Master.h"
#include "../DelayUtility/DelayUtility.h"

#define	EEPROM_DEVICE_ADR000			0xA0	//0b1010A2A1A0x
#define EEPROM_DEVICE_ADR001			0xA2
#define	EEPROM_DEVICE_ADR010			0xA4

#define	EEPROM_TMP_DATA_SIZE			128		//	must >= max page size

#define EEPROM_PAGE_SIZE_128			128		//	24LC512
#define EEPROM_PAGE_SIZE_64				64		//	24LC256
#define EEPROM_PAGE_SIZE_32				32		//	24LC64

//	EEPROM instance
typedef struct {
	I2CM_IDX			I2C_idx;	//	I2C port to use
	unsigned char		sadr;		//	slaver address in 8-bit
	unsigned int		page_size;	//	page size
//	unsigned int		Wait_ms;	//	wait time for program data
}EEPROM;

//	public functions
int EEPROM_Init(EEPROM *epm, I2CM_IDX I2C_idx, unsigned char sadr, unsigned int page_size);
int EEPROM_Write_nB(EEPROM *epm, unsigned short adr, unsigned char *data, unsigned int n);
int EEPROM_Write_Page(EEPROM *epm, unsigned short adr, unsigned char *data);
int EEPROM_Read_nB(EEPROM *epm, unsigned short adr, unsigned char *data, unsigned int n);

#endif	//	__WSS_EEPROM_2023_

