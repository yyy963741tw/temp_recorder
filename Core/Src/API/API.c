#include "API.h"
#define MCU_VERSION_STR		"V2.10.02"

API API0;
//static GRAPH_DATA_Handle  _ahData;   // 資�?�物件控?���????�????
GRAPH_DATA_Handle  _ahData[MAX_TMP117_SENSORS];   // *** 修改：改為資料控制代碼的陣列 ***
//GRAPH_DATA_Handle  _ahData_LTC2983[20];   // *** 修改：改為資料控制代碼的陣列 ***


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
	API0.num_tmp117_sensors = 3+4;
	//	API0.num_tmp117_sensors = 2;


	// 3. 在陣列的任意位置，初始化您需要用到的感測器
	//    初始化位於陣列索引 0 的感測器
	TMP117_Init(&API0.tmp117_sensors[0], &(struct tmp117_init_param){
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
	return 0; // 所有 TMP117 都設定成功

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
	if (API_LTC2983_Init() != 0) {
		return -2; // LTC2983 初始化失敗
	}

	if (SG_ECP5_GetVersion(&API0.fpga_version) !=0){
		return -3;
	}
//	GUI_Init();
//	GUI_SetBkColor(GUI_YELLOW);
//	GUI_Clear();

	API_Graph_Init();


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
				if (current_ltc->sensors[ch_idx] != NULL) {
					int read_status = ltc298x_read_channel_temp(
							&current_ltc->dev_instance,
							ch_idx + 1,
							&current_ltc->temperatures[ch_idx]
					);

					if (read_status != 0) {
						current_ltc->temperatures[ch_idx] = -999.0f;
					}
					HAL_Delay(100);
				}
			}

			if (found_count >= num_devices_to_find) {
				break;
			}
		}
	}
	return 0;
}
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
void API_Graph_UpdateTask(void)
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
			sprintf(lcd_buffer, "CH%02d: %.2f C",
					API0.tmp117_sensors[i].channel,
					API0.tmp117_sensors[i].temperature_C);
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

				// 過濾前 2 筆 (這個邏輯現在在 rescale 後會重新開始)
				if (_aDataSkipCounter[i] < 2)
				{
					_aDataSkipCounter[i]++;
					continue;
				}

				float current_temp = API0.tmp117_sensors[i].temperature_C;

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

                // --- 開始比對 ---
                GUI_SetFont(GUI_FONT_20_ASCII);
                GUI_SetBkColor(GUI_YELLOW);
                GUI_SetColor(GUI_RED);

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
                else {
                    // 顯示收到但不認識的指令 (除錯用)
                     char debug_msg[50];
                     sprintf(debug_msg, "Unknown: %s", rx_buffer);
                     GUI_DispStringAt(debug_msg, 10, 200);
                }
                // ----------------

                // 重置索引，準備接收下一條指令
                rx_index = 0;
            }
        }
        else {
            // 如果是一般字元，存入緩衝區
            if (rx_index < CMD_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = c;
            } else {
                // 緩衝區滿了，強制重置 (避免溢位)
                rx_index = 0;
            }
        }
    }
}
