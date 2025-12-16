#include "SPI_Master.h"
SPIMCH SPIMCHS[SPIM_TOTAL];
//int SPIM_Init(SPIM_IDX idx){
//	switch(idx){
//		case SPIM_1:
////			MX_SPI1_Init();
//			break;
//		case SPIM_2:
//			MX_SPI2_Init();
//			break;
//		default:
//			return 1;
//	}
//	return 0;
//}

void SPIMCH_Init(void *init(void), SPI_HandleTypeDef  *hspi, unsigned int idx)
{
	init();
	SPIMCHS[idx].hspim =				hspi;
	SPIMCHS[idx].SPIM_idx =				idx;
}

//int SPIM_Close(SPIM_IDX idx){
//	switch(idx){
//		case SPIM_1:
//			SPI1STATbits.SPIEN 		= 0;	// disable SPI port
//			break;
//		case SPIM_2:
//			SPI2STATbits.SPIEN 		= 0;	// disable SPI port
//			break;
//		default:
//			return 1;
//	}
//	return 0;
//}
//
//int SPIM_SetBitMode(SPIM_IDX idx, SPIM_BIT_MODE bm){
//	switch(idx){
//		case SPIM_1:
//			SPI1STATbits.SPIEN 		= 0;
//			SPI1CON1bits.MODE16 	= bm;
//			SPI1STATbits.SPIEN 		= 1;
//			break;
//		case SPIM_2:
//			SPI2STATbits.SPIEN 		= 0;
//			SPI2CON1bits.MODE16 	= bm;
//			SPI2STATbits.SPIEN 		= 1;
//			break;
//		default:
//			return 1;
//	}
//	return 0;
//}

//	SPI transfer 1 strobe, return 1 if timeout
int SPIM_TransBlocking(SPIM_IDX idx, unsigned short wd, unsigned short *rd){
	int	cnt;
	unsigned short Rxdata;

	// 每次執行 16 位元傳輸前，都確保 SPI 處於 16 位元模式
	if (SPIMCHS[idx].hspim->Init.DataSize != SPI_DATASIZE_16BIT) {
		__HAL_SPI_DISABLE(SPIMCHS[idx].hspim);
		SPIMCHS[idx].hspim->Init.DataSize = SPI_DATASIZE_16BIT;
		HAL_SPI_Init(SPIMCHS[idx].hspim);
	}

	switch(idx){
	case SPIM_FPGA:
		//			HAL_SPI_Transmit(&hspi1,&wd,1, 1000);
		////			while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE ) == RESET);
		//			for(cntT=SPIM_TIMEOUT_COUNT; cntT; cntT--){
		//				if(__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE ) != RESET)
		//					break;
		//			}
		//
		//			HAL_SPI_Receive(&hspi1,&Rxdata,1, 1000);
		////			while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE ) != RESET);
		//			for(cntR=SPIM_TIMEOUT_COUNT; cntR; cntR--){
		//				if(__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE ) == RESET)
		//					break;
		//			}
//		// --- 修正點 ---
//		// 將指向 unsigned short 的指標，強制轉換為 HAL 函式期望的 uint8_t 指標
//		HAL_SPI_TransmitReceive(SPIMCHS[idx].hspim, (uint8_t*)&wd, (uint8_t*)&Rxdata, 1, 1000);
		HAL_SPI_TransmitReceive(SPIMCHS[idx].hspim, &wd, &Rxdata, 1, 1000);
		for(cnt=SPIM_TIMEOUT_COUNT; cnt; cnt--){
			if(__HAL_SPI_GET_FLAG(SPIMCHS[idx].hspim, SPI_FLAG_RXNE ) == RESET || __HAL_SPI_GET_FLAG(SPIMCHS[idx].hspim, SPI_FLAG_TXE ) != RESET)
				break;
		}

		//	check if time out
		if(cnt)		//	pass
			*rd = Rxdata;
		else			//	timeout
			return 1;
		break;
		//		case SPIM_2:
		//			HAL_SPI_Transmit(&hspi2,&wd,1, 1000);
		////			while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE ) == RESET);
		//			for(cntT=SPIM_TIMEOUT_COUNT; cntT; cntT--){
		//				if(__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE ) != RESET)
		//					break;
		//			}
		//
		//			HAL_SPI_Receive(&hspi2,&Rxdata,1, 1000);
		////			while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE ) != RESET);
		//			for(cntR=SPIM_TIMEOUT_COUNT; cntR; cntR--){
		//				if(__HAL_SPI_GET_FLAG(&hspi2, SPI_FLAG_RXNE ) == RESET)
		//					break;
		//			}
		//
		//			//	check if time out
		//			if(cntT&&cntR)		//	pass
		//				*rd = Rxdata;
		//			else			//	timeout
		//				return 1;
		//			break;
	default:
		return 2;
	}
	return 0;
}

////	SPI transfer 1 strobe, return 1 if timeout
//int SPIM_TransBlocking_n(SPIM_IDX idx, unsigned char *data, unsigned short size){
//	int	cnt;
////	unsigned char *Rxdata;
//	unsigned char Rxdata[100];
//
//	HAL_SPI_TransmitReceive(SPIMCHS[idx].hspim, data, Rxdata, size, 1000);
//	for(cnt=SPIM_TIMEOUT_COUNT; cnt; cnt--){
//		if(__HAL_SPI_GET_FLAG(SPIMCHS[idx].hspim, SPI_FLAG_RXNE ) == RESET || __HAL_SPI_GET_FLAG(SPIMCHS[idx].hspim, SPI_FLAG_TXE ) != RESET)
//			break;
//	}
//
//	//	check if time out
//	if(cnt)		//	pass
//		memcpy(data, Rxdata, size);
////		*data = Rxdata;
//	else			//	timeout
//		return 1;
//	return 0;
//}

/**
 * @brief Transmits and receives a block of data (n bytes) in polling mode.
 * @note This is the corrected version with proper buffer handling.
 */
int SPIM_TransBlocking_n(SPIM_IDX idx, unsigned char *txd, unsigned char *rxd, unsigned int n)
{
	if (idx >= SPIM_TOTAL || SPIMCHS[idx].hspim == NULL) {
		return 2; // Invalid SPI index
	}
	if (n == 0) {
		return 0; // Nothing to transfer
	}

	// Make sure datasize is 8-bit for this function
	if (SPIMCHS[idx].hspim->Init.DataSize != SPI_DATASIZE_8BIT) {
		__HAL_SPI_DISABLE(SPIMCHS[idx].hspim);
		SPIMCHS[idx].hspim->Init.DataSize = SPI_DATASIZE_8BIT;
		HAL_SPI_Init(SPIMCHS[idx].hspim);
	}

	// The HAL function is blocking and includes a timeout.
	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(SPIMCHS[idx].hspim, txd, rxd, n, 1000);

	if (status != HAL_OK) {
		return 1; // HAL function reported an error (e.g., timeout)
	}

	return 0; // Success
}

void SPIM_SetDataSize(SPIM_IDX idx, uint16_t datasize)
{
	//    // 關閉 SPI
	//	SPIMCHS[idx].hspim->Instance->CR1 &= ~SPI_CR1_SPE;
	//
	//    // 清除原本的 DS 設定
	//	SPIMCHS[idx].hspim->Instance->CR2 &= ~(SPI_CR2_DS | SPI_CR2_FRXTH);
	//
	//    // 設定新的 DataSize (datasize-1)，例如 8bit => 7，16bit => 15
	//	SPIMCHS[idx].hspim->Instance->CR2 |= ((datasize - 1) << SPI_CR2_DS_Pos);
	//
	//	if (datasize <= 8)
	//		SPI1->CR2 |= SPI_CR2_FRXTH;   // Quarter FIFO (8-bit)
	//	else
	//		SPI1->CR2 &= ~SPI_CR2_FRXTH;  // Half FIFO (16-bit)
	//
	//    // 重新啟動 SPI
	//	SPIMCHS[idx].hspim->Instance->CR1 |= SPI_CR1_SPE;
	// 1. 關閉 SPI
	__HAL_SPI_DISABLE(SPIMCHS[idx].hspim);

	// 2. 更新 handle 內部的 DataSize 設定
	SPIMCHS[idx].hspim->Init.DataSize = datasize;

	// 3. 重新初始化 SPI (會改 CR1/CR2)
	HAL_SPI_Init(SPIMCHS[idx].hspim);

	// 4. 啟動 SPI
	__HAL_SPI_ENABLE(SPIMCHS[idx].hspim);
}
