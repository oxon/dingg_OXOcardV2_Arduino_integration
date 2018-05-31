/*******************************************************************************
* \file    wave_form_generator.cpp
********************************************************************************
* \author  Jascha Haldemann 		jh@oxon.ch
* \author  Tobias Meerstetter		tm@oxon.ch
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
#include <math.h>
#include "wave_form_generator.h"

/* ======================= Module constant declaration ====================== */
//...

/* ======================== Module macro declaration ======================== */
//...

/* ======================== Module global variables ========================= */
//...

/* ====================== Module class instantiations ======================= */
//...

/* ======================== Public member Functions ========================= */
/** -------------------------------------------------------------------------
* \brief	calculates a wave form with the given parameters
--------------------------------------------------------------------------- */
void WaveFormGenerator::setup(WaveFormGenerator::waveForm_t waveForm, uint32_t frequency)
{
	/* calculate one period of the given wave form */
	waveForm_ = waveForm;
	ESP_LOGD(this->LOG_TAG, "waveForm_ = %d", waveForm_);
	if (frequency < MIN_FREQUENCY) {frequency = MIN_FREQUENCY;  ESP_LOGW(this->LOG_TAG, "frequency was limited to %dHz", frequency);}
	else if (frequency > MAX_FREQUENCY) {frequency = MAX_FREQUENCY; ESP_LOGW(this->LOG_TAG, "frequency was limited to %dHz", frequency);}
	frequency_ = frequency;
	ESP_LOGD(this->LOG_TAG, "frequency_ = %d", frequency_);
	newOnePeriodBufferSize_ = sampleRate_/frequency_;
	if(onePeriodBufferSize_ == 0)
		onePeriodBufferSize_ = newOnePeriodBufferSize_;
	ESP_LOGD(this->LOG_TAG, "newOnePeriodBufferSize_ = %d", newOnePeriodBufferSize_);
	uint8_t data = 0;				// only allow 8bit data for now
	// uint32_t us = micros();
	nextBuffer = activeBuffer + 1;
	if(nextBuffer >= NUMBER_OF_BUFFERS)
		nextBuffer = 0;
	for (uint16_t t = 0; t < newOnePeriodBufferSize_; t++)
	{
		switch(waveForm_)
		{
			case WAVE_SQUARE:
			{
				if (t < newOnePeriodBufferSize_/2) data = 0;
				else data = MAX_AMPLITUDE;
			}break;
			case WAVE_SAWTOOTH:
			{
				data = (uint8_t)((uint32_t)(t)*MAX_AMPLITUDE/newOnePeriodBufferSize_);
			}break;
			case WAVE_TRIANGLE:
			{
				const uint8_t HALF_BUF = newOnePeriodBufferSize_/2;
				if (t < HALF_BUF) data = (uint8_t)((uint32_t)(t)*MAX_AMPLITUDE/HALF_BUF);
				else data = (uint8_t)(MAX_AMPLITUDE-(uint32_t)(t-HALF_BUF)*MAX_AMPLITUDE/HALF_BUF);
			}break;
			case WAVE_SINE:
			{
				const double CONST_RAD = 2UL*PI*frequency_/sampleRate_;//+tOffset;	// 2*PI*f*dt
				const uint8_t HALF_AMP = MAX_AMPLITUDE/2;

				data = (uint8_t)(HALF_AMP*sin(t*CONST_RAD)+HALF_AMP);
			}break;
			case WAVE_RANDOM:
			{
				data = (uint8_t)(random(MAX_AMPLITUDE));
			}break;
			default:
			{
				ESP_LOGE(this->LOG_TAG, "Invalid wave form!");
				return;
			}
		}
		// ESP_LOGD(this->LOG_TAG, "f = %d", frequency_);	//blup
		// ESP_LOGD(this->LOG_TAG, "t = %d; data = %d", t, data);	//blup
		waveFormBuffer[nextBuffer][t] = data;
		changeBuffer = true;
		// if (t == 0) us = micros()-us;
		// ESP_LOGD(this->LOG_TAG, "waveFormBuffer[%d] = %d", t, waveFormBuffer[t]);	//blup
	}
	// ESP_LOGD(this->LOG_TAG, "dt = %dus", us);
}

/** -------------------------------------------------------------------------
* \brief  writes the next few calculated values into the given buffer
--------------------------------------------------------------------------- */
void WaveFormGenerator::getNextValues(uint32_t buffer[])
{
	for (uint16_t n = 0; n < bufferSize_; n++)
	{
		if ((onePeriodBufferIndex >= onePeriodBufferSize_))
		{
			onePeriodBufferIndex = 0;
			if(changeBuffer)
			{
				activeBuffer = nextBuffer;
				onePeriodBufferSize_ = newOnePeriodBufferSize_;
				changeBuffer = false;
			}
		}
		uint8_t data = waveFormBuffer[activeBuffer][onePeriodBufferIndex];
		data = (uint16_t)(data) * amplitude_ / MAX_AMPLITUDE;	// scale within amplitude range
		uint8_t invertedData = amplitude_ - data;
		buffer[n] = toI2sDac(data, invertedData);
		onePeriodBufferIndex++;
	}
}

/** -------------------------------------------------------------------------
* \brief	updates the wave form by calculating it anew
--------------------------------------------------------------------------- */
void WaveFormGenerator::updateWaveForm(WaveFormGenerator::waveForm_t waveForm)
{
	setup(waveForm, frequency_);
}

/** -------------------------------------------------------------------------
* \brief	updates the frequency by calculating a new wave form
--------------------------------------------------------------------------- */
void WaveFormGenerator::updateFrequency(uint16_t frequency)
{
	setup(waveForm_, frequency);
}

/* ======================= Private member Functions ========================= */

/* ======================== Private class Functions ========================= */
/** -------------------------------------------------------------------------
  * \fn     sgn
  * \brief  returns the sign of the given variable
  *
  * \param  f   float value
  * \return 1 if > 0, 0 if 0 or -1 if < 0
  --------------------------------------------------------------------------- */
int8_t WaveFormGenerator::sgn(double f)
{
	int8_t i = 0;
	i += f > 0 ? 1 : -1;
	return i;
}
