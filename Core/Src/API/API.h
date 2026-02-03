#ifndef __API_H
#define __API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "TMP117/TMP117.h"
//#include "Version.h"

//#include "LTC2983/ltc2983.h"
#include "SG_ECP5/SG_ECP5.h"
#include "SPI_Master/SPI_Master.h"
#include "DelayUtility/DelayUtility.h"
#include "LTC298X/ltc298x.h"
#include "../../Inc/ltdc.h"
#include "../../Inc/fmc.h"

#include "tftlcd/tftlcd.h"
//#include "tftlcd/font.h"
#include <stdio.h> // 為了使用 sprintf
#include <string.h> // 為了使用 strncmp 解析指令

#include "sdram/sdram.h"

#include "../STemWin/GUIConf.h"
#include "../STemWin/LCDConf.h"
#include "../STemWin/inc/GRAPH.h"


//#define MAX_TMP117_SENSORS 16+20
#define MAX_TMP117_SENSORS 16

#define MAX_LTC2983_DEVICES 4
#define GRAPH_X_START   40
#define GRAPH_Y_START   50 // �????下移�????點給??��?�空???
#define GRAPH_WIDTH     400
#define GRAPH_HEIGHT    220
#define MAX_TEMP_POINTS 400 // ??�表顯示??��?大數??��?�數

#define Y_MIN_TEMP  99.0f
#define Y_MAX_TEMP  -99.9f

typedef enum {
    LTC_STATE_IDLE,         // 沒事做
    LTC_STATE_RESET_START,  //剛收到指令，準備拉低 Reset
    LTC_STATE_WAIT_BOOT,    // Reset 拉高了，正在等 250ms 暖機
    LTC_STATE_CHECK_READY,  // 檢查 Status Ready
    LTC_STATE_WRITE_CONFIG  // 開始寫入 SPI
} LTC_Init_State_t;

extern LTC_Init_State_t g_LTC_State;

// 定義儲存結構 (讓 API.c 的 g_LTC_SensorPool 知道這是什麼)
typedef union {
    struct ltc2983_sensor       base;
    struct ltc2983_thermocouple tc;
    struct ltc2983_diode        diode;
//    struct ltc2983_rtd          rtd;
} ltc_sensor_storage_t;

// 宣告全域變數 (讓其他檔案也能用)
extern ltc_sensor_storage_t g_LTC_SensorPool[MAX_LTC2983_DEVICES][20];
extern ltc298x_dev g_LTC_Devices[MAX_LTC2983_DEVICES];
extern struct ltc298x_init_param g_LTC_InitParams[MAX_LTC2983_DEVICES];



typedef struct {
	// --- TMP117 感測器管理 ---
	// 直接在結構中包含感測器實例的陣列
	tmp117_sensor tmp117_sensors[MAX_TMP117_SENSORS];
	// 記錄實際配置並使用的感測器數量
	unsigned char num_tmp117_sensors;

	// --- LTC2983 設備管理  ---
	ltc2983_device_manager ltc2983_devices[MAX_LTC2983_DEVICES];
	// 記錄實際配置並使用的 LTC2983 晶片數量
	unsigned char num_ltc2983_devices;
	unsigned long fpga_version;

	float graph_y_min; // Y 軸當前的最小值
	float graph_y_max; // Y 軸當前的最大值

} API;

extern API API0;


int API_Init(void);

int API_LTC2983_ReadMultipleTemperatures(ltc2983_device_manager devices[], unsigned char num_devices_to_find, unsigned char array_size);
int API_TMP117_ReadMultipleTemperatures(tmp117_sensor sensors[], unsigned char num_sensors_to_find, unsigned char array_size);

char* ulong_to_ascii_union(unsigned long value, char* dest_buffer, size_t buffer_size);

void GPIO_Set_O(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Update_LCD_Task(uint8_t font_size);


// --- *** 宣告圖表 API 函式 *** ---
void API_Graph_Init_OnlyBKColor(void);

void API_Graph_Init(void);
void API_Graph_UpdateTask(void);

void API_Graph_DrawBarChart(void);
void API_RunLineChartLogic(void);

// --- *** 結束 *** ---
void API_USB_ProcessRx(uint8_t* Buf, uint32_t Len);
void API_USB_SendTelemetry(void);
void API_ReInit_Single_Sensor(int ch, int addr);
void API_LTC_Apply_Config(int dev_idx, int ch_idx, int type, uint32_t config_val, uint32_t data_val);

extern GRAPH_DATA_Handle  _ahData[MAX_TMP117_SENSORS];   // *** 修改：改為資料控制代碼的陣列 ***
//extern GRAPH_DATA_Handle  _ahData_LTC2983[20];   // *** for_LTC2983_0  ***

extern GRAPH_SCALE_Handle _hScaleV;  // ??�直?��度控?���????�????
extern GRAPH_SCALE_Handle _hScaleH;  // 水平?��度控?���????�????
extern const GUI_COLOR _aSensorColors[];
extern const unsigned int NUM_SENSOR_COLORS;


#ifdef __cplusplus
}
#endif

#endif
