#define MCU_VERSION_STR		"V2.01.01"
#define MCU_VERSION_DATE	"2025.05.13"

#define SYS_TOTAL_X			3
#define SYS_TOTAL_Y			7

//#define CJ_DEBUG	//	mark if not debug

//	config data for debug
const unsigned char DEBUG_CONFIG_DATA[]={
		//	Power config
		0x96,0x00,	//	VBL0		150
		0x9B,0x00,	//	VBL0 OVP	155
		0x91,0x00,	//	VBL0 UVP	145
		0xD0,0x07,	//	VBL0 OCP	2000
		//	power on delay
		0xE8,0x03,  //	VBL0 On		1000
		//	power off delay
		0x00,0x00,  //	VBL0 Off	0
		//	others
		0x10,0x00,	//	OCP delay time	16
};


