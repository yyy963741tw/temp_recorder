//#include "../comm.h"
//
//#ifdef FCY_16M
//#define	 FCY    16000000UL
//#else
//#define	 FCY    8000000UL    // Instruction cycle frequency, Hz - required for __delayXXX() to work
//#endif
//#include <libpic30.h>        // __delayXXX() functions macros defined here
//#define delayLoop 			1900
//#define delayLoop2 			18

#define Delay(x)			HAL_Delay(x);
#define DelayuSec(x)		HAL_DelayUS(x);

//inline void DelaymSec(unsigned int uiMillisecond);
//inline void Delay10uSec(unsigned int uiMicrosecond);
