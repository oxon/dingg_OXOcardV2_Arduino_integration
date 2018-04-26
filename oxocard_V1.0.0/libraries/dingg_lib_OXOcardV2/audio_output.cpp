/*******************************************************************************
* \file    audio_output.cpp
********************************************************************************
* \author		Tobias Meerstetter		tm@oxon.ch
* \author		Jascha Haldemann			jh@oxon.ch
* \date			29.01.2018
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
#include "audio_output.h"

/* FreeRTOS includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/* esp32 driver includes */
#include <driver/i2s.h>

/* ======================= Module constant declaration ====================== */

static const i2s_port_t I2S_PORT_NUM = i2s_port_t(0);

/* ======================== Module macro declaration ======================== */
//...

/* ======================== Module global variables ========================= */
i2s_config_t i2s_config = {
	.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
	.sample_rate = AudioRenderer::DEFAULT_SAMPLE_RATE,
	.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
	.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
	.communication_format = I2S_COMM_FORMAT_I2S_MSB,
	.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
	.dma_buf_count = 3,
	.dma_buf_len = AudioRenderer::DEFAULT_BUFFER_SIZE,
	.use_apll = false
	// .fixed_mclk = 0
};

/* Handle of the current running timer (NULL if no is running) */
TimerHandle_t xCurrentTimer;

/* Handle for the audio task. Used to delete it */
TaskHandle_t audioTaskHandle = NULL;

/* Reference to the audio renderer */
AudioRenderer *audioSource;

/* Buffer to get the data from the audio renderer */
uint32_t *sampleBuffer = NULL;

/* General variables */
bool isStart, isEnd;
uint8_t endPhase;

/* ====================== Module class instantiations ======================= */
// ...

/* ====================== Constructor & Deconstructor ======================= */
AudioOutput::AudioOutput(uint8_t shutdownPin, uint16_t sampleRate, uint16_t updateRate)
{
	shutdownPin_ = shutdownPin;
	/* Initialisize the I2S interface */
	init(sampleRate, updateRate);
}


AudioOutput::~AudioOutput()
{
	/* Deinitialisize the I2S interface and free the memory */
	deinit();
	free(sampleBuffer);
	sampleBuffer = NULL;
}

/* ========================== Callbacks and Tasks =========================== */
void vTimerCallback( TimerHandle_t xTimer )
{
	isEnd = true;
}

void tone_task(void *shutdownPinVoidPtr)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint16_t updateTicRate = i2s_config.dma_buf_len * 1000 /
		i2s_config.sample_rate / portTICK_PERIOD_MS;

	while(1)
	{
		if(endPhase <= 1)
		{
			/* Get the next buffer of data from the renderer */
			audioSource->getNextValues(sampleBuffer);

			if(isStart)
			{
				for(int cnt = 0; cnt < i2s_config.dma_buf_len; cnt++)
				{
					uint8_t data = ((sampleBuffer[cnt] >> 24) & 0x000000FF);
					uint8_t invertedData = audioSource->getAmplitude() - data;
					data *= (1.0f/i2s_config.dma_buf_len * cnt);
					invertedData *= (1.0f/i2s_config.dma_buf_len * cnt);
					sampleBuffer[cnt] = AudioRenderer::toI2sDac(data, invertedData);
				}
				isStart = false;
			}

			if(isEnd)
			{
				if(endPhase == 0)
				{
					for(int cnt = 0; cnt < i2s_config.dma_buf_len; cnt++)
					{
						uint8_t data = ((sampleBuffer[cnt] >> 24) & 0x000000FF);
						uint8_t invertedData = audioSource->getAmplitude() - data;
						data *= (1 - 1.0f/i2s_config.dma_buf_len * cnt);
						invertedData *= (1 - 1.0f/i2s_config.dma_buf_len * cnt);
						sampleBuffer[cnt] = AudioRenderer::toI2sDac(data, invertedData);
					}
				}
				else
				{
					for(int cnt = 0; cnt < i2s_config.dma_buf_len; cnt++)
					{
						sampleBuffer[cnt] = 0;
					}
				}

				endPhase++;
			}

			i2s_write_bytes(I2S_PORT_NUM, (const char*)sampleBuffer,
				i2s_config.dma_buf_len * sizeof(uint32_t), 0); //updateTicRate * 2

			vTaskDelayUntil( &xLastWakeTime, updateTicRate);
		}
		else
		{
			digitalWrite(*(uint8_t*)shutdownPinVoidPtr, LOW);
			i2s_zero_dma_buffer(I2S_PORT_NUM);
			isEnd = false;
			audioTaskHandle = NULL;
			vTaskDelete(NULL);
		}
	}
}

/* ======================== Public member Functions ========================= */
/**
 * Inits the audio output
 *
 * \param  sampleRate  Sampe rate of the audio output [sps] (default = 32000)
 * \param  updateRate  Update rate of the buffer [ms] (default = 10ms)
 */
void AudioOutput::init(uint16_t sampleRate, uint16_t updateRate)
{
	pinMode(shutdownPin_, OUTPUT);
	digitalWrite(shutdownPin_, LOW);
	updateRate_ = updateRate;
	/* Set the given rates in the init struct */
	i2s_config.sample_rate = sampleRate;
	i2s_config.dma_buf_len = sampleRate * updateRate / 1000;
	/* Initialize the I2S interface */
	ESP_LOGI(this->LOG_TAG, "Start I2S with sampleRate = %d and updateRate = %d",
			 sampleRate, updateRate);
	i2s_driver_install(I2S_PORT_NUM, &i2s_config, 0, NULL);
	i2s_set_pin(I2S_PORT_NUM, NULL);
	i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
	i2s_zero_dma_buffer(I2S_PORT_NUM);
	delay(updateRate);	// wait until at least one dma_buf is filled
	/* Allocate memory for the sample buffer */
	if (sampleBuffer == NULL)
		sampleBuffer = (uint32_t*)malloc(i2s_config.dma_buf_len * sizeof(uint32_t));
}

/**
 * Deinits the audio output
 */
void AudioOutput::deinit()
{
	digitalWrite(shutdownPin_, HIGH);
	i2s_stop(I2S_PORT_NUM);
	i2s_driver_uninstall(I2S_PORT_NUM);
	ESP_LOGI(this->LOG_TAG, "I2S deinitialized");
}

/**
 * Finction to set the actual audio renderer used to get the data from
 *
 * \param  TODO  Pointer to the audio renderer object
 */
void AudioOutput::setAudioRenderer(AudioRenderer *generator)
{
	audioSource = generator;
}

/**
 * Function to play a waveform with the given frequency and duration
 *
 * \param  duration  TODO
 */
void AudioOutput::play(uint32_t duration)
{
	if(endPhase >= 1)
		isStart = true;
	isEnd = false;
	endPhase = 0;

	if(audioTaskHandle == NULL)
		xTaskCreate(&tone_task, "tone_task", 4096, (void*)(&shutdownPin_), 7, &audioTaskHandle);

	/* If the timer should only play for a given time,
	   start a timer to stop it after the given duration */
	if(xCurrentTimer != NULL)
	{
		xTimerStop( xCurrentTimer, 0 );
		xTimerDelete( xCurrentTimer, 0 );
		xCurrentTimer = NULL;
	}

	if(duration < PLAY_FOREVER)
	{
		xCurrentTimer = xTimerCreate ("AudioTimer", (duration) / portTICK_PERIOD_MS,
			pdFALSE, (void*)0, vTimerCallback);
		xTimerStart( xCurrentTimer, 0 );
	}

	digitalWrite(shutdownPin_, HIGH);
}

/**
 * Function to stop the tone
 */
void AudioOutput::stop()
{
	isEnd = true;
	if(xCurrentTimer != NULL)
	{
		xTimerStop( xCurrentTimer, 0 );
		xTimerDelete( xCurrentTimer, 0 );
		xCurrentTimer = NULL;
	}
	delay(updateRate_);	// wait long enough that a running tone_task can finish
}

/* ======================= Private member Functions ========================= */

/* ======================== Private class Functions ========================= */
