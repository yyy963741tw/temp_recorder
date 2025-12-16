#ifndef __FONT_H
#define __FONT_H 	   

struct Cn32CharTypeDef
{
	unsigned char Index[2];
	unsigned char Msk[116];
};

// 使用 extern 關鍵字進行宣告
extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
extern const unsigned char asc2_3216[95][128];
extern struct Cn32CharTypeDef const CnChar32x29[];

#endif
