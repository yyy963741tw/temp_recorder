#ifndef __TMP117_H
#define __TMP117_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../DelayUtility/DelayUtility.h"
#include "../SG_ECP5/SG_ECP5.h"
#include "../SPI_Master/SPI_Master.h"
#include <string.h>

// I2C Address definitions
#define I2C_ADDR_GND 0x48
#define I2C_ADDR_VCC 0x49
#define I2C_ADDR_SDA 0x4A
#define I2C_ADDR_SCL 0x4B

// Temperature resolution
#define TMP117_RESOLUTION 0.0078125f

/**
 * @brief Represents a single, self-contained TMP117 sensor.
 * It holds all information needed to interact with it via its controlling FPGA.
 */
typedef struct {
    SPIM_IDX spi_idx;           // SPI interface for the FPGA
    int cs_idx;                 // Chip Select index for the FPGA
    unsigned char channel;      // FPGA I2C channel (0-15)
    unsigned char i2c_address;  // The 7-bit I2C slave address of this sensor
    float temperature_C;        // Stores the last read temperature in Celsius
} tmp117_sensor;

/**
 * @brief Initialization parameter structure for a single TMP117 sensor.
 */
struct tmp117_init_param {
    SPIM_IDX spi_idx;
    int cs_idx;
    unsigned char channel;
    unsigned char i2c_address;
};


// --- Function Declarations ---
int TMP117_StartConversion(const tmp117_sensor *sensor, unsigned short channel_mask);
int TMP117_CheckTransactionStatus(const tmp117_sensor *sensor, unsigned short channel_mask, unsigned int timeout_ms);
int TMP117_ReadAllTempData_Ordered(const tmp117_sensor *sensor, unsigned char *ordered_temp_buffer);

/**
 * @brief Initializes a single TMP117 sensor structure.
 * This function simply copies configuration data into the struct. It does not communicate with hardware.
 * @param sensor - Pointer to the sensor instance to be initialized.
 * @param init_param - Pointer to the initialization parameters.
 * @return 0 on success.
 */
int TMP117_Init(tmp117_sensor *sensor, const struct tmp117_init_param *init_param);

/**
 * @brief Configures the sensor's I2C address on the FPGA.
 * This is the hardware initialization part.
 * @param sensor - Pointer to the initialized sensor instance.
 * @return 0 on success, non-zero on SPI communication failure.
 */
int TMP117_ConfigureOnFPGA(const tmp117_sensor *sensor);

/**
 * @brief Reads the temperature of a single sensor in a blocking operation.
 * The result is stored in the sensor's temperature_C member and also returned via the pointer.
 * @param sensor - Pointer to the initialized sensor instance.
 * @param temperature_celsius - Pointer to a float where the result will be stored. Can be NULL if not needed.
 * @return 0 on success, negative error code on failure.
 */
int TMP117_ReadTemperature(tmp117_sensor *sensor, float *temperature_celsius);

/**
 * @brief Reads data and converts temperatures for a target number of sensors within an array.
 * @param sensors - The array of sensor instances to search within.
 * @param num_sensors_to_find - The target number of valid sensors to find and process.
 * @param array_size - The total size of the sensors array to iterate through.
 * @return 0 on success, non-zero on failure.
 */
//GO TO API
//int TMP117_ReadMultipleTemperatures(tmp117_sensor sensors[], unsigned char num_sensors_to_find, unsigned char array_size);



float TMP117_ConvertToCelsius(unsigned long raw_data);
int TMP117_ReadSingleChannelData(const tmp117_sensor *sensor, uint8_t channel, uint16_t *raw_data);


#ifdef __cplusplus
}
#endif

#endif // __TMP117_H
