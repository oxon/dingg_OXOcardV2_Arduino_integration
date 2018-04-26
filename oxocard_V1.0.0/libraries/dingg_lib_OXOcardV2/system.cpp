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
#include "accelerometer.h"
#include "images.h"
#include "pin_defines.h"
#include "system.h"
#include "persistentmemory.h"
#include "statistics.h"
#include "wireless.h"

/* FreeRTOS includes */
#include <freertos/task.h>

char System::deviceId[UUID::LENGTH+1] = {0};
char System::deviceName[System::DEVICE_NAME_LENGTH+1] = {0};

/**
 * Description of the constructor
 */
System::System(Accelerometer *accel, Audio *audio, Matrix *matrix){
	this->accel = accel;
	this->audio = audio;
	this->matrix = matrix;
}

/**
 * Description of the deconstructor
 */
System::~System(){
}

void System::init(){
	this->initPins();
}

/**
 * Turns off the device by putting it in a deepSleep-mode
 *
 * \param  buttonByte	masked byte to determine the wake-up buttons
 */
void System::turnOff(uint8_t buttonByte)
{
	showTurnOff();
	ESP_LOGI(this->LOG_TAG, "Disable periphery");
	this->disablePeriphery();
	if (buttonByte){
		while(button1Pressed || button2Pressed || button3Pressed ||
			button4Pressed || button5Pressed || button6Pressed) {
			vTaskDelay(5/portTICK_PERIOD_MS);
		}
	}
	ESP_LOGI(this->LOG_TAG, "Turn off");
	vTaskDelay(1/portTICK_PERIOD_MS);
	this->deepSleep(0, buttonByte);
}

/**
 * Turns off the device by putting it in a deepSleep-mode for a given time in us
 *
 * \param  us sleep duration in us
 * \param  buttonByte	masked byte to determine the wake-up buttons
 */
void System::turnOffFor(uint64_t us, uint8_t buttonByte){
	showTurnOff();
	ESP_LOGI(this->LOG_TAG, "Disable periphery");
	this->disablePeriphery();
	if (buttonByte){
		while(button1Pressed || button2Pressed || button3Pressed ||
			button4Pressed || button5Pressed || button6Pressed) {
			vTaskDelay(5/portTICK_PERIOD_MS);
		}
	}
	ESP_LOGI(this->LOG_TAG, "Turn off for %d us", buttonByte);
	vTaskDelay(1/portTICK_PERIOD_MS);
	this->deepSleep(us, buttonByte);
}

void System::reboot(){
	ESP_LOGI(this->LOG_TAG, "Rebooting device");
	esp_restart();	// reset reason = SW_CPU_RESET
}

System::wakeUpCause_t System::getWakeUpCause()
{
	wakeUpCause_t wakeUpCause = WUC_UNDEF;
	switch(esp_sleep_get_wakeup_cause())
	{
		case ESP_SLEEP_WAKEUP_EXT0:
		{
		  wakeUpCause = WUC_BUTTON_L1;
		} break;
		case ESP_SLEEP_WAKEUP_EXT1:
		{
			uint64_t wkUpState = esp_sleep_get_ext1_wakeup_status();
			if      (wkUpState & (1ULL << BUTTON_L2)) wakeUpCause = WUC_BUTTON_L2;
			else if (wkUpState & (1ULL << BUTTON_L3)) wakeUpCause = WUC_BUTTON_L3;
			else if (wkUpState & (1ULL << BUTTON_R1)) wakeUpCause = WUC_BUTTON_R1;
			else if (wkUpState & (1ULL << BUTTON_R2)) wakeUpCause = WUC_BUTTON_R2;
			else if (wkUpState & (1ULL << BUTTON_R3)) wakeUpCause = WUC_BUTTON_R3;
		} break;
		case ESP_SLEEP_WAKEUP_TIMER:
		{
		  wakeUpCause = WUC_INT_RTC;
		} break;
		case ESP_SLEEP_WAKEUP_UNDEFINED:
		{
			if      (button5Pressed && button6Pressed)
				wakeUpCause = WUC_DEBUG_MODE_REQUEST;
			else if (button6Pressed) wakeUpCause = WUC_AP_MODE_REQUEST;
			else if (button4Pressed) wakeUpCause = WUC_UPDATE_REQUEST;
			else if (button5Pressed) wakeUpCause = WUC_PAIRING_REQUEST;
			else wakeUpCause = WUC_UNDEF;
		} break;
		default:
		{
			wakeUpCause = WUC_UNDEF;
		}
	}
	return wakeUpCause;
}

/**
 * Returns current wakeUpCause as a string text
 *
 * \return The wakeup as a string text
 */
const char* System::getWakeUpCauseText()
{
	return wakeUpCauseText[getWakeUpCause()];
}

/**
 * Inits used I/Os of the uC
 */
void System::initPins()
{
	#ifdef USE_V24
		pinMode(M_DATA_IN, INPUT);
		pinMode(M_CLK, OUTPUT);
		pinMode(BUTTON_L1, INPUT);
		pinMode(BUTTON_L2, INPUT);
		pinMode(BUTTON_L3, INPUT);
		pinMode(BUTTON_R1, INPUT);
		pinMode(BUTTON_R2, INPUT);
		pinMode(BUTTON_R3, INPUT);
		pinMode(IO16, INPUT_PULLUP);
		pinMode(IO17, INPUT_PULLUP);
	#endif
}


/**
 * disables all peripheies
 */
void System::disablePeriphery()
{
	this->accel->disable();
	this->audio->noTone();
	this->matrix->disableDisplay();
	Serial.end();
	pinMode(TXD, INPUT);
	pinMode(RXD, INPUT);
	vTaskDelay(10/portTICK_PERIOD_MS);
}

/**
 * Puts the ESP32 inta a deep sleep mode
 *
 * \param  us	sleep duration in us
 * \param  buttonByte	masked byte to determine the wake-up buttons
 */
void System::deepSleep(uint64_t us, uint8_t buttonByte)
{
	uint64_t buttonLongLong = 0;
	if (buttonByte & 0b00000001)
		esp_sleep_enable_ext0_wakeup(gpio_num_t(BUTTON_L1), LOW);
	if (buttonByte & 0b00000010) buttonLongLong |= (1ULL << BUTTON_L2);
	if (buttonByte & 0b00000100) buttonLongLong |= (1ULL << BUTTON_L3);
	if (buttonByte & 0b00001000) buttonLongLong |= (1ULL << BUTTON_R1);
	if (buttonByte & 0b00010000) buttonLongLong |= (1ULL << BUTTON_R2);
	if (buttonByte & 0b00100000) buttonLongLong |= (1ULL << BUTTON_R3);
	esp_sleep_enable_ext1_wakeup(buttonLongLong, ESP_EXT1_WAKEUP_ANY_HIGH);

	// esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);    // ~0.1uA less -> if off, an ext. pull-up/down is requiered and ext0 doesn't work anymore!
	// esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);  // ~0.5uA less
	// esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);  // ~0.5uA less -> required for wake up button recognation!
	esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_AUTO);

	if (us) esp_deep_sleep(us);
	else esp_deep_sleep_start();
}

char *System::getId(){
	if(System::deviceId[0] == '\0'){
		PersistentMemory pm = PersistentMemory("system");
		int8_t length = pm.getStr("deviceId", System::deviceId, UUID::LENGTH+1);
		if(length < 0){
			ESP_LOGE("getId()", "Reading the device ID failed!");
		}
	}
	return System::deviceId;
}

char *System::getName(){
	if(System::deviceName[0] == '\0'){
		PersistentMemory pm = PersistentMemory("system");
		int8_t length = pm.getStr("deviceName", System::deviceName, 33);
		if(length < 0){
			ESP_LOGE("getName()", "Reading the device Name failed!");
		}
	}
	return System::deviceName;
}

int8_t System::setName(char *name){
	PersistentMemory pm = PersistentMemory("system",
		PersistentMemory::WRITE_MODE);
	if(pm.saveStr((char*)"deviceName", name)<0){
		ESP_LOGE("setName()", "Something is really wrong with saving name.");
		return -1;
	}
	System::deviceName[0] = '\0';
	return 0;
}

void System::printInfo(){

	ESP_LOGI("CARDINFO", "ID:   %s", System::getId());
	ESP_LOGI("CARDINFO", "Name:   %s",  System::getName());
	ESP_LOGI("CARDINFO", "Build by:   %s", COMPILED_BY_STR);
	ESP_LOGI("CARDINFO", "Build on:   %s", COMPILED_ON_STR);
	ESP_LOGI("CARDINFO", "Build date: %s", BUILD_DATE_STR);
	ESP_LOGI("CARDINFO", "Branch:     %s", BUILD_BRANCH_STR);
	ESP_LOGI("CARDINFO", "Commit:     %s", BUILD_COMMIT_STR);
	ESP_LOGI("CARDINFO", "storedWiFiCount: %d", Wireless::storedWifis.size());
	ESP_LOGI("CARDINFO", "availableWiFiCount: %d", Wireless::availableWifis.size());
	Statistics s = Statistics();
	ESP_LOGI("CARDINFO", "BootCount: %d",s.getCounter("boot"));
	ESP_LOGI("CARDINFO", "FlashSuccessCount: %d",s.getCounter("flash_success"));
	ESP_LOGI("CARDINFO", "FlashFailureCount: %d",s.getCounter("flash_failure"));
	//TODO more info from statistics here
}

void System::showTurnOff(){
	ESP_LOGI(this->LOG_TAG, "Show turnOff");
	drawOxocardLogo(this->matrix);
	vTaskDelay(500/portTICK_PERIOD_MS);
	for (uint8_t n = 99; n > 0; n--) {
		drawOxocardLogo(this->matrix, 0.01*n);
		vTaskDelay(5/portTICK_PERIOD_MS);
	}
	vTaskDelay(500/portTICK_PERIOD_MS);
}
