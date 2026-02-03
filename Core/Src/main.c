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
#include "crc.h"
#include "dma2d.h"
#include "eth.h"
#include "ltdc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "API/API.h"
// #include "API/temp_graph/temp_graph.h" // ?�STemWin 註解?��?��?��?要�?��?��?�表標頭�??????


extern const GUI_BITMAP bmBmpCVST_1;
extern const GUI_BITMAP bmBmpCVST_2;
extern const GUI_BITMAP bmBmpCVST_3;
extern const GUI_BITMAP bmBmpCVST_4;
extern const GUI_BITMAP bmBmpCVST_5;
extern const GUI_BITMAP bmBmpCVST_6;

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
// TempGraph myTempGraph; // ?�STemWin 註解?�宣??��??��?�表實�?? (??��??)

//  FIFO 環形緩衝區 (大小設大一點，例如 1024 byte，應付連發指令)
#define RX_FIFO_SIZE 1024
uint8_t g_RxFIFO[RX_FIFO_SIZE];
volatile uint32_t g_RxHead = 0; // 寫入位置 (ISR 用)
volatile uint32_t g_RxTail = 0; // 讀取位置 (Main 用)

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

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MPU Configuration--------------------------------------------------------*/
	MPU_Config();

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	//  Cache_Enable();

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	//  MX_GPIO_Init();
	//  MX_ETH_Init();
	//  MX_SPI1_Init();
	//  MX_SPI3_Init();
	//  MX_SPI5_Init();
	//  MX_TIM1_Init();
	//  MX_TIM2_Init();
	//  MX_USART1_UART_Init();
	//  MX_LTDC_Init();
	//  MX_DMA2D_Init();
	//  MX_FMC_Init();
	//  MX_CRC_Init();
	//  MX_USB_DEVICE_Init();
	/* USER CODE BEGIN 2 */
	if (API_Init() != 0)
	{
		HAL_Delay(500);
	}

	HAL_Delay(1);
	while (0)
	{
		GUI_DrawBitmap(&bmBmpCVST_6, 0, 0);
		GUI_Delay(100);
	}


	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		// --- 任�?? 1: �????????��???? LTC2983 設�?��?�溫�?????? ---
		//		API_LTC2983_ReadMultipleTemperatures(API0.ltc2983_devices, API0.num_ltc2983_devices, MAX_LTC2983_DEVICES);
		//		if (g_LTC_State == LTC_STATE_IDLE)
		//		{
		// 只有閒置時，才去讀溫度
		// 注意：這個函式內部必須負責發送 0x80 (Start Conversion) 指令
		API_LTC2983_ReadMultipleTemperatures(API0.ltc2983_devices, API0.num_ltc2983_devices, MAX_LTC2983_DEVICES);
		//		}
		// --- 任�?? 2: �????????��???�已??�置??? TMP117 ??�測?��??�溫�?????? ---
		API_TMP117_ReadMultipleTemperatures(API0.tmp117_sensors, API0.num_tmp117_sensors, MAX_TMP117_SENSORS);


		//		// --- ?�STemWin 註解?�添??�數??�到??��?�表緩�?��? ---
		//		/*
		//		if (API0.num_tmp117_sensors > 0 && API0.tmp117_sensors[0].i2c_address != 0) {
		//			// ??�濾??�可?��??�錯誤�?? (-999)
		//			if(API0.tmp117_sensors[0].temperature_C > -270.0f) { // ??�設 -270 ?��?��?�可?��??�正常溫�??????
		//				Graph_AddData(&myTempGraph, API0.tmp117_sensors[0].temperature_C);
		//			}
		//		}
		//		 */


		//		// --- ?�STemWin 註解?��?�除?��幕並顯示??��?�溫�?????? (??��??) ---
		//		// Update_LCD_Task(24); // ?��?��?�除?���??????
		//
		//		// --- ?�STemWin 註解?�在??��?��?�方??�新繪製??�表 (??��??) ---
		//		// Graph_Draw(&myTempGraph); // 繪製??�景?�軸??�曲�??????

		//		API_Graph_DrawBarChart();

		//		GUI_Delay(100); // 延遲 100 毫�??

		// ==========================================
		//  檢查 FIFO 是否有資料
		// ==========================================
		if (g_RxHead != g_RxTail)
		{
			// 準備一個小暫存區，一次撈一串出來處理 (提升效率)
			uint8_t process_buf[64];
			uint32_t count = 0;

			// 從 FIFO 搬移資料到暫存區
			while (g_RxHead != g_RxTail && count < 64)
			{
				process_buf[count++] = g_RxFIFO[g_RxTail]; // 讀取
				g_RxTail = (g_RxTail + 1) % RX_FIFO_SIZE;  // 更新讀取位置
			}

			// 呼叫原本的處理函式
			// 因為 API_USB_ProcessRx 內部有狀態機 (rx_index)，它可以完美處理分段的資料
			API_USB_ProcessRx(process_buf, count);
			if (g_RxHead != g_RxTail)
			{
				continue;
			}
			//			if (g_LTC_State == LTC_STATE_IDLE)
			//			{
			//				g_LTC_State = LTC_STATE_RESET_START;
			//			}
		}

		//		API_LTC_AsyncTask();
		API_Graph_UpdateTask();

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
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 15;
	RCC_OscInitStruct.PLL.PLLN = 144;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 5;
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
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{

	/* Disables the MPU */
	HAL_MPU_Disable();
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
