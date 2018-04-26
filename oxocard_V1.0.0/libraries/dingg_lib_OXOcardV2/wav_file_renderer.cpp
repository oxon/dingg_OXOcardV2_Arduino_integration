/*******************************************************************************
* \file    wav_file_renderer.cpp
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
#include "wav_file_renderer.h"

/* ======================= Module constant declaration ====================== */
//...

/* ======================== Module macro declaration ======================== */
//...

/* ======================== Module global variables ========================= */
//...

/* ====================== Module class instantiations ======================= */
//...

/* ======================== Public member Functions ========================= */
/* --------------------------------------------------------------------------
 * \brief	setup renderer to play given wav-file
 * -------------------------------------------------------------------------- */
void WavFileRenderer::setup(uint8_t *wavFileArray, uint32_t arraySize)
{
	if (!isWavFile(wavFileArray, arraySize)) {
		ESP_LOGE(LOG_TAG, "Given file is not a WAVE-file!");
		return;
	}
	wavFileData = getWavFileData(wavFileArray, arraySize);
	soundDataArray = wavFileArray + wavFileData.soundDataPosition;	// using ptr arithmetics here
	this->arraySize = wavFileData.soundDataSize;
	currentArrayPos = 0;

	ESP_LOGI(LOG_TAG, "WavFileData:");
	ESP_LOGI(LOG_TAG, "fmtCode = %d", wavFileData.fmtCode);
	ESP_LOGI(LOG_TAG, "channels = %d", wavFileData.channels);
	ESP_LOGI(LOG_TAG, "sampleRate = %d Hz", wavFileData.sampleRate);
	ESP_LOGI(LOG_TAG, "soundDataByteRate = %d bytes/s",wavFileData.soundDataByteRate);
	ESP_LOGI(LOG_TAG, "fmtBlockAlign = %d", wavFileData.fmtBlockAlign);
	ESP_LOGI(LOG_TAG, "bitDepth = %d", wavFileData.bitDepth);
	ESP_LOGI(LOG_TAG, "soundDataPosition = %d", wavFileData.soundDataPosition);
	ESP_LOGI(LOG_TAG, "soundDataSize = %d bytes", wavFileData.soundDataSize);
	ESP_LOGI(LOG_TAG, "playDuration = %d ms", wavFileData.playDuration);
}

/* ======================= Private member Functions ========================= */

/* ======================== Private class Functions ========================= */
/* --------------------------------------------------------------------------
 * \brief	returns true if given file is a RIFF file
 * -------------------------------------------------------------------------- */
bool WavFileRenderer::isWavFile(uint8_t *wavFileArray, uint32_t arraySize)
{
	if (arraySize < 4) return false;
	uint32_t chunkID = byteArray2Uint32(wavFileArray);
	return (chunkID == 0x46464952);		// "RIFF"
}

/* --------------------------------------------------------------------------
 * \brief	returns the wavFileData of given wav-file
 * -------------------------------------------------------------------------- */
WavFileRenderer::wavFileData_t WavFileRenderer::getWavFileData(
	uint8_t *wavFileArray, uint32_t arraySize)
{
	wavFileData_t wavFileData;
	ESP_LOGD(LOG_TAG, "arraySize = %d", arraySize);
	uint8_t chunkBuf[5] = {0};
	uint32_t endPosition = arraySize;
	uint32_t pos = 0;
	/* handle the WAVE-fiel chunks */
	ESP_LOGD(LOG_TAG, "Check the WAVE-file chunks...");
	while((pos < endPosition))
	{
		/* read current chunk ID */
		memcpy(chunkBuf, &wavFileArray[pos], 4); pos += 4;
		ESP_LOGD(LOG_TAG, "  current chunkID = \"%s\"", (char*)chunkBuf);
		uint32_t chunkID = byteArray2Uint32(chunkBuf);
		/* get current chunk size */
		memcpy(chunkBuf, &wavFileArray[pos], 4); pos += 4;
		uint32_t chunkSize = byteArray2Uint32(chunkBuf);
		ESP_LOGD(LOG_TAG, "  current chunkSize = %d", chunkSize);
		/* handle current chunk */
		switch(chunkID)
		{
			case 0x46464952:    // "RIFF"
			{
				memcpy(chunkBuf, &wavFileArray[pos], 4); pos += 4;
				ESP_LOGD(LOG_TAG, "    riffType = %s", (char*)chunkBuf);
			}break;
			/* ------------------------------------------- */
			case 0x20746d66:    // "fmt "
			{
				memcpy(chunkBuf, &wavFileArray[pos], 2); pos += 2;
				wavFileData.fmtCode = byteArray2Uint16(chunkBuf);
				ESP_LOGD(LOG_TAG, "    fmtCode = %d", wavFileData.fmtCode);
				memcpy(chunkBuf, &wavFileArray[pos], 2); pos += 2;
				wavFileData.channels = byteArray2Uint16(chunkBuf);
				ESP_LOGD(LOG_TAG, "    channels = %d", wavFileData.channels);
				memcpy(chunkBuf, &wavFileArray[pos], 4); pos += 4;
				wavFileData.sampleRate = byteArray2Uint32(chunkBuf);
				ESP_LOGD(LOG_TAG, "    sampleRate = %d", wavFileData.sampleRate);
				memcpy(chunkBuf, &wavFileArray[pos], 4); pos += 4;
				wavFileData.soundDataByteRate = byteArray2Uint32(chunkBuf);
				ESP_LOGD(LOG_TAG, "    soundDataByteRate = %d",
					wavFileData.soundDataByteRate);
				memcpy(chunkBuf, &wavFileArray[pos], 2); pos += 2;
				wavFileData.fmtBlockAlign = byteArray2Uint16(chunkBuf);
				ESP_LOGD(LOG_TAG, "    fmtBlockAlign = %d", wavFileData.fmtBlockAlign);
				memcpy(chunkBuf, &wavFileArray[pos], 2); pos += 2;
				wavFileData.bitDepth = byteArray2Uint16(chunkBuf);
				ESP_LOGD(LOG_TAG, "    bitDepth = %d", wavFileData.bitDepth);
				/* read any extra bytes */
				if (chunkSize > 16)
				{
					for (uint8_t n = 0; n < (chunkSize - 16); n++) pos++;
				}
			}break;
			/* ------------------------------------------- */
			case 0x5453494C:   // "LIST"
			{
				// placeholder
				/* read any extra bytes */
				for (uint8_t n = 0; n < chunkSize; n++) pos++;
			}break;
			/* ------------------------------------------- */
			case 0x4F464E49:   // "INFO"
			{
				// placeholder
				/* read any extra bytes */
				for (uint8_t n = 0; n < chunkSize; n++) pos++;
			}break;
			/* ------------------------------------------- */
			case 0x61746164:   // "data"
			{
				wavFileData.soundDataPosition = pos;
				ESP_LOGD(LOG_TAG, "    soundDataPosition = %d",
					wavFileData.soundDataPosition);
				wavFileData.soundDataSize = chunkSize;
				ESP_LOGD(LOG_TAG, "    soundDataSize = %d", wavFileData.soundDataSize);
				wavFileData.playDuration = wavFileData.soundDataSize * 1000 /
					wavFileData.sampleRate;
				pos = endPosition;
			}break;
			/* ------------------------------------------- */
			default:
			{
				ESP_LOGD(LOG_TAG, "unknow chunk type!");
				pos = endPosition;
			}break;
		}
	}
	ESP_LOGD(LOG_TAG, "end of file reached!");
	return wavFileData;
}

/* --------------------------------------------------------------------------
 * \brief	...
 * -------------------------------------------------------------------------- */
uint16_t WavFileRenderer::byteArray2Uint16(uint8_t *byteArray)
{
	return (((uint16_t)(byteArray[1]) << 8) | (uint16_t)(byteArray[0])); // LSB was first sent
}

/* --------------------------------------------------------------------------
 * \brief	...
 * -------------------------------------------------------------------------- */
uint32_t WavFileRenderer::byteArray2Uint32(uint8_t *byteArray)
{
	return (((uint32_t)(byteArray[3]) << 24) | ((uint32_t)(byteArray[2]) << 16) |
	        ((uint32_t)(byteArray[1]) << 8) | (uint32_t)(byteArray[0])); // LSB was first sent
}
