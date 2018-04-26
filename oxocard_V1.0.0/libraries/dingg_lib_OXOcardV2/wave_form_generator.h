#ifndef _WAVE_FORM_GENERATOR_H_
#define _WAVE_FORM_GENERATOR_H_
/*******************************************************************************
* \file		wave_form_generator.h
********************************************************************************
* \author		Jascha Haldemann			jh@oxon.ch
* \author		Tobias Meerstetter		tm@oxon.ch
* \date			29.11.2017
* \version	1.0
*
* \brief	Use the WaveFormGenerator class to setup a waveFormGenerator object
*					and use the getNextValue()-function with given data struct to
*					get the next values calculated according the configured parameters.
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
********************************************************************************
* OXOcard2
*******************************************************************************/

/* ============================== Global imports ============================ */
#include <Arduino.h>
#include "audio_renderer.h"

/* ==================== Global module constant declaration ================== */

/* ========================= Global macro declaration ======================= */

/* ============================ Class declaration =========================== */
class WaveFormGenerator : public AudioRenderer
{
public:

	/* Public member typedefs */
	typedef enum : uint8_t
	{
		WAVE_SQUARE = 0,
		WAVE_SAWTOOTH,
		WAVE_TRIANGLE,
		WAVE_SINE,
		WAVE_RANDOM
	} waveForm_t;

	/* Public constant declerations (static) */
	static const uint16_t MIN_FREQUENCY = 100;

	/* Public member data */
	//...

	/* Constructor and Destructor */
	WaveFormGenerator(uint16_t sampleRate, uint16_t updateRate, uint8_t amplitude,
		uint8_t resolution = AudioRenderer::DEFAULT_RESOLUTION) :
		AudioRenderer(sampleRate, updateRate, amplitude, resolution) {
		activeBuffer = 0;
	}
	~WaveFormGenerator() {}

	/* Public member functions */
	void setup(waveForm_t waveForm, uint32_t frequency);
	void getNextValues(uint32_t buffer[]);
	void updateWaveForm(waveForm_t waveForm);
	void updateFrequency(uint16_t frequency);

private:
	/* Private constant declerations (static) */
	const char *LOG_TAG = "WaveFormGenerator";
	static const uint16_t MAX_FREQUENCY = 6400;		// onePeriodBufferSize is only 5 in this case
	static const uint16_t MAX_WAVE_FORM_BUFFER_SIZE = 320;	//sampleRate/MIN_FREQUENCY = 32kHz/100Hz
	static const uint8_t NUMBER_OF_BUFFERS = 2;

	/* Private member data */
	waveForm_t waveForm_;
	uint32_t frequency_;
	uint8_t waveFormBuffer[NUMBER_OF_BUFFERS][MAX_WAVE_FORM_BUFFER_SIZE];
	uint8_t activeBuffer, nextBuffer;
	bool changeBuffer;
	uint16_t newOnePeriodBufferSize_, onePeriodBufferSize_ = 0;
	uint16_t onePeriodBufferIndex = 0;

	/* Private member functions */
	//...

	/* Private class functions (static) */
	static int8_t sgn(double f);

};

#endif
