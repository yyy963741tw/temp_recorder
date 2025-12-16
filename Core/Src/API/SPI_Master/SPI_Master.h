#ifndef __SPI_H_2023_
#define __SPI_H_2023_

#include "../Inc/comm.h"
#include "spi.h"
//	SPI timeout counter for a transfer
#define SPIM_TIMEOUT_COUNT			50000

typedef struct{
	SPI_HandleTypeDef *hspim;
	unsigned int SPIM_idx;
}SPIMCH;

typedef enum {
	SPIM_FPGA = 0,
	SPIM_LTC2983_1,
	SPIM_LTC2983_2,
	SPIM_LTC2983_3,
	SPIM_LTC2983_4,

	SPIM_TOTAL
}SPIM_IDX;

//	SPI clock div set
typedef enum {
	SPIM_CLK_PPRE_64_1 = 0,
	SPIM_CLK_PPRE_16_1,
	SPIM_CLK_PPRE_4_1,
	SPIM_CLK_PPRE_1_1,
	SPIM_CLK_PPRE_TOTAL
}SPIM_CLK_PPRE;

typedef enum {
	SPIM_CLK_SPRE_8_1 = 0,
	SPIM_CLK_SPRE_7_1,
	SPIM_CLK_SPRE_6_1,
	SPIM_CLK_SPRE_5_1,
	SPIM_CLK_SPRE_4_1,
	SPIM_CLK_SPRE_3_1,
	SPIM_CLK_SPRE_2_1,
	SPIM_CLK_SPRE_1_1,
	SPIM_CLK_SPRE_TOTAL
}SPIM_CLK_SPRE;

typedef enum {
	SPIM_BIT_MODE_8 = 0,
	SPIM_BIT_MODE_16,
	SPIM_BIT_MODE_TOTAL
}SPIM_BIT_MODE;

int SPIM_Init(SPIM_IDX idx);
int SPIM_TransBlocking(SPIM_IDX idx, unsigned short wd, unsigned short *rd);
//	SPI transfer 1 strobe, return 1 if timeout
//int SPIM_TransBlocking_n(SPIM_IDX idx, unsigned char *data, unsigned short size);
void SPIM_SetDataSize(SPIM_IDX idx, uint16_t datasize);

/**
 * @brief Transmits and receives a block of data in polling mode.
 * @param idx The SPI channel index.
 * @param txd Pointer to the transmission buffer.
 * @param rxd Pointer to the reception buffer.
 * @param n The number of bytes to transfer.
 * @return 0 on success, non-zero on failure.
 */
int SPIM_TransBlocking_n(SPIM_IDX idx, unsigned char *txd, unsigned char *rxd, unsigned int n);

extern SPIMCH SPIMCHS[SPIM_TOTAL];
#endif		//	__SPI_H_2023_
