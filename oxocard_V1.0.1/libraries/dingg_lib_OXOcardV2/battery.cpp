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
const float Battery::MAX_BATTERY_VOLTAGE = 4.4f;	// = 1.1V / 100kOhm * 400kOhm
const float Battery::LOW_BATTERY_VOLTAGE = 3.25f;
const float Battery::MIN_BATTERY_VOLTAGE = 3.05f;
const uint16_t Battery::MAX_ADC_VALUE = 4096;
const uint16_t Battery::MAX_SHOW_LOW_BAT_DURATION = 5000;	// in ms
volatile SemaphoreHandle_t Battery::adcReadSemaphore;

Battery::Battery(uint8_t scanBatPin, uint8_t chargedPin,
	uint8_t pluggedInPinRead, uint8_t pluggedInPinEnable)
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
	this->pluggedInPinRead = pluggedInPinRead;
	this->pluggedInPinEnable = pluggedInPinEnable;
	adcReadSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(adcReadSemaphore);
}

Battery::~Battery() {

}

float Battery::getVoltage(uint32_t nSamples)  // max: 1'048'832 @ 12bit-ADC
{
	return (MAX_BATTERY_VOLTAGE / MAX_ADC_VALUE * getAdcValue(nSamples));
}

uint16_t Battery::getVoltageInMillivolt(uint32_t nSamples)
{
	return (uint16_t)((MAX_BATTERY_VOLTAGE*1000) / MAX_ADC_VALUE * getAdcValue(nSamples));
}

float Battery::getMaxVoltage(uint32_t nSamples)
{
	return (MAX_BATTERY_VOLTAGE / MAX_ADC_VALUE * getMaxAdcValue(nSamples));
}

uint16_t Battery::getMaxVoltageInMillivolt(uint32_t nSamples)
{
	return (uint16_t)((MAX_BATTERY_VOLTAGE*1000) / MAX_ADC_VALUE * getMaxAdcValue(nSamples));
}

bool Battery::isUsbPluggedIn() {
	bool pluggedIn = false;
	pinMode(this->pluggedInPinEnable, INPUT);	// to prevent sourcing the CH340G through TXD
	pinMode(this->pluggedInPinRead, INPUT_PULLDOWN);
	vTaskDelay(1/portTICK_PERIOD_MS);
	pluggedIn = digitalRead(this->pluggedInPinRead);
	vTaskDelay(1/portTICK_PERIOD_MS);
	#ifdef CONFIG_MONITOR_BAUD
		Serial.begin(CONFIG_MONITOR_BAUD);	// necessary to reinitialize RX and TX correctly
	#else
		Serial.begin(115200);
	#endif
	vTaskDelay(1/portTICK_PERIOD_MS);
	return pluggedIn;
}

bool Battery::isFullyCharged() {
	pinMode(this->chargedPin, INPUT);
	vTaskDelay(1/portTICK_PERIOD_MS);
	return digitalRead(this->chargedPin);
}

bool Battery::isLowBattery() {
	float vBat = getMaxVoltage();	//getVoltage();
	ESP_LOGD(LOG_TAG, "vBat = %f", vBat);
	return (vBat <= LOW_BATTERY_VOLTAGE && vBat != 0 && !isUsbPluggedIn());
}

void Battery::handleLowBattery(Matrix *matrix, System *system)
{
	matrix->disableAutoUpdate();
	matrix->clear();
	draw(matrix);
	uint32_t timeRef = millis();
	while(1) {
		matrix->drawLine(2, 6, 5, 6, oxocardBrandColors[2]);
		matrix->update();
		vTaskDelay(250/portTICK_PERIOD_MS);
		/* reboot when USB was plugged in */
		if (isUsbPluggedIn()) {
			vTaskDelay(750/portTICK_PERIOD_MS);
			system->reboot();
		} else {
			matrix->drawLine(2, 6, 5, 6, rgb(255, 255, 255));
			matrix->update();
			vTaskDelay(250/portTICK_PERIOD_MS);
		}
		/* turn off when vBat <= MIN_BAT_VOLTAGE or after 6 seconds */
		if (getMaxVoltage() <= MIN_BATTERY_VOLTAGE || (millis() - timeRef) >= MAX_SHOW_LOW_BAT_DURATION) {
			system->turnOff();
		}
	}
}

void Battery::show(Matrix *matrix, uint16_t delayInMs)
{
	matrix->clear();
	draw(matrix, delayInMs);
	matrix->fill();
	uint16_t vBat = getMaxVoltageInMillivolt();
	if (vBat > 3800) matrix->drawRectangle(2, 2, 4, 5, oxocardBrandColors[1]);
	else if (vBat > 3600) matrix->drawRectangle(2, 3, 4, 4, oxocardBrandColors[1]);
	else if (vBat > 3400) matrix->drawRectangle(2, 4, 4, 3, oxocardBrandColors[3]);
	else if (vBat > 3200) matrix->drawRectangle(2, 5, 4, 2, oxocardBrandColors[2]);
	else matrix->drawLine(2, 6, 5, 6, oxocardBrandColors[2]);
	matrix->noFill();
}

int16_t Battery::getAdcValue(uint32_t nSamples)  // max: 1'048'832 @ 12bit-ADC
{
	uint32_t adcSum = 0;
	xSemaphoreTake(adcReadSemaphore, portMAX_DELAY);
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(adc1Channel_, ADC_ATTEN_DB_0);
	for (uint32_t n = 0; n < nSamples; n++)
	{
	  adcSum += adc1_get_raw(adc1Channel_);
	}
	xSemaphoreGive(adcReadSemaphore);
	int16_t adc = adcSum / nSamples;
	if (adc == 0) {ESP_LOGE(LOG_TAG, "reading adc1 failed!");}
	return adc;
}

int16_t Battery::getMaxAdcValue(uint32_t nSamples)
{
	uint16_t adc = 0;
	xSemaphoreTake(adcReadSemaphore, portMAX_DELAY);
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(adc1Channel_, ADC_ATTEN_DB_0);
	for (uint32_t n = 0; n < nSamples; n++)
	{
		uint16_t raw = adc1_get_raw(adc1Channel_);
		if (raw > adc) adc = raw;
	}
	xSemaphoreGive(adcReadSemaphore);
	if (adc == 0) {ESP_LOGE(LOG_TAG, "reading adc1 failed!");}
	return adc;
}

void Battery::draw(Matrix *matrix, uint16_t delayInMs)
{
	matrix->drawLine(2, 6, 5, 6, rgb(255, 255, 255));
	vTaskDelay(delayInMs/5/portTICK_PERIOD_MS);
	matrix->drawLine(2, 5, 5, 5, rgb(255, 255, 255));
	vTaskDelay(delayInMs/5/portTICK_PERIOD_MS);
	matrix->drawLine(2, 4, 5, 4, rgb(255, 255, 255));
	vTaskDelay(delayInMs/5/portTICK_PERIOD_MS);
	matrix->drawLine(2, 3, 5, 3, rgb(255, 255, 255));
	vTaskDelay(delayInMs/5/portTICK_PERIOD_MS);
	matrix->drawLine(2, 2, 5, 2, rgb(255, 255, 255));
	vTaskDelay(delayInMs/5/portTICK_PERIOD_MS);
	matrix->drawLine(3, 1, 4, 1, rgb(255, 255, 255));
}
