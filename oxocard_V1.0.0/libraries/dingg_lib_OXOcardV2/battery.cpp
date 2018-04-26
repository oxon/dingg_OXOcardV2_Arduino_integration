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

#include <esp_log.h>
#include "battery.h"
#include "images.h"
#include "persistentmemory.h"

/* ADC defines */
const float Battery::MAX_BATTERY_VOLTAGE = 4.2f;
const float Battery::LOW_BATTERY_VOLTAGE = 3.3f;
const float Battery::MIN_BATTERY_VOLTAGE = 3.1f;
const uint16_t Battery::MAX_ADC_VALUE = 4096;

volatile SemaphoreHandle_t adcReadSemaphore;

Battery::Battery(uint8_t scanBatPin, uint8_t chargedPin, Matrix *matrix,
	System *system)
{
	pinMode(scanBatPin, INPUT);
	switch(scanBatPin)
	{
		case 36: adc1Channel_ = ADC1_CHANNEL_0; break;
		case 37: adc1Channel_ = ADC1_CHANNEL_1; break;
		case 38: adc1Channel_ = ADC1_CHANNEL_2; break;
		case 39: adc1Channel_ = ADC1_CHANNEL_3; break;
		case 32: adc1Channel_ = ADC1_CHANNEL_4; break;
		case 33: adc1Channel_ = ADC1_CHANNEL_5; break;
		case 34: adc1Channel_ = ADC1_CHANNEL_6; break;
		case 35: adc1Channel_ = ADC1_CHANNEL_7; break;
		default:
			ESP_LOGE(LOG_TAG, "invalid GPIO number for adc1");
			while(1);
	}
	this->chargedPin = chargedPin;
	this->matrix = matrix;
	this->system = system;
	adcReadSemaphore = xSemaphoreCreateMutex();
}

Battery::~Battery() {

}

float Battery::getVoltage(uint32_t nMax)  // max: 1'048'832 @ 12bit-ADC
{
	uint32_t adcSum = 0;
	xSemaphoreTake(adcReadSemaphore, portMAX_DELAY);
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(adc1Channel_, ADC_ATTEN_DB_0);
	for (uint32_t n = 0; n < nMax; n++)
	{
	  adcSum += adc1_get_raw(adc1Channel_);
	}
	xSemaphoreGive(adcReadSemaphore);
	float adc = (float)(adcSum) / nMax;
	if (adc == 0) {ESP_LOGE(LOG_TAG, "reading adc1 failed!");}
	return (MAX_BATTERY_VOLTAGE/MAX_ADC_VALUE*adc);
}

float Battery::getMaxVoltage(uint32_t nMax)
{
	uint16_t adc = 0;
	xSemaphoreTake(adcReadSemaphore, portMAX_DELAY);
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(adc1Channel_, ADC_ATTEN_DB_0);
	for (uint32_t n = 0; n < nMax; n++)
	{
		uint16_t raw = adc1_get_raw(adc1Channel_);
		if (raw > adc) adc = raw;
	}
	xSemaphoreGive(adcReadSemaphore);
	if (adc == 0) {ESP_LOGE(LOG_TAG, "reading adc1 failed!");}
	return (MAX_BATTERY_VOLTAGE/MAX_ADC_VALUE*adc);
}

bool Battery::isFullyCharged() {
	pinMode(this->chargedPin, INPUT);
	return digitalRead(this->chargedPin);
}

void Battery::handleLowBattery(bool isCharging, TaskHandle_t *taskToSusbendPtr1,
	TaskHandle_t *taskToSusbendPtr2, TaskHandle_t *taskToSusbendPtr3)
{
	float vBat = getMaxVoltage();	//getVoltage();
	if (vBat <= LOW_BATTERY_VOLTAGE && vBat != 0)
	{
		if (!isCharging)
		{
			xSemaphoreTake(matrix->updateSemaphore, portMAX_DELAY);
			if (*taskToSusbendPtr1 != NULL) vTaskSuspend(*taskToSusbendPtr1);
			if (*taskToSusbendPtr2 != NULL) vTaskSuspend(*taskToSusbendPtr2);
			if (*taskToSusbendPtr3 != NULL) vTaskSuspend(*taskToSusbendPtr3);
			xSemaphoreGive(matrix->updateSemaphore);
			matrix->disableAutoUpdate();
			matrix->clear();
			matrix->drawImage(imageBatIcon, 0, 0, rgb(0, 0, 255));
			uint32_t timeRef = millis();
			while(1)
			{
				matrix->drawLine(2, 6, 5, 6, rgb(200, 0, 0));
				matrix->update();
				vTaskDelay(250/portTICK_PERIOD_MS);
				if (isChargingCtr >= MAX_CHARGING_CTR/2)
				{
					if (isChargingCtr == MAX_CHARGING_CTR && !isCharging)
					{
						/* set is charging in the NVS */
						PersistentMemory pm = PersistentMemory("battery",
							PersistentMemory::WRITE_MODE);
						if(pm.saveByte((char*)"isCharging", 1) < 0) {
							ESP_LOGE(LOG_TAG, "Something is really wrong with saving isCharging.");
							return;
						}
						ESP_LOGI(LOG_TAG, "isCharging = true");
						vTaskDelay(5/portTICK_PERIOD_MS);
						system->reboot();
					}
				}
				else
				{
					matrix->drawLine(2, 6, 5, 6, rgb(0, 0, 0));
					matrix->update();
				}
				vTaskDelay(250/portTICK_PERIOD_MS);
				if (vBat <= MIN_BATTERY_VOLTAGE || (millis() - timeRef) >= 6000) {
					system->turnOff();
				}
				/* try to determine if USB was plugged in */
				float prevVBat = vBat;
				vBat = getVoltage();
				if (vBat >= prevVBat && isChargingCtr < MAX_CHARGING_CTR) {
					isChargingCtr++;
				}
				else if (isChargingCtr > 0) {
					isChargingCtr--;
				}
			}
		}
		else
		{
			/* try to determine if USB was unplugged */
			float prevVBat = vBat;
			vBat = getVoltage();
			if (vBat < prevVBat && isChargingCtr > 0) {
				isChargingCtr--;
			}
			else if (isChargingCtr < MAX_CHARGING_CTR) {
				isChargingCtr++;
			}
			if (isChargingCtr <= 0) {
				system->reboot();
			}
		}
	}
}
