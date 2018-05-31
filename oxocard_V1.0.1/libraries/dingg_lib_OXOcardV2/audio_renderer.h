#ifndef _AUDIO_RENDERER_H_
#define _AUDIO_RENDERER_H_
/*******************************************************************************
* \file		audio_renderer.h
********************************************************************************
* \author		Jascha Haldemann			jh@oxon.ch
* \author		Tobias Meerstetter		tm@oxon.ch
* \date			27.11.2017
* \version	1.0
*
* \brief	Virtual audio renderer class
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

/* ==================== Global module constant declaration ================== */

/* ========================= Global macro declaration ======================= */

/* ============================ Class declaration =========================== */
class AudioRenderer
{
public:

	/* Public member typedefs */
	//...

	/* Public member data */
	static const uint16_t DEFAULT_SAMPLE_RATE = 32000;	// in Hz
	static const uint16_t DEFAULT_UPDATE_RATE = 10;			// in ms
	static const uint16_t DEFAULT_BUFFER_SIZE = DEFAULT_SAMPLE_RATE *
		DEFAULT_UPDATE_RATE / 1000;												// in bytes
	static const uint8_t DEFAULT_RESOLUTION = 8;				// in bits
	static const uint8_t DEFAULT_AMPLITUDE = 128;
	static const uint8_t MAX_AMPLITUDE = 255;

	/* Constructor and Destructor */
	AudioRenderer(uint16_t sampleRate, uint16_t updateRate, uint8_t amplitude,
		uint8_t resolution = DEFAULT_RESOLUTION) :
		sampleRate_(sampleRate), updateRate_(updateRate), amplitude_(amplitude),
		resolution_(resolution) {
		bufferSize_ = sampleRate * updateRate / 1000;
	}
	~AudioRenderer() {}

	/* Public member functions */
	virtual void getNextValues(uint32_t buffer[]);
	void setAmplitude(uint8_t amplitude = DEFAULT_AMPLITUDE) {
		amplitude_ = amplitude;
	}
	uint8_t getAmplitude() {return amplitude_;}
	uint16_t getSampleRate() {return sampleRate_;}
	uint16_t getUpdateRate() {return updateRate_;}
	uint16_t getBufferSize() {return bufferSize_;}
	uint16_t getResolution() {return resolution_;}

	/* Public class functions (static) */
	static uint32_t toI2sDac(uint8_t data, uint8_t invertedData)	// data must be within 0...amplitude
	{
		uint16_t sampleChannelLeft  = ((uint16_t)(data)         & 0x00FF) << 8;	// shift by 8 since the DAC only takes the upper 8 bits of an int16
		uint16_t sampleChannelRight = ((uint16_t)(invertedData) & 0x00FF) << 8;	// shift by 8 since the DAC only takes the upper 8 bits of an int16
		return ((uint32_t)(sampleChannelLeft << 16) | sampleChannelRight);			// second channel is inverted for differential output
	}

protected:
	/* Protected constant declerations (static) */
	//...

  /* Protected member data */
	uint16_t sampleRate_ = DEFAULT_SAMPLE_RATE;
	uint16_t updateRate_ = DEFAULT_UPDATE_RATE;
	uint8_t amplitude_ = DEFAULT_AMPLITUDE;
	uint8_t resolution_ = DEFAULT_RESOLUTION;
	uint16_t bufferSize_ = DEFAULT_BUFFER_SIZE;

  /* Protected member functions */
	void updateBufferSize() {
		bufferSize_ = sampleRate_ * updateRate_ / 1000;
	}

	/* Protected class functions (static) */
	//...

};

#endif
