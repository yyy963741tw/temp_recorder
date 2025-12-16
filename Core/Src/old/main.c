/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "eth.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "API/API.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// --- 將 LTC2983 相關變數宣告為全域變數 ---
LTC298X_TEMP ltc2983_inst;

// --- 使用 extern 宣告在其他檔案中定義的感測器配置 ---
extern struct ltc2983_thermocouple ltc2983_thermocouple_k_9;
extern struct ltc2983_diode ltc2983_diode_10;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	MPU_Config();

	API_Init();

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	// 在 main() 函式中，取代現有的 while(1) 迴圈

	/* Infinite loop */
	int test0=0;
	int test1=0;
//	int test2=1;


	while (test0)
	{
		unsigned char tx_buffer[4] = {0x03, 0x00, 0x00, 0x00}; // 指令: 讀取, 位址: 0x0000 (狀態暫存器)
		unsigned char rx_buffer[4] = {0};
		unsigned char status_result = 0;

		// 確保 SPI 的數據寬度是 8-bit，因為 LTC2983 的指令是逐字節的
		SPIM_SetDataSize(SPIM_LTC2983_4, SPI_DATASIZE_8BIT);

		// 1. 手動拉低 CS
		LTC298X_CS4(0);

		// 2. 執行一次 4-byte 的 SPI 傳輸
		//    發送: [讀取指令, 位址高位, 位址低位, 啞元數據]
		//    接收: [啞元, 啞元, 啞元, 實際狀態值]
		HAL_SPI_TransmitReceive(&hspi4, tx_buffer, rx_buffer, 4, 100);

		// 3. 手動拉高 CS
		LTC298X_CS4(1);

		// 4. 取得結果
		status_result = rx_buffer[3];

		// 5. 在這裡設定中斷點 (breakpoint)
		//    然後在除錯模式下，查看 status_result 的值
		__NOP(); // 設置中斷點的好位置

		HAL_Delay(100); // 每秒重複測試
	}

//	GPIO_Set_O(LTC2983_SDO4_GPIO_Port, LTC2983_SDO4_Pin);
	while(test1){
		LTC298X_CS4(1);
		HAL_Delay(10);
		LTC298X_CS4(0);
		HAL_Delay(10);
//		LTC298X_SDO4(1);
//		HAL_Delay(10);
//		LTC298X_SDO4(0);
//		HAL_Delay(10);
	}

	// --- LTC2983 Initialization ---
	// 1. 設定要使用的感測器 (將指標存入陣列)
	//    索引值必須與感測器結構中定義的通道號 (chan) 一致
	ltc2983_inst.sensors[9] = (struct ltc2983_sensor *)&ltc2983_thermocouple_k_9;
	ltc2983_inst.sensors[10] = (struct ltc2983_sensor *)&ltc2983_diode_10;
	LTC2986_SelectCS(4);
	int init=1;

	// 2. 呼叫初始化函式，指定使用 SPI4
	while (init){
		int init_status = ltc298x_init(&ltc2983_inst, SPIM_LTC2983_4, ID_LTC2983, LTC2983_50_60HZ_REJECTION, 1000);

		// 3. 檢查初始化結果。如果失敗，則進入 Error_Handler
		if (init_status != 0) {
			//			Error_Handler();
			__NOP(); // 設置中斷點的好位置
		}
		else{
			init=0;
		}
	}

	/* USER CODE END 2 */

	/* Infinite loop */
	while (1) // 主應用程式迴圈
	{
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */

		// --- 您可以在此處加入 LTC2983 的溫度讀取程式碼 ---
		int temperature_milli_c = 0;
		int read_status;

		API_STEP_CFG_SET = 0; // 重置讀取狀態機
		do {
			read_status = ltc2983_chan_read(&ltc2983_inst, 9, &temperature_milli_c); // 讀取通道 9
			if (read_status < 0) {
				// 處理錯誤
				break;
			}
		} while (read_status & 0x080); // 持續輪詢直到完成

		if (read_status == 0) {
			float final_temp_c = (float)temperature_milli_c / 1000.0f;
			// 成功讀取溫度，可在此處理 final_temp_c
		}

		HAL_Delay(1000); // 每秒讀取一次

		// -------------------------------------------------------
		// 以下是您原有的 FPGA 測試程式碼，您可以將 while(0) 改為 while(1) 來啟用它
		// 注意：同時執行兩邊的邏輯可能會導致 SPI 衝突，除非您有做好 CS 的管理
		// -------------------------------------------------------
		if (0) // 將 0 改為 1 來啟用這段 FPGA 測試碼
		{
			// ... (您原有的 SG_ECP5 和 TMP117 相關程式碼) ...
		}
	}





	SG_ECP5_Enable(1);
	DelayuSec(1);
	//	SG_ECP5_Enable(0);
	DelayuSec(1);
	// 選擇要溝通的 FPGA 晶片，0xff 表示廣播 (同時對所有 CS 作用)
	SG_ECP5_SelectCS(0xff);
	unsigned long fpga_version = 0;
	//	unsigned long busy_o = 0;
	unsigned long data_ready = 0;
	//	unsigned long ack_error_o = 0;
	//	unsigned long ALT = 0;
	unsigned long rd = 0;
	unsigned long TEST = 0;
	unsigned long TEST2 = 0;

	int	fail;
	float temp_Data=0;
	//	float temp_Data2=0;

	SG_ECP5_GetVersion(&fpga_version);
	HAL_Delay(1000); // 延遲一秒，避免過於頻繁地傳輸
	// 定義要啟動的通道遮罩
	unsigned short conversion_mask = 0x8001; // 通道 0 和 15
	fail = TMP117_SetAllI2CAddresses(API0.i2c_addresses);

	while (0)
	{
		/* USER CODE END WHILE */
		// --- 在主迴圈中使用 SG_ECP5_Transfer ---
		// 啟動轉換
		fail = TMP117_StartConversion(conversion_mask);
		//		HAL_Delay(1000); // 延遲一秒，避免過於頻繁地傳輸

		unsigned long ack_error_details = 0;
		int status = TMP117_CheckTransactionStatus(conversion_mask, 2000, &ack_error_details);

		if (status != 0)
		{
			// 現在可以精確地處理各種錯誤
			// status == -1 -> 超時
			// status == -2 -> I2C ACK 錯誤
			// status == -3 -> 狀態異常
			// status == -10 -> SPI 錯誤

			// 在此處加入您的錯誤處理程式碼

			continue; // 出錯時跳過本次迴圈
		}

		// 成功，繼續執行...

		fail  = SG_ECP5_Transfer(SG_ECP5_CMD_READ,SPI_RDATA_data_TEST,&TEST);
		fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ,0x0000,&TEST);
		HAL_Delay(10);

		fail  = SG_ECP5_Transfer(SG_ECP5_CMD_READ,SPI_RDATA_data_TEST2,&TEST2);
		HAL_Delay(10);
		fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ,SPI_RDATA_data_TEST2,&TEST2);
		HAL_Delay(10);

		temp_Data= TMP117_ConvertToCelsius(TEST2);
		HAL_Delay(10);

		//		unsigned char  temp_data_buffer[32];
		float  temp_Data_Array[16];

		/* USER CODE BEGIN 3 */

		fail=TMP117_GetAllTemperatures(temp_Data_Array);
		HAL_Delay(10);

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 128;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct = {0};

	/* Disables the MPU */
	HAL_MPU_Disable();

	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.BaseAddress = 0x0;
	MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
	MPU_InitStruct.SubRegionDisable = 0x87;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/* Enables the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
