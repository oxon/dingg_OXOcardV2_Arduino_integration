#ifndef _LIB_BATTERY_H_
#define _LIB_BATTERY_H_
/*
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
 */

#include <Arduino.h>
#include <driver/adc.h>
#include "matrix.h"
#include "system.h"

/* FreeRTOS includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class Battery
{
public:
	Battery(uint8_t scanBatPin, uint8_t chargedPin, uint8_t pluggedInPinRead,
		uint8_t pluggedInPinEnable);
	~Battery();

	float getVoltage(uint32_t nSamples = 5000);
	uint16_t getVoltageInMillivolt(uint32_t nSamples = 5000);
	float getMaxVoltage(uint32_t nSamples = 500);
	uint16_t getMaxVoltageInMillivolt(uint32_t nSamples = 500);
	bool isUsbPluggedIn();
	bool isFullyCharged();
	bool isLowBattery();
	void handleLowBattery(Matrix *matrix, System *system);
	void show(Matrix *matrix, uint16_t delayInMs);

protected:

private:
	const char *LOG_TAG = "Battery";
	static const float MAX_BATTERY_VOLTAGE;
	static const float LOW_BATTERY_VOLTAGE;
	static const float MIN_BATTERY_VOLTAGE;
	static const uint16_t MAX_ADC_VALUE;
	static const uint16_t MAX_SHOW_LOW_BAT_DURATION;
	static volatile SemaphoreHandle_t adcReadSemaphore;

	uint8_t chargedPin, pluggedInPinRead, pluggedInPinEnable;
	adc1_channel_t adc1Channel_;

	int16_t getAdcValue(uint32_t nSamples);
	int16_t getMaxAdcValue(uint32_t nSamples);
	void draw(Matrix *matrix, uint16_t delayInMs = 0);

};

#endif
