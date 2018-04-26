#ifndef _LIB_AUDIO_H_
#define _LIB_AUDIO_H_
/*******************************************************************************
* \file		audio.h
********************************************************************************
* \author		Tobias Meerstetter		tm@oxon.ch
* \author		Jascha Haldemann			jh@oxon.ch
* \date			30.01.2018
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
#include <stdint.h>
#include "audio_output.h"
#include "audio_renderer.h"
#include "melody.h"
#include "mems_mic.h"
#include "noise_renderer.h"
#include "sound_data_array_renderer.h"
#include "sound_effects.h"
#include "tones.h"
#include "wav_file_renderer.h"
#include "wave_form_generator.h"

/* ==================== Global module constant declaration ================== */

/* ========================= Global macro declaration ======================= */

/* ============================ Class declaration =========================== */
class Audio
{
	public:
		Audio(uint8_t shutdownPin);
		~Audio();


		void tone(uint16_t frequency, uint32_t duration = AudioOutput::PLAY_FOREVER);
		void tone(Note note, uint32_t duration = AudioOutput::PLAY_FOREVER);
		void noTone();
		void playMelody();

		void recordFor(uint16_t milliSeconds,
			uint16_t micSampleRate = AudioRenderer::DEFAULT_SAMPLE_RATE);
		void playRecorded(bool playForever = true);
		void deleteRecorded();

		bool waitForLoudNoise(uint32_t maxDuration = -1, int16_t threshold = 32768*0.8,
			uint16_t micSampleRate = AudioRenderer::DEFAULT_SAMPLE_RATE);

		void playSoundDataArray(uint8_t *soundDataArray, uint32_t arraySize,
			bool playForever = true);
		void playWavFile(uint8_t *wavFileArray, uint32_t arraySize,
			bool playForever = true);

		void setVolume(uint8_t volume);
		uint8_t getVolume();
		void volumeUp();
		void volumeDown();

		uint32_t noteToMs(float duration) {return 60000.0f/(globalBPM / duration);}

		WaveFormGenerator* getWaveFormGenerator() {return waveFormGenerator;}
		AudioOutput* getAudioOutput() {return audioOut;}
		Melody* getMelody() {return melody;}
		MemsMic* getMicrophone() {return microphone;}

		void setGlobalBPM(uint16_t bpm) {globalBPM = bpm;}
		void setGlobalOctave(Octave octave) {globalOctave = octave;}
		void setGlobalWaveForm(WaveFormGenerator::waveForm_t waveForm) {
			globalWaveForm = waveForm;
		}
		void setToneBlocking(bool isBlocking) {isToneBlocking = isBlocking;}

	private:

		const char *LOG_TAG = "Audio";
		static const uint8_t VOLUME_STEP = 16;

		AudioOutput *audioOut;
		WaveFormGenerator *waveFormGenerator;
		Melody *melody;
		MemsMic *microphone;

		uint16_t sampleRate_ = AudioRenderer::DEFAULT_SAMPLE_RATE;
		uint16_t updateRate_ = AudioRenderer::DEFAULT_UPDATE_RATE;
		uint8_t amplitude_ = AudioRenderer::DEFAULT_AMPLITUDE;

		bool isWaveFormGeneratorInit = false;
		Octave globalOctave = Octave::C5;
		uint16_t globalBPM = 120;
		WaveFormGenerator::waveForm_t globalWaveForm = WaveFormGenerator::WAVE_SINE;
		bool isToneBlocking = false;


		void handlePlaySoundDataArray(
			SoundDataArrayRenderer *soundDataArrayRenderer,
			uint8_t *soundDataArray,
			uint32_t arraySize,
			bool playForever
		);

};

#endif
