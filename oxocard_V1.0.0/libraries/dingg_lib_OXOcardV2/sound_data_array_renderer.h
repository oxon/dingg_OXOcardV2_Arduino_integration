#ifndef _SOUND_DATA_ARRAY_RENDERER_H_
#define _SOUND_DATA_ARRAY_RENDERER_H_
/*******************************************************************************
* \file		sound_data_array_renderer.h
********************************************************************************
* \author		Jascha Haldemann			jh@oxon.ch
* \date			26.01.2018
* \version	1.0
*
* \note			given soundDataArray must be recorded with a sampleRate of 32kHz
*						and a bitDepth of 8bit for now (TODO: allow other data)
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
class SoundDataArrayRenderer : public AudioRenderer
{
public:

	/* Public member typedefs */
	//...

	/* Public member data */
	//...

	/* Constructor and Destructor */
	SoundDataArrayRenderer(uint16_t sampleRate, uint16_t updateRate,
		uint8_t amplitude, uint8_t resolution = AudioRenderer::DEFAULT_RESOLUTION) :
		AudioRenderer(sampleRate, updateRate, amplitude, resolution) {}
	~SoundDataArrayRenderer() {}

	/* Public member functions */
	virtual void setup(uint8_t *soundDataArray, uint32_t arraySize);	// can be overridden by a subclass
	void getNextValues(uint32_t buffer[]);
	uint32_t getPlayDuration();		// in ms

protected:
	/* Protected constant declerations (static) */
	//...

	/* Protected member data */
	uint8_t *soundDataArray;
	uint32_t arraySize = 0;
	uint32_t currentArrayPos = 0;

	/* Protected member functions */
	//...

	/* Protected class functions (static) */
	//...

private:
	/* Private constant declerations (static) */
	//...

	/* Private member data */
	//...

	/* Private member functions */
	//...

	/* Private class functions (static) */
	//...

};

#endif
