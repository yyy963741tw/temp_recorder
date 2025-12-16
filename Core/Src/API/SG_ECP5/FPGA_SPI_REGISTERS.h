#ifndef FPGA_SPI_REGISTERS_H
#define FPGA_SPI_REGISTERS_H

//================================================================
// SPI 命令定義：I2C 控制命令
// Verilog 對應 CMDValue: 16'h33
//================================================================

/**
 * @brief 觸發指定通道開始一次 I2C 溫度讀取。
 * @note  Data 欄位是一個 16 位元的 bitmask，對應 16 個 I2C 通道。
 * 例如，要觸發通道 0，Data = 0x0001。
 * 要觸發通道 15，Data = 0x8000。
 */
#define SPI_CMD_IIC_START               0x0033

#define SPI_RDATA_TEMP_CH_BASE          0x0040

//================================================================
// SPI 命令定義：I2C 通道位址設定
// Verilog 對應 CMDValue: 16'h5000 ~ 16'h500F
//================================================================

/**
 * @brief 設定 I2C 通道位址的基礎命令。
 * @note  MCU 發送的 Data 欄位應為 7 位元的 I2C 位址。
 * 例如，要將通道 5 的位址設為 0x4A，
 * MCU 應發送 CMD=0x5005, Data=0x004A。
 */
#define SPI_CMD_SET_IIC_ADDR_BASE       0x5000
//#define SPI_CMD_SET_IIC_ADDR_BASE       0x0050

#define SPI_CMD_SET_IIC_ADDR_CH0        (SPI_CMD_SET_IIC_ADDR_BASE + 0)  // 0x5000
#define SPI_CMD_SET_IIC_ADDR_CH1        (SPI_CMD_SET_IIC_ADDR_BASE + 1)  // 0x5001
#define SPI_CMD_SET_IIC_ADDR_CH2        (SPI_CMD_SET_IIC_ADDR_BASE + 2)  // 0x5002
#define SPI_CMD_SET_IIC_ADDR_CH3        (SPI_CMD_SET_IIC_ADDR_BASE + 3)  // 0x5003
#define SPI_CMD_SET_IIC_ADDR_CH4        (SPI_CMD_SET_IIC_ADDR_BASE + 4)  // 0x5004
#define SPI_CMD_SET_IIC_ADDR_CH5        (SPI_CMD_SET_IIC_ADDR_BASE + 5)  // 0x5005
#define SPI_CMD_SET_IIC_ADDR_CH6        (SPI_CMD_SET_IIC_ADDR_BASE + 6)  // 0x5006
#define SPI_CMD_SET_IIC_ADDR_CH7        (SPI_CMD_SET_IIC_ADDR_BASE + 7)  // 0x5007
#define SPI_CMD_SET_IIC_ADDR_CH8        (SPI_CMD_SET_IIC_ADDR_BASE + 8)  // 0x5008
#define SPI_CMD_SET_IIC_ADDR_CH9        (SPI_CMD_SET_IIC_ADDR_BASE + 9)  // 0x5009
#define SPI_CMD_SET_IIC_ADDR_CH10       (SPI_CMD_SET_IIC_ADDR_BASE + 10) // 0x500A
#define SPI_CMD_SET_IIC_ADDR_CH11       (SPI_CMD_SET_IIC_ADDR_BASE + 11) // 0x500B
#define SPI_CMD_SET_IIC_ADDR_CH12       (SPI_CMD_SET_IIC_ADDR_BASE + 12) // 0x500C
#define SPI_CMD_SET_IIC_ADDR_CH13       (SPI_CMD_SET_IIC_ADDR_BASE + 13) // 0x500D
#define SPI_CMD_SET_IIC_ADDR_CH14       (SPI_CMD_SET_IIC_ADDR_BASE + 14) // 0x500E
#define SPI_CMD_SET_IIC_ADDR_CH15       (SPI_CMD_SET_IIC_ADDR_BASE + 15) // 0x500F


//================================================================
// SPI 命令定義：讀取 FPGA 內部狀態與資料
// 這些是組合命令，CMD 固定為 0x00FF，由 Data 欄位指定要讀取的內容。
//================================================================

// 讀取命令的固定 CMD 欄位
#define SPI_CMD_READ                    0x00FF

// --- Data 欄位定義 ---
#define SPI_RDATA_BUSY_STATUS           0x0001 // 讀取 I2C 忙碌狀態 (16-bit bitmask)
#define SPI_RDATA_DATA_READY            0x0002 // 讀取資料就緒旗標 (16-bit bitmask)
#define SPI_RDATA_ACK_ERROR             0x0003 // 讀取 ACK 錯誤旗標 (16-bit bitmask)
#define SPI_RDATA_VERSION               0x0004 // 讀取 FPGA 版本號
#define SPI_RDATA_ALT_INPUT             0x0006 // 讀取 ALT 輸入腳位狀態 (16-bit)
#define SPI_RDATA_TEST_OUTPUT           0x0007 // 讀取 test 輸出
#define SPI_RDATA_CHECKSUM              0x0019 // 讀取 Checksum
#define SPI_RDATA_LAST_CMD              0x00FF // 讀取 FPGA 收到的上一筆完整指令

// 讀取指定通道的溫度數據
#define SPI_RDATA_TEMP_CH15             0x0008 // 讀取通道 15 的 16 位元溫度資料

// 觸發 256 位元溫度資料的連續傳輸模式
#define SPI_RDATA_STREAM_TEMP_ALL       0x0005

#endif // FPGA_SPI_REGISTERS_H
