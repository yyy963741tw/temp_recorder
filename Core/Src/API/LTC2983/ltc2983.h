/***************************************************************************//**
 *   @file   ltc2983.c
 *   @brief  Implementation of LTC2983 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef __LTC298X_H__
#define __LTC298X_H__

//#ifndef __LTC2983_H__
//#define __LTC2983_H__

#include <stdbool.h>
//#include "no_os_gpio.h"
//#include "no_os_spi.h"
#include "../SPI_Master/SPI_Master.h"
#include "NO_OS/no_os_util.h"
#include "LTC298x_comm.h" // 包含範例感測器的宣告

/**
 * @brief Global state machine variable for the non-blocking read function.
 */
extern unsigned int	API_STEP_CFG_SET;

#define LTC2983_STATUS_REG 				 				0x0000
#define LTC2983_TEMP_RES_START_REG		 				0x0010
#define LTC2983_EEPROM_KEY_REG			 				0x00B0
#define LTC2983_EEPROM_READ_STATUS_REG		 			0x00D0
#define LTC2983_GLOBAL_CONFIG_REG 		 				0x00F0
#define LTC2986_EEPROM_STATUS_REG		 				0x00F9
#define LTC2983_MUX_CONFIG_REG 			 				0x00FF
#define LTC2983_CHAN_ASSIGN_START_REG 	 				0x0200
#define LTC2983_CUST_SENS_TBL_START_REG  				0x0250
#define LTC2983_CUST_SENS_TBL_END_REG 	 				0x03CF

#define LTC2983_CUST_SENS_TBL_SIZE 				LTC2983_CUST_SENS_TBL_END_REG - \
				    LTC2983_CUST_SENS_TBL_START_REG + 1
#define LTC2983_SPI_READ_BYTE			0x3
#define LTC2983_SPI_WRITE_BYTE			0x2

#define LTC2983_EEPROM_KEY					0xA53C0F5A
#define LTC2983_EEPROM_WRITE_CMD			0x15
#define LTC2983_EEPROM_READ_CMD				0x16
#define LTC2983_EEPROM_STATUS_FAILURE_MASK	NO_OS_GENMASK(3, 1)
#define LTC2983_EEPROM_READ_FAILURE_MASK	NO_OS_GENMASK(7, 0)

#define LTC2983_EEPROM_WRITE_TIME_MS	2600
#define LTC2983_EEPROM_READ_TIME_MS		20

#define LTC2983_CHAN_START_ADDR(chan) \
			(((chan - 1) * 4) + LTC2983_CHAN_ASSIGN_START_REG)
#define LTC2983_CHAN_RES_ADDR(chan) \
			(((chan - 1) * 4) + LTC2983_TEMP_RES_START_REG)

#define LTC2983_COMMON_HARD_FAULT_MASK	NO_OS_GENMASK(31, 30)
#define LTC2983_COMMON_SOFT_FAULT_MASK	NO_OS_GENMASK(27, 25)

#define	LTC2983_STATUS_START_MASK	NO_OS_BIT(7)
#define	LTC2983_STATUS_START(x)	no_os_field_prep(LTC2983_STATUS_START_MASK, x)
#define	LTC2983_STATUS_UP_MASK	NO_OS_GENMASK(7, 6)
#define	LTC2983_STATUS_UP(reg)	no_os_field_get(LTC2983_STATUS_UP_MASK, reg)

#define	LTC2983_STATUS_CHAN_SEL_MASK	NO_OS_GENMASK(4, 0)
#define	LTC2983_STATUS_CHAN_SEL(x) \
			no_os_field_prep(LTC2983_STATUS_CHAN_SEL_MASK, x)

#define LTC2983_NOTCH_FREQ_MASK	NO_OS_GENMASK(1, 0)

#define LTC2983_RES_VALID_MASK		NO_OS_BIT(24)
#define LTC2983_DATA_SIGN_BIT		23
#define LTC2983_DATA_MASK		NO_OS_GENMASK(LTC2983_DATA_SIGN_BIT, 0)

#define LTC2983_CHAN_TYPE_MASK	NO_OS_GENMASK(31, 27)
#define LTC2983_CHAN_TYPE(x)	no_os_field_prep(LTC2983_CHAN_TYPE_MASK, x)

#define LTC2983_CHAN_ASSIGN_MASK	NO_OS_GENMASK(26, 22)
#define LTC2983_CHAN_ASSIGN(x)	no_os_field_prep(LTC2983_CHAN_ASSIGN_MASK, x)

#define LTC2983_CUSTOM_LEN_MASK	NO_OS_GENMASK(5, 0)
#define LTC2983_CUSTOM_LEN(x)	no_os_field_prep(LTC2983_CUSTOM_LEN_MASK, x)

#define LTC2983_CUSTOM_ADDR_MASK	NO_OS_GENMASK(11, 6)
#define LTC2983_CUSTOM_ADDR(x)	no_os_field_prep(LTC2983_CUSTOM_ADDR_MASK, x)

#define LTC2983_THERMOCOUPLE_CFG_MASK	NO_OS_GENMASK(21, 18)
#define LTC2983_THERMOCOUPLE_CFG(x) \
			no_os_field_prep(LTC2983_THERMOCOUPLE_CFG_MASK, x)
#define LTC2983_THERMOCOUPLE_HARD_FAULT_MASK	NO_OS_GENMASK(31, 29)
#define LTC2983_THERMOCOUPLE_SOFT_FAULT_MASK	NO_OS_GENMASK(28, 25)

#define LTC2983_RTD_CFG_MASK	NO_OS_GENMASK(21, 18)
#define LTC2983_RTD_CFG(x)	no_os_field_prep(LTC2983_RTD_CFG_MASK, x)
#define LTC2983_RTD_EXC_CURRENT_MASK	NO_OS_GENMASK(17, 14)
#define LTC2983_RTD_EXC_CURRENT(x) \
			no_os_field_prep(LTC2983_RTD_EXC_CURRENT_MASK, x)
#define LTC2983_RTD_CURVE_MASK		NO_OS_GENMASK(13, 12)
#define LTC2983_RTD_CURVE(x)	no_os_field_prep(LTC2983_RTD_CURVE_MASK, x)

#define LTC2983_THERMISTOR_CFG_MASK	NO_OS_GENMASK(21, 19)
#define LTC2983_THERMISTOR_CFG(x) \
				no_os_field_prep(LTC2983_THERMISTOR_CFG_MASK, x)
#define LTC2983_THERMISTOR_EXC_CURRENT_MASK	NO_OS_GENMASK(18, 15)
#define LTC2983_THERMISTOR_EXC_CURRENT(x) \
			no_os_field_prep(LTC2983_THERMISTOR_EXC_CURRENT_MASK, x)

#define LTC2983_DIODE_CFG_MASK		NO_OS_GENMASK(26, 24)
#define LTC2983_DIODE_CFG(x)	no_os_field_prep(LTC2983_DIODE_CFG_MASK, x)
#define LTC2983_DIODE_EXC_CURRENT_MASK	NO_OS_GENMASK(23, 22)
#define LTC2983_DIODE_EXC_CURRENT(x) \
			no_os_field_prep(LTC2983_DIODE_EXC_CURRENT_MASK, x)
#define LTC2983_DIODE_IDEAL_FACTOR_MASK	NO_OS_GENMASK(21, 0)
#define LTC2983_DIODE_IDEAL_FACTOR(x) \
			no_os_field_prep(LTC2983_DIODE_IDEAL_FACTOR_MASK, x)

#define LTC2983_R_SENSE_VAL_MASK	NO_OS_GENMASK(26, 0)
#define LTC2983_R_SENSE_VAL(x)	no_os_field_prep(LTC2983_R_SENSE_VAL_MASK, x)

#define LTC2983_ADC_SINGLE_ENDED_MASK	NO_OS_BIT(26)
#define LTC2983_ADC_SINGLE_ENDED(x) \
			no_os_field_prep(LTC2983_ADC_SINGLE_ENDED_MASK, x)

/**
 * @struct device_variant
 * @brief LTC298x Device selection.
 */
enum ltc298x_id {
	ID_LTC2983,
	ID_LTC2984,
	ID_LTC2986,
};

/**
 * @brief Notch frequency of the digital filter
 */
enum ltc2983_filter_notch_freq {
	LTC2983_50_60HZ_REJECTION,
	LTC2983_60HZ_REJECTION,
	LTC2983_50HZ_REJECTION,
};

/**
 * @brief LTC2983 Sensor Type
 */
enum ltc2983_sensor_type {
	LTC2983_UNASSIGNED,
	LTC2983_THERMOCOUPLE_J,
	LTC2983_THERMOCOUPLE_K, //now used
	LTC2983_THERMOCOUPLE_E,
	LTC2983_THERMOCOUPLE_N,
	LTC2983_THERMOCOUPLE_R,
	LTC2983_THERMOCOUPLE_S,
	LTC2983_THERMOCOUPLE_T,
	LTC2983_THERMOCOUPLE_B,
	LTC2983_THERMOCOUPLE_CUSTOM,
	LTC2983_RTD_PT_10,
	LTC2983_RTD_PT_50,
	LTC2983_RTD_PT_100,
	LTC2983_RTD_PT_200,
	LTC2983_RTD_PT_500,
	LTC2983_RTD_PT_1000,
	LTC2983_RTD_1000,
	LTC2983_RTD_NI_120,
	LTC2983_RTD_CUSTOM,
	LTC2983_THERMISTOR_44004_44033,
	LTC2983_THERMISTOR_44005_44030,
	LTC2983_THERMISTOR_44007_44034,
	LTC2983_THERMISTOR_44006_44031,
	LTC2983_THERMISTOR_44008_44032,
	LTC2983_THERMISTOR_YSI_400,
	LTC2983_THERMISTOR_SPECTRUM_1003K,
	LTC2983_THERMISTOR_CUSTOM_STEINHART_HART,
	LTC2983_THERMISTOR_CUSTOM,
	LTC2983_DIODE,
	LTC2983_RSENSE,
	LTC2983_DIRECT_ADC,
	LTC2983_ACTIVE_TEMP,
};

#define to_thermocouple(_sensor) \
		NO_OS_CONTAINER_OF(_sensor, struct ltc2983_thermocouple, sensor)

#define to_rtd(_sensor) \
		NO_OS_CONTAINER_OF(_sensor, struct ltc2983_rtd, sensor)

#define to_thermistor(_sensor) \
		NO_OS_CONTAINER_OF(_sensor, struct ltc2983_thermistor, sensor)

#define to_diode(_sensor) \
		NO_OS_CONTAINER_OF(_sensor, struct ltc2983_diode, sensor)

#define to_rsense(_sensor) \
		NO_OS_CONTAINER_OF(_sensor, struct ltc2983_rsense, sensor)

#define to_adc(_sensor) \
		NO_OS_CONTAINER_OF(_sensor, struct ltc2983_adc, sensor)

#define to_temp(_sensor) \
		NO_OS_CONTAINER_OF(_sensor, struct ltc2983_temp, sensor)

/**
 * @brief LTC2983 descriptor
 */
typedef struct{
//	/** SPI descriptor. */
//	struct no_os_spi_desc *comm_desc;

	/** Reset GPIO descriptor */
	struct no_os_gpio_desc *gpio_rstn;
	/** MUX configuration delay in us */
	uint32_t mux_delay_config_us;
	/** Notch frequency of the digital filter */
	enum ltc2983_filter_notch_freq filter_notch_freq;
//	/** Num of channels used */
//	uint8_t num_channels;
	/** Sensors */
	struct ltc2983_sensor *sensors[20];
	/** Custom address pointer */
	uint16_t custom_addr_ptr;
	/** max number of channels */
	uint8_t max_channels_nr;
}LTC298X_TEMP;


/**
 * @brief common LTC2983 sensor struct
 */
struct ltc2983_sensor {
	/** sensor channel */
	uint8_t chan;
	/** sensor type */
	uint8_t type : 5;
};

/**
 * @brief custom LTC2983 sensor struct
 */
struct ltc2983_custom_sensor {
	/** raw table sensor data */
	uint32_t *table;
	/** data length */
	size_t len;
	/** indicates if sensor is steinhart */
	bool is_steinhart;
};

/**
 * @brief thermocouple sensor descriptor for LTC2983
 */
struct ltc2983_thermocouple {
	/** common LTC2983 sensor */
	struct ltc2983_sensor sensor;
	/** custom LTC2983 sensor */
	struct ltc2983_custom_sensor *custom;
	/** thermocouple sensor config */
	uint32_t sensor_config;
	/** Cold Junction Channel Pointer */
	uint32_t cold_junction_chan;
};

/**
 * @brief RTD sensor descriptor for LTC2983
 */
struct ltc2983_rtd {
	/** common LTC2983 sensor */
	struct ltc2983_sensor sensor;
	/** custom LTC2983 sensor */
	struct ltc2983_custom_sensor *custom;
	/** rtd sensor config */
	uint32_t sensor_config;
	/** Sense Resistor Channel Pointer */
	uint32_t r_sense_chan;
	/** Excitation Current selector */
	uint32_t excitation_current;
	/** RTD Curve selector */
	uint32_t rtd_curve;
};

/**
 * @brief thermistor sensor descriptor for LTC2983
 */
struct ltc2983_thermistor {
	/** common LTC2983 sensor */
	struct ltc2983_sensor sensor;
	/** custom LTC2983 sensor */
	struct ltc2983_custom_sensor *custom;
	/** thermistor sensor config */
	uint32_t sensor_config;
	/** Sense Resistor Channel Pointer */
	uint32_t r_sense_chan;
	/** Excitation Current selector */
	uint32_t excitation_current;
};

/**
 * @brief LTC2983 diode ideal factor value
 */
struct ideal_factor_value {
	/** integer part */
	uint32_t ideal_factor_val_int;
	/** decimal part with 1/1048576 precision */
	uint32_t ideal_factor_val_dec_1048576;
};

/**
 * @brief diode sensor descriptor for LTC2983
 */
struct ltc2983_diode {
	/** common LTC2983 sensor */
	struct ltc2983_sensor sensor;
	/** diode sensor config */
	uint32_t sensor_config;
	/** Excitation Current selector */
	uint32_t excitation_current;
	/** Diode Ideality Factor */
	struct ideal_factor_value ideal_factor_val;
};

/**
 * @brief LTC2983 sense resistance value
 */
struct r_sense_value {
	/** integer part */
	uint32_t r_sense_val_int;
	/** decimal part with 1/1024 precision */
	uint32_t r_sense_val_dec_1024;
};

/**
 * @brief sense resistor descriptor for LTC2983
 */
struct ltc2983_rsense {
	/** common LTC2983 sensor */
	struct ltc2983_sensor sensor;
	/** sense resistance value */
	struct r_sense_value r_sense_val;
};

/**
 * @brief direct ADC descriptor for LTC2983
 */
struct ltc2983_adc {
	/** common LTC2983 sensor */
	struct ltc2983_sensor sensor;
	/** indicates single-ended or differential */
	bool single_ended;
};

/**
 * @brief analog temperature sensor descriptor for LTC2983
 */
struct ltc2983_temp {
	/** common LTC2983 sensor */
	struct ltc2983_sensor sensor;
	/** custom LTC2983 sensor */
	struct ltc2983_custom_sensor *custom;
	/** indicates single-ended or differential */
	bool single_ended;
};

/**
 * @brief Device and comm init function
 * @param inst - The LTC298X instance.
 * @param spi_idx - The SPI port index.
 * @param dev_type - The device type (LTC2983, LTC2984, or LTC2986).
 * @param fnf - Filter notch frequency.
 * @param udelay - MUX configuration delay in microseconds.
 * @return 0 on success, error code otherwise.
 */
int ltc298x_init(LTC298X_TEMP *inst, unsigned char spi_idx, unsigned char dev_type, unsigned char fnf, unsigned char udelay);

/**
 * @brief Free resources allocated by the init function.
 * @param inst - The LTC298X instance to remove.
 * @return Currently not implemented, should return 0 on success.
 */
// int ltc298x_remove(LTC298X_TEMP *inst); // This function is not defined in your .c file yet

/**
 * @brief Read a single byte from a register.
 * @param reg_addr - The 16-bit register address.
 * @param rd - Pointer to an unsigned char to store the read data.
 * @return 0 on success, error code otherwise.
 */
int ltc298x_reg_read(unsigned short reg_addr, unsigned char *rd);

/**
 * @brief Write a single byte to a register.
 * @param reg_addr - The 16-bit register address.
 * @param d - The unsigned char data to write.
 * @return 0 on success, error code otherwise.
 */
int ltc298x_reg_write(unsigned short reg_addr, unsigned char d);

/**
 * @brief Update specific bits in a register.
 * @param reg_addr - The 16-bit register address.
 * @param mask - The bitmask of the bits to change.
 * @param val - The new value for the masked bits.
 * @return 0 on success, error code otherwise.
 */
int ltc298x_reg_update_bits(uint16_t reg_addr, uint8_t mask, uint8_t val);

/**
 * @brief Configure the LTC298X device based on the instance settings.
 * @param inst - The LTC298X instance to configure.
 * @return 0 on success, error code otherwise.
 */
int ltc298x_setup(LTC298X_TEMP *inst);

/** Read channel data / temperature */
int ltc2983_chan_read(LTC298X_TEMP *inst, const int chan, int *val);

/** Read raw channel data / temperature */
int ltc2983_chan_read_raw(LTC298X_TEMP *inst, const int chan, uint32_t *val);

/** Set scale of raw channel data / temperature */
int ltc2983_chan_read_scale(LTC298X_TEMP *inst, const int chan,
			    uint32_t *val, uint32_t *val2);

/** Channel assignment for thermocouple sensors */
int ltc2983_thermocouple_assign_chan(LTC298X_TEMP *inst,
				     const struct ltc2983_sensor *sensor);

/** Channel assignment for RTD sensors */
int ltc2983_rtd_assign_chan(LTC298X_TEMP *inst,
			    const struct ltc2983_sensor *sensor);

/** Channel assignment for thermistor sensors */
int ltc2983_thermistor_assign_chan(LTC298X_TEMP *inst,
				   const struct ltc2983_sensor *sensor);

/** Channel assignment for diode sensors */
int ltc2983_diode_assign_chan(LTC298X_TEMP *inst,
			      const struct ltc2983_sensor *sensor);

/** Channel assignment for rsense */
int ltc2983_r_sense_assign_chan(LTC298X_TEMP *inst,
				const struct ltc2983_sensor *sensor);

/** Channel assignment for direct ADC */
int ltc2983_adc_assign_chan(LTC298X_TEMP *inst,
			    const struct ltc2983_sensor *sensor);

/** Channel assignment for analog temp sensor */
int ltc2983_temp_assign_chan(LTC298X_TEMP *inst,
			     const struct ltc2983_sensor *sensor);

/** Fault handling of thermocouple sensors */
int ltc2983_thermocouple_fault_handler(const uint32_t);

/** Fault handling of sensors other than thermocouple */
int ltc2983_common_fault_handler(const uint32_t);

#endif
