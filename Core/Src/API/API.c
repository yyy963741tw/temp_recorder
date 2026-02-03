#include "API.h"
#define MCU_VERSION_STR		"V2.10.02"

API API0;

// ============================================================
// [LTC2983 全域變數定義區]
// 修正重點：補上缺少的變數，並統一名稱以符合函式呼叫
// ============================================================

// 1. LTC 狀態機與計時器
LTC_Init_State_t g_LTC_State = LTC_STATE_IDLE;
uint32_t g_LTC_Timer = 0;

// 2. 裝置實例 (Device Instances) - [之前缺少]
ltc298x_dev g_LTC_Devices[MAX_LTC2983_DEVICES];

// 3. 初始化參數 (Init Params) - [之前導致編譯錯誤的原因]
struct ltc298x_init_param g_LTC_InitParams[MAX_LTC2983_DEVICES];

// 4. 感測器設定池 (Sensor Config Pool)
// [修正] 名稱改為 g_LTC_SensorPool 以配合 API_LTC_Apply_Config
ltc_sensor_storage_t g_LTC_SensorPool[MAX_LTC2983_DEVICES][20];
//////////////////////////////////////////////////////////////////////

//static GRAPH_DATA_Handle  _ahData;   // 資�?�物件控?���????�????
GRAPH_DATA_Handle  _ahData[MAX_TMP117_SENSORS];   // *** 修改：改為資料控制代碼的陣列 ***
//GRAPH_DATA_Handle  _ahData_LTC2983[20];   // *** 修改：改為資料控制代碼的陣列 ***

// [新增] 全域變數：控制傳送間隔 (預設 1000ms)
uint32_t g_TxInterval = 1000;

GRAPH_SCALE_Handle _hScaleV;  // ??�直?��度控?���????�????
GRAPH_SCALE_Handle _hScaleH;  // 水平?��度控?���????�????
const GUI_COLOR _aSensorColors[] = {
		GUI_BLUE,
		GUI_GREEN,
		GUI_RED,
		GUI_CYAN,
		GUI_MAGENTA,
		GUI_YELLOW,
		GUI_LIGHTBLUE,
		GUI_LIGHTGREEN,

		GUI_DARKBLUE   ,
		GUI_DARKGREEN  ,
		GUI_DARKRED    ,
		GUI_DARKCYAN   ,
		GUI_DARKMAGENTA,
		GUI_DARKYELLOW ,
		GUI_GRAY,  
		GUI_ORANGE,
};



const unsigned int NUM_SENSOR_COLORS = (sizeof(_aSensorColors) / sizeof(GUI_COLOR));
// 預設全部初始化為 0 (0 = 尚未過濾, 1 = 已過濾)
static uint8_t _aDataSkipCounter[MAX_TMP117_SENSORS] = {0};
static GRAPH_Handle hGraph;

static int API_TMP117_Init()
{
	// 1. 先將整個感測器陣列的記憶體清零
	memset(API0.tmp117_sensors, 0, sizeof(API0.tmp117_sensors));

	// 2. 手動設定您期望的有效感測器總數
	//	API0.num_tmp117_sensors = 3+4;
	API0.num_tmp117_sensors = 0;

	//	API0.num_tmp117_sensors = 2;


	// 3. 在陣列的任意位置，初始化您需要用到的感測器
	//    初始化位於陣列索引 0 的感測器
	/*	TMP117_Init(&API0.tmp117_sensors[0], &(struct tmp117_init_param){
		.spi_idx = SPIM_FPGA, .cs_idx = 0xff, .channel = 0, .i2c_address = I2C_ADDR_SCL
	});

	//    初始化位於陣列索引 1 的感測器
	TMP117_Init(&API0.tmp117_sensors[1], &(struct tmp117_init_param){
		.spi_idx = SPIM_FPGA, .cs_idx = 0xff, .channel = 1, .i2c_address = I2C_ADDR_SCL
	});

	//    初始化位於陣列索引 15 的感測器
	TMP117_Init(&API0.tmp117_sensors[15], &(struct tmp117_init_param){
		.spi_idx = SPIM_FPGA, .cs_idx = 0xff, .channel = 15, .i2c_address = I2C_ADDR_SCL
	});
	//    初始化位於陣列索引 5 的感測器
	TMP117_Init(&API0.tmp117_sensors[5], &(struct tmp117_init_param){
		.spi_idx = SPIM_FPGA, .cs_idx = 0xff, .channel = 5, .i2c_address = I2C_ADDR_SCL
	});
	//    初始化位於陣列索引 6 的感測器
	TMP117_Init(&API0.tmp117_sensors[6], &(struct tmp117_init_param){
		.spi_idx = SPIM_FPGA, .cs_idx = 0xff, .channel = 6, .i2c_address = I2C_ADDR_SCL
	});
	//    初始化位於陣列索引 8 的感測器
	TMP117_Init(&API0.tmp117_sensors[8], &(struct tmp117_init_param){
		.spi_idx = SPIM_FPGA, .cs_idx = 0xff, .channel = 8, .i2c_address = I2C_ADDR_SCL
	});
	//    初始化位於陣列索引 9 的感測器
	TMP117_Init(&API0.tmp117_sensors[9], &(struct tmp117_init_param){
		.spi_idx = SPIM_FPGA, .cs_idx = 0xff, .channel = 9, .i2c_address = I2C_ADDR_SCL
	});
	// 4. 遍歷整個陣列，將所有被設定過的感測器配置到 FPGA
	unsigned char configured_count = 0; // 用於計數已配置的感測器
	for (int i = 0; i < MAX_TMP117_SENSORS; i++) {
		// 檢查該位置的感測器是否有效
		if (API0.tmp117_sensors[i].i2c_address != 0) {
			if (TMP117_ConfigureOnFPGA(&API0.tmp117_sensors[i]) != 0) {
				return -1; // FPGA 通訊失敗
			}
			configured_count++;

			// 如果已配置的數量達到了我們設定的目標，就提前結束迴圈
			if (configured_count >= API0.num_tmp117_sensors) {
				break;
			}
		}
	}
	 */
	return 0; // 所有 TMP117 都設定成功

}



// [新增] 真正的記憶體池：2 個裝置，每個有 20 個通道
ltc_sensor_storage_t g_LTC_SensorPool[MAX_LTC2983_DEVICES][20];

// [修正版] 系統啟動時呼叫，連結所有指標與參數
static int API_LTC_Hardware_Map()
{
    // 1. 設定總數
    API0.num_ltc2983_devices = 2;

    // 2. 清空記憶體
    memset(&API0.ltc2983_devices, 0, sizeof(API0.ltc2983_devices));
    memset(g_LTC_SensorPool, 0, sizeof(g_LTC_SensorPool));

    // 清空 g_LTC 相關全域變數
    memset(g_LTC_Devices, 0, sizeof(g_LTC_Devices));
    memset(g_LTC_InitParams, 0, sizeof(g_LTC_InitParams));

    // ==========================================
    // 設定迴圈
    // ==========================================
    for (int i = 0; i < MAX_LTC2983_DEVICES; i++)
    {
        // --- A. 設定 SPI/CS 硬體參數 ---
        int spi_index = 0;
        int cs_index  = 0;

        if (i == 0) {
            spi_index = SPIM_LTC2983_1; // Device 1
            cs_index  = 1;
        } else if (i == 1) {
            // spi_index = SPIM_LTC2983_2; // Device 2
            // cs_index  = 2;
        }

        // --- B. 設定 API0 (建立指標陣列) ---
        ltc2983_device_manager *mgr = &API0.ltc2983_devices[i];
        mgr->dev_instance.spi_idx = spi_index;
        mgr->dev_instance.cs_idx  = cs_index;

        // [關鍵步驟 1] 這裡您正確地建立了一個「指標陣列」
        // mgr->sensors 是一個陣列，裡面存放指向 SensorPool 的「指標」
        for (int ch = 0; ch < 20; ch++) {
            mgr->sensors[ch] = (struct ltc2983_sensor *)&g_LTC_SensorPool[i][ch];
        }

        // --- C. 設定 g_LTC (給 USB Init 用) ---
        g_LTC_Devices[i].spi_idx = spi_index;
        g_LTC_Devices[i].cs_idx  = cs_index;

        g_LTC_InitParams[i].dev_id  = ID_LTC2983;
        g_LTC_InitParams[i].spi_idx = spi_index;
        g_LTC_InitParams[i].cs_idx  = cs_index;

        // ⭐⭐⭐ [最重要修正] ⭐⭐⭐
        // 不要直接轉型 SensorPool (會當機！)
        // 而是要指向我們剛剛建立好的「指標陣列」 (mgr->sensors)
        g_LTC_InitParams[i].sensors = mgr->sensors;
    }

    return 0;
}

// 初始化所有 LTC2983 設備的輔助函式
static int API_LTC2983_Init()
{
	// 1. 設定 LTC2983 設備的總數。
	API0.num_ltc2983_devices = 2;
	// 2. 為了安全起見，先將所有設備管理器的記憶體清零
	for (int i = 0; i < MAX_LTC2983_DEVICES; i++) {
		memset(&API0.ltc2983_devices[i], 0, sizeof(ltc2983_device_manager));
	}


	// --- 【已更正】手動設定第一個 LTC2983 (位於陣列索引 0) ---

	ltc2983_device_manager *ltc_dev0 = &API0.ltc2983_devices[0];


	// 填寫感測器陣列
	ltc_dev0->sensors[8] = (struct ltc2983_sensor *)&ltc2983_thermocouple_k_9; // Channel 9
	ltc_dev0->sensors[9] = (struct ltc2983_sensor *)&ltc2983_diode_10;      // Channel 10

	static struct ltc2983_diode tc_ch20;
	tc_ch20.sensor.chan = 20; // 通道 20
	tc_ch20.sensor.type = LTC2983_DIODE;
	tc_ch20.sensor_config = 0x5;
	tc_ch20.excitation_current = 0x5;
	ltc_dev0->sensors[19] = (struct ltc2983_sensor *)&tc_ch20 ;      // Channel 20


	static struct ltc2983_diode tc_ch5;
	memcpy(&tc_ch5, &ltc2983_thermocouple_k_9, sizeof(struct ltc2983_thermocouple));
	tc_ch5.sensor.chan = 5;
	ltc_dev0->sensors[4] = (struct ltc2983_sensor *)&tc_ch5 ;      // Channel 5

	// 準備初始化參數
	struct ltc298x_init_param init_params_0 = {
			.dev_id = ID_LTC2983,
			.spi_idx = SPIM_LTC2983_1, // <-- 對應 SPI5
			.cs_idx = 1,               // <-- 對應 LTC298X_CS1
			.sensors = ltc_dev0->sensors
	};

	if (ltc298x_init(&ltc_dev0->dev_instance, &init_params_0) != 0) {
		return -2; // LTC2983[0] 初始化失敗
	}


	/*
	// --- 【已更正】手動設定第四個 LTC2983 (位於陣列索引 3) ---

	ltc2983_device_manager *ltc_dev3 = &API0.ltc2983_devices[3];

	// 填寫感測器陣列
	ltc_dev3->sensors[8] = (struct ltc2983_sensor *)&ltc2983_thermocouple_k_9; // Channel 9
	ltc_dev3->sensors[9] = (struct ltc2983_sensor *)&ltc2983_diode_10;      // Channel 10

	// 準備初始化參數
	struct ltc298x_init_param init_params_3 = {
			.dev_id = ID_LTC2983,
			.spi_idx = SPIM_LTC2983_4, // <-- 對應 SPI4
			.cs_idx = 4,               // <-- 對應 LTC298X_CS4
			.sensors = ltc_dev3->sensors
	};

	if (ltc298x_init(&ltc_dev3->dev_instance, &init_params_3) != 0) {
		return -3; // LTC2983[3] 初始化失敗
	}
	 */
	return 0; // 所有 LTC2983 都設定成功

}
int API_Init(){

	/* MPU Configuration--------------------------------------------------------*/

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	//	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	//	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	//	MX_ETH_Init();
	MX_SPI1_Init();

	//	MX_SPI2_Init();
	MX_SPI3_Init();
	//		MX_SPI4_Init();
	MX_SPI5_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	MX_USART1_UART_Init();
	MX_DMA2D_Init();

	MX_CRC_Init();//for GUI_INIT
	MX_USB_DEVICE_Init();

	SDRAM_Init(MX_FMC_Init, &hsdram1, SDRAM_A);           //初始化SDRAM
	HAL_Delay(100); //  100ms 的延遲

	TFTLCD_Init(MX_LTDC_Init);			//LCD初始化
	//	unsigned char				PG_REG[100];
	//
	//	API_VersionASCII(MCU_VERSION_STR, PG_REG+4);
	/*	char			LCDM_str[20]=MCU_VERSION_STR;

	FRONT_COLOR=BLACK;
	//	sprintf(LCDM_str,"ECP0:%x %x %x %x %x %x", PG_REG[4],PG_REG[5], PG_REG[6], PG_REG[7],PG_REG[8], PG_REG[9]);
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,12,LCDM_str);
	LCD_ShowString(10,30,tftlcd_data.width,tftlcd_data.height,16,"Hello World!");
	LCD_ShowString(10,50,tftlcd_data.width,tftlcd_data.height,24,"Hello World!");
	LCD_ShowString(10,80,tftlcd_data.width,tftlcd_data.height,32,"Hello World!");
	LCD_ShowString(10,180,tftlcd_data.width,tftlcd_data.height,32,"AAAAAA");

	//	LCD_ShowFontHZ(10, 120,"普中科技");
	LCD_ShowHZ_Debug(10, 120, CnChar32x29[0].Msk);
	LCD_ShowHZ_Debug(42, 120, CnChar32x29[1].Msk);
	LCD_ShowHZ_Debug(74, 120, CnChar32x29[2].Msk);
	LCD_ShowHZ_Debug(106, 120, CnChar32x29[3].Msk);

	LCD_ShowString(10,150,tftlcd_data.width,tftlcd_data.height,24,"www.prechin.cn");
	 */

	SPIMCH_Init(MX_SPI1_Init, &hspi1, SPIM_FPGA);
	//		SPIMCH_Init(MX_SPI4_Init, &hspi4, SPIM_LTC2983_4);
	SPIMCH_Init(MX_SPI5_Init, &hspi5, SPIM_LTC2983_1);

	SG_ECP5_Init(SPIM_FPGA);



	/* Initialize all sensors and check return status */
	if (API_TMP117_Init() != 0) {
		return -1; // TMP117 初始化失敗
	}
	//	if (API_LTC2983_Init() != 0) {
	if (API_LTC_Hardware_Map() != 0) {
		return -2; // LTC2983 初始化失敗
	}

	if (SG_ECP5_GetVersion(&API0.fpga_version) !=0){
		return -3;
	}
	//	GUI_Init();
	//	GUI_SetBkColor(GUI_YELLOW);
	//	GUI_Clear();

	//	API_Graph_Init();
	API_Graph_Init_OnlyBKColor();


	return 0; // 所有初始化成功


}
/**
 */
int API_LTC2983_ReadMultipleTemperatures(ltc2983_device_manager devices[], unsigned char num_devices_to_find, unsigned char array_size)
{
	if (!devices || num_devices_to_find == 0) {
		return -1; // 沒有東西需要讀取
	}

	unsigned char found_count = 0;

	// 遍歷所有指定的 LTC2983 設備，最多遍歷 array_size 次
	for (int i = 0; i < array_size; i++) {
		ltc2983_device_manager *current_ltc = &devices[i];

		if (current_ltc->dev_instance.spi_idx != 0) {
			found_count++;

			for (int ch_idx = 0; ch_idx < 20; ch_idx++) {
				if (current_ltc->sensors[ch_idx] != NULL &&
						current_ltc->sensors[ch_idx]->type != 0)
				{
					int read_status = ltc298x_read_channel_temp(
							&current_ltc->dev_instance,
							ch_idx + 1,
							&current_ltc->temperatures[ch_idx]
					);

					if (read_status != 0) {
						current_ltc->temperatures[ch_idx] = -999.0f;
					}
				}
				else
				{
					// (選用) 如果通道關閉，強制歸零
					//							current_ltc->temperatures[ch_idx] = 0.0f;
				}
			}

			if (found_count >= num_devices_to_find) {
				break;
			}
		}
	}
	return 0;
}

//int API_LTC2983_ReadMultipleTemperatures(ltc2983_device_manager devices[], unsigned char num_devices_to_find, unsigned char array_size)
//{
//    if (!devices || num_devices_to_find == 0) {
//        return -1;
//    }
//
//    unsigned char found_count = 0;
//
//    // 遍歷所有設備
//    for (int i = 0; i < array_size; i++)
//    {
//        ltc2983_device_manager *current_ltc = &devices[i];
//
//        // 只有當 SPI Index 有設定時才處理
//        if (current_ltc->dev_instance.spi_idx != 0)
//        {
//            found_count++;
//
//            // 遍歷該設備的所有通道 (0~19)
//            for (int ch_idx = 0; ch_idx < 20; ch_idx++)
//            {
//                // 只針對有設定的通道 (非 NULL) 進行測量
//                if (current_ltc->sensors[ch_idx] != NULL)
//                {
//                    // ====================================================
//                    // [步驟 1] 踩油門：發送轉換指令 (Kick)
//                    // ====================================================
//                    // 0x80 (Bit 7) = Start Conversion
//                    // (ch_idx + 1) = 通道編號 (1~20)
//                    uint8_t cmd = 0x80 | (ch_idx + 1);
//
//                    // 寫入 Command Status Register (0x000)
//                    ltc298x_spi_rw(&current_ltc->dev_instance, LTC298X_SPI_WRITE_BYTE, 0x000, &cmd, 1);
//
//                    // ====================================================
//                    // [步驟 2] 等紅燈：輪詢等待轉換完成 (Polling)
//                    // ====================================================
//                    // 我們不盲目 delay，而是不斷問晶片 "好了沒?"
//                    // 這樣最穩定，不會讀到舊資料
//
//                    uint32_t start_tick = HAL_GetTick();
//                    uint8_t status = 0xFF;
//
//                    do {
//                        // 讀取狀態暫存器 (0x000)
//                        ltc298x_spi_rw(&current_ltc->dev_instance, LTC298X_SPI_READ_BYTE, 0x000, &status, 1);
//
//                        // 判斷條件：
//                        // Bit 7 (Start) 必須為 0 (代表不忙了)
//                        // Bit 6 (Done)  必須為 1 (代表量好了)
//                        // 0x40 = 0100 0000
//                        if ((status & 0xC0) == 0x40)
//                        {
//                            break; // 完成！跳出等待迴圈
//                        }
//
//                        // 超時保護 (Timeout)：避免硬體壞掉時程式卡死在這裡
//                        // LTC2983 轉換時間通常 < 200ms，我們給它 300ms 寬限
//                        if (HAL_GetTick() - start_tick > 300) {
//                            // (選用) 可以在這裡印出錯誤訊息 GUI_DispString("Timeout");
//                            break;
//                        }
//
//                        // 稍微讓出 CPU，避免輪詢太密集佔用 Bus (可選)
//                        // HAL_Delay(1);
//
//                    } while (1);
//
//                    // ====================================================
//                    // [步驟 3] 收成：讀取結果 (Harvest)
//                    // ====================================================
//                    // 呼叫原本的讀取函式，這時候暫存器裡已經是最新的溫度了
//                    int read_status = ltc298x_read_channel_temp(
//                            &current_ltc->dev_instance,
//                            ch_idx + 1,
//                            &current_ltc->temperatures[ch_idx]
//                    );
//
//                    // 錯誤處理
//                    if (read_status != 0) {
//                        current_ltc->temperatures[ch_idx] = -999.0f;
//                    }
//
//                    // 注意：原本的 HAL_Delay(100) 已經不需要了，
//                    // 因為上面的 [步驟 2] 已經精準等待過了。
//                }
//            }
//
//            if (found_count >= num_devices_to_find) {
//                break;
//            }
//        }
//    }
//    return 0;
//}

/**
 */
int API_TMP117_ReadMultipleTemperatures(tmp117_sensor sensors[], unsigned char num_sensors_to_find, unsigned char array_size)
{
	if (num_sensors_to_find == 0 || !sensors) return 0;

	const tmp117_sensor *context_sensor = NULL;
	for (int i = 0; i < array_size; i++) {
		if (sensors[i].i2c_address != 0) {
			context_sensor = &sensors[i];
			break;
		}
	}
	if (context_sensor == NULL) return 0;

	unsigned short channel_mask = 0;
	unsigned char found_count = 0;
	for (int i = 0; i < array_size; i++) {
		if (sensors[i].i2c_address != 0) {
			channel_mask |= (1 << sensors[i].channel);
			found_count++;
			if (found_count >= num_sensors_to_find) {
				break;
			}
		}
	}

	if (channel_mask == 0)
		return 0;

	int status = TMP117_StartConversion(context_sensor, channel_mask);
	if (status != 0)
		return -10;

	status = TMP117_CheckTransactionStatus(context_sensor, channel_mask, 500);
	if (status != 0)
		//		return status;
		HAL_Delay(1);


	unsigned char ordered_temp_buffer[32];
	status = TMP117_ReadAllTempData_Ordered(context_sensor, ordered_temp_buffer);
	if (status != 0)
		return status;

	found_count = 0;
	for (int i = 0; i < array_size; i++) {
		if (sensors[i].i2c_address != 0) {
			unsigned char channel = sensors[i].channel;
			unsigned short raw_value = (ordered_temp_buffer[channel * 2] << 8) | ordered_temp_buffer[channel * 2 + 1];
			sensors[i].temperature_C = TMP117_ConvertToCelsius(raw_value);
			found_count++;
			if (found_count >= num_sensors_to_find) {
				break;
			}
		}
	}

	return 0;
}


/**
 * @brief  一個持續更新 LCD 顯示溫度的任務函式。
 * @param  font_size - 用於顯示溫度的字體大小 (例如 12, 16, 24, 32)。
 * @retval None
 */
void Update_LCD_Task(uint8_t font_size)
{
	char lcd_buffer[50];

	// 將 nowY 設為內部變數，並給定一個起始值
	uint16_t nowY = 20;
	// 根據字體大小計算行高，增加一點間距
	uint16_t line_height = font_size + 4;

	FRONT_COLOR = BLACK;
	BACK_COLOR = WHITE;

	// 清除溫度顯示區域 (從起始 Y 往下清除足夠的高度)
	LCD_Clear(WHITE);

	char result_str1[sizeof(unsigned long) + 1];

	if (ulong_to_ascii_union(API0.fpga_version, result_str1, sizeof(result_str1)) != NULL) {
		//          printf("範例 1 成功：\n");
		//          printf("  - 原始 Hex 值: 0x%lX\n", val1);
		//          printf("  - 轉換後 ASCII: %s\n\n", result_str1);
	} else {
		//          printf("範例 1 轉換失敗！\n\n");
	}

	sprintf(lcd_buffer, "FPGA Version:%s",
			result_str1);
	LCD_ShowString(10, nowY, 400, font_size, font_size, (uint8_t*)lcd_buffer);
	nowY += line_height;

	// 顯示 TMP117 溫度
	for (int i = 0; i < MAX_TMP117_SENSORS; i++) {
		if (API0.tmp117_sensors[i].i2c_address != 0) {
			sprintf(lcd_buffer, "TMP117 CH%02d: %.2f C",
					API0.tmp117_sensors[i].channel,
					API0.tmp117_sensors[i].temperature_C);

			// 使用 nowY 來決定顯示位置
			LCD_ShowString(10, nowY, 400, font_size, font_size, (uint8_t*)lcd_buffer);
			// 更新 nowY 到下一行的位置
			nowY += line_height;
		}
	}

	// 顯示 LTC2983 溫度
	for (int i = 0; i < API0.num_ltc2983_devices; i++) {
		if (API0.ltc2983_devices[i].dev_instance.spi_idx != 0) {
			for (int ch = 0; ch < 20; ch++) {
				if (API0.ltc2983_devices[i].sensors[ch] != NULL) {
					sprintf(lcd_buffer, "LTC2983[%d] CH%02d: %.2f C",
							i,
							ch + 1,
							API0.ltc2983_devices[i].temperatures[ch]);

					// 繼續使用 nowY 往下顯示
					LCD_ShowString(10, nowY, 400, font_size, font_size, (uint8_t*)lcd_buffer);
					// 更新 nowY 到下一行的位置
					nowY += line_height;
				}
			}
		}
	}
}

/**
 * @brief 使用 union 將一個 unsigned long 的記憶體內容解讀為一個 ASCII 字串。
 * * @param value 要轉換的 unsigned long 數值。
 * @param dest_buffer 用於儲存結果字串的目標緩衝區。
 * @param buffer_size 目標緩衝區的大小。必須至少為 sizeof(unsigned long) + 1。
 * @return 成功時返回指向 dest_buffer 的指標，若緩衝區太小則返回 NULL。
 */
char* ulong_to_ascii_union(unsigned long value, char* dest_buffer, size_t buffer_size) {
	// 1. 安全檢查：確保目標緩衝區足夠大，以容納所有字元和一個結尾的 '\0'
	if (buffer_size < sizeof(unsigned long) + 1) {
		return NULL; // 緩衝區太小，返回 NULL 表示失敗
	}

	// 2. 定義一個 union 來重新解讀記憶體
	union ValueConverter {
		unsigned long ul_value;
		char char_array[sizeof(unsigned long)];
	};

	// 3. 執行轉換
	union ValueConverter converter;
	converter.ul_value = value; // 將值存入 union 的 long 成員

	// 4. 從 union 的 char 陣列成員讀取資料，並複製到目標緩衝區
	// memcpy 是一個高效的記憶體複製函式
	memcpy(dest_buffer, converter.char_array, sizeof(unsigned long));

	// 5. 在字串結尾加上 null terminator
	dest_buffer[sizeof(unsigned long)] = '\0';

	// 6. 返回指向結果字串的指標
	return dest_buffer;
}


void GPIO_Set_O(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
// --- *** 圖表初始化函式 *** ---
/**
 * @brief  初始化 STemWin 圖表 (Graph Widget)
 * @retval None
 */
void API_Graph_Init_OnlyBKColor(void)
{
	API0.graph_y_min = Y_MIN_TEMP;
	API0.graph_y_max = Y_MAX_TEMP;

	GUI_Init();
	GUI_SetBkColor(GUI_YELLOW);
	GUI_Clear();
}

// --- *** 圖表初始化函式 *** ---
/**
 * @brief  初始化 STemWin 圖表 (Graph Widget)
 * @retval None
 */
void API_Graph_Init(void)
{
	API0.graph_y_min = Y_MIN_TEMP;
	API0.graph_y_max = Y_MAX_TEMP;

	GUI_Init();
	GUI_SetBkColor(GUI_YELLOW);
	GUI_Clear();
	// 設定 GRAPH Widget
	hGraph = GRAPH_CreateEx(GRAPH_X_START, GRAPH_Y_START, GRAPH_WIDTH, GRAPH_HEIGHT, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_GRAPH0);
	GRAPH_SetBorder(hGraph, 30, 5, 5, 20); // (左, 上, 右, 下)
	GRAPH_SetColor(hGraph, GUI_WHITE, GRAPH_CI_BK);     // 背景色
	GRAPH_SetColor(hGraph, GUI_GRAY, GRAPH_CI_BORDER);  // 邊框色
	GRAPH_SetGridVis(hGraph, 1);                        // 顯示網格
	GRAPH_SetGridDistX(hGraph, 50);                     // X 軸網格間距
	GRAPH_SetGridDistY(hGraph, 39);                     // Y 軸網格間距
	GRAPH_SetGridFixedX(hGraph, 1);                     // 固定 X 軸網格

	// *** 迴圈：建立資料物件 ***
	for (int i = 0; i < MAX_TMP117_SENSORS; i++)
	{
		if (API0.tmp117_sensors[i].i2c_address != 0)
		{
			// 建立資料物件 (YT 模式)
			_ahData[i] = GRAPH_DATA_YT_Create(_aSensorColors[i % NUM_SENSOR_COLORS],
					MAX_TEMP_POINTS,
					0, 0);
			GRAPH_AttachData(hGraph, _ahData[i]);
		}
	}

	//	// --- *** 建立 LTC[0] 的資料物件 *** ---
	//	int dev_idx = 0;
	//	for (int ch_idx = 0; ch_idx < 20; ch_idx++)
	//	{
	//		// 檢查 LTC 感測器是否被定義 (這會找到 [0][4], [0][8], [0][9], [0][19])
	//		if (API0.ltc2983_devices[dev_idx].sensors[ch_idx] != NULL)
	//		{
	//			// 計算統一索引
	//			// (dev_idx 永遠是 0)
	//			int graph_idx = 0 + (dev_idx * 20) + ch_idx;
	//			// 例如： 16 + (0*20) + 4 = 索引 20
	//			// 例如： 16 + (0*20) + 8 = 索引 24
	//
	//			_ahData[graph_idx] = GRAPH_DATA_YT_Create(_aSensorColors[graph_idx % NUM_SENSOR_COLORS],
	//					MAX_TEMP_POINTS,
	//					0, 0);
	//			GRAPH_AttachData(hGraph, _ahData[graph_idx]);
	//		}
	//	}

	// 1. 建立垂直 (Y 軸) 刻度
	_hScaleV = GRAPH_SCALE_Create(25, GUI_TA_RIGHT | GUI_TA_VCENTER, GRAPH_SCALE_CF_VERTICAL, 39);
	GRAPH_SCALE_SetFont(_hScaleV, GUI_FONT_6X8);
	GRAPH_SCALE_SetTextColor(_hScaleV, GUI_BLACK);
	GRAPH_AttachScale(hGraph, _hScaleV);

	// 2. 設定 Y 軸範圍 (20~30 度)
	//	float Y_MIN_TEMP = 0.0f;
	//	float Y_MAX_TEMP = 30.0f;
	float Y_RANGE = API0.graph_y_max - API0.graph_y_min; // 10.0
	if (Y_RANGE <= 0) Y_RANGE = 1.0f; // 防止除以零

	// 舊程式碼
	// GRAPH_SCALE_SetOff(_hScaleV, -1*GRAPH_HEIGHT*(API0.graph_y_min/Y_RANGE));
	// GRAPH_SCALE_SetFactor(_hScaleV, (Y_RANGE / (float)GRAPH_HEIGHT));

	// 新程式碼
	float data_height = (float)(GRAPH_HEIGHT - 5 - 20); // 扣除上下邊框 (5+20)
	GRAPH_SCALE_SetOff(_hScaleV, -1 * data_height * (API0.graph_y_min / Y_RANGE));
	GRAPH_SCALE_SetFactor(_hScaleV, (Y_RANGE / data_height));
	GRAPH_SCALE_SetNumDecs(_hScaleV, 1); // 鎖定為 0 位小數
	//	WM_SetBkColor(_hScaleV, GUI_WHITE);

	// 設定水平 (X 軸) 刻度
	_hScaleH = GRAPH_SCALE_Create(GRAPH_HEIGHT - 18, GUI_TA_HCENTER | GUI_TA_TOP, GRAPH_SCALE_CF_HORIZONTAL, 50);
	GRAPH_SCALE_SetFont(_hScaleH, GUI_FONT_6X8);
	GRAPH_SCALE_SetTextColor(_hScaleH, GUI_BLACK);
	GRAPH_AttachScale(hGraph, _hScaleH);
	//	GRAPH_SCALE_SetNumDecs(_hScaleH, 0);
}


// --- *** 圖表更新任務 *** ---
/**
 * @brief  STemWin 圖表的更新迴圈 (應在 while(1) 中呼叫)
 * @retval None
 */
void API_RunLineChartLogic(void)
{

	WM_ShowWindow(hGraph); // 關鍵：顯示 GRAPH 元件，以便繪製折線圖
	// 1. 顯示文字資訊
	GUI_SetFont(GUI_FONT_16_ASCII);
	GUI_SetBkColor(GUI_WHITE); // 設定文字區域的背景色
	GUI_ClearRect(0, 0, XSIZE_PHYS - 1, GRAPH_Y_START - 1); // 清除圖表上方的文字區域

	char lcd_buffer[60];
	int x_pos = 10;
	int y_pos = 5;


	// *** 迴圈顯示所有 TMP117 溫度 ***
	for (int i = 0; i < MAX_TMP117_SENSORS; i++)
	{
		if (API0.tmp117_sensors[i].i2c_address != 0) {
			GUI_SetColor(_aSensorColors[i % NUM_SENSOR_COLORS]);
			//			sprintf(lcd_buffer, "CH%02d: %.2f C",
			//					API0.tmp117_sensors[i].channel,
			//					API0.tmp117_sensors[i].temperature_C);
			float temp = API0.tmp117_sensors[i].temperature_C;

			// [修改 3] 判斷是否為 0.0 (故障)，若故障則顯示 Error
			if (temp > -0.001f && temp < 0.001f) {
				sprintf(lcd_buffer, "CH%02d: Error", API0.tmp117_sensors[i].channel);
			}
			else {
				sprintf(lcd_buffer, "CH%02d: %.2f C",
						API0.tmp117_sensors[i].channel,
						temp);
			}
			GUI_DispStringAt(lcd_buffer, x_pos, y_pos);

			// 更新下一個文字的位置
			x_pos += GUI_GetStringDistX(lcd_buffer) + 15;
			if (x_pos > (XSIZE_PHYS - 100)) {
				x_pos = 10;
				y_pos += 18;
			}
			if (y_pos >= (GRAPH_Y_START - GUI_GetFontSizeY())) {
				break;
			}
		}
	}
	////	// --- *** 新增：迴圈顯示 LTC[0] 的溫度 *** ---
	//	int dev_idx = 0; // (我們只關心 dev_idx = 0)
	//	if (API0.ltc2983_devices[dev_idx].dev_instance.spi_idx != 0) // 檢查設備[0]是否有效
	//	{
	//		for (int ch_idx = 0; ch_idx < 20; ch_idx++)
	//		{
	//			// 檢查 LTC 感測器是否被定義 (這會找到您定義的 4 個通道)
	//			if (API0.ltc2983_devices[dev_idx].sensors[ch_idx] != NULL)
	//			{
	//				// 計算統一索引
	//				int graph_idx = MAX_TMP117_SENSORS + (dev_idx * 20) + ch_idx;
	//
	//				GUI_SetColor(_aSensorColors[graph_idx % NUM_SENSOR_COLORS]);
	//
	//				sprintf(lcd_buffer, "LTC%d_CH%02d: %.2f C",
	//						dev_idx,
	//						ch_idx + 1, // LTC 通道是 1-based (ch_idx=4 顯示 CH05)
	//						API0.ltc2983_devices[dev_idx].temperatures[ch_idx]);
	//
	//				GUI_DispStringAt(lcd_buffer, x_pos, y_pos);
	//
	//				// (複製貼上 x_pos, y_pos 的更新邏輯)
	//				x_pos += GUI_GetStringDistX(lcd_buffer) + 15;
	//				if (x_pos > (XSIZE_PHYS - 100)) {
	//					x_pos = 10;
	//					y_pos += 18;
	//				}
	//				if (y_pos >= (GRAPH_Y_START - GUI_GetFontSizeY())) {
	//					goto text_display_done; // 空間不足，跳出
	//				}
	//			}
	//		}
	//	}
	//	text_display_done: // goto 的標籤

	//
	// 階段 1：第一輪迴圈 - 僅檢查是否需要重新縮放
	// (遍歷所有感測器，找出「真正的」最大和最小值)
	//
	int needs_rescale = 0;
	float new_min = API0.graph_y_min;
	float new_max = API0.graph_y_max;

	for (int i = 0; i < MAX_TMP117_SENSORS; i++)
	{
		// 僅檢查有效的感測器和有效的溫度
		if (API0.tmp117_sensors[i].i2c_address != 0 && API0.tmp117_sensors[i].temperature_C > -270.0f)
		{
			// (不過濾 skip counter，我們需要所有點來決定範圍)
			float temp = API0.tmp117_sensors[i].temperature_C;
			//忽略0度
			if (temp > -0.001f && temp < 0.001f) {
				continue;
			}

			if(_aDataSkipCounter[i] <2)
				continue;
			if (temp < new_min) {
				new_min = temp;
				needs_rescale = 1;
			}
			if (temp > new_max) {
				new_max = temp;
				needs_rescale = 1;
			}
		}
	}

	//
	// 階段 2：在迴圈 *外部*，執行一次重新縮放 (如果需要)
	//
	if (needs_rescale)
	{
		// 1. 設定新的 Y 軸範圍
		API0.graph_y_min = new_min - 0.1f; // 比最小值再低 0.1 度
		API0.graph_y_max = new_max + 0.1f; // 比最大值再高 0.1 度

		// (確保最小範圍，例如至少 5 度)
		//		if ((API0.graph_y_max - API0.graph_y_min) < 5.0f) {
		//			API0.graph_y_max = API0.graph_y_min + 5.0f;
		//		}

		// 2. 重新計算 Y_RANGE
		float Y_RANGE = API0.graph_y_max - API0.graph_y_min;
		if (Y_RANGE <= 0) Y_RANGE = 1.0f; // 防止除以零

		// 3. 更新 Y 軸 *標籤*
		//		GRAPH_SCALE_SetOff(_hScaleV, -1 * GRAPH_HEIGHT * (API0.graph_y_min / Y_RANGE));
		//		GRAPH_SCALE_SetFactor(_hScaleV, (Y_RANGE / (float)GRAPH_HEIGHT));

		// 4. (強健性) 檢查 _hScaleV 是否有效，然後 *只執行一次* Invalidate
		// ... 在 API_Graph_UpdateTask 函式中 ...

		// 3. 從圖表中分離並刪除 *舊* 的 Y 軸物件
		if (_hScaleV) {
			GRAPH_DetachScale(hGraph, _hScaleV);
			GRAPH_SCALE_Delete(_hScaleV);
		}

		// --- [修正] 強制清除舊刻度 ---
		// 1. 切換繪圖目標為 hGraph 視窗 (這樣 (0,0) 就是圖表左上角)
		WM_HWIN hOldWin = WM_SelectWindow(hGraph);

		// 2. 設定顏色為邊框色 (GUI_YELLOW)
		GUI_SetColor(GUI_YELLOW);

		// 3. 填滿左側邊框區域，物理上蓋掉舊的數字
		//    邊框寬度設定為 30，所以清除 0~29
		GUI_FillRect(0, 0, 29, GRAPH_HEIGHT - 1);

		// 4. 切換回原本的視窗，以免影響後續繪圖
		WM_SelectWindow(hOldWin);
		// ---------------------------


		// 4. 重新建立一個 *新* 的 Y 軸物件 (複製自 API_Graph_Init)
		_hScaleV = GRAPH_SCALE_Create(25, GUI_TA_RIGHT | GUI_TA_VCENTER, GRAPH_SCALE_CF_VERTICAL, 39);
		GRAPH_SCALE_SetFont(_hScaleV, GUI_FONT_6X8);
		GRAPH_SCALE_SetTextColor(_hScaleV, GUI_BLACK);
		GRAPH_AttachScale(hGraph, _hScaleV);

		// 5. 使用 *新* 的範圍來設定這個 *新* 的物件
		GRAPH_SCALE_SetOff(_hScaleV, -1 * GRAPH_HEIGHT * (API0.graph_y_min / Y_RANGE));
		GRAPH_SCALE_SetFactor(_hScaleV, (Y_RANGE / (float)GRAPH_HEIGHT));
		GRAPH_SCALE_SetNumDecs(_hScaleV, 1); //
		// 5. *** 關鍵：清除所有舊的圖形資料 ***
		// (使用 'j' 避免迴圈變數混淆)
		for (int j = 0; j < MAX_TMP117_SENSORS; j++) {
			if (_ahData[j]) { // 檢查控制代碼是否有效
				GRAPH_DATA_YT_Clear(_ahData[j]); //
			}
		}


	}

	//
	// 階段 3：第二輪迴圈 - 將 *當前* 溫度值加入圖表
	// (使用在階段 2 中可能已更新的 Y 軸範圍)
	//
	float Y_MIN_CURRENT = API0.graph_y_min;
	float Y_RANGE_CURRENT = API0.graph_y_max - Y_MIN_CURRENT;
	if (Y_RANGE_CURRENT <= 0) Y_RANGE_CURRENT = 1.0f;

	for (int i = 0; i < MAX_TMP117_SENSORS; i++)
	{
		if (API0.tmp117_sensors[i].i2c_address != 0) {
			if(API0.tmp117_sensors[i].temperature_C > -270.0f) { //

				// --- 如果是 0.0，不要畫圖 ---
				float current_temp = API0.tmp117_sensors[i].temperature_C;
				if (current_temp > -0.001f && current_temp < 0.001f) {
					continue;
				}

				// 過濾前 2 筆 (這個邏輯現在在 rescale 後會重新開始)
				if (_aDataSkipCounter[i] < 2)
				{
					_aDataSkipCounter[i]++;
					continue;
				}


				// 映射 Y 軸
				float mapped_temp = (current_temp - Y_MIN_CURRENT);
				I16 graph_val = (I16)((mapped_temp / Y_RANGE_CURRENT) * (float)GRAPH_HEIGHT);

				// 限制範圍
				if (graph_val < 0) graph_val = 0; //
				if (graph_val > GRAPH_HEIGHT) graph_val = GRAPH_HEIGHT; //

				// 添加資料點
				GRAPH_DATA_YT_AddValue(_ahData[i], graph_val); //
			}
		}
	}



	//	// --- *** 新增：迴圈更新 LTC[0] 的資料 *** ---
	//	// (dev_idx = 0)
	//	if (API0.ltc2983_devices[dev_idx].dev_instance.spi_idx != 0)
	//	{
	//		for (int ch_idx = 0; ch_idx < 20; ch_idx++)
	//		{
	//			// 檢查 LTC 感測器是否被定義
	//			if (API0.ltc2983_devices[dev_idx].sensors[ch_idx] != NULL)
	//			{
	//				// 計算統一索引
	////				int graph_idx = MAX_TMP117_SENSORS + (dev_idx * 20) + ch_idx;
	//				int graph_idx =  (dev_idx * 20) + ch_idx;
	//
	//				// 檢查溫度是否有效
	//				if(API0.ltc2983_devices[dev_idx].temperatures[ch_idx] > -270.0f) {
	//
	//					// (使用統一索引的計數器)
	//					if (_aDataSkipCounter[graph_idx] < 2)
	//					{
	//						_aDataSkipCounter[graph_idx]++;
	//						continue;
	//					}
	//
	//					// (複製貼上 Y 軸映射邏輯)
	//					float current_temp = API0.ltc2983_devices[dev_idx].temperatures[ch_idx];
	////					float Y_MIN_TEMP = 0.0f;
	////					float Y_MAX_TEMP = 30.0f;
	//					float Y_RANGE = Y_MAX_TEMP - Y_MIN_TEMP;
	//					float mapped_temp = (current_temp - Y_MIN_TEMP);
	//					I16 graph_val = (I16)((mapped_temp / Y_RANGE) * (float)GRAPH_HEIGHT);
	//					if (graph_val < 0) graph_val = 0;
	//					if (graph_val > GRAPH_HEIGHT) graph_val = GRAPH_HEIGHT;
	//
	//					GRAPH_DATA_YT_AddValue(_ahData[graph_idx], graph_val);
	//				}
	//			}
	//		}
	//	}
	// --- *** 結束 *** ---

	//
	// STemWin 會自動重繪 Widget
	//
	GUI_Delay(1); // 延遲 1 毫秒
}



/**
 * @brief  將當前的溫度資料以靜態直方圖 (Bar Chart) 的形式繪製出來
 * @retval None
 */
void API_Graph_DrawBarChart(void)
{
	char lcd_buffer[60];
	WM_HideWindow(hGraph); // 隱藏舊的 GRAPH 元件，防止它重繪蓋掉我們的圖

	// === 1. 顯示頂端文字 (邏輯不變) ===
	GUI_SetFont(GUI_FONT_16_ASCII);
	GUI_SetBkColor(GUI_WHITE);
	GUI_ClearRect(0, 0, XSIZE_PHYS - 1, GRAPH_Y_START - 1);

	int x_pos = 10;
	int y_pos = 5;

	for (int i = 0; i < MAX_TMP117_SENSORS; i++)
	{
		if (API0.tmp117_sensors[i].i2c_address != 0) {
			GUI_SetColor(_aSensorColors[i % NUM_SENSOR_COLORS]);
			sprintf(lcd_buffer, "CH%02d: %.2f C",
					API0.tmp117_sensors[i].channel,
					API0.tmp117_sensors[i].temperature_C);
			GUI_DispStringAt(lcd_buffer, x_pos, y_pos);

			x_pos += GUI_GetStringDistX(lcd_buffer) + 15;
			if (x_pos > (XSIZE_PHYS - 100)) {
				x_pos = 10;
				y_pos += 18;
			}
			if (y_pos >= (GRAPH_Y_START - GUI_GetFontSizeY())) {
				break;
			}
		}
	}

	// === 2. 清除並繪製圖表背景和邊框 ===
	GUI_SetBkColor(GUI_WHITE);
	GUI_ClearRect(GRAPH_X_START - 30, GRAPH_Y_START,
			GRAPH_X_START + GRAPH_WIDTH, GRAPH_Y_START + GRAPH_HEIGHT + 20);

	GUI_SetColor(GUI_GRAY);
	GUI_DrawRect(GRAPH_X_START, GRAPH_Y_START,
			GRAPH_X_START + GRAPH_WIDTH - 1, GRAPH_Y_START + GRAPH_HEIGHT - 1);

	// === 3. *** (改良) *** 自動繪製 Y 軸刻度與網格線 ===
	// (使用 API.h 中定義的 Y_MIN_TEMP 和 Y_MAX_TEMP)
	float Y_RANGE = Y_MAX_TEMP - Y_MIN_TEMP;
	GUI_SetFont(GUI_FONT_6X8);
	GUI_SetColor(GUI_BLACK);
	GUI_SetBkColor(GUI_WHITE);

	// 假設我們想畫 4 條網格線 (不含底部和頂部)
	const int num_grid_lines = 4;
	for (int i = 1; i <= num_grid_lines; i++) {
		// 計算該網格線代表的溫度
		float temp = Y_MIN_TEMP + (Y_RANGE * i / (num_grid_lines + 1));
		// 計算 Y 像素位置
		int y_pixel_offset = (int)(((float)GRAPH_HEIGHT * i) / (num_grid_lines + 1));
		int y = GRAPH_Y_START + GRAPH_HEIGHT - y_pixel_offset;

		// 畫網格線
		GUI_SetColor(GUI_LIGHTGRAY);
		GUI_DrawHLine(y, GRAPH_X_START, GRAPH_X_START + GRAPH_WIDTH - 1);

		// 畫 Y 軸標籤
		GUI_SetColor(GUI_BLACK);
		GUI_SetBkColor(GUI_WHITE);
		sprintf(lcd_buffer, "%.1f", temp);
		GUI_DispStringAt(lcd_buffer, GRAPH_X_START - 25, y - 4);
	}
	// 繪製 Y 軸 0 點 (Y_MIN_TEMP)
	sprintf(lcd_buffer, "%.1f", Y_MIN_TEMP);
	GUI_DispStringAt(lcd_buffer, GRAPH_X_START - 25, GRAPH_Y_START + GRAPH_HEIGHT - 8);


	// === 4. 統計啟用的感測器數量 ===
	int active_sensor_count = 0;
	for (int i = 0; i < MAX_TMP117_SENSORS; i++) {
		if (API0.tmp117_sensors[i].i2c_address != 0) {
			active_sensor_count++;
		}
	}
	if (active_sensor_count == 0) {
		GUI_Delay(100);
		return; // 沒有感測器，直接返回
	}

	// === 5. 計算並繪製長條圖 ===
	int bar_slot_width = GRAPH_WIDTH / active_sensor_count;
	int bar_width = (bar_slot_width * 8) / 10;
	if (bar_width < 1) bar_width = 1;
	int bar_padding_left = (bar_slot_width - bar_width) / 2;

	int current_sensor_idx = 0;
	for (int i = 0; i < MAX_TMP117_SENSORS; i++) {
		if (API0.tmp117_sensors[i].i2c_address != 0) {

			float current_temp = API0.tmp117_sensors[i].temperature_C;

			// --- *** 關鍵修正：檢查溫度是否有效 *** ---
			if (current_temp > -270.0f)
			{
				// 1. 設定顏色
				GUI_SetColor(_aSensorColors[i % NUM_SENSOR_COLORS]);

				// 2. 映射 溫度
				float mapped_temp = (current_temp - Y_MIN_TEMP);
				int bar_height_pixels = (int)((mapped_temp / Y_RANGE) * (float)GRAPH_HEIGHT);

				// 3. 限制高度範圍
				if (bar_height_pixels < 0) bar_height_pixels = 0;
				if (bar_height_pixels > GRAPH_HEIGHT) bar_height_pixels = GRAPH_HEIGHT;

				// 4. 計算長條的座標
				int x0 = GRAPH_X_START + (current_sensor_idx * bar_slot_width) + bar_padding_left;
				int y0 = GRAPH_Y_START + GRAPH_HEIGHT - bar_height_pixels; // 頂部 Y
				int x1 = x0 + bar_width - 1;
				int y1 = GRAPH_Y_START + GRAPH_HEIGHT - 1; // 底部 Y

				// 5. 繪製長條
				GUI_FillRect(x0, y0, x1, y1);
			}
			// --- *** 結束修正 *** ---

			// 6. 繪製 X 軸標籤 (無論是否有效都繪製)
			// (計算 x0 和 bar_width 需要移到 if 之外)
			int x0_label = GRAPH_X_START + (current_sensor_idx * bar_slot_width) + bar_padding_left;
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(GUI_WHITE);
			sprintf(lcd_buffer, "CH%02d", API0.tmp117_sensors[i].channel);
			int text_x = x0_label + (bar_width / 2) - (GUI_GetStringDistX(lcd_buffer) / 2);
			int text_y = GRAPH_Y_START + GRAPH_HEIGHT + 2;
			GUI_DispStringAt(lcd_buffer, text_x, text_y);

			current_sensor_idx++;
		}
	}

	GUI_Delay(100); // 延遲 100 毫秒
}

// --- [新增] 圖表模式管理變數 ---
// 0: 折線圖 (Line Chart), 1: 長條圖 (Bar Chart)
static int g_ChartMode = 0;
/*
// --- [新增] USB 指令處理與模式切換 ---
void API_USB_ProcessRx(uint8_t* Buf, uint32_t Len) {
    if (Len < 7) return; // 忽略太短的雜訊

    // 指令：切換到折線圖
    if (strncmp((char*)Buf, "CMD:LINE", 8) == 0) {
        if (g_ChartMode != 0) {
            g_ChartMode = 0;
            GUI_Clear();      // 清除畫面
            API_Graph_Init(); // 重新初始化折線圖
        }
    }
    // 指令：切換到長條圖
    else if (strncmp((char*)Buf, "CMD:BAR", 7) == 0) {
        if (g_ChartMode != 1) {
            g_ChartMode = 1;
            // 刪除折線圖視窗，釋放資源並避免干擾
            if (hGraph) {
                WM_DeleteWindow(hGraph);
                hGraph = 0; // 把 Handle 清零
            }
            GUI_Clear(); // 清除畫面準備畫 Bar
        }
    }
}
 */

// [修改] 支援手動打字的指令處理函式
#define CMD_BUFFER_SIZE 64
static char rx_buffer[CMD_BUFFER_SIZE]; // 靜態緩衝區
static int rx_index = 0;                // 目前存到第幾個字

void API_USB_ProcessRx(uint8_t* Buf, uint32_t Len)
{
	// 遍歷收到的每一個字元
	for (uint32_t i = 0; i < Len; i++) {
		char c = (char)Buf[i];

		// 檢查是否為結束符號 (Enter鍵: \r 或 \n)
		if (c == '\r' || c == '\n') {
			// 如果緩衝區有資料，就進行比對
			if (rx_index > 0) {
				rx_buffer[rx_index] = '\0'; // 補上字串結尾

				// --- UI 顯示設定 ---
				GUI_SetFont(GUI_FONT_20_ASCII);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_SetColor(GUI_RED);

				// --- 開始比對指令 ---

				// 1. 切換圖表模式指令
				if (strncmp(rx_buffer, "CMD:LINE", 8) == 0)
				{
					g_ChartMode = 0;
					GUI_DispStringAt("DEBUG: Switch to LINE ", 10, 200);
				}
				else if (strncmp(rx_buffer, "CMD:BAR", 7) == 0)
				{
					g_ChartMode = 1;
					GUI_DispStringAt("DEBUG: Switch to BAR  ", 10, 200);
				}
				// 2. 設定 TMP117 指令
				else if (strncmp(rx_buffer, "CMD:SET_117", 11) == 0)
				{
					int ch, addr;
					// 建議：直接從第 11 個字元開始解析參數，略過指令頭
					if (sscanf(rx_buffer + 11, "%d %x", &ch, &addr) == 2)
					{
						if (ch >= 0 && ch < 16)
						{
							API0.tmp117_sensors[ch].temperature_C = 0.0f;
						}
						API_ReInit_Single_Sensor(ch, addr);
						char debug_msg[50];
						sprintf(debug_msg, "SET_117 CH:%d ADDR:0x%X OK", ch, addr);
						GUI_DispStringAt(debug_msg, 10, 200);
					}
					else
					{
						GUI_DispStringAt("DEBUG: CMD Format Error", 10, 200);
					}
				}
				// 3. 設定 LTC2983 指令
				else if (strncmp(rx_buffer, "CMD:SET_LTC", 11) == 0)
				{
					int dev, ch, type;
					uint32_t config = 0;
					uint32_t data = 0;

					// [修正] 移除原本導致判斷鍊斷裂的 else，並將錯誤處理移至內部
					// sscanf 讀取參數 (略過前 11 個字元的指令頭)
					int args_parsed = sscanf(rx_buffer + 11, "%d %d %d %x %x", &dev, &ch, &type, &config, &data);

					// 至少要讀到 type (3個)
					if (args_parsed >= 3)
					{
						if (dev >= 1 && dev <= MAX_LTC2983_DEVICES && ch >= 1 && ch <= 20)
						{
							int dev_idx = dev - 1;
							int ch_idx = ch - 1;

							if (type == 0)
							{
								// Disable
								memset(&g_LTC_SensorPool[dev_idx][ch_idx], 0, sizeof(ltc_sensor_storage_t));
								GUI_DispStringAt("LTC Ch Disabled", 10, 200);
							}
							else
							{
								// Enable
								API_LTC_Apply_Config(dev_idx, ch_idx, type, config, data);

								char dbg[64];
								sprintf(dbg, "Set:%d Cfg:%X Dat:%X", type, (unsigned int)config, (unsigned int)data);

								GUI_DispStringAt("LTC Updating...", 10, 200);
								GUI_DispStringAt(dbg, 10, 220);

								ltc298x_init(&g_LTC_Devices[dev_idx], &g_LTC_InitParams[dev_idx]);
							}
						}
						else
						{
							GUI_DispStringAt("DEBUG: Invalid Dev/Ch", 10, 200);
						}
					}
					else
					{
						// [修正] 這是 sscanf 解析失敗的 else，放在這裡才對
						GUI_DispStringAt("DEBUG: LTC Format Error", 10, 200);
					}
				}
				// 4. 設定傳送頻率指令
				// [修正] 這個 else if 現在可以正確執行了，因為上方的 if-else 結構已修復
				else if (strncmp(rx_buffer, "CMD:INTERVAL", 12) == 0)
				{
					int ms;
					if (sscanf(rx_buffer + 12, "%d", &ms) == 1)
					{
						if (ms >= 10)
						{
							g_TxInterval = (uint32_t)ms;
							char debug_msg[50];
							sprintf(debug_msg, "SET FREQ: %d ms", ms);
							GUI_DispStringAt(debug_msg, 10, 220);
						}
					}
				}
				// 5. 未知指令
				else {
					char debug_msg[50];
					// 防止字串過長導致 Buffer Overflow，限制印出長度
					snprintf(debug_msg, sizeof(debug_msg), "Unknown: %.10s...", rx_buffer);
					GUI_DispStringAt(debug_msg, 10, 200);
				}

				// --- 指令處理結束，重置 buffer 索引 ---
				rx_index = 0;
			}
		}
		else {
			// [修正] 這個 else 現在正確地包含在 for 迴圈內
			// 如果是一般字元，存入緩衝區
			if (rx_index < CMD_BUFFER_SIZE - 1) {
				rx_buffer[rx_index++] = c;
			} else {
				// 緩衝區滿了，強制重置 (避免溢位)
				rx_index = 0;
			}
		}
	} // [修正] for 迴圈的結束括號在這裡
}

// [新增] 總圖表更新任務 (負責根據模式分發)
void API_Graph_UpdateTask(void)
{
	// 持續向電腦發送數據
	API_USB_SendTelemetry();
	return;
	if (g_ChartMode == 0) {
		// --- 模式 0: 折線圖 ---
		API_RunLineChartLogic(); // 呼叫剛剛被改名的舊函式
	}
	else {
		// --- 模式 1: 長條圖 ---
		// 為了避免折線圖視窗干擾，先隱藏它
		if (hGraph) {
			WM_HideWindow(hGraph);
		}

		API_Graph_DrawBarChart(); // 呼叫長條圖繪製函式
	}
}


void API_USB_SendTelemetry(void)
{
	static uint32_t last_send_time = 0;
	//使用 g_TxInterval 變數來決定傳送頻率
	if (HAL_GetTick() - last_send_time < g_TxInterval) return;
	last_send_time = HAL_GetTick();


	char msg_buffer[2048];
	int offset = 0;

	// 1. JSON 表頭
	//	offset += sprintf(msg_buffer + offset, "{\"data\":[");
	offset += sprintf(msg_buffer + offset, "{\"TMP117\":[");


	int first = 1;
	for (int i = 0; i < MAX_TMP117_SENSORS; i++) {
		// 只傳送有效 (I2C 地址不為 0) 的感測器
		if (API0.tmp117_sensors[i].i2c_address != 0) {
			float temp = API0.tmp117_sensors[i].temperature_C;

			// 加入逗號分隔
			if (!first) {
				offset += sprintf(msg_buffer + offset, ",");
			}

			// [修改重點] 增加 "a" (Address) 欄位
			// 格式: {"c":通道, "a":位址, "v":溫度}
			// c = Channel, a = Address (十進位), v = Value
			// 網頁端收到 "a":72 會自動轉成 0x48 顯示
			offset += sprintf(msg_buffer + offset, "{\"c\":%d,\"a\":%d,\"v\":%.2f}",
					API0.tmp117_sensors[i].channel,      // 通道編號
					API0.tmp117_sensors[i].i2c_address,  // [新增] I2C 位址
					temp);                               // 溫度值
			first = 0;
		}
	}
	// 結束 "117" 陣列，並加上逗號準備接 "ltc"
	offset += sprintf(msg_buffer + offset, "],");

	// =================================================================
	// 2. LTC2983 區塊 (使用 API0.ltc2983_devices 結構)
	// =================================================================
	offset += sprintf(msg_buffer + offset, "\"LTC2983\":[");

	first = 1;

	// 遍歷所有 LTC2983 設備 (通常 MAX_LTC2983_DEVICES 為 1)
	for (int dev = 0; dev < MAX_LTC2983_DEVICES; dev++)
	{
		// 遍歷該晶片的 20 個通道 (Channel 1~20)
		for (int ch = 0; ch < 20; ch++)
		{
			// [修正重點] 取得該通道的指針，方便後續判斷
			struct ltc2983_sensor *sensor = API0.ltc2983_devices[dev].sensors[ch];

			// [判斷有效性]
			// 1. 指針不能為 NULL (基本安全檢查)
			// 2. sensor->type 不能為 0 (這才是判斷是否有設定的關鍵！)
			if (sensor != NULL && sensor->type != 0)
			{
				float ltc_val = API0.ltc2983_devices[dev].temperatures[ch];

				// [過濾無效值] (可選)
				// 您可以決定要不要顯示 -999 (Error)，如果只想顯示正常數值可保留此行
				// 如果連錯誤代碼都想看，可以把這個 if 拿掉
				if (ltc_val > -273.0f)
				{
					if (!first) offset += sprintf(msg_buffer + offset, ",");

					// d: Device ID (1, 2...)
					// c: Channel ID (1~20)
					// v: Value
					offset += sprintf(msg_buffer + offset, "{\"d\":%d,\"c\":%d,\"v\":%.2f}",
							dev + 1,  // 轉成 1-base (LTC_1, LTC_2...)
							ch + 1,   // 轉成 1-base (CH1 ~ CH20)
							ltc_val);
					first = 0;
				}
			}
		}
	}

	// 2. JSON 結尾 + 換行符號
	offset += sprintf(msg_buffer + offset, "]}\r\n");

	// 3. 透過 USB 發送
	// 注意: 確認 CDC_Transmit_FS 能一次發送這麼長的長度，有些實作限制 64 bytes
	// 如果您的 USB 函式庫有自動分包功能則沒問題
	CDC_Transmit_FS((uint8_t*)msg_buffer, offset);
}

// ch: 通道編號
// addr: I2C 地址 (例如 0x48), 如果傳入 0 代表關閉該通道
void API_ReInit_Single_Sensor(int ch, int addr)
{
	// 1. 安全檢查
	if (ch < 0 || ch >= MAX_TMP117_SENSORS) return;

	// =================================================================
	// [關鍵修改] 檢查舊狀態
	// 在被 TMP117_Init 覆蓋之前，先檢查這個位置原本有沒有掛載有效的感測器
	// 假設 i2c_address 為 0 代表無效/未初始化
	// =================================================================
	int was_active = (API0.tmp117_sensors[ch].i2c_address != 0);

	// 2. 初始化結構體 (這一步會覆蓋掉舊的 i2c_address)
	TMP117_Init(&API0.tmp117_sensors[ch], &(struct tmp117_init_param){
		.spi_idx = SPIM_FPGA,
				.cs_idx = 0xff,
				.channel = ch,       // 指定通道
				.i2c_address = addr  // 指定地址
	});

	// 3. 硬體配置與計數器管理 (如果地址有效)
	if (addr != 0) {

		// =============================================================
		// [關鍵修改] 更新總數
		// 只有在「原本沒啟用 (was_active == 0)」且「現在啟用」時，總數才 +1
		// 如果原本就是啟用的 (was_active == 1)，代表只是重置參數，不增加總數
		// =============================================================
		if (!was_active) {
			API0.num_tmp117_sensors++;
		}

		// 嘗試寫入設定到 FPGA
		int ret = TMP117_ConfigureOnFPGA(&API0.tmp117_sensors[ch]);

		// (選用) 可以在這裡順便做一次讀取測試
		// if (ret != 0 || TMP117_ReadTemperature(...) != 0) { ... }
	}
	else {
		// [補充邏輯] 如果傳入 addr == 0 (要關閉這個通道)
		// 且原本是啟用的 (was_active == 1)，則總數應該要扣除
		if (was_active && API0.num_tmp117_sensors > 0) {
			API0.num_tmp117_sensors--;
		}
	}

	// 4. 清除該通道的舊圖表數據 (避免舊線條干擾)
	if (_ahData[ch]) {
		GRAPH_DATA_YT_Clear(_ahData[ch]);
	}

	// 5. 重置過濾計數器
	_aDataSkipCounter[ch] = 0;
}

// [API.c] 修改後的配置套用函式
// 增加 uint32_t data_val 參數
void API_LTC_Apply_Config(int dev_idx, int ch_idx, int type, uint32_t config_val, uint32_t data_val)
{
	// 取得指向 Memory Pool 的指標
	ltc_sensor_storage_t *storage = &g_LTC_SensorPool[dev_idx][ch_idx];

	// 定義不同類型的指標別名方便操作
	struct ltc2983_thermocouple *tc_cfg = &storage->tc;
	struct ltc2983_diode *diode_cfg = &storage->diode;
//	struct ltc2983_rtd *rtd_cfg = &storage->rtd;

	// 1. 清空舊設定
	memset(storage, 0, sizeof(ltc_sensor_storage_t));

	// 2. 設定通用參數
	storage->base.chan = ch_idx + 1; // 轉成 1-based 通道
	storage->base.type = type;

	// 3. 根據類型進行分配
	switch (type)
	{
	// =========================================================
	// Thermocouple (熱電偶)
	// =========================================================
	case LTC2983_THERMOCOUPLE_J:
	case LTC2983_THERMOCOUPLE_K:
	case LTC2983_THERMOCOUPLE_T:
	case LTC2983_THERMOCOUPLE_E:
	case LTC2983_THERMOCOUPLE_N:
	case LTC2983_THERMOCOUPLE_R:
	case LTC2983_THERMOCOUPLE_S:
	case LTC2983_THERMOCOUPLE_B:
		// [Config]: SE, OpenDetect, OC Current
		tc_cfg->sensor_config = config_val;

		// [Data]: Cold Junction Channel (從網頁傳過來的通道號)
		// 如果網頁傳來 0，表示沒有冷接點 (通常 TC 都需要，但也許是測試用)
		tc_cfg->cold_junction_chan = (int)data_val;
		break;

		// =========================================================
		// Diode (二極體)
		// =========================================================
	case LTC2983_DIODE:
		// [Config]: SE, 3-Reading, Average (Bit 0~2)
		// 這裡不需要 mask，因為網頁端已經算好只送這幾位，但加了保險
		diode_cfg->sensor_config = config_val & 0x07;

		// [Data]: Excitation Current (0~3)
		// 直接賦值即可，Driver 層會自己做 << 22
		diode_cfg->excitation_current = data_val & 0x03;
		break;

		// =========================================================
		// RTD (電阻測溫體)
		// =========================================================
//	case LTC2983_RTD_PT_100:
//	case LTC2983_RTD_PT_1000:
//	case LTC2983_RTD_PT_500:
//		// [Config]: 包含 Wires, Current, Curve
//		rtd_cfg->sensor_config = config_val;
//
//		// [Data]: 目前網頁 RTD 傳送的是 0，未來可以用來傳 Rsense Channel
//		// 這裡暫時固定 Sense Resistor 在 Ch2 (或根據您的硬體修改)
//		rtd_cfg->rtd_rsense_chan = 2;
//		break;

	default:
		// 其他未知類型
		tc_cfg->sensor_config = config_val;
		break;
	}
}


void API_LTC_AsyncTask(void)
{
	static int dev_idx = 0; // 用來記錄寫到第幾顆 IC

	switch (g_LTC_State)
	{
	case LTC_STATE_IDLE:
		// 沒事做，直接離開
		break;

	case LTC_STATE_RESET_START:
		// 1. 硬體重置拉低
		LTC298X_RESET(0);
		g_LTC_Timer = HAL_GetTick(); // 記下現在時間
		// 轉態：等待 10ms
		g_LTC_State = LTC_STATE_WAIT_BOOT;
		break;

	case LTC_STATE_WAIT_BOOT:
		// 這裡有兩個階段：
		// A. 前 10ms 維持 Reset Low
		// B. 之後拉高 Reset High，再等 250ms

		// 檢查是否超過 10ms (Reset Pulse Width)
		if (HAL_GetTick() - g_LTC_Timer > 10)
		{
			LTC298X_RESET(1); // 拉高
		}

		// 檢查是否超過 260ms (10ms Reset + 250ms Boot)
		if (HAL_GetTick() - g_LTC_Timer > 260)
		{
			// 暖機完成，準備寫入
			g_LTC_State = LTC_STATE_WRITE_CONFIG;
		}
		break;

		/* 若需要嚴格檢查 Ready bit 可加回來，但通常延遲夠久就可以直接寫 */

	case LTC_STATE_WRITE_CONFIG:
		// 時間到了！開始把 RAM 裡的設定寫入硬體

		for (int i = 0; i < API0.num_ltc2983_devices; i++)
		{
			ltc2983_device_manager *mgr = &API0.ltc2983_devices[i];

			// 這裡我們直接用底層 SPI 寫入，不再呼叫那個會 Delay 的 init 函式
			// 1. 寫入 Channel 設定
			for (int ch = 0; ch < 20; ch++)
			{
				// 取得我們動態池裡的設定
				ltc_sensor_storage_t *storage = &g_LTC_SensorPool[i][ch];

				// 如果 Type 是 0 (Unassigned)，就不用寫，或者寫 0 覆蓋
				// 為了加速，我們可以只寫有設定的
				if (storage->base.type != 0)
				{
					uint32_t config_word = 0;

					// 這裡需要把原本 build_channel_word 的邏輯搬過來
					// 或者直接把 build_channel_word 宣告改成非 static 讓這裡呼叫
					config_word = build_channel_word((struct ltc2983_sensor*)storage);

					if (config_word != 0)
					{
						uint8_t buffer[4];
						buffer[0] = (config_word >> 24) & 0xFF;
						buffer[1] = (config_word >> 16) & 0xFF;
						buffer[2] = (config_word >> 8) & 0xFF;
						buffer[3] = config_word & 0xFF;

					}
				}
			}
		}

		// 全部寫完，收工
		g_LTC_State = LTC_STATE_IDLE;
		// 可以在這裡顯示一行 Debug: "LTC Config Done!"
		break;
	}
}
