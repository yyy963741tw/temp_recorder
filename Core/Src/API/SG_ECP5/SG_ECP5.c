#include "SG_ECP5.h"

SPIM_IDX	SG_ECP5_SPI_IDX;

int			SG_ECP5_CS_IDX;

int SG_ECP5_Init(SPIM_IDX idx){
	SG_ECP5_SPI_IDX = idx;
	return 0;
}

void SG_ECP5_SelectCS(int cs_idx){
	SG_ECP5_CS_IDX = cs_idx;
}

void SG_ECP5_CS_Set(int v){
	switch(SG_ECP5_CS_IDX){
//		case 0:
//			SG_ECP5_CS0(v);
//			break;
//		case 1:
//			SG_ECP5_CS1(v);
//			break;
//		case 2:
//			SG_ECP5_CS2(v);
//			break;
//		case 3:
//			SG_ECP5_CSEX(v);
//			break;
//		case 0xfe:		//	broadcast without EX
//			SG_ECP5_CS0(v);
//			SG_ECP5_CS1(v);
//			SG_ECP5_CS2(v);
//			break;
		case 0xff:		//	broadcast
			SG_ECP5_CS0(v);
//			SG_ECP5_CS1(v);
//			SG_ECP5_CS2(v);
//			SG_ECP5_CSEX(v);
			break;
		default:
			break;
	}
}

void SG_ECP5_Enable(int en){
	if(en)		SG_ECP5_RST(1);//SG_ECP5_RST_OFF(SG_ECP5_RST);
	else		SG_ECP5_RST(0);
}

int SG_ECP5_Transfer(unsigned short cmd, unsigned short wd, unsigned long *rd){
	int fail;
	unsigned short td;
	SG_ECP5_CS_Set(0);
	fail  = SPIM_TransBlocking(SG_ECP5_SPI_IDX,cmd,&td);

	*rd = td;
	*rd <<= 16;
//	SG_ECP5_CS_Set(1);
//	SG_ECP5_CS_Set(0);
	fail |= SPIM_TransBlocking(SG_ECP5_SPI_IDX,wd,&td);
	SG_ECP5_CS_Set(1);
	*rd |= td;
	return fail;
}

int SG_ECP5_GetVersion(unsigned long *version){
	int	fail;
	fail  = SG_ECP5_Transfer(SG_ECP5_CMD_READ,SG_ECP5_RDATA_VERSION,version);
	fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ,0x0000,version);
	return fail;
}

int SG_ECP5_GetX_ADDR(unsigned long *adx){
	int	fail;
	fail  = SG_ECP5_Transfer(SG_ECP5_CMD_READ,SG_ECP5_RDATA_ADX,adx);
	fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ,0x0000,adx);
	return fail;
}

int SG_ECP5_SetFlicker_color(sFlicker_Color * sFlicker_Color0){
	int fail=0;
	unsigned long rd;
	//flicker_color

	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_00_LSB,sFlicker_Color0->color_flicker_color_00_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_00_MSB,sFlicker_Color0->color_flicker_color_00_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_01_LSB,sFlicker_Color0->color_flicker_color_01_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_01_MSB,sFlicker_Color0->color_flicker_color_01_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_02_LSB,sFlicker_Color0->color_flicker_color_02_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_02_MSB,sFlicker_Color0->color_flicker_color_02_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_03_LSB,sFlicker_Color0->color_flicker_color_03_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_03_MSB,sFlicker_Color0->color_flicker_color_03_MSB,&rd);

	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_10_LSB,sFlicker_Color0->color_flicker_color_10_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_10_MSB,sFlicker_Color0->color_flicker_color_10_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_11_LSB,sFlicker_Color0->color_flicker_color_11_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_11_MSB,sFlicker_Color0->color_flicker_color_11_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_12_LSB,sFlicker_Color0->color_flicker_color_12_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_12_MSB,sFlicker_Color0->color_flicker_color_12_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_13_LSB,sFlicker_Color0->color_flicker_color_13_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_13_MSB,sFlicker_Color0->color_flicker_color_13_MSB,&rd);

	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_20_LSB,sFlicker_Color0->color_flicker_color_20_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_20_MSB,sFlicker_Color0->color_flicker_color_20_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_21_LSB,sFlicker_Color0->color_flicker_color_21_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_21_MSB,sFlicker_Color0->color_flicker_color_21_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_22_LSB,sFlicker_Color0->color_flicker_color_22_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_22_MSB,sFlicker_Color0->color_flicker_color_22_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_23_LSB,sFlicker_Color0->color_flicker_color_23_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_23_MSB,sFlicker_Color0->color_flicker_color_23_MSB,&rd);

	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_30_LSB,sFlicker_Color0->color_flicker_color_30_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_30_MSB,sFlicker_Color0->color_flicker_color_30_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_31_LSB,sFlicker_Color0->color_flicker_color_31_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_31_MSB,sFlicker_Color0->color_flicker_color_31_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_32_LSB,sFlicker_Color0->color_flicker_color_32_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_32_MSB,sFlicker_Color0->color_flicker_color_32_MSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_33_LSB,sFlicker_Color0->color_flicker_color_33_LSB,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_flicker_color_33_MSB,sFlicker_Color0->color_flicker_color_33_MSB,&rd);



	return fail;
}

// sPic_Data sPic_Data0; //No change
void Pic_No_to_sPic_Data(int Pic_No,sPic_Data * sPic_Data0,unsigned char *param)
{
	unsigned long t32=0;
	unsigned short LSB=0;
	unsigned short MSB=0;


	switch(Pic_No){
	case P_Luster_R255:
		sPic_Data0->S_PG_PIC_LSB	=0x0000;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_color0_in_LSB=0x0000;
		// sPic_Data0->S_color0_in_MSB=0x3ff0;
		t32=param[1];
		t32<<=22;//10*2+2
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_color0_in_LSB=LSB;
		sPic_Data0->S_color0_in_MSB=MSB;
		break;
	case P_Luster_G255:
		sPic_Data0->S_PG_PIC_LSB	=0x0000;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_color0_in_LSB=0xfc00;
		// sPic_Data0->S_color0_in_MSB=0x000f;
		t32=param[2];
		t32<<=12;//10*1+2
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_color0_in_LSB=LSB;
		sPic_Data0->S_color0_in_MSB=MSB;
		break;
	case P_Luster_B255:
		sPic_Data0->S_PG_PIC_LSB	=0x0000;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_color0_in_LSB=0x03ff;
		// sPic_Data0->S_color0_in_MSB=0x0000;
		t32=param[3];
		t32<<=2;//10*0+2
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_color0_in_LSB=LSB;
		sPic_Data0->S_color0_in_MSB=MSB;
		break;
	case P_Luster_W0:
		// sPic_Data0->S_PG_PIC_LSB	=0x0000;
		// sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_color0_in_LSB=0x0000;
		// sPic_Data0->S_color0_in_MSB=0x0000;
		// break;
	case P_Luster_W255:
		// sPic_Data0->S_PG_PIC_LSB	=0x0000;
		// sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_color0_in_LSB=0xffff;
		// sPic_Data0->S_color0_in_MSB=0x3fff;
		// break;
	case P_Luster_W127       :
		// sPic_Data0->S_PG_PIC_LSB	=0x0000;
		// sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_color0_in_LSB=0xf1fc;
		// sPic_Data0->S_color0_in_MSB=0x1fc7;
		// break;
	case P_Luster_W92        :
		// sPic_Data0->S_PG_PIC_LSB	=0x0000;
		// sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_color0_in_LSB=0xC170;
		// sPic_Data0->S_color0_in_MSB=0x1705;
		// break;
	case P_Luster_W181       :
	case P_Gray_Level0		 :
	case P_Gray_Level1		 :
		sPic_Data0->S_PG_PIC_LSB	=0x0000;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_color0_in_LSB=0x52D4;
		// sPic_Data0->S_color0_in_MSB=0x2D4B;
		t32 = param[1];
		t32<<=10;
		t32 |= param[1];
		t32<<=10;
		t32 |= param[1];
		t32<<=2;
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_color0_in_LSB=LSB;
		sPic_Data0->S_color0_in_MSB=MSB;
		break;
	case P_V_Gray256         :
		// sPic_Data0->S_PG_PIC_LSB	=0x0004;
		// sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_v_number_in_LSB=0x0100;
		// sPic_Data0->S_v_number_in_MSB=0x0000;
		// break;
	case P_V_Gray64          :
		// sPic_Data0->S_PG_PIC_LSB	=0x0004;
		// sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_v_number_in_LSB=0x0040;
		// sPic_Data0->S_v_number_in_MSB=0x0000;
		// break;
	case P_V_Gray8           :
		sPic_Data0->S_PG_PIC_LSB	=0x0004;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_v_number_in_LSB=0x0008;
		// sPic_Data0->S_v_number_in_MSB=0x0000;
		t32 = param[1];
		if(t32 == 255)	t32++;
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_v_number_in_LSB=LSB;
		sPic_Data0->S_v_number_in_MSB=MSB;
		break;
	case P_H_Gray256   :
		// sPic_Data0->S_PG_PIC_LSB	=0x000f;
		// sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_h_number_in_LSB=0x0100;
		// sPic_Data0->S_h_number_in_MSB=0x0000;
		// break;
	case P_H_Gray64    :
		// sPic_Data0->S_PG_PIC_LSB	=0x000f;
		// sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_h_number_in_LSB=0x0040;
		// sPic_Data0->S_h_number_in_MSB=0x0000;
		// break;
	case P_H_Gray8           :
		sPic_Data0->S_PG_PIC_LSB	=0x000f;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_h_number_in_LSB=0x0008;
		// sPic_Data0->S_h_number_in_MSB=0x0000;
		t32 = param[1];
		if(t32 == 255)	t32++;
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_h_number_in_LSB=LSB;
		sPic_Data0->S_h_number_in_MSB=MSB;
		break;
	case P_V_BW              :
		sPic_Data0->S_PG_PIC_LSB	=0x0002;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_h_number_in_LSB=0x0002;
		sPic_Data0->S_h_number_in_MSB=0x0000;
		sPic_Data0->S_v_number_in_LSB=0x0001;
		sPic_Data0->S_v_number_in_MSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_LSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_MSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_01_LSB=0xffff;
		sPic_Data0->sFlicker_Color.color_flicker_color_01_MSB=0x3fff;
		break;
	case P_3C                :
		sPic_Data0->S_PG_PIC_LSB	=0x0017;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_color0_in_LSB=0x52D4;
		sPic_Data0->S_color0_in_MSB=0x2D4B;
		break;
	case P_B_Window          :
		sPic_Data0->S_PG_PIC_LSB	=0x001A;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_color0_in_LSB=0x0000;
		sPic_Data0->S_color0_in_MSB=0x0000;
		sPic_Data0->S_color1_in_LSB=0x52D4;
		sPic_Data0->S_color1_in_MSB=0x2D4B;
		break;
	case P_8C_8G             :
		sPic_Data0->S_PG_PIC_LSB	=0x0005;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
//		sPic_Data0->S_h_number_in_LSB=0x0008;
//		sPic_Data0->S_h_number_in_MSB=0x8000;
		t32 = param[1];
		if(t32 == 255)	t32++;
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_h_number_in_LSB=LSB;
		sPic_Data0->S_h_number_in_MSB=MSB|0x8000;
		sPic_Data0->S_h0_v0_MSB=0x0000;
		break;
	case P_8C_256G           :
		sPic_Data0->S_PG_PIC_LSB	=0x0005;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
//		sPic_Data0->S_h_number_in_LSB=0x0100;
//		sPic_Data0->S_h_number_in_MSB=0x0000;
		t32 = param[1];
		if(t32 == 255)	t32++;
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_h_number_in_LSB=LSB;
		sPic_Data0->S_h_number_in_MSB=MSB;
		sPic_Data0->S_h0_v0_MSB=0x0000;
		break;
	case P_Gray_Black_Flicker_0:
		sPic_Data0->S_PG_PIC_LSB	=0x0002;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_h_number_in_LSB=0x0001;
		sPic_Data0->S_h_number_in_MSB=0x0000;
		sPic_Data0->S_v_number_in_LSB=0x0002;
		sPic_Data0->S_v_number_in_MSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_LSB=0xffff;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_MSB=0x3fff;
		sPic_Data0->sFlicker_Color.color_flicker_color_10_LSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_10_MSB=0x0000;
		break;
	case P_Cyan              :
		sPic_Data0->S_PG_PIC_LSB	=0x0000;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		// sPic_Data0->S_color0_in_LSB=0x52D4;
		// sPic_Data0->S_color0_in_MSB=0x000B;
		t32 = 0;//param[1];//R
		t32<<=10;
		t32 |= param[2];//G
		t32<<=10;
		t32 |= param[3];//B
		t32<<=2;
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_color0_in_LSB=LSB;
		sPic_Data0->S_color0_in_MSB=MSB;

		break;
	case P_Cyan_Flicker      :
		sPic_Data0->S_PG_PIC_LSB	=0x0002;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_h_number_in_LSB=0x0002;
		sPic_Data0->S_h_number_in_MSB=0x0000;
		sPic_Data0->S_v_number_in_LSB=0x0004;
		sPic_Data0->S_v_number_in_MSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_LSB=0xffff;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_MSB=0x000f;
		sPic_Data0->sFlicker_Color.color_flicker_color_01_LSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_01_MSB=0x0000;
		
		sPic_Data0->sFlicker_Color.color_flicker_color_10_LSB=0xffff;
		sPic_Data0->sFlicker_Color.color_flicker_color_10_MSB=0x000f;
		sPic_Data0->sFlicker_Color.color_flicker_color_11_LSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_11_MSB=0x0000;
		
		sPic_Data0->sFlicker_Color.color_flicker_color_20_LSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_20_MSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_21_LSB=0xffff;
		sPic_Data0->sFlicker_Color.color_flicker_color_21_MSB=0x000f;
		
		sPic_Data0->sFlicker_Color.color_flicker_color_30_LSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_30_MSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_31_LSB=0xffff;
		sPic_Data0->sFlicker_Color.color_flicker_color_31_MSB=0x000f;
		
		break;
	case P_Check_Subpixel    :
		sPic_Data0->S_PG_PIC_LSB	=0x0002;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_h_number_in_LSB=0x0002;
		sPic_Data0->S_h_number_in_MSB=0x0000;
		sPic_Data0->S_v_number_in_LSB=0x0004;
		sPic_Data0->S_v_number_in_MSB=0x0000;

		sPic_Data0->sFlicker_Color.color_flicker_color_00_LSB=0x03ff;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_MSB=0x3ff0;
		sPic_Data0->sFlicker_Color.color_flicker_color_01_LSB=0xfc00;
		sPic_Data0->sFlicker_Color.color_flicker_color_01_MSB=0x000f;


		sPic_Data0->sFlicker_Color.color_flicker_color_10_LSB=0x03ff;
		sPic_Data0->sFlicker_Color.color_flicker_color_10_MSB=0x3ff0;
		sPic_Data0->sFlicker_Color.color_flicker_color_11_LSB=0xfc00;
		sPic_Data0->sFlicker_Color.color_flicker_color_11_MSB=0x000f;


		sPic_Data0->sFlicker_Color.color_flicker_color_20_LSB=0xfc00;
		sPic_Data0->sFlicker_Color.color_flicker_color_20_MSB=0x000f;
		sPic_Data0->sFlicker_Color.color_flicker_color_21_LSB=0x03ff;
		sPic_Data0->sFlicker_Color.color_flicker_color_21_MSB=0x3ff0;


		sPic_Data0->sFlicker_Color.color_flicker_color_30_LSB=0xfc00;
		sPic_Data0->sFlicker_Color.color_flicker_color_30_MSB=0x000f;
		sPic_Data0->sFlicker_Color.color_flicker_color_31_LSB=0x03ff;
		sPic_Data0->sFlicker_Color.color_flicker_color_31_MSB=0x3ff0;

		break;
	case P_H_Gomi1           :
		sPic_Data0->S_PG_PIC_LSB	=0x000E;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		break;
	case P_H_Gomi2           :
		sPic_Data0->S_PG_PIC_LSB	=0x000D;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		break;
	case P_Jiugongge         :
		sPic_Data0->S_PG_PIC_LSB	=0x0001;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		break;
	case P_1_4_3C            :
		sPic_Data0->S_PG_PIC_LSB	=0x0017;
		sPic_Data0->S_PG_PIC_MSB	=0x8000;
		sPic_Data0->S_color0_in_LSB=0x52D4;
		sPic_Data0->S_color0_in_MSB=0x2D4B;
		break;
	case P_1_4_H_Gray64      :
		sPic_Data0->S_PG_PIC_LSB	=0x000f;
		sPic_Data0->S_PG_PIC_MSB	=0x8000;
		sPic_Data0->S_h_number_in_LSB=0x0040;
		sPic_Data0->S_h_number_in_MSB=0x0000;
		break;
	case P_1_4_V_Gray64      :
		sPic_Data0->S_PG_PIC_LSB	=0x0004;
		sPic_Data0->S_PG_PIC_MSB	=0x8000;
		sPic_Data0->S_v_number_in_LSB=0x0040;
		sPic_Data0->S_v_number_in_MSB=0x0000;
		break;
	case P_chess             :
		sPic_Data0->S_PG_PIC_LSB	=0x001b;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_color0_in_LSB=0xffff;
		sPic_Data0->S_color0_in_MSB=0x3fff;
		sPic_Data0->S_color1_in_LSB=0x0000;
		sPic_Data0->S_color1_in_MSB=0x0000;
		break;
	case P_4c                :
		sPic_Data0->S_PG_PIC_LSB	=0x0010;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_color0_in_LSB=0x52D4;
		sPic_Data0->S_color0_in_MSB=0x2D4B;
		break;
	case P_frame             :
		sPic_Data0->S_PG_PIC_LSB	=0x0019;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		break;
	case P_1_4_4c            :
		sPic_Data0->S_PG_PIC_LSB	=0x0010;
		sPic_Data0->S_PG_PIC_MSB	=0x8000;
		sPic_Data0->S_color0_in_LSB=0x52D4;
		sPic_Data0->S_color0_in_MSB=0x2D4B;
		break;
	case P_8G_8G             :
		sPic_Data0->S_PG_PIC_LSB	=0x000C;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		t32 = param[1];
		if(t32 == 255)	t32++;
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_h_number_in_LSB=LSB;
		sPic_Data0->S_h_number_in_MSB=MSB;
		sPic_Data0->S_h0_v0_MSB=0x0000;
		break;
	case P_B_windows         :
		sPic_Data0->S_PG_PIC_LSB	=0x000A;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_color0_in_LSB=0x0000;
		sPic_Data0->S_color0_in_MSB=0x0000;
		sPic_Data0->S_color1_in_LSB=0x0100;
		sPic_Data0->S_color1_in_MSB=0x1004;
		break;
	case P_Stress_OP         :
		sPic_Data0->S_PG_PIC_LSB	=0x000B;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		break;
	case P_Column            :
		sPic_Data0->S_PG_PIC_LSB	=0x0002;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_h_number_in_LSB=0x0002;
		sPic_Data0->S_h_number_in_MSB=0x0000;
		sPic_Data0->S_v_number_in_LSB=0x0001;
		sPic_Data0->S_v_number_in_MSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_LSB=0xfc00;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_MSB=0x000f;
		sPic_Data0->sFlicker_Color.color_flicker_color_01_LSB=0x03ff;
		sPic_Data0->sFlicker_Color.color_flicker_color_01_MSB=0x3ff0;
		break;
	case P_8G_8C			 :
		sPic_Data0->S_PG_PIC_LSB	=0x0009;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_h_number_in_LSB=0x0008;
		sPic_Data0->S_h_number_in_MSB=0x0000;
		sPic_Data0->S_h0_v0_MSB=0x0000;
		break;
	case P_Gray_Black_Flicker_1:
		sPic_Data0->S_PG_PIC_LSB	=0x0002;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		sPic_Data0->S_h_number_in_LSB=0x0002;
		sPic_Data0->S_h_number_in_MSB=0x0000;
		sPic_Data0->S_v_number_in_LSB=0x0002;
		sPic_Data0->S_v_number_in_MSB=0x0000;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_LSB=0xfc00;
		sPic_Data0->sFlicker_Color.color_flicker_color_00_MSB=0x000f;
		sPic_Data0->sFlicker_Color.color_flicker_color_01_LSB=0x03ff;
		sPic_Data0->sFlicker_Color.color_flicker_color_01_MSB=0x3ff0;
		sPic_Data0->sFlicker_Color.color_flicker_color_10_LSB=0x03ff;
		sPic_Data0->sFlicker_Color.color_flicker_color_10_MSB=0x3ff0;
		sPic_Data0->sFlicker_Color.color_flicker_color_11_LSB=0xfc00;
		sPic_Data0->sFlicker_Color.color_flicker_color_11_MSB=0x000f;
		break;
	case P_custom_RGB:
		sPic_Data0->S_PG_PIC_LSB	=0x0000;
		sPic_Data0->S_PG_PIC_MSB	=0x0000;
		t32 = param[1];//R
		t32<<=10;
		t32 |= param[2];//G
		t32<<=10;
		t32 |= param[3];//B
		t32<<=2;
		LSB=t32&0xffff;
		MSB=t32>>16;
		sPic_Data0->S_color0_in_LSB=LSB;
		sPic_Data0->S_color0_in_MSB=MSB;
		break;
	default:
		break;
	}

}

//	TOP Call function

int SG_ECP5_SetTiming(sPG_Timing *inst){
	int fail=0;
	unsigned long rd;
	unsigned short htotal=inst->H	+
	           inst->HBP	+
	           inst->HFP	+
	           inst->HSW	;
	
	unsigned short vtotal=inst->V	+
	           inst->VBP	+
	           inst->VFP	+
	           inst->VSW	;
	
	fail |= SG_ECP5_Transfer(SG_ECP5_Panel_Vtotal		 ,vtotal,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_Panel_Vbp           ,inst->VBP,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_Panel_Vsync         ,inst->VSW,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_Panel_Vactive       ,inst->V,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_Panel_Htotal        ,htotal>>inst->PPC,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_Panel_Hbp           ,inst->HBP>>inst->PPC,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_Panel_Hsync         ,inst->HSW>>inst->PPC,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_Panel_Hactive       ,inst->H>>inst->PPC,&rd);

	return fail;
}
int SG_ECP5_SetAttr(sPG_Timing *inst){
	unsigned long rd;
	return SG_ECP5_Transfer(SG_ECP5_Panel_ControlSetting,inst->ControlSetting,&rd);
}
int SG_ECP5_GPIO_Ctrl(unsigned short gpio){
	unsigned long rd;
	return SG_ECP5_Transfer(SG_ECP5_CMD_GPIO,gpio,&rd);
}

int SG_ECP5_PatternChange(unsigned char *param){
	int fail=0;
	unsigned long rd;
//	int Pic_No;

	sPic_Data sPic_Data0;
	Pic_No_to_sPic_Data(param[0],&sPic_Data0,param);


	fail |= SG_ECP5_Transfer(SG_ECP5_PIC_Update			,PIC_Update_Stop,&rd);

	fail |= SG_ECP5_Transfer(SG_ECP5_PG_PIC_LSB			,sPic_Data0.S_PG_PIC_LSB		,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_PG_PIC_MSB			,sPic_Data0.S_PG_PIC_MSB		,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_h_number_in_LSB	,sPic_Data0.S_h_number_in_LSB	,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_h_number_in_MSB	,sPic_Data0.S_h_number_in_MSB	,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_v_number_in_LSB	,sPic_Data0.S_v_number_in_LSB	,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_v_number_in_MSB	,sPic_Data0.S_v_number_in_MSB	,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_h0_v0_LSB			,sPic_Data0.S_h0_v0_LSB			,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_h0_v0_MSB			,sPic_Data0.S_h0_v0_MSB			,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_h1_v1_LSB			,sPic_Data0.S_h1_v1_LSB			,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_h1_v1_MSB			,sPic_Data0.S_h1_v1_MSB			,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_color0_in_LSB		,sPic_Data0.S_color0_in_LSB		,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_color0_in_MSB		,sPic_Data0.S_color0_in_MSB		,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_color1_in_LSB		,sPic_Data0.S_color1_in_LSB		,&rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_color1_in_MSB		,sPic_Data0.S_color1_in_MSB		,&rd);

	int Set_flicker_color=0;
	switch(param[0]){
	case P_V_BW:
	case P_Gray_Black_Flicker_0:
	case P_Cyan_Flicker:
	case P_Check_Subpixel:
	case P_Gray_Black_Flicker_1:
	case P_Column:
		Set_flicker_color=1;
		break;
	default:
		Set_flicker_color=0;
		break;
	}

	//flicker_color
	if (Set_flicker_color)
		fail |=SG_ECP5_SetFlicker_color(&(sPic_Data0.sFlicker_Color));

	fail |= SG_ECP5_Transfer(SG_ECP5_PIC_Update			,PIC_Update_En,&rd);

	return fail;
}

//20240131 NEW
int SG_ECP5_SetOXP_Time(sPower_Data *inst){
	unsigned long rd;
	int fail;
	
	fail	 = SG_ECP5_Transfer(Power_OCP_Time,inst->OCP_Time,&rd);
	fail	|= SG_ECP5_Transfer(Power_OVP_Time,inst->OVP_Time,&rd);
	fail	|= SG_ECP5_Transfer(Power_OVP_Time,inst->Update,&rd);

	return fail;
}

void SG_ECP5_SetV_Start(sPower_Data *inst){
//	unsigned long rd;
//	return SG_ECP5_Transfer(Power_V_Start,inst->V_Start,&rd);
	if(inst->V_Start&CH0_VDD_Start) VDD_VOUT_EN(1);
	else VDD_VOUT_EN(0);
	if(inst->V_Start&CH0_VBL_Start) VBL_VOUT_EN(1);
	else VBL_VOUT_EN(0);
}

int SG_ECP5_SetACH_OFF(sPower_Data *inst){
	unsigned long rd;
	return SG_ECP5_Transfer(Power_ACH_OFF,inst->ACH_OFF,&rd);
}

int SG_ECP5_Get_OXPStatus(sPower_Data *inst){
	int	fail;
	fail  = SG_ECP5_Transfer(SG_ECP5_CMD_READ,SG_ECP5_RDATA_OCP,&(inst->OCP_Status));
	fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ,0x0000,&(inst->OCP_Status));
	fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ,SG_ECP5_RDATA_OVP,&(inst->OVP_Status));
	fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ,0x0000,&(inst->OVP_Status));
	return fail;
}

int SG_ECP5_Get_CheckSum(unsigned long *CheckSum){
	int	fail;
	fail  = SG_ECP5_Transfer(SG_ECP5_CMD_READ,SG_ECP5_RDATA_CHECKSUM,CheckSum);
	fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ,0x0000,CheckSum);
	return fail;
}

int SG_ECP5_Read(unsigned short adr, unsigned long *rd){
	int	fail;
	fail  = SG_ECP5_Transfer(SG_ECP5_CMD_READ,adr,rd);
	fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ,0x0000,rd);
	return fail;
}

int SG_ECP5_OXP_Reset(){
	unsigned long rd;
	return SG_ECP5_Transfer(Power_OXP_Reset,0x0001,&rd);
}
