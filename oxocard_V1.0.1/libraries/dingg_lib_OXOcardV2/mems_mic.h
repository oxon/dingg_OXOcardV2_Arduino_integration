#ifndef _LIB_MEMS_MIC_H_
#define _LIB_MEMS_MIC_H_
/*******************************************************************************
* \file		mems_mic.h
********************************************************************************
* \author		Jascha Haldemann			jh@oxon.ch
* \date			29.11.2017
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
#define MIC_AMP			((float)(10))		// found empirically: ~112...124...137 => 0... 255

/* ========================= Global macro declaration ======================= */

/* ============================ Class declaration =========================== */
class MemsMic : public AudioRenderer
{
public:
	/* Public member typedefs */
	//...

	/* Public member data */
	//...

	/* Constructor and Destructor */
	MemsMic(uint16_t sampleRate, uint16_t updateRate, uint8_t amplitude,
		uint8_t resolution = AudioRenderer::DEFAULT_RESOLUTION,
		uint8_t pinNrWS = 4, uint8_t pinNrDataIn = 36) :
	  AudioRenderer(sampleRate, updateRate, amplitude, resolution),
		pinNrWS_(pinNrWS), pinNrDataIn_(pinNrDataIn) {}
	~MemsMic() {}

	/* Public member functions */
	void init(uint16_t sampleRate = AudioRenderer::DEFAULT_SAMPLE_RATE,
		uint16_t updateRate = AudioRenderer::DEFAULT_UPDATE_RATE);
	void deinit();
	int16_t read();
	void read(uint8_t *buffer, uint16_t numberOfValues);
	void getNextValues(uint32_t buffer[]);

private:
	/* Private constant declerations (static) */
	const char *LOG_TAG = "Microphone";

  /* Private member data */
	uint8_t pinNrWS_, pinNrDataIn_;

  /* Private member functions */
	//...

	/* Private class functions (static) */
	//...

};

#endif
