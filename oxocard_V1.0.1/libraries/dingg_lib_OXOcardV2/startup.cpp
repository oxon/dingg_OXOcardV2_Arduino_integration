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
#include <esp_partition.h>
#include <esp_system.h>
#include <nvs.h>
#include <nvs_flash.h>
#include "auto_turn_off.h"
#include "images.h"
#include "startup.h"
#include "system.h"
#include "backendlog.h"
#include "persistentmemory.h"
#include "random.h"
#include "statistics.h"
#include "uuid.h"
#include "backend.h"
#include "wificonfiguration.h"

/* FreeRTOS includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

Startup::Startup(OXOcard *oxocard){
	this->oxocard = oxocard;
}

Startup::~Startup(){

}

void Startup::run(){
	this->oxocard->system->setWakeUpCause();	// must be called only once as early as possible
	if(this->oxocard->system->getWakeUpCause() == System::WUC_DEBUG_MODE_REQUEST){
		ESP_LOGI(this->LOG_TAG, "Switching to debug-mode.");
		esp_log_level_set("*", ESP_LOG_INFO);
	}else{
		esp_log_level_set("*", ESP_LOG_INFO);
	}
	ESP_LOGI(this->LOG_TAG, "We're starting everything up.");

	this->printHelloMessage();

	this->initNVS();

	Statistics s = Statistics();
	s.increaseCounter("boot");

	this->memory = new PersistentMemory("system", PersistentMemory::WRITE_MODE);

	ESP_LOGI(this->LOG_TAG, "enable the autoTurnOff function");
	AutoTurnOff& ato = AutoTurnOff::getInstance();
	ato.enable();

	this->checkDeviceId();
	this->checkDeviceName();
	this->initializeSettings();

	this->oxocard->init();
	this->oxocard->system->init();
	this->oxocard->wifi->init();

	this->oxocard->accelerometer->init();

	this->initMatrix();
	this->drawBootImage();
	this->checkBackendLogging();
}

void Startup::printHelloMessage(){
	ESP_LOGI(this->LOG_TAG, "*******************************************************");
	ESP_LOGI(this->LOG_TAG, "*      Welcome to the Oxocard. Built by OXON AG.      *");
	ESP_LOGI(this->LOG_TAG, "*******************************************************");
	ESP_LOGI(this->LOG_TAG, "* Build by:   %s", COMPILED_BY_STR);
	ESP_LOGI(this->LOG_TAG, "* Build on:   %s", COMPILED_ON_STR);
	ESP_LOGI(this->LOG_TAG, "* Build date: %s", BUILD_DATE_STR);
	ESP_LOGI(this->LOG_TAG, "* Branch:     %s", BUILD_BRANCH_STR);
	ESP_LOGI(this->LOG_TAG, "* Commit:     %s", BUILD_COMMIT_STR);
	ESP_LOGI(this->LOG_TAG, "********************************************************");
}

int8_t Startup::initNVS(){
	esp_err_t err = nvs_flash_init();
	if(err == ESP_ERR_NVS_NO_FREE_PAGES){
		const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
		if (partition != NULL) {
			err = esp_partition_erase_range(partition, 0, partition->size);
			if(!err){
				err = nvs_flash_init();
			} else {
				ESP_LOGE(this->LOG_TAG, "Failed to format the broken NVS partition!");
				return -1;
			}
		}
	}
	if(err) {
		ESP_LOGE(this->LOG_TAG, "Failed to initialize NVS! Error: %u", err);
		return -2;
	}
	ESP_LOGI(this->LOG_TAG, "NVS initialied.");
	return 0;
}

int8_t Startup::initMatrix(){

	this->oxocard->matrix->init();	// sets the leds and frameBuffer objects
	// empty all frame buffers
	for (uint8_t n = 0; n < this->oxocard->matrix->getFrameBufferListLength(); n++) {
		this->oxocard->matrix->setActiveFrameBuffer(n, false);
		this->oxocard->matrix->clear();
	}
	this->oxocard->matrix->setActiveFrameBuffer(1, false);
	this->oxocard->matrix->enableDisplay();
	this->oxocard->matrix->update();
	return 0;
}

int8_t Startup::checkDeviceId(){
	char uuid[UUID::LENGTH+1] = {0};
	int32_t len = this->memory->getStr("deviceId", uuid, UUID::LENGTH+1);
	if(len < 0){
		ESP_LOGW(this->LOG_TAG, "We stop checking deviceId. PersistentMemory produced and error: %i", len);
		return -1;
	}
	if(len != UUID::LENGTH+1){
		ESP_LOGI(this->LOG_TAG, "UUID length mismatch. Assuming no UUID.");
		(UUID()).generateV4(uuid);
		int8_t result = this->memory->saveStr((char*)"deviceId", uuid);
		if(result < 0){
			ESP_LOGE(this->LOG_TAG, "Couldn't save UUID, because of failure in PM: %i", result);
			return -2;
		}
		// When we have to generate a DeviceID we safely can assume that this is first-boot
		this->firstBoot = 1;
		// We also want to set default settings
	}
	ESP_LOGI(this->LOG_TAG, "Device ID: %s", uuid);
	return 0;
}

int8_t Startup::initializeSettings(){
	PersistentMemory pm = PersistentMemory("settings", PersistentMemory::WRITE_MODE);
	char value[128] = {0};
	if(pm.getStr("ota_hostname", (char *)&value, sizeof(value)) == 0){
		pm.saveStr("ota_hostname", CONFIG_OTA_HOSTNAME);
	}
	if(pm.getStr("ota_path_prefix", (char *)&value, sizeof(value)) == 0){
		pm.saveStr("ota_path_prefix", CONFIG_OTA_PATH_PREFIX);
	}
	uint32_t valueint = 0;
	if(pm.getUInt32("ota_port", &valueint) == 0){
		pm.saveUInt32("ota_port", CONFIG_OTA_PORT);
	}
	if(pm.getUInt32("backend_log", &valueint) == 0){
		pm.saveUInt32("backend_log", 0);
	}
	return 0;
}

int8_t Startup::checkDeviceName(){
	char name[System::DEVICE_NAME_LENGTH+1] = {0};
	int32_t len = this->memory->getStr("deviceName", name, System::DEVICE_NAME_LENGTH+1);
	if(len < 0){
		ESP_LOGW(this->LOG_TAG, "We stop checking deviceName. PersistentMemory produced and error: %i", len);
		return -1;
	}
	if(len == 0){
		ESP_LOGI(this->LOG_TAG, "Assuming no name is set as lenght == 0.");
		snprintf(name, System::DEVICE_NAME_LENGTH+1, "OXOcard %i", Random::getByte());
		int8_t result = this->memory->saveStr((char*)"deviceName", name);
		if(result < 0){
			ESP_LOGE(this->LOG_TAG, "Couldn't save deviceName, because of failure in PM: %i", result);
			return -2;
		}
	}
	ESP_LOGI(this->LOG_TAG, "Device Name: %s", name);
	return 0;
}

int8_t Startup::checkBackendLogging(){

	PersistentMemory pm = PersistentMemory("settings", PersistentMemory::WRITE_MODE);
	uint32_t backendLoggingActive = 0;
	pm.getUInt32("backend_log", &backendLoggingActive);
	if(this->oxocard->system->getWakeUpCause() == System::WUC_DEBUG_MODE_REQUEST){
		rgbColor_t flashColor;
		if(backendLoggingActive == 0){
			ESP_LOGI(this->LOG_TAG, "Activating backend-logging in settings and reboot.");
			pm.saveUInt32("backend_log", 1);
			flashColor = rgb(0, 255, 0);
		}else{
			ESP_LOGI(this->LOG_TAG, "Deactivating backend-logging in settings and reboot.");
			pm.saveUInt32("backend_log", 0);
			flashColor = rgb(255, 0, 0);
		}

		for(int i=0; i<40; i++){
			this->oxocard->matrix->fillScreen(flashColor);
			vTaskDelay(20/portTICK_PERIOD_MS);
			this->oxocard->matrix->clearScreen();
			vTaskDelay(20/portTICK_PERIOD_MS);
		}
		this->oxocard->system->reboot();
		return 1;
	}
	if(backendLoggingActive == 1){
		this->oxocard->wifi->autoConnect();
		BackendLog &bl = BackendLog::getInstance();
		bl.activate();
	}
	return 0;
}

void Startup::drawBootImage(void){
	System::wakeUpCause_t wakeUpCause = this->oxocard->system->getWakeUpCause();
	if (wakeUpCause >= System::WUC_AP_MODE_REQUEST) return; // do not show anything if it's a special scenario
	ESP_LOGI(this->LOG_TAG, "Show turnOn");
	for (uint8_t n = 1; n <= 100; n++) {
		drawOxocardLogo(this->oxocard->matrix, 0.01*n);
		vTaskDelay(5/portTICK_PERIOD_MS);
	}
	vTaskDelay(500/portTICK_PERIOD_MS);
	if (!this->oxocard->battery->isUsbPluggedIn()) {
		this->oxocard->battery->show(this->oxocard->matrix, 500);
		vTaskDelay(1500/portTICK_PERIOD_MS);
		drawOxocardLogo(this->oxocard->matrix);
	}
}

uint8_t Startup::isFirstBoot(){
	return this->firstBoot;
}
