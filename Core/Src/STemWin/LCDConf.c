/* LCDConf.c */
#include "inc/GUI.h"
#include "inc/LCD_ConfDefaults.h" // 來自 STemWin/inc
#include "inc/GUIDRV_Lin.h"       // 來自 STemWin/inc

//
// 從您的專案中引入 LTDC 和 FMC (SDRAM) 的標頭檔
//
#include "ltdc.h"
#include "fmc.h"

//=====================================================================
//
//       硬體與顯示屬性定義
//
//=====================================================================
//
// LCD 實體解析度 (從 ltdc.c 的 MX_LTDC_Init() 得知)
//
#define XSIZE_PHYS 480
#define YSIZE_PHYS 272

//
// SDRAM 中的 Frame Buffer 起始位址 (從 ltdc_ex.h 得知)
//
#define VRAM_ADDR ((U32)0xC0000000)

//
// 顏色轉換模式 (您的專案使用 RGB565)
//
#define COLOR_CONVERSION GUICC_M565

//#define COLOR_CONVERSION GUICC_M565I
// 顯示驅動 (適用於 LTDC 的線性幀緩衝區驅動)
//
#define DISPLAY_DRIVER GUIDRV_LIN_16 // 16 代表 16bpp (RGB565)

//=====================================================================
//
//       LCD_X_Config
//
//=====================================================================
void LCD_X_Config(void) {
  //
  // 建立並連結顯示裝置
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);


  //
  // 設定顯示尺寸和 Frame Buffer
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
  }
  LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);

  //
  // (可選) 設定觸控螢幕支援
  // GUI_TOUCH_SetOrientation(GUI_GetOrientation());
  //
}

//=====================================================================
//
//       LCD_X_DisplayDriver
//
//=====================================================================
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r = 0;
  (void)LayerIndex; // 避免編譯器警告

  switch (Cmd) {
  case LCD_X_INITCONTROLLER: {
    //
    // 您的 LTDC 和 FMC 初始化已經在 main.c -> API_Init() 中被呼叫，
    // 所以這裡通常不需要做額外的事情。
    //
    break;
  }
  //
  // 其他指令如 LCD_X_SETORG, LCD_X_SHOW, etc. 對於 LTDC + GUIDRV_Lin
  // 驅動來說，通常由 STemWin 內部處理，我們不需要實作。
  //
  default:
    r = -1;
  }
  return r;
}
