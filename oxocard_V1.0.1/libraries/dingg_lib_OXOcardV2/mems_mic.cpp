/*******************************************************************************
* \file    mems_mic.cpp
********************************************************************************
* \author  Jascha Haldemann 		jh@oxon.ch
* \date    29.11.2017
* \version 1.0
*
* \license LGPL-V2.1
* Copyright (c) 2017 OXON AG. All rights reserved.
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, see 'http://www.gnu.org/licenses/'
*******************************************************************************/

/* ================================= Imports ================================ */
#include <esp_log.h>
#include "mems_mic.h"

/* esp32 driver includes */
#include <driver/i2s.h>

/* FreeRTOS includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/* ======================= Module constant declaration ====================== */
static const i2s_port_t I2S_PORT_NUM = i2s_port_t(0);

/* ======================== Module macro declaration ======================== */
//...

/* ======================== Module global variables ========================= */
i2s_config_t i2s_config_mic = {
	.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
	.sample_rate = AudioRenderer::DEFAULT_SAMPLE_RATE,
	.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
	.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,	// _ONLY_LEFT and _RIGHT_LEFT aren't working!
	.communication_format = I2S_COMM_FORMAT_PCM,
	.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
	.dma_buf_count = 2,
	.dma_buf_len = AudioRenderer::DEFAULT_BUFFER_SIZE,
	.use_apll = false
	// .fixed_mclk = 0
};

i2s_pin_config_t pin_config_mic = {
  .bck_io_num = I2S_PIN_NO_CHANGE,		// not used
  .ws_io_num = I2S_PIN_NO_CHANGE,			// set in init()
  .data_out_num = I2S_PIN_NO_CHANGE,	// not used
  .data_in_num = I2S_PIN_NO_CHANGE,		// set in init()
};

/* ====================== Module class instantiations ======================= */
// ...

/* ======================== Public member Functions ========================= */
/** ----------------------------------------------------------------------------
* \brief  Inits the i2s dma to read the PDM microphone
----------------------------------------------------------------------------- */
void MemsMic::init(uint16_t sampleRate, uint16_t updateRate)
{
	sampleRate_ = sampleRate;
	updateRate_ = updateRate;
	updateBufferSize();
	i2s_config_mic.sample_rate = sampleRate_;
	i2s_config_mic.dma_buf_len = bufferSize_;
	ESP_LOGI(this->LOG_TAG, "Start I2S with sampleRate = %d and updateRate = %d",
		sampleRate_, updateRate_);

	pin_config_mic.ws_io_num = pinNrWS_;
	pin_config_mic.data_in_num = pinNrDataIn_;

	i2s_driver_install(I2S_PORT_NUM, &i2s_config_mic, 0, NULL);
  i2s_set_pin(I2S_PORT_NUM, &pin_config_mic);
  i2s_zero_dma_buffer(I2S_PORT_NUM);
	vTaskDelay(updateRate_/portTICK_PERIOD_MS);	// wait until at least one dma_buf is filled
}

/** ----------------------------------------------------------------------------
* \brief  Deinits the i2s dma
----------------------------------------------------------------------------- */
void MemsMic::deinit()
{
	i2s_stop(I2S_PORT_NUM);
	i2s_driver_uninstall(I2S_PORT_NUM);
	ESP_LOGI(this->LOG_TAG, "I2S deinitialized");
}

/** ----------------------------------------------------------------------------
* \brief  reads an analog value from the microphone dma buffer
----------------------------------------------------------------------------- */
int16_t MemsMic::read()
{
	uint8_t sampleVal[2];	// bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT
	i2s_read_bytes(I2S_PORT_NUM, (char*)sampleVal, sizeof(uint16_t), 1000);
	return (((int16_t)(sampleVal[1]) << 8) | sampleVal[0]);	// int16 MSB
}

/** ----------------------------------------------------------------------------
* \brief  reads given number of values from the microphone dma buffer
----------------------------------------------------------------------------- */
void MemsMic::read(uint8_t *buffer, uint16_t numberOfValues)
{
	if (numberOfValues > i2s_config_mic.dma_buf_len)
	{
		ESP_LOGE(this->LOG_TAG, "Cannot read more bytes than dma_buf_size!");
		return;
	}
	for (uint16_t n = 0; n < numberOfValues; n++)
	{
		uint8_t sampleVal[2];	// bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT
		i2s_read_bytes(I2S_PORT_NUM, (char*)sampleVal, sizeof(uint16_t), 1000);
		uint8_t data = sampleVal[1] + (1 << 7);
		#ifdef MIC_AMP
			// amplify difference to 124
			int16_t ampData = ((int16_t)(data)-124) * MIC_AMP + 124;
			if (ampData > 255) ampData = 255;
			else if (ampData < 0) ampData = 0;
			data = ampData;
		#endif
		data = (uint16_t)(data) * amplitude_ / MAX_AMPLITUDE;	// scale within amplitude range
		memset(buffer+n, data, 1);	// uint8 MSB
	}
}

/** ----------------------------------------------------------------------------
* \brief  writes the next few calculated values into the given buffer
----------------------------------------------------------------------------- */
void MemsMic::getNextValues(uint32_t buffer[])
{
	for (uint16_t n = 0; n < bufferSize_; n++)
	{
		uint16_t uSample = read() + (1 << 15);							// read signed value and convert to unsigned
		uint8_t data = uSample >> 8;
		data = (uint16_t)(data) * amplitude_ / MAX_AMPLITUDE;	// scale within amplitude range
		// data = ((int16_t)(data)-124)*2+124;								// amplify difference to 124
		uint8_t invertedData = amplitude_ - data;
		buffer[n] = toI2sDac(data, invertedData);
	}
}

/* ======================= Private member Functions ========================= */

/* ======================== Private class Functions ========================= */
