#include "ltdc_ex.h"
#include "ltdc.h"
#include "tftlcd.h"
#include "dma2d.h"
#include "core_cm7.h"

//LTDC_HandleTypeDef  LTDC_Handler;	    //LTDC���
//DMA2D_HandleTypeDef DMA2D_Handler; 	    //DMA2D���


//���ݲ�ͬ����ɫ��ʽ,����֡��������
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	u32 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));	//����������ֱ���ʱ,LCD�����֡���������С
#else
	u16 ltdc_lcd_framebuf[1280][800] __attribute__((section(".lcd_framebuf")));	//����������ֱ���ʱ,LCD�����֡���������С
#endif

u32 *ltdc_framebuf[2];					//LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ�����
_ltdc_dev lcdltdc;						//����LCD LTDC����Ҫ����


//��LCD����
//lcd_switch:1 ��,0���ر�
void LTDC_Switch(u8 sw)
{
	if(sw==1) __HAL_LTDC_ENABLE(&hltdc);
	else if(sw==0)__HAL_LTDC_DISABLE(&hltdc);
}

//����ָ����
//layerx:���,0,��һ��; 1,�ڶ���
//sw:1 ��;0�ر�
void LTDC_Layer_Switch(u8 layerx,u8 sw)
{
	if(sw==1) __HAL_LTDC_LAYER_ENABLE(&hltdc,layerx);
	else if(sw==0) __HAL_LTDC_LAYER_DISABLE(&hltdc,layerx);
	__HAL_LTDC_RELOAD_CONFIG(&hltdc);
}

//ѡ���
//layerx:���;0,��һ��;1,�ڶ���;
void LTDC_Select_Layer(u8 layerx)
{
	lcdltdc.activelayer=layerx;
}

//����LCD��ʾ����
//dir:0,������1,����
void LTDC_Display_Dir(u8 dir)
{
    lcdltdc.dir=dir; 	//��ʾ����
	if(dir==0)			//����
	{
		lcdltdc.width=lcdltdc.pheight;
		lcdltdc.height=lcdltdc.pwidth;	
	}else if(dir==1)	//����
	{
		lcdltdc.width=lcdltdc.pwidth;
		lcdltdc.height=lcdltdc.pheight;
	}
}

//���㺯��
//x,y:д������
//color:��ɫֵ
void LTDC_Draw_Point(u16 x,u16 y,u32 color)
{ 
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	if(lcdltdc.dir)	//����
	{
        *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x))=color;
	}
	else 			//����
	{
        *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x)+y))=color; 
	}
#else
	if(lcdltdc.dir)	//����
	{
        *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x))=color;
	}
	else 			//����
	{
        *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+y))=color; 
	}
#endif
}

//���㺯��
//x,y:��ȡ�������
//����ֵ:��ɫֵ
u32 LTDC_Read_Point(u16 x,u16 y)
{ 
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	if(lcdltdc.dir)	//����
	{
		return *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x));
	}
	else 			//����
	{
		return *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x)+y)); 
	}
#else
	if(lcdltdc.dir)	//����
	{
		return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x));
	}
	else 			//����
	{
		return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+y)); 
	}
#endif 
}

//LTDC������,DMA2D���
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
//��ʱ����ҪƵ���ĵ�����亯��������Ϊ���ٶȣ���亯�����üĴ����汾��
//���������ж�Ӧ�Ŀ⺯���汾�Ĵ��롣
void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{
	u32 psx,psy,pex,pey;	//��LCD���Ϊ��׼������ϵ,����������仯���仯
	u32 timeout=0;
	u16 offline;
	u32 addr;
	//����ϵת��
	if(lcdltdc.dir)	//����
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//����
	{
		psx=sy;psy=lcdltdc.pheight-ex-1;
		pex=ey;pey=lcdltdc.pheight-sx-1;
	}
	offline=lcdltdc.pwidth-(pex-psx+1);
	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
	__HAL_RCC_DMA2D_CLK_ENABLE();	//ʹ��DM2Dʱ��
	DMA2D->CR&=~(DMA2D_CR_START);	//��ֹͣDMA2D
	DMA2D->CR=DMA2D_R2M;			//�Ĵ������洢��ģʽ
	DMA2D->OPFCCR=LCD_PIXFORMAT;	//������ɫ��ʽ
	DMA2D->OOR=offline;				//������ƫ��

	DMA2D->OMAR=addr;				//����洢����ַ
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//�趨�����Ĵ���
	DMA2D->OCOLR=color;						//�趨�����ɫ�Ĵ���
	DMA2D->CR|=DMA2D_CR_START;				//����DMA2D
	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0)	//�ȴ��������
	{
		timeout++;
		if(timeout>0X1FFFFF)
			break;	//��ʱ�˳�
	}
	DMA2D->IFCR|=DMA2D_FLAG_TC;		//���������ɱ�־
}



//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
//void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
//{
//	u32 psx,psy,pex,pey;	//��LCD���Ϊ��׼������ϵ,����������仯���仯
//	u32 timeout=0; 
//	u16 offline;
//	u32 addr;  
//    if(ex>=lcdltdc.width)ex=lcdltdc.width-1;
//	if(ey>=lcdltdc.height)ey=lcdltdc.height-1;
//	//����ϵת��
//	if(lcdltdc.dir)	//����
//	{
//		psx=sx;psy=sy;
//		pex=ex;pey=ey;
//	}else			//����
//	{
//		psx=sy;psy=lcdltdc.pheight-ex-1;
//		pex=ey;pey=lcdltdc.pheight-sx-1;
//	}
//	offline=lcdltdc.pwidth-(pex-psx+1);
//	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
//    if(LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB565)  //�����RGB565��ʽ�Ļ���Ҫ������ɫת������16bitת��Ϊ32bit��
//    {
//        color=((color&0X0000F800)<<8)|((color&0X000007E0)<<5)|((color&0X0000001F)<<3);
//    }
//	//����DMA2D��ģʽ
//	DMA2D_Handler.Instance=DMA2D;
//	DMA2D_Handler.Init.Mode=DMA2D_R2M;			//�ڴ浽�洢��ģʽ
//	DMA2D_Handler.Init.ColorMode=LCD_PIXFORMAT;	//������ɫ��ʽ
//	DMA2D_Handler.Init.OutputOffset=offline;		//���ƫ�� 
//	HAL_DMA2D_Init(&DMA2D_Handler);              //��ʼ��DMA2D
//    HAL_DMA2D_ConfigLayer(&DMA2D_Handler,lcdltdc.activelayer); //������
//    HAL_DMA2D_Start(&DMA2D_Handler,color,(u32)addr,pex-psx+1,pey-psy+1);//��������
//    HAL_DMA2D_PollForTransfer(&DMA2D_Handler,1000);//��������
//    while((__HAL_DMA2D_GET_FLAG(&DMA2D_Handler,DMA2D_FLAG_TC)==0)&&(timeout<0X5000))//�ȴ�DMA2D���
//    {
//        timeout++;
//    }
//    __HAL_DMA2D_CLEAR_FLAG(&DMA2D_Handler,DMA2D_FLAG_TC);       //���������ɱ�־
//}


//��ָ�����������ָ����ɫ��,DMA2D���	
//�˺�����֧��u16,RGB565��ʽ����ɫ�������.
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)  
//ע��:sx,ex,���ܴ���tftlcd_data.width-1;sy,ey,���ܴ���tftlcd_data.height-1!!!
//color:Ҫ������ɫ�����׵�ַ
void LTDC_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
	u32 psx,psy,pex,pey;	//��LCD���Ϊ��׼������ϵ,����������仯���仯
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	//����ϵת��
	if(lcdltdc.dir)	//����
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//����
	{
		psx=sy;psy=lcdltdc.pheight-ex-1;
		pex=ey;pey=lcdltdc.pheight-sx-1;
	}
	offline=lcdltdc.pwidth-(pex-psx+1);
	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
	__HAL_RCC_DMA2D_CLK_ENABLE();	//ʹ��DM2Dʱ��
	DMA2D->CR&=~(DMA2D_CR_START);	//��ֹͣDMA2D
	DMA2D->CR=DMA2D_M2M;			//�洢�����洢��ģʽ
	DMA2D->FGPFCCR=LCD_PIXFORMAT;	//������ɫ��ʽ
	DMA2D->FGOR=0;					//ǰ������ƫ��Ϊ0
	DMA2D->OOR=offline;				//������ƫ�� 

	DMA2D->FGMAR=(u32)color;		//Դ��ַ
	DMA2D->OMAR=addr;				//����洢����ַ
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//�趨�����Ĵ��� 
	DMA2D->CR|=DMA2D_CR_START;					//����DMA2D
	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0)		//�ȴ��������
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//��ʱ�˳�
	} 
	DMA2D->IFCR|=DMA2D_FLAG_TC;				//���������ɱ�־  	
}  

////LCD����
////color:��ɫֵ
void LTDC_Clear(u32 color)
{
	LTDC_Fill(0,0,lcdltdc.width-1,lcdltdc.height-1,color);
}

//LCD清屏
//color:颜色值
//void LTDC_Clear(u32 color)
//{
//    // 根據您在 ltdc_ex.h 中定義的顏色格式，來決定指標類型和顏色轉換
//#if defined(LCD_PIXFORMAT) && (LCD_PIXFORMAT==LCD_PIXEL_FORMAT_ARGB8888 || LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB888)
//    // 32位元顏色格式 (ARGB8888 或 RGB888)
//    u32 *p_framebuf = (u32*)ltdc_framebuf[0];
//    u32 total_pixels = lcdltdc.pwidth * lcdltdc.pheight;
//    u32 i;
//
//    // 確保幀緩衝區指標有效
//    if (p_framebuf == NULL) return;
//
//    // 使用 CPU 循環直接寫入每一個像素的資料
//    for (i = 0; i < total_pixels; i++)
//    {
//        p_framebuf[i] = color;
//    }
//#else
//    // 16位元顏色格式 (假設為 RGB565)
//    u16 *p_framebuf = (u16*)ltdc_framebuf[0];
//    u32 total_pixels = lcdltdc.pwidth * lcdltdc.pheight;
//    u16 color16 = (u16)color; // 將 32 位元顏色值轉換為 16 位元
//    u32 i;
//
//    // 確保幀緩衝區指標有效
//    if (p_framebuf == NULL) return;
//
//    // 使用 CPU 循環直接寫入每一個像素的資料
//    for (i = 0; i < total_pixels; i++)
//    {
//        p_framebuf[i] = color16;
//    }
//#endif
//
//    // ** 這是最關鍵的一步 **
//    // 在 CPU 將資料寫入 Cache 後，執行 "Clean Data Cache" 操作。
//    // 這個指令會強制 CPU 將 D-Cache 中所有被修改過的資料 (我們剛剛寫入的顏色)
//    // 同步寫回到實體的 SDRAM 記憶體中。
//    // 這樣，LTDC 硬體控制器才能從 SDRAM 讀取到最新的畫面資料。
//    SCB_CleanDCache();
//}


//LTDCʱ��(Fdclk)���ú���
//Fvco=Fin*pllsain; 
//Fdclk=Fvco/pllsair/2*2^pllsaidivr=Fin*pllsain/pllsair/2*2^pllsaidivr;

//Fvco:VCOƵ��
//Fin:����ʱ��Ƶ��һ��Ϊ1Mhz(����ϵͳʱ��PLLM��Ƶ���ʱ��,��ʱ����ͼ)
//pllsain:SAIʱ�ӱ�Ƶϵ��N,ȡֵ��Χ:50~432.  
//pllsair:SAIʱ�ӵķ�Ƶϵ��R,ȡֵ��Χ:2~7
//pllsaidivr:LCDʱ�ӷ�Ƶϵ��,ȡֵ��Χ:RCC_PLLSAIDIVR_2/4/8/16,��Ӧ��Ƶ2~16 
//����:�ⲿ����Ϊ25M,pllm=25��ʱ��,Fin=1Mhz.
//����:Ҫ�õ�20M��LTDCʱ��,���������:pllsain=400,pllsair=5,pllsaidivr=RCC_PLLSAIDIVR_4
//Fdclk=1*400/5/4=400/20=20Mhz
//����ֵ:0,�ɹ�;1,ʧ�ܡ�
u8 LTDC_Clk_Set(u32 pllsain,u32 pllsair,u32 pllsaidivr)
{
	RCC_PeriphCLKInitTypeDef PeriphClkIniture;
	
	//LTDC�������ʱ�ӣ���Ҫ�����Լ���ʹ�õ�LCD�����ֲ������ã�
    PeriphClkIniture.PeriphClockSelection=RCC_PERIPHCLK_LTDC;	//LTDCʱ�� 	
	PeriphClkIniture.PLLSAI.PLLSAIN=pllsain;    
	PeriphClkIniture.PLLSAI.PLLSAIR=pllsair;  
	PeriphClkIniture.PLLSAIDivR=pllsaidivr;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkIniture)==HAL_OK)    //��������ʱ��
    {
        return 0;   //�ɹ�
    }
    else return 1;  //ʧ��    
}

//LTDC,���մ�������,������LCD�������ϵΪ��׼
//ע��:�˺���������LTDC_Layer_Parameter_Config֮��������.
//layerx:��ֵ,0/1.
//sx,sy:��ʼ����
//width,height:��Ⱥ͸߶�
void LTDC_Layer_Window_Config(u8 layerx,u16 sx,u16 sy,u16 width,u16 height)
{
    HAL_LTDC_SetWindowPosition(&hltdc,sx,sy,layerx);  //���ô��ڵ�λ��
    HAL_LTDC_SetWindowSize(&hltdc,width,height,layerx);//���ô��ڴ�С
}

//LTDC,������������.
//ע��:�˺���,������LTDC_Layer_Window_Config֮ǰ����.
//layerx:��ֵ,0/1.
//bufaddr:����ɫ֡������ʼ��ַ
//pixformat:��ɫ��ʽ.0,ARGB8888;1,RGB888;2,RGB565;3,ARGB1555;4,ARGB4444;5,L8;6;AL44;7;AL88
//alpha:����ɫAlphaֵ,0,ȫ͸��;255,��͸��
//alpha0:Ĭ����ɫAlphaֵ,0,ȫ͸��;255,��͸��
//bfac1:���ϵ��1,4(100),�㶨��Alpha;6(101),����Alpha*�㶨Alpha
//bfac2:���ϵ��2,5(101),�㶨��Alpha;7(111),����Alpha*�㶨Alpha
//bkcolor:��Ĭ����ɫ,32λ,��24λ��Ч,RGB888��ʽ
//����ֵ:��
void LTDC_Layer_Parameter_Config(u8 layerx,u32 bufaddr,u8 pixformat,u8 alpha,u8 alpha0,u8 bfac1,u8 bfac2,u32 bkcolor)
{
	LTDC_LayerCfgTypeDef pLayerCfg;
	
	pLayerCfg.WindowX0=0;                       //������ʼX����
	pLayerCfg.WindowX1=lcdltdc.pwidth;          //������ֹX����
	pLayerCfg.WindowY0=0;                       //������ʼY����
	pLayerCfg.WindowY1=lcdltdc.pheight;         //������ֹY����
	pLayerCfg.PixelFormat=pixformat;		    //���ظ�ʽ
	pLayerCfg.Alpha=alpha;				        //Alphaֵ���ã�0~255,255Ϊ��ȫ��͸��
	pLayerCfg.Alpha0=alpha0;			        //Ĭ��Alphaֵ
	pLayerCfg.BlendingFactor1=(u32)bfac1<<8;    //���ò���ϵ��
	pLayerCfg.BlendingFactor2=(u32)bfac2<<8;	//���ò���ϵ��
	pLayerCfg.FBStartAdress=bufaddr;	        //���ò���ɫ֡������ʼ��ַ
	pLayerCfg.ImageWidth=lcdltdc.pwidth;        //������ɫ֡�������Ŀ��    
	pLayerCfg.ImageHeight=lcdltdc.pheight;      //������ɫ֡�������ĸ߶�
	pLayerCfg.Backcolor.Red=(u8)(bkcolor&0X00FF0000)>>16;   //������ɫ��ɫ����
	pLayerCfg.Backcolor.Green=(u8)(bkcolor&0X0000FF00)>>8;  //������ɫ��ɫ����
	pLayerCfg.Backcolor.Blue=(u8)bkcolor&0X000000FF;        //������ɫ��ɫ����
	if (HAL_LTDC_ConfigLayer(&hltdc,&pLayerCfg,layerx) != HAL_OK)   //������ѡ�еĲ�
	{
		Error_Handler();
	}
}  

//��ȡ������
//PG6=R7(M0);PI2=G7(M1);PI7=B7(M2);
//M2:M1:M0
//0 :0 :0	//4.3��480*272 RGB��,ID=0X4342
//0 :0 :1	//7��800*480 RGB��,ID=0X7084
//0 :1 :0	//7��1024*600 RGB��,ID=0X7016
//0 :1 :1	//7��1280*800 RGB��,ID=0X7018
//1 :0 :0	//8��1024*600 RGB��,ID=0X8016 
//1 :0 :1	//10.1��1280*800 RGB��,ID=0X1018 
//����ֵ:LCD ID:0,�Ƿ�;����ֵ,ID;
u16 LTDC_PanelID_Read(void)
{
	u8 idx=0;
////    GPIO_InitTypeDef GPIO_Initure;
////    __HAL_RCC_GPIOG_CLK_ENABLE();       //ʹ��GPIOGʱ��
////	__HAL_RCC_GPIOI_CLK_ENABLE();       //ʹ��GPIOIʱ��
////
////    GPIO_Initure.Pin=GPIO_PIN_6;        //PG6
////    GPIO_Initure.Mode=GPIO_MODE_INPUT;  //����
////    GPIO_Initure.Pull=GPIO_PULLUP;      //����
////    GPIO_Initure.Speed=GPIO_SPEED_HIGH; //����
////    HAL_GPIO_Init(GPIOG,&GPIO_Initure); //��ʼ��
////
////    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_7; //PI2,7
////    HAL_GPIO_Init(GPIOI,&GPIO_Initure);     //��ʼ��
////
//    idx=(u8)HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_6); //��ȡM0
//    idx|=(u8)HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_2)<<1;//��ȡM1
//    idx|=(u8)HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_7)<<2;//��ȡM2
	switch(idx)
	{
		case 0://4.3����,480*272�ֱ���
			lcdltdc.pwidth=480;			    //�����,��λ:����
			lcdltdc.pheight=272;		    //���߶�,��λ:����
			lcdltdc.hsw=1;				    //ˮƽͬ�����
			lcdltdc.vsw=1;				    //��ֱͬ�����
			lcdltdc.hbp=40;				    //ˮƽ����
			lcdltdc.vbp=8;				    //��ֱ����
			lcdltdc.hfp=5;				    //ˮƽǰ��
			lcdltdc.vfp=8;				    //��ֱǰ��
//			LTDC_Clk_Set(288,4,RCC_PLLSAIDIVR_8);   //��������ʱ�� 9Mhz
			return 0X4342;

		case 1://7����,800*480�ֱ���
			lcdltdc.pwidth=800;			    //�����,��λ:����
			lcdltdc.pheight=480;		    //���߶�,��λ:����
			lcdltdc.hsw=1;				    //ˮƽͬ�����
			lcdltdc.vsw=1;				    //��ֱͬ�����
			lcdltdc.hbp=46;				    //ˮƽ����
			lcdltdc.vbp=23;				    //��ֱ����
			lcdltdc.hfp=210;			    //ˮƽǰ��
			lcdltdc.vfp=22;				    //��ֱǰ��
			LTDC_Clk_Set(396,3,RCC_PLLSAIDIVR_4); //��������ʱ�� 33M(�����˫��,��Ҫ����DCLK��:18.75Mhz  300/4/4,�Ż�ȽϺ�)
			return 0X7084;

		case 2://7����,1024*600�ֱ���
			lcdltdc.pwidth=1024;			//�����,��λ:����
			lcdltdc.pheight=600;			//���߶�,��λ:����
			lcdltdc.hsw=20;				    //ˮƽͬ�����
			lcdltdc.vsw=3;				    //��ֱͬ�����
			lcdltdc.hbp=140;			    //ˮƽ����
			lcdltdc.vbp=20;				    //��ֱ����
			lcdltdc.hfp=160;			    //ˮƽǰ��
			lcdltdc.vfp=12;				    //��ֱǰ��
			LTDC_Clk_Set(360,2,RCC_PLLSAIDIVR_4);//��������ʱ��  45Mhz
			//������������.
			return 0X7016;

		case 3://7����,1280*800�ֱ���
			lcdltdc.pwidth=1280;			//�����,��λ:����
			lcdltdc.pheight=800;			//���߶�,��λ:����
			//������������.
			return 0X7018;

		case 4://8����,1024*600�ֱ���
			lcdltdc.pwidth=1024;			//�����,��λ:����
			lcdltdc.pheight=768;			//���߶�,��λ:����
			//������������.
			return 0X8016;

		case 5://10.1����,1280*800�ֱ���
			lcdltdc.pwidth=1280;		//�����,��λ:����
			lcdltdc.pheight=800;		//���߶�,��λ:����
			lcdltdc.hbp=140;			//ˮƽ����
			lcdltdc.hfp=10;			    //ˮƽǰ��
			lcdltdc.hsw=10;				//ˮƽͬ�����
			lcdltdc.vbp=10;				//��ֱ����
			lcdltdc.vfp=10;				//��ֱǰ��
			lcdltdc.vsw=3;				//��ֱͬ�����
			LTDC_Clk_Set(360,2,RCC_PLLSAIDIVR_4);//��������ʱ��  45Mhz
			return 0X1018;

		default:
			return 0;
	}
}

//LCD��ʼ������
void LTDC_Init(void *init(void))
{
	tftlcd_data.width=lcdltdc.pwidth;
	tftlcd_data.height=lcdltdc.pheight;

#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	ltdc_framebuf[0]=(u32*)&ltdc_lcd_framebuf;
	lcdltdc.pixsize=4;				//ÿ������ռ4���ֽ�
#else
    lcdltdc.pixsize=2;				//ÿ������ռ2���ֽ�
	ltdc_framebuf[0]=(u32*)&ltdc_lcd_framebuf;
#endif
	init();
//    //LTDC����
//	hltdc.Instance=LTDC;
//	hltdc.Init.HSPolarity=LTDC_HSPOLARITY_AL;         //ˮƽͬ������
//	hltdc.Init.VSPolarity=LTDC_VSPOLARITY_AL;         //��ֱͬ������
//	hltdc.Init.DEPolarity=LTDC_DEPOLARITY_AL;         //����ʹ�ܼ���
//    if(tftlcd_data.id==0X1018)hltdc.Init.PCPolarity=LTDC_PCPOLARITY_IIPC;//����ʱ�Ӽ���
//	else hltdc.Init.PCPolarity=LTDC_PCPOLARITY_IPC;   	//����ʱ�Ӽ���
//    hltdc.Init.HorizontalSync=lcdltdc.hsw-1;          //ˮƽͬ�����
//    hltdc.Init.VerticalSync=lcdltdc.vsw-1;            //��ֱͬ�����
//    hltdc.Init.AccumulatedHBP=lcdltdc.hsw+lcdltdc.hbp-1; //ˮƽͬ�����ؿ��
//    hltdc.Init.AccumulatedVBP=lcdltdc.vsw+lcdltdc.vbp-1; //��ֱͬ�����ظ߶�
//    hltdc.Init.AccumulatedActiveW=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth-1;//��Ч���
//    hltdc.Init.AccumulatedActiveH=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight-1;//��Ч�߶�
//    hltdc.Init.TotalWidth=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth+lcdltdc.hfp-1;   //�ܿ��
//    hltdc.Init.TotalHeigh=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight+lcdltdc.vfp-1;  //�ܸ߶�
//    hltdc.Init.Backcolor.Red=0;           //��Ļ�������ɫ����
//    hltdc.Init.Backcolor.Green=0;         //��Ļ��������ɫ����
//    hltdc.Init.Backcolor.Blue=0;          //��Ļ����ɫ��ɫ����
//    if (HAL_LTDC_Init(&hltdc) != HAL_OK)
//    {
//    	Error_Handler();
//    }
//	//������
//	LTDC_Layer_Parameter_Config(0,(u32)ltdc_framebuf[0],LCD_PIXFORMAT,255,0,6,7,0X000000);//���������
	LTDC_Layer_Window_Config(0,0,0,lcdltdc.pwidth,lcdltdc.pheight);	//�㴰������,��LCD�������ϵΪ��׼,��Ҫ����޸�!
//
 	LTDC_Display_Dir(TFTLCD_DIR);			//Ĭ������
	LTDC_Select_Layer(0); 			//ѡ���1��
//    LTDC_Clear(0XFF000000);			//����
    LTDC_Clear(0X00000000);			//����

}

////LTDC�ײ�IO��ʼ����ʱ��ʹ��
////�˺����ᱻHAL_LTDC_Init()����
////hltdc:LTDC���
//void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
//{
//    GPIO_InitTypeDef GPIO_Initure;
//
//    __HAL_RCC_LTDC_CLK_ENABLE();                //ʹ��LTDCʱ��
//    __HAL_RCC_DMA2D_CLK_ENABLE();               //ʹ��DMA2Dʱ��
//    __HAL_RCC_GPIOF_CLK_ENABLE();               //ʹ��GPIOFʱ��
//    __HAL_RCC_GPIOG_CLK_ENABLE();               //ʹ��GPIOGʱ��
//    __HAL_RCC_GPIOH_CLK_ENABLE();               //ʹ��GPIOHʱ��
//    __HAL_RCC_GPIOI_CLK_ENABLE();               //ʹ��GPIOIʱ��
//
//
//    //��ʼ��PF10
//    GPIO_Initure.Pin=GPIO_PIN_10;
//    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //����
//    GPIO_Initure.Pull=GPIO_NOPULL;
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //����
//    GPIO_Initure.Alternate=GPIO_AF14_LTDC;      //����ΪLTDC
//    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
//
//    //��ʼ��PG6,7,11
//    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11;
//    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
//
//    //��ʼ��PH9,10,11,12,13,14,15
//    GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|\
//                     GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
//    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
//
//    //��ʼ��PI0,1,2,4,5,6,7,9,10
//    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|\
//                     GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10;
//    HAL_GPIO_Init(GPIOI,&GPIO_Initure);
//}


