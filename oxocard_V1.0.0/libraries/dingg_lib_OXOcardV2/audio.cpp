/*******************************************************************************
* \file    audio_output.cpp
********************************************************************************
* \author		Tobias Meerstetter		tm@oxon.ch
* \author		Jascha Haldemann			jh@oxon.ch
* \date			30.01.2018
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
#include <esp_heap_caps.h>
#include <esp_log.h>
#include "audio.h"

/* ======================= Module constant declaration ====================== */
//...

/* ======================== Module macro declaration ======================== */
//...

/* ======================== Module global variables ========================= */
uint8_t *micData = NULL;
uint32_t micDataSize = 0;

/* ====================== Module class instantiations ======================= */
// ...

/* ====================== Constructor & Deconstructor ======================= */
Audio::Audio(uint8_t shutdownPin)
{
	audioOut = new AudioOutput(shutdownPin, sampleRate_, updateRate_);
	melody = new Melody(120);
	microphone = new MemsMic(sampleRate_, updateRate_, amplitude_);
}

Audio::~Audio()
{
	delete(audioOut);
	delete(melody);
	// delete(microphone);
	// if(isWaveFormGeneratorInit) delete(waveFormGenerator);
}

/* ========================== Callbacks and Tasks =========================== */
void melodyTask(void *pvParameter)
{
	Audio *audio = (Audio *)pvParameter;
	Melody *melody = audio->getMelody();
	melody->fromStart();
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while(melody->hasNext())
	{
		tone_t tone = melody->getNextNote();
		if(tone.note != Note::REST)
		{
			audio->getWaveFormGenerator()->setup(WaveFormGenerator::WAVE_SINE,
				static_cast<uint8_t>(tone.note) << static_cast<uint8_t>(tone.octave));
			audio->getAudioOutput()->play(250 * tone.duration);
		}
		vTaskDelayUntil( &xLastWakeTime,
			(250 * tone.duration + NOTE_PAUSE) / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

void recordingTask(void *pvParameter)
{
	Audio *audio = (Audio *)pvParameter;
	MemsMic *microphone = audio->getMicrophone();
	uint16_t dmaBufLen = microphone->getBufferSize();
	uint16_t updateRate_ = microphone->getUpdateRate();
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for (uint16_t n = 0; n < micDataSize/dmaBufLen; n++)
	{
		microphone->read(micData+dmaBufLen*n, dmaBufLen);
		vTaskDelayUntil(&xLastWakeTime, updateRate_ / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

/* ======================== Public member Functions ========================= */
/**
 * Function to play a waveform with the given frequency and duration
 *
 * \param  frequency  TODO
 * \param  duration  TODO
 */
void Audio::tone(uint16_t frequency, uint32_t duration)
{
	if (frequency >= WaveFormGenerator::MIN_FREQUENCY)
	{
		if(!isWaveFormGeneratorInit)
		{
			waveFormGenerator = new WaveFormGenerator(sampleRate_, updateRate_,
				amplitude_);
			isWaveFormGeneratorInit = true;
		}
		waveFormGenerator->setup(globalWaveForm, frequency);
		waveFormGenerator->setAmplitude(amplitude_);
		audioOut->setAudioRenderer(waveFormGenerator);
		audioOut->play(duration);
	}
	if(isToneBlocking && duration < AudioOutput::PLAY_FOREVER)
		delay(duration+20); //20 ms delay between tones
}

void Audio::tone(Note note, uint32_t duration) {
	tone(static_cast<uint8_t>(note) << static_cast<uint8_t>(globalOctave), duration);
}

/**
 * Function to stop the tone
 */
void Audio::noTone()
{
	audioOut->stop();
}

/**
 * Function to play a melody
 *
 * \param  tones  TODO
 */
void Audio::playMelody()
{
	if(!isWaveFormGeneratorInit)
	{
		waveFormGenerator = new WaveFormGenerator(sampleRate_, updateRate_,
			amplitude_);
		isWaveFormGeneratorInit = true;
	}
	waveFormGenerator->setAmplitude(amplitude_);
	audioOut->setAudioRenderer(waveFormGenerator);

	xTaskCreate(&melodyTask, "melodyTask", 4096, (void *)this, 10, NULL);
}

void Audio::recordFor(uint16_t milliSeconds, uint16_t micSampleRate)
{
	const uint32_t RESERVED_RAM = 100000;	//blup
	micDataSize = milliSeconds*micSampleRate/1000;
	ESP_LOGD(this->LOG_TAG, "free heap = %d", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
	if (micData != NULL) free(micData);
	ESP_LOGD(this->LOG_TAG, "free heap = %d", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
	if ((heap_caps_get_free_size(MALLOC_CAP_DEFAULT)-RESERVED_RAM) > micDataSize)
		micData = (uint8_t*)malloc(micDataSize);
	else
	{
		ESP_LOGE(this->LOG_TAG, "not enough RAM to record this long!");
		return;
	}
	ESP_LOGD(this->LOG_TAG, "free heap = %d", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));

	audioOut->stop();
	delay(50);	// extra delay to improve stability
	audioOut->deinit();
	microphone->init(micSampleRate, updateRate_);
	ESP_LOGI(this->LOG_TAG, "recording...");
	xTaskCreate(&recordingTask, "recordingTask", 4096, (void *)this, 10, NULL);
	delay(milliSeconds);	// block the programm while recording
	microphone->deinit();
	audioOut->init(sampleRate_, updateRate_);
}

void Audio::playRecorded(bool playForever)
{
	if (micData == NULL)
	{
		ESP_LOGW(this->LOG_TAG, "Nothing recorded yet");
		return;
	}
	if (sampleRate_ != microphone->getSampleRate())
	{
		audioOut->stop();
		delay(50);	// extra delay to improve stability
		audioOut->deinit();
		uint16_t oldSampleRate = sampleRate_;
		uint16_t oldUpdateRate = updateRate_;
		sampleRate_ = microphone->getSampleRate();	// needs to be set globally
		updateRate_ = microphone->getUpdateRate();	// needs to be set globally
		audioOut->init(sampleRate_, updateRate_);
		playSoundDataArray(micData, micDataSize, playForever);
		delay(micDataSize*1000/sampleRate_+50);
		sampleRate_ = oldSampleRate;
		updateRate_ = oldUpdateRate;
		audioOut->deinit();
		audioOut->init(sampleRate_, updateRate_);
	}
	else
	{
		playSoundDataArray(micData, micDataSize, playForever);
		delay(micDataSize*1000/sampleRate_+50);
	}
}

void Audio::deleteRecorded()
{
	free(micData);
	micData = NULL;
	micDataSize = 0;
	ESP_LOGI(this->LOG_TAG, "Deleted recorded data");
}

bool Audio::waitForLoudNoise(uint32_t maxDuration, int16_t threshold,
	uint16_t micSampleRate)
{
	bool loudNoise = false;
	int16_t micData = 0;
	audioOut->stop();
	delay(50);	// extra delay to improve stability
	audioOut->deinit();
	microphone->init(micSampleRate, updateRate_);
	delay(10);	// additional delay needed to not instantly detect a loud noise
	ESP_LOGI(this->LOG_TAG, "listening...");
	uint32_t millisRef = millis();
	while((millis() - millisRef) < maxDuration && !loudNoise)
	{
		micData = microphone->read();
		if ((micData >= threshold) || (micData <= -threshold)) loudNoise = true;
	}
	if (loudNoise) {ESP_LOGI(this->LOG_TAG, "Loud noise detected.");}
	else {ESP_LOGW(this->LOG_TAG, "No loud noise detected within the given time.");}
	microphone->deinit();
	audioOut->init(sampleRate_, updateRate_);
	return loudNoise;
}

void Audio::playSoundDataArray(uint8_t *soundDataArray, uint32_t arraySize,
	bool playForever)
{
	static SoundDataArrayRenderer *soundDataArrayRenderer = new
		SoundDataArrayRenderer(sampleRate_, updateRate_, amplitude_);
	handlePlaySoundDataArray(soundDataArrayRenderer, soundDataArray, arraySize,
		playForever);
	//TODO: delete(soundDataArrayRenderer)
}

void Audio::playWavFile(uint8_t *wavFileArray, uint32_t arraySize,
	bool playForever)
{
	static WavFileRenderer *wavFileRenderer = new
		WavFileRenderer(sampleRate_, updateRate_, amplitude_);
	handlePlaySoundDataArray(wavFileRenderer, wavFileArray, arraySize,
		playForever);
	//TODO: delete(wavFileRenderer)
}

// void Audio::playMp3File(uint8_t *mp3FileArray, uint32_t arraySize,
// 	bool playForever)
// {
// 	static Mp3FileRenderer *mp3FileRenderer = new
// 		Mp3FileRenderer(sampleRate_, updateRate_, amplitude_);
// 	handlePlaySoundDataArray(mp3FileRenderer, mp3FileArray, arraySize,
// 		playForever);
// 	//TODO: delete(mp3FileRenderer)
// }

void Audio::setVolume(uint8_t volume)
{
	amplitude_ = volume;
	ESP_LOGI(this->LOG_TAG, "volume = %d/%d", amplitude_, AudioRenderer::MAX_AMPLITUDE);
}

uint8_t Audio::getVolume()
{
	return amplitude_;
}

void Audio::volumeUp()
{
	int16_t volume = getVolume();
	volume += VOLUME_STEP;
	if (volume > AudioRenderer::MAX_AMPLITUDE) {
		volume = AudioRenderer::MAX_AMPLITUDE;
	}
	setVolume(volume);
}

void Audio::volumeDown()
{
	int16_t volume = getVolume();
	volume -= VOLUME_STEP;
	if (volume < 0) volume = 0;
	setVolume(volume);
}

/* ======================= Private member Functions ========================= */
void Audio::handlePlaySoundDataArray(
	SoundDataArrayRenderer *soundDataArrayRenderer, uint8_t *soundDataArray,
	uint32_t arraySize, bool playForever)
{
	soundDataArrayRenderer->setup(soundDataArray, arraySize);
	soundDataArrayRenderer->setAmplitude(amplitude_);
	audioOut->setAudioRenderer(soundDataArrayRenderer);
	if (playForever) audioOut->play();
	else audioOut->play(soundDataArrayRenderer->getPlayDuration());
}

/* ======================== Private class Functions ========================= */
