#ifndef _NOISE_RENDERER_H_
#define _NOISE_RENDERER_H_
/*******************************************************************************
* \file		noise_renderer.h
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

/* ========================= Global macro declaration ======================= */

/* ============================ Class declaration =========================== */
class NoiseRenderer : public AudioRenderer
{
public:

	/* Public member typedefs */
	//...

	/* Public member data */
	//...

	/* Constructor and Destructor */
	NoiseRenderer(uint16_t sampleRate, uint16_t updateRate, uint8_t amplitude,
		uint8_t resolution = AudioRenderer::DEFAULT_RESOLUTION) :
		AudioRenderer(sampleRate, updateRate, amplitude, resolution) {}
	~NoiseRenderer() {}

	/* Public member functions */
	void getNextValues(uint32_t buffer[]);

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
