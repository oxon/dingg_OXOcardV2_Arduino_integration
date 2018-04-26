#ifndef _LIB_AUDIO_OUTPUT_H_
#define _LIB_AUDIO_OUTPUT_H_
/*******************************************************************************
* \file		audio_output.h
********************************************************************************
* \author		Tobias Meerstetter		tm@oxon.ch
* \author		Jascha Haldemann			jh@oxon.ch
* \date			29.01.2018
* \version	1.0
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
class AudioOutput
{
public:
	/* Public member typedefs */
	//...

	/* Public constant declerations (static) */
	static const uint32_t PLAY_FOREVER = 0xFFFFFFFF;

	/* Public member data */
	//...

	/* Constructor and Destructor */
	AudioOutput(uint8_t shutdownPin, uint16_t sampleRate, uint16_t updateRate);
	~AudioOutput();

	/* Public member functions */
	void init(uint16_t sampleRate = AudioRenderer::DEFAULT_SAMPLE_RATE,
		uint16_t updateRate = AudioRenderer::DEFAULT_UPDATE_RATE);
	void deinit();
	void setAudioRenderer(AudioRenderer *generator);
	void play(uint32_t duration = PLAY_FOREVER);
	void stop();

private:
	/* Private constant declerations (static) */
	const char *LOG_TAG = "AudioOutput";

	/* Private member data */
	uint8_t shutdownPin_;
	uint16_t updateRate_ = AudioRenderer::DEFAULT_UPDATE_RATE;

	/* Private member functions */
	//...

	/* Private class functions (static) */
	//...

};

#endif
