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

#include <errno.h>
#include "ltc2983.h"
#include "..\..\..\Inc\comm.h"
#include "..\..\..\Inc\tim.h"

#include "..\DelayUtility\DelayUtility.h"
#include "NO_OS/no_os_alloc.h"
#include "NO_OS/no_os_print_log.h"

SPIM_IDX	ltc298x_SPI_IDX;

int			ltc298x_CS_IDX;
unsigned int	API_STEP_CFG_SET;
/******************************************************************************/

void LTC2986_SelectCS(int cs_idx){
	ltc298x_CS_IDX = cs_idx;
}

void LTC2986_CS_Set(int v){
	switch(ltc298x_CS_IDX){
		case 0:
			LTC298X_CS2(v);
			break;
		case 1:
			LTC298X_CS1(v);
			break;
		case 2:
			LTC298X_CS2(v);
			break;
		case 3:
			LTC298X_CS3(v);
			break;
        case 4:
            LTC298X_CS4(v);
            break;
//		case 0xfe:		//	broadcast without EX
//			SG_ECP5_CS0(v);
//			SG_ECP5_CS1(v);
//			SG_ECP5_CS2(v);
//			break;
		case 0xff:		//	broadcast
			LTC298X_CS2(v);
//			SG_ECP5_CS1(v);
//			SG_ECP5_CS2(v);
//			SG_ECP5_CSEX(v);
			break;
		default:
			break;
	}
}

/**
 * @brief Device and comm init function
 * @param spi_idx - Select which spi to use
 * @param dev_type -  Select which device to use
 * @param sensor_type - Select the sensor to use
 * @return 0 in case of success, errno errors otherwise
 */
int ltc298x_init(LTC298X_TEMP *inst, unsigned char spi_idx, unsigned char dev_type, unsigned char fnf, unsigned char udelay)
{
	int ret;
	unsigned char num_channels=0;
	ltc298x_SPI_IDX = spi_idx;


	switch (dev_type) {
	case ID_LTC2983:
	case ID_LTC2984:
		inst->max_channels_nr = 20;
		break;
	case ID_LTC2986:
		inst->max_channels_nr = 10;
		break;
	default:
		return -EINVAL;
	}

	inst->mux_delay_config_us = udelay;
	inst->filter_notch_freq = fnf;

	//Calculate active channels
//	for (int i = 0; i < inst->max_channels_nr+1; i++) {
	for (int i = 0; i < inst->max_channels_nr; i++) {
//		inst->sensors[i] = inst->sensors[i];
		if (inst->sensors[i] && inst->sensors[i]->type != LTC2983_RSENSE)
			num_channels++;
	}

	// should be at least one channel
	if (!num_channels) return 1;

	/* bring the device out of reset */
//	DelayuSec(1200);
	LTC298X_RESET(1);

	ret = ltc298x_setup(inst);
	if (ret)
		return 1;

	return 0;
}


int ltc298x_reg_read(unsigned short reg_addr, unsigned char *rd){
	int fail;
	unsigned char raw_array[4];
//	SPIM_SetDataSize(SPIM_FPGA, SPI_DATASIZE_8BIT);
	LTC2986_CS_Set(0);

	raw_array[0] = LTC2983_SPI_READ_BYTE;
	raw_array[1] = reg_addr >> 8;
	raw_array[2] = reg_addr & 0xFF;
	raw_array[3] = 0;

	fail  = SPIM_TransBlocking_n(ltc298x_SPI_IDX,raw_array, sizeof(raw_array));

	LTC2986_CS_Set(1);
//	SPIM_SetDataSize(SPIM_FPGA, SPI_DATASIZE_16BIT);
	*rd = raw_array[3];
	return fail;
}

///**
// * @brief Read raw register value
// * @param device - LTC2983 descriptor
// * @param reg_addr - register address
// * @param val - register value
// * @return 0 in case of success, negative error code otherwise
// */
//int ltc298x_reg_read(struct ltc2983_desc *device, uint16_t reg_addr,
//		     uint8_t *val)
//{
//	int ret;
//	uint8_t raw_array[4];
//
//	raw_array[0] = LTC2983_SPI_READ_BYTE;
//	no_os_put_unaligned_be16(reg_addr, raw_array + 1);
//	raw_array[3] = 0;
//
//	ret = no_os_spi_write_and_read(device->comm_desc, raw_array,
//				       NO_OS_ARRAY_SIZE(raw_array));
//	if (ret)
//		return ret;
//	*val = raw_array[3];
//
//	return 0;
//}

int ltc298x_reg_write(unsigned short reg_addr, unsigned char d){
	int fail;
	unsigned char raw_array[4];
//	SPIM_SetDataSize(SPIM_FPGA, SPI_DATASIZE_8BIT);
	LTC2986_CS_Set(0);

	raw_array[0] = LTC2983_SPI_WRITE_BYTE;
	raw_array[1] = reg_addr >> 8;
	raw_array[2] = reg_addr & 0xFF;
	raw_array[3] = d;

	fail  = SPIM_TransBlocking_n(ltc298x_SPI_IDX, raw_array, sizeof(raw_array));

	LTC2986_CS_Set(1);
//	SPIM_SetDataSize(SPIM_FPGA, SPI_DATASIZE_16BIT);
	return fail;
}

int ltc298x_reg_rw_n(unsigned char cmd, unsigned short reg_addr, unsigned char *d,unsigned short data_size){
	int fail;
	unsigned char raw_array[3+data_size];
//	SPIM_SetDataSize(SPIM_FPGA, SPI_DATASIZE_8BIT);
	LTC2986_CS_Set(0);

	raw_array[0] = cmd;
	raw_array[1] = reg_addr >> 8;
	raw_array[2] = reg_addr & 0xFF;
	if(cmd==LTC2983_SPI_WRITE_BYTE)memcpy(raw_array+3,d,data_size);
	else memset(raw_array+3,0,data_size);

	fail  = SPIM_TransBlocking_n(ltc298x_SPI_IDX,raw_array, 3+data_size);

	LTC2986_CS_Set(1);
//	SPIM_SetDataSize(SPIM_FPGA, SPI_DATASIZE_16BIT);
	if(cmd==LTC2983_SPI_READ_BYTE){
		for(int i=0; i<data_size; i++)
			*(d+i) = raw_array[3+i];
	}
//		memcpy(d,&raw_array[3],data_size);

//	*rd = raw_array[3];
	return fail;
}

///**
// * @brief Write raw register value
// * @param device - LTC2983 descriptor
// * @param reg_addr - register address
// * @param val - register value
// * @return 0 in case of success, negative error code otherwise
// */
//int ltc298x_reg_write(struct ltc2983_desc *device, uint16_t reg_addr,
//		      uint8_t val)
//{
//	int ret;
//	uint8_t raw_array[4];
//
//	raw_array[0] = LTC2983_SPI_WRITE_BYTE;
//	no_os_put_unaligned_be16(reg_addr, raw_array + 1);
//	raw_array[3] = val;
//
//	ret = no_os_spi_write_and_read(device->comm_desc, raw_array,
//				       NO_OS_ARRAY_SIZE(raw_array));
//	if (ret)
//		return ret;
//
//	return 0;
//}

/**
 * @brief Update register value
 * @param device - LTC2983 descriptor
 * @param reg_addr - register address
 * @param mask - Mask for specific register bits to be updated
 * @param val - register value to be written
 * @return 0 in case of success, negative error code otherwise
 */
int ltc298x_reg_update_bits(uint16_t reg_addr,
			    uint8_t mask, uint8_t val)
{
	int ret;
	uint8_t data;

	ret = ltc298x_reg_read(reg_addr, &data);
	if (ret)
		return ret;

	data = (data & ~mask) | no_os_field_prep(mask, val);

	return ltc298x_reg_write(reg_addr, data);
}

/**
 * @brief Device setup
 * @param device - LTC2983 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc298x_setup(LTC298X_TEMP *inst)
{
	int ret, i;
	uint8_t status = 0;
	uint32_t timeout = 250000;

	/**
	 * make sure the device is up: start bit (7) is 0 and done bit (6) is 1
	 */
	do {
		ret = ltc298x_reg_read(LTC2983_STATUS_REG, &status);
		if (ret)
			return ret;
//		DelayuSec(25000);
	} while (LTC2983_STATUS_UP(status) != 1 && --timeout);
	if (!timeout)
		return -EINVAL;

	ret = ltc298x_reg_update_bits(LTC2983_GLOBAL_CONFIG_REG,
				      LTC2983_NOTCH_FREQ_MASK,
					  inst->filter_notch_freq);
	if (ret)
		return ret;

	ret = ltc298x_reg_write(LTC2983_MUX_CONFIG_REG,
			inst->mux_delay_config_us / 100);
	if (ret)
		return ret;

	for (i = 0; i < inst->max_channels_nr+1; i++) {
		if (!inst->sensors[i])
			continue;

		switch (inst->sensors[i]->type) {
		case LTC2983_THERMOCOUPLE_J:
		case LTC2983_THERMOCOUPLE_K:
		case LTC2983_THERMOCOUPLE_E:
		case LTC2983_THERMOCOUPLE_N:
		case LTC2983_THERMOCOUPLE_R:
		case LTC2983_THERMOCOUPLE_S:
		case LTC2983_THERMOCOUPLE_T:
		case LTC2983_THERMOCOUPLE_B:
		case LTC2983_THERMOCOUPLE_CUSTOM:
			ltc2983_thermocouple_assign_chan(inst, inst->sensors[i]);
			break;
		case LTC2983_RTD_PT_10:
		case LTC2983_RTD_PT_50:
		case LTC2983_RTD_PT_100:
		case LTC2983_RTD_PT_200:
		case LTC2983_RTD_PT_500:
		case LTC2983_RTD_PT_1000:
		case LTC2983_RTD_1000:
		case LTC2983_RTD_NI_120:
		case LTC2983_RTD_CUSTOM:
			ltc2983_rtd_assign_chan(inst, inst->sensors[i]);
			break;
		case LTC2983_THERMISTOR_44004_44033:
		case LTC2983_THERMISTOR_44005_44030:
		case LTC2983_THERMISTOR_44007_44034:
		case LTC2983_THERMISTOR_44006_44031:
		case LTC2983_THERMISTOR_44008_44032:
		case LTC2983_THERMISTOR_YSI_400:
		case LTC2983_THERMISTOR_SPECTRUM_1003K:
		case LTC2983_THERMISTOR_CUSTOM_STEINHART_HART:
		case LTC2983_THERMISTOR_CUSTOM:
			ltc2983_thermistor_assign_chan(inst, inst->sensors[i]);
			break;
		case LTC2983_DIODE:
			ltc2983_diode_assign_chan(inst, inst->sensors[i]);
			break;
		case LTC2983_RSENSE:
			ltc2983_r_sense_assign_chan(inst, inst->sensors[i]);
			break;
		case LTC2983_DIRECT_ADC:
			ltc2983_adc_assign_chan(inst, inst->sensors[i]);
			break;
		case LTC2983_ACTIVE_TEMP:
			ltc2983_temp_assign_chan(inst, inst->sensors[i]);
			break;
		default:
			return -EINVAL;
		}
	}

	return 0;
}

/**
 * @brief Read channel data / temperature
 * @param device - LTC2983 descriptor
 * @param chan - channel number
 * @param val - channel data / temperature
 * @return 0 in case of success, errno errors otherwise
 */
//int ltc2983_chan_read(LTC298X_TEMP *inst, const int chan, int *val)
//{
//	static uint32_t raw_val, scale_val, scale_val2;
//	int ret;
//	int	fail;
//
////	if (inst->sensors[chan]->type == LTC2983_RSENSE) {
////		*val = -1;
////		return 0;
////	}
////
////	ret = ltc2983_chan_read_raw(inst, chan, &raw_val);
////	if (ret)
////		return ret;
////
////	ltc2983_chan_read_scale(inst, chan, &scale_val, &scale_val2);
////	if (ret)
////		return ret;
////
////	*val = no_os_sign_extend32(raw_val, 23) * scale_val / scale_val2;
//
//	ret = 0;
//	switch(API_STEP_CFG_SET){
//	case 0:		//
//		if (inst->sensors[chan]->type == LTC2983_RSENSE) {
//			*val = -1;
//			API_STEP_CFG_SET=0;
//			return ret&0xF7F;
//		}
//		raw_val=scale_val=scale_val2=0;
//		*val = 0;
//		API_STEP_CFG_SET++;
//		break;
//	case 1:
//		ret = ltc2983_chan_read_raw(inst, chan, &raw_val);
//		API_STEP_CFG_SET++;
//		break;
//	case 2:
//		ret = ltc2983_chan_read_scale(inst, chan, &scale_val, &scale_val2);
//		API_STEP_CFG_SET++;
//		break;
//	case 3:
//		*val = no_os_sign_extend32(raw_val, 23) * scale_val / scale_val2;
//		API_STEP_CFG_SET=0;
//		return ret&0xF7F;
//
//	}
//
//	return ret|0x080;
//}

int ltc2983_chan_read(LTC298X_TEMP *inst, const int chan, int *val)
{
	static uint32_t raw_val, scale_val, scale_val2;
	int ret;

	ret = 0;
	switch(API_STEP_CFG_SET){
	case 0:		//
		if (inst->sensors[chan]->type == LTC2983_RSENSE) {
			*val = -1;
			API_STEP_CFG_SET=0;
			return ret&0xF7F;
		}
		raw_val=scale_val=scale_val2=0;
		*val = 0;
		API_STEP_CFG_SET++;
		break;
//	case 1:
//		ret = ltc2983_chan_read_raw(inst, chan, &raw_val);
//		API_STEP_CFG_SET++;
//		break;
	case 1:
		uint32_t start_conversion = 0;
		start_conversion = LTC2983_STATUS_START(true);
		start_conversion |= LTC2983_STATUS_CHAN_SEL(chan);
		/* start conversion */
		ret = ltc298x_reg_write(LTC2983_STATUS_REG, start_conversion);
		if (ret) {
			API_STEP_CFG_SET=0;
			break;
		}
		//Delay(300);
		Timer2_SetChannel(TIMER2_CH_LTC2983DELAY,300,1);	// 300ms
		API_STEP_CFG_SET++;
		break;
	case 2://	wait
		if(!Timer2_GetChannel(TIMER2_CH_LTC2983DELAY)){	//	time up
			//	disable timer channel
			Timer2_SetChannel(TIMER2_CH_LTC2983DELAY,300,0);
			API_STEP_CFG_SET++;
//			API_PowerSetReset(0);
//			SG_ECP5_Enable(1);
		}
		break;
	case 3:
		uint8_t raw_array[7];
		no_os_put_unaligned_be32(0, raw_array);
	//	ret = no_os_spi_write_and_read(device->comm_desc, raw_array,
	//				       NO_OS_ARRAY_SIZE(raw_array));
		ret = ltc298x_reg_rw_n(LTC2983_SPI_READ_BYTE,LTC2983_CHAN_RES_ADDR(chan), raw_array, 4);
		if (ret) {
			API_STEP_CFG_SET=0;
			break;
		}

		raw_val = no_os_get_unaligned_be32(raw_array);

		if (!(LTC2983_RES_VALID_MASK & raw_val)) {
			pr_err("Channel %d: Invalid conversion detected\r\n", chan);
			ret = -EIO;
			API_STEP_CFG_SET=0;
			break;
		}
		API_STEP_CFG_SET++;
		break;
	case 4:
		if (inst->sensors[chan]->type <= LTC2983_THERMOCOUPLE_CUSTOM)
			ret = ltc2983_thermocouple_fault_handler(raw_val);
		else
			ret = ltc2983_common_fault_handler(raw_val);

		if (ret) {
			API_STEP_CFG_SET=0;
			break;
		}

		API_STEP_CFG_SET++;
		break;
	case 5:
		raw_val = no_os_sign_extend32((raw_val) & LTC2983_DATA_MASK, LTC2983_DATA_SIGN_BIT);
		API_STEP_CFG_SET++;
		break;
	case 6:
		ret = ltc2983_chan_read_scale(inst, chan, &scale_val, &scale_val2);
		API_STEP_CFG_SET++;
		break;
	case 7:
		*val = no_os_sign_extend32(raw_val, 23) * scale_val / scale_val2;
		API_STEP_CFG_SET=0;
		return ret&0xF7F;

	}

	return ret|0x080;
}

/**
 * @brief Read raw channel data / temperature
 * @param device - LTC2983 descriptor
 * @param chan - channel number
 * @param val - raw channel data / temperature
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_chan_read_raw(LTC298X_TEMP *inst, const int chan, uint32_t *val)
{
	uint32_t start_conversion = 0;
	uint8_t raw_array[7];
	int ret;

	start_conversion = LTC2983_STATUS_START(true);
	start_conversion |= LTC2983_STATUS_CHAN_SEL(chan);
	/* start conversion */
	ret = ltc298x_reg_write(LTC2983_STATUS_REG, start_conversion);
	if (ret)
		return ret;

	Delay(300);

	/* read the converted data */
//	raw_array[0] = LTC2983_SPI_READ_BYTE;
//	no_os_put_unaligned_be16(LTC2983_CHAN_RES_ADDR(chan), raw_array + 1);
	no_os_put_unaligned_be32(0, raw_array);
//	ret = no_os_spi_write_and_read(device->comm_desc, raw_array,
//				       NO_OS_ARRAY_SIZE(raw_array));
	ret = ltc298x_reg_rw_n(LTC2983_SPI_READ_BYTE,LTC2983_CHAN_RES_ADDR(chan), raw_array, 4);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be32(raw_array);

	if (!(LTC2983_RES_VALID_MASK & *val)) {
		pr_err("Channel %d: Invalid conversion detected\r\n", chan);
		return -EIO;
	}

	if (inst->sensors[chan]->type <= LTC2983_THERMOCOUPLE_CUSTOM)
		ret = ltc2983_thermocouple_fault_handler(*val);
	else
		ret = ltc2983_common_fault_handler(*val);
	if (ret)
		return ret;

	*val = no_os_sign_extend32((*val) & LTC2983_DATA_MASK,
				   LTC2983_DATA_SIGN_BIT);
	return 0;
}

/**
 * @brief Set scale of raw channel data / temperature
 * @param device - LTC2983 descriptor
 * @param chan - channel number
 * @param val - scale numerator
 * @param val2 - scale denominator
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_chan_read_scale(LTC298X_TEMP *inst, const int chan,
			    uint32_t *val, uint32_t *val2)
{
	if (inst->sensors[chan]->type == LTC2983_DIRECT_ADC) {
		/* value in millivolt */
		*val = 1000;
		/* 2^21 */
		*val2 = 2097152;
	} else {
		/* value in milli degrees */
		*val = 1000;
		/* 2^10 */
		*val2 = 1024;
	}
	return 0;
}

/**
 * @brief Channel assignment for common register fields
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @param chan_val - channel assignment value to be written
 * @return 0 in case of success, errno errors otherwise
 */
static int __ltc2983_chan_assign_common(LTC298X_TEMP *inst,
					const struct ltc2983_sensor *sensor,
					uint32_t chan_val)
{
	uint8_t raw_array[7];

	chan_val |= LTC2983_CHAN_TYPE(sensor->type);

	raw_array[0] = LTC2983_SPI_WRITE_BYTE;
	no_os_put_unaligned_be16(LTC2983_CHAN_START_ADDR(sensor->chan),
				 raw_array + 1);
	no_os_put_unaligned_be32(chan_val, raw_array);

//	return no_os_spi_write_and_read(device->comm_desc, raw_array,
//					NO_OS_ARRAY_SIZE(raw_array));
	return ltc298x_reg_rw_n(LTC2983_SPI_WRITE_BYTE,LTC2983_CHAN_START_ADDR(sensor->chan), raw_array, 4);
}

/**
 * @brief Channel assignment for custom register fields
 * @param device - LTC2983 descriptor
 * @param custom - LTC2983 custom sensor descriptor
 * @param chan_val - channel assignment value to be written
 * @return 0 in case of success, errno errors otherwise
 */
static int __ltc2983_chan_custom_sensor_assign(LTC298X_TEMP *inst,
		struct ltc2983_custom_sensor *custom,
		uint32_t *chan_val)
{
	int ret;
	uint32_t i;
	uint8_t raw_array[6];
	uint8_t step = custom->is_steinhart ? 4 : 3;

	if (inst->custom_addr_ptr + (custom->len * step) >
	    LTC2983_CUST_SENS_TBL_SIZE)
		return -EINVAL;

	*chan_val |= LTC2983_CUSTOM_LEN(custom->len - 1);
	*chan_val |= LTC2983_CUSTOM_ADDR(inst->custom_addr_ptr);

	raw_array[0] = LTC2983_SPI_WRITE_BYTE;
	for (i = 0; i < custom->len; i++, inst->custom_addr_ptr += step) {
//		no_os_put_unaligned_be16(inst->custom_addr_ptr,
//					 raw_array + 1);
		no_os_put_unaligned_be24(custom->table[i], raw_array);
//		ret = no_os_spi_write_and_read(device->comm_desc, raw_array,
//					       NO_OS_ARRAY_SIZE(raw_array));
		ret = ltc298x_reg_rw_n(LTC2983_SPI_WRITE_BYTE,inst->custom_addr_ptr, raw_array, 3);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Channel assignment for thermocouple sensors
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_thermocouple_assign_chan(LTC298X_TEMP *inst,
				     const struct ltc2983_sensor *sensor)
{
	struct ltc2983_thermocouple *thermo = to_thermocouple(sensor);
	uint32_t chan_val;

	chan_val = LTC2983_CHAN_ASSIGN(thermo->cold_junction_chan);
	chan_val |= LTC2983_THERMOCOUPLE_CFG(thermo->sensor_config);

	if (thermo->custom) {
		int ret;

		ret = __ltc2983_chan_custom_sensor_assign(inst,
				thermo->custom,
				&chan_val);
		if (ret)
			return ret;
	}
	return __ltc2983_chan_assign_common(inst, sensor, chan_val);
}

/**
 * @brief Channel assignment for RTD sensors
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_rtd_assign_chan(LTC298X_TEMP *inst,
			    const struct ltc2983_sensor *sensor)
{
	struct ltc2983_rtd *rtd = to_rtd(sensor);
	uint32_t chan_val;

	chan_val = LTC2983_CHAN_ASSIGN(rtd->r_sense_chan);
	chan_val |= LTC2983_RTD_CFG(rtd->sensor_config);
	chan_val |= LTC2983_RTD_EXC_CURRENT(rtd->excitation_current);
	chan_val |= LTC2983_RTD_CURVE(rtd->rtd_curve);

	if (rtd->custom) {
		int ret;

		ret = __ltc2983_chan_custom_sensor_assign(inst, rtd->custom,
				&chan_val);
		if (ret)
			return ret;
	}
	return __ltc2983_chan_assign_common(inst, sensor, chan_val);
}

/**
 * @brief Channel assignment for thermistor sensors
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_thermistor_assign_chan(LTC298X_TEMP *inst,
				   const struct ltc2983_sensor *sensor)
{
	struct ltc2983_thermistor *thermistor = to_thermistor(sensor);
	uint32_t chan_val;

	chan_val = LTC2983_CHAN_ASSIGN(thermistor->r_sense_chan);
	chan_val |= LTC2983_THERMISTOR_CFG(thermistor->sensor_config);
	chan_val |=
		LTC2983_THERMISTOR_EXC_CURRENT(thermistor->excitation_current);

	if (thermistor->custom) {
		int ret;

		ret = __ltc2983_chan_custom_sensor_assign(inst,
				thermistor->custom,
				&chan_val);
		if (ret)
			return ret;
	}
	return __ltc2983_chan_assign_common(inst, sensor, chan_val);
}

/**
 * @brief Channel assignment for diode sensors
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_diode_assign_chan(LTC298X_TEMP *inst,
			      const struct ltc2983_sensor *sensor)
{
	struct ltc2983_diode *diode = to_diode(sensor);
	uint32_t chan_val;

	chan_val = LTC2983_DIODE_CFG(diode->sensor_config);
	chan_val |= LTC2983_DIODE_EXC_CURRENT(diode->excitation_current);
	chan_val |=
		LTC2983_DIODE_IDEAL_FACTOR(
			(diode->ideal_factor_val.ideal_factor_val_int << 20) +
			diode->ideal_factor_val.ideal_factor_val_dec_1048576);

	return __ltc2983_chan_assign_common(inst, sensor, chan_val);
}

/**
 * @brief Channel assignment for rsense
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_r_sense_assign_chan(LTC298X_TEMP *inst,
				const struct ltc2983_sensor *sensor)
{
	struct ltc2983_rsense *rsense = to_rsense(sensor);
	uint32_t chan_val;

	chan_val =
		LTC2983_R_SENSE_VAL((rsense->r_sense_val.r_sense_val_int << 10) +
				    rsense->r_sense_val.r_sense_val_dec_1024);

	return __ltc2983_chan_assign_common(inst, sensor, chan_val);
}

/**
 * @brief Channel assignment for direct ADC
 * @param device - LTC2983 descriptor
 * @param sensor - LTC2983 common sensor descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_adc_assign_chan(LTC298X_TEMP *inst,
			    const struct ltc2983_sensor *sensor)
{
	struct ltc2983_adc *adc = to_adc(sensor);
	uint32_t chan_val;

	chan_val = LTC2983_ADC_SINGLE_ENDED(adc->single_ended);

	return __ltc2983_chan_assign_common(inst, sensor, chan_val);
}

int ltc2983_temp_assign_chan(LTC298X_TEMP *inst,
			     const struct ltc2983_sensor *sensor)
{
	struct ltc2983_temp *temp = to_temp(sensor);
	uint32_t chan_val;
	int ret;

	chan_val = LTC2983_ADC_SINGLE_ENDED(temp->single_ended);

	ret = __ltc2983_chan_custom_sensor_assign(inst, temp->custom,
			&chan_val);
	if (ret)
		return ret;

	return __ltc2983_chan_assign_common(inst, sensor, chan_val);
}

/**
 * @brief Fault handling of LTC2983 sensors
 * @param result - result captured that contains the faults
 * @param hard_mask - mask for hard faults
 * @param soft_mask - mask for soft faults
 * @return 0 in case of success, errno errors otherwise
 */
static int __ltc2983_fault_handler(const uint32_t result,
				   const uint32_t hard_mask,
				   const uint32_t soft_mask)
{
	if (result & hard_mask) {
		pr_info("Hard fault!\r\n");
		return -EIO;
	} else if (result & soft_mask) {
		/* just print a warning */
		pr_info("Soft fault!\r\n");
	}

	return 0;
}

/**
 * @brief Fault handling of thermocouple sensors
 * @param result - result captured that contains the faults
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_thermocouple_fault_handler(const uint32_t result)
{
	return __ltc2983_fault_handler(result,
				       LTC2983_THERMOCOUPLE_HARD_FAULT_MASK,
				       LTC2983_THERMOCOUPLE_SOFT_FAULT_MASK);
}

/**
 * @brief Fault handling of sensors other than thermocouple
 * @param result - result captured that contains the faults
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_common_fault_handler(const uint32_t result)
{
	return __ltc2983_fault_handler(result,
				       LTC2983_COMMON_HARD_FAULT_MASK,
				       LTC2983_COMMON_SOFT_FAULT_MASK);
}
