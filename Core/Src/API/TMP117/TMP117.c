#include "TMP117.h"


/**
 * @brief Initializes a single TMP117 sensor structure. (Software only)
 */
int TMP117_Init(tmp117_sensor *sensor, const struct tmp117_init_param *init_param)
{
	if (!sensor || !init_param) {
		return -1;
	}
	sensor->spi_idx = init_param->spi_idx;
	sensor->cs_idx = init_param->cs_idx;
	sensor->channel = init_param->channel;
	sensor->i2c_address = init_param->i2c_address;
	sensor->temperature_C = -273.15f; // Initialize to an invalid temperature
	return 0;
}

/**
 * @brief Writes the sensor's I2C address to the FPGA. (Hardware setup)
 */
int TMP117_ConfigureOnFPGA(const tmp117_sensor *sensor)
{
	if (!sensor || sensor->channel > 15) return -1;
	unsigned long rd;

	SG_ECP5_SelectCS(sensor->cs_idx);
	unsigned short cmd = SPI_CMD_SET_IIC_ADDR_BASE + sensor->channel;
	unsigned short addr_data = sensor->i2c_address;

	return SG_ECP5_Transfer(cmd, addr_data, &rd);
}

/**
 * @brief Reads the temperature of a single sensor.
 */
int TMP117_ReadTemperature(tmp117_sensor *sensor, float *temperature_celsius)
{
	if (!sensor) return -1;

	// 1. Start conversion for this specific channel
	unsigned short channel_mask = (1 << sensor->channel);
	int status = TMP117_StartConversion(sensor, channel_mask);
	if (status != 0) return -10; // SPI failure

	// 2. Wait for the transaction to complete
	status = TMP117_CheckTransactionStatus(sensor, channel_mask, 200); // 200ms timeout
	if (status != 0) return status; // Returns timeout or I2C error

	// 3. Read all data from FPGA
	unsigned char ordered_temp_buffer[32];
	status = TMP117_ReadAllTempData_Ordered(sensor, ordered_temp_buffer);
	if (status != 0) return status;

	// 4. Extract and convert the data for our channel
	unsigned short raw_value = (ordered_temp_buffer[sensor->channel * 2] << 8) | ordered_temp_buffer[sensor->channel * 2 + 1];
	float temp = TMP117_ConvertToCelsius(raw_value);

	// 5. Store the result in the struct and return via pointer if provided
	sensor->temperature_C = temp;
	if (temperature_celsius) {
		*temperature_celsius = temp;
	}

	return 0; // Success
}



//int TMP117_ReadMultipleTemperatures(tmp117_sensor sensors[], unsigned char num_sensors_to_find, unsigned char array_size)
//{
//    if (num_sensors_to_find == 0 || !sensors) return 0;
//
//    const tmp117_sensor *context_sensor = NULL;
//    // 尋找第一個有效感測器，迴圈最多執行 array_size 次
//    for (int i = 0; i < array_size; i++) {
//        if (sensors[i].i2c_address != 0) {
//            context_sensor = &sensors[i];
//            break;
//        }
//    }
//    if (context_sensor == NULL) return 0; // 在整個陣列中都找不到有效感測器
//
//    // 1. 建立通道遮罩，在找到 num_sensors_to_find 個感測器後停止
//    unsigned short channel_mask = 0;
//    unsigned char found_count = 0;
//    for (int i = 0; i < array_size; i++) {
//        if (sensors[i].i2c_address != 0) {
//            channel_mask |= (1 << sensors[i].channel);
//            found_count++;
//            if (found_count >= num_sensors_to_find) {
//                break;
//            }
//        }
//    }
//
//    if (channel_mask == 0) return 0;
//
//    // 2. 啟動轉換
//    int status = TMP117_StartConversion(context_sensor, channel_mask);
//    if (status != 0) return -10;
//
//    // 3. 等待完成
//    status = TMP117_CheckTransactionStatus(context_sensor, channel_mask, 500);
//    if (status != 0) return status;
//
//    // 4. 讀取所有資料
//    unsigned char ordered_temp_buffer[32];
//    status = TMP117_ReadAllTempData_Ordered(context_sensor, ordered_temp_buffer);
//    if (status != 0) return status;
//
//    // 5. 轉換並儲存溫度，同樣在處理完 num_sensors_to_find 個感測器後停止
//    found_count = 0;
//    for (int i = 0; i < array_size; i++) {
//        if (sensors[i].i2c_address != 0) {
//            unsigned char channel = sensors[i].channel;
//            unsigned short raw_value = (ordered_temp_buffer[channel * 2] << 8) | ordered_temp_buffer[channel * 2 + 1];
//            sensors[i].temperature_C = TMP117_ConvertToCelsius(raw_value);
//            found_count++;
//            if (found_count >= num_sensors_to_find) {
//                break;
//            }
//        }
//    }
//
//    return 0; // Success
//}

// --- Internal (static) Helper Functions ---

int TMP117_StartConversion(const tmp117_sensor *sensor, unsigned short channel_mask)
{
	unsigned long rd;
	SG_ECP5_SelectCS(sensor->cs_idx);
	return SG_ECP5_Transfer(IIC_START, channel_mask, &rd);
}

int TMP117_CheckTransactionStatus(const tmp117_sensor *sensor, unsigned short channel_mask, unsigned int timeout_ms)
{
	int fail = 0;
	unsigned long busy_status = 1;
	unsigned long start_tick = HAL_GetTick();

	SG_ECP5_SelectCS(sensor->cs_idx);

	while ((busy_status & channel_mask) != 0)
	{
		if ((HAL_GetTick() - start_tick) > timeout_ms) return -1; // Timeout

		fail = SG_ECP5_Transfer(SG_ECP5_CMD_READ, SPI_RDATA_BUSY_STATUS, &busy_status);
		fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ, 0x0000, &busy_status);
		if (fail) return -10; // SPI failure

		HAL_Delay(5);
	}

	// Check for ACK errors after completion
	unsigned long ack_status = 0;
	fail = SG_ECP5_Transfer(SG_ECP5_CMD_READ, SPI_RDATA_ACK_ERROR, &ack_status);
	fail |= SG_ECP5_Transfer(SG_ECP5_CMD_READ, 0x0000, &ack_status);
	if (fail)
		return -10;

	if ((ack_status & channel_mask) != 0)
		return -2; // I2C ACK error

	return 0;
}


//int TMP117_ReadAllTempData_Ordered(const tmp117_sensor *sensor, unsigned char *ordered_temp_buffer)
//{
//	unsigned char raw_buffer[32];
//	SG_ECP5_SelectCS(sensor->cs_idx);
//
//	// Trigger stream mode
//	unsigned long rd_chunk;
//	int fail = SG_ECP5_Transfer(SPI_CMD_READ, SPI_RDATA_STREAM_TEMP_ALL, &rd_chunk);
//	if (fail) return -1;
//	HAL_Delay(1);
//
//	// Read 256 bits
//	SG_ECP5_CS_Set(0);
//	for (int i = 0; i < 16; i++) {
//		unsigned short temp_chunk;
//		if (SPIM_TransBlocking(sensor->spi_idx, 0x0000, &temp_chunk) != 0) {
//			SG_ECP5_CS_Set(1);
//			return -2;
//		}
//		raw_buffer[i * 2] = (temp_chunk >> 8) & 0xFF;
//		raw_buffer[i * 2 + 1] = temp_chunk & 0xFF;
//		HAL_Delay(1);
//
//	}
//
//
//	// Reorder data
//	for (int i = 0; i < 8; i++) {
//		int ch_even = i * 2, ch_odd = i * 2 + 1;
//		ordered_temp_buffer[ch_even * 2]     = raw_buffer[i * 4 + 2];
//		ordered_temp_buffer[ch_even * 2 + 1] = raw_buffer[i * 4 + 3];
//		ordered_temp_buffer[ch_odd * 2]      = raw_buffer[i * 4];
//		ordered_temp_buffer[ch_odd * 2 + 1]  = raw_buffer[i * 4 + 1];
//	}
//	return 0;
//}



/**
 * @brief 【最終修正版】透過呼叫單通道讀取函式，循序讀取所有溫度資料。
 * @param sensor - 用於提供通訊上下文的感測器指標。
 * @param ordered_temp_buffer - 用於存放最終排序好的 32 位元組資料的緩衝區。
 * @return 0 代表成功。
 */
int TMP117_ReadAllTempData_Ordered(const tmp117_sensor *sensor, unsigned char *ordered_temp_buffer)
{
	uint16_t raw_data;

	// 遍歷所有 16 個可能的通道
	for (int channel = 0; channel < 16; channel++) {
		// 【核心修改】直接呼叫已經封裝好的單通道讀取函式
		int status = TMP117_ReadSingleChannelData(sensor, channel, &raw_data);

		if (status == 0) {
			// 如果讀取成功，將 16 位元資料存放到 8 位元陣列的正確位置
			ordered_temp_buffer[channel * 2]     = (raw_data >> 8) & 0xFF;
			ordered_temp_buffer[channel * 2 + 1] = raw_data & 0xFF;
		} else {
			// 如果讀取失敗，可以選擇用一個錯誤碼填充，或保留舊資料。
			// 這裡我們用 0xFFFF (對應 -0.0078125 度) 來標示一個無效讀取，
			// 但上層的 ack_error_status 檢查機制會優先處理，所以這裡的資料通常不會被使用。
			ordered_temp_buffer[channel * 2]     = 0xFF;
			ordered_temp_buffer[channel * 2 + 1] = 0xFF;
		}
	}

	return 0; // 函式本身總是回報成功，由上層邏輯根據 ACK 狀態決定如何使用資料
}




float TMP117_ConvertToCelsius(unsigned long raw_data)
{
	short signed_temp_code = (short)(raw_data & 0xFFFF);
	return signed_temp_code * TMP117_RESOLUTION;
}


/**
 * @brief 【新增】從 FPGA 讀取指定單一通道的 16 位元原始溫度資料。
 * @param sensor - 用於提供通訊上下文的感測器指標。
 * @param channel - 要讀取的通道編號 (0-15)。
 * @param raw_data - 用於存放讀取到的 16 位元原始資料的指標。
 * @return 0 代表成功，負值代表失敗。
 */
int TMP117_ReadSingleChannelData(const tmp117_sensor *sensor, uint8_t channel, uint16_t *raw_data)
{
    if (!sensor || channel > 15 || !raw_data) return -1;

    unsigned long rd_buffer;

    // 根據 FPGA 的 SPI 協議，讀取一個暫存器需要兩個傳輸
    // 第一次傳輸：發送讀取指令 (0x00FF) 和目標位址 (0x0040 + channel)
    int fail = SG_ECP5_Transfer(SPI_CMD_READ, SPI_RDATA_TEMP_CH_BASE + channel, &rd_buffer);
//	HAL_Delay(100);

    // 第二次傳輸：發送 dummy word (0x0000) 來接收真正的資料
    fail |= SG_ECP5_Transfer(SPI_CMD_READ, 0x0000, &rd_buffer);

    if (fail) {
        return -10; // SPI 通訊失敗
    }

    *raw_data = (uint16_t)(rd_buffer & 0xFFFF);

    return 0;
}


