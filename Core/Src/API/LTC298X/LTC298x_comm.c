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

#include "LTC298x_comm.h"
/**
 * Thermistor fixed simulator connected to channel 10
 * Sense Resistor: Channel 2
 * Expected: -30.59 C
 */
struct ltc2983_thermocouple ltc2983_thermocouple_k_9 = {
	.sensor = {
		.chan = 9,
		.type = LTC2983_THERMOCOUPLE_K,
	},
	.sensor_config = 0xC,
	.cold_junction_chan = 0xA,
};

/**
 * RTD fixed simulator connected to channels 3-6
 * Type: PT-100
 * Sense resistor: channel 3
 */
struct ltc2983_diode ltc2983_diode_10 = {
	.sensor = {
		.chan = 10,
		.type = LTC2983_DIODE,
	},
	.sensor_config = 0x5,
	.excitation_current = 0x5,
//	.ideal_factor_val = {
//			.ideal_factor_val_int = 1,
//			.ideal_factor_val_dec_1048576 = 3145,
//	},
};
