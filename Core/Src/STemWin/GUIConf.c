/* GUIConf.c */
#include "inc/GUI.h"

//
// 設定 STemWin 可用的記憶體大小 (單位: bytes)
// 這個值需要根據您的應用複雜度調整，對於顯示圖表和一些文字，32KB 是一個合理的起點。
//
#define GUI_NUMBYTES  (1024 * 32)

//
// 分配記憶體給 STemWin
//
U32 GUI_Memory_Pool[GUI_NUMBYTES / 4];

void GUI_X_Config(void) {
  //
  // 將靜態分配的記憶體區塊指派給 STemWin
  //
  GUI_ALLOC_AssignMemory(GUI_Memory_Pool, GUI_NUMBYTES);
  //
  // (可選) 設定預設字型
  //
  GUI_SetDefaultFont(GUI_FONT_6X8);
}
