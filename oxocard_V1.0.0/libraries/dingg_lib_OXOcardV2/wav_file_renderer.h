#ifndef _WAV_FILE_RENDERER_H_
#define _WAV_FILE_RENDERER_H_
/*******************************************************************************
* \file		wav_file_renderer.h
********************************************************************************
* \author		Jascha Haldemann			jh@oxon.ch
* \date			30.11.2017
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
#include "sound_data_array_renderer.h"

/* ==================== Global module constant declaration ================== */

/* ========================= Global macro declaration ======================= */

/* ============================ Class declaration =========================== */
class WavFileRenderer : public SoundDataArrayRenderer
{
public:

	/* Public member typedefs */
	typedef struct
	{
		uint16_t fmtCode;
		uint16_t channels;
		uint32_t sampleRate;					// in Hz
		uint32_t soundDataByteRate;		// bytes/s
		uint16_t fmtBlockAlign;
		uint16_t bitDepth;
		uint32_t soundDataPosition;
		uint32_t soundDataSize;				// in bytes
		uint32_t playDuration;				// in ms
	} wavFileData_t;

	/* Public member data */
	//...

	/* Constructor and Destructor */
	WavFileRenderer(uint16_t sampleRate, uint16_t updateRate, uint8_t amplitude,
		uint8_t resolution = AudioRenderer::DEFAULT_RESOLUTION) :
		SoundDataArrayRenderer(sampleRate, updateRate, amplitude, resolution) {}
	~WavFileRenderer() {}

	/* Public member functions */
	void setup(uint8_t *wavFileArray, uint32_t arraySize);

private:
	/* Private constant declerations (static) */
	static constexpr char *LOG_TAG = (char*)"WavFileRenderer";

  /* Private member data */
	wavFileData_t wavFileData;

  /* Private member functions */
	//...

	/* Private class functions (static) */
	static bool isWavFile(uint8_t *wavFileArray, uint32_t arraySize);
	static wavFileData_t getWavFileData(uint8_t *wavFileArray, uint32_t arraySize);
	static uint16_t byteArray2Uint16(uint8_t *byteArray);
	static uint32_t byteArray2Uint32(uint8_t *byteArray);

};

#endif
