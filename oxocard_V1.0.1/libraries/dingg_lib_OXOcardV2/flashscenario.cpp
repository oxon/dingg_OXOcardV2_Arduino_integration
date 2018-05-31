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
#include "flashscenario.h"
#include "images.h"
#include "oxocard.h"
#include "statistics.h"
#include "urlutil.h"

static void static_progress_callback(void *scenarioPointer){
	((FlashScenario *)scenarioPointer)->progressCallback();
}

void FlashScenario::progressCallback(){
	rgbColor_t color = oxocardBrandColors[4];
	double current = this->ota->getProgress()*8;
	for(uint8_t i=0; i<8; i++){
		if(1.0 < current){
			this->oxocard->matrix->drawPixel(i, 0, color);
			current -= 1;
		}else if(current <= 1){
			this->oxocard->matrix->drawPixel(
				i,
				0,
				rgb(color.r*current, color.g*current, color.b*current)
			);
			current = 0;
		}
	}
	this->oxocard->matrix->update();
}


int8_t FlashScenario::execute(void){
	ESP_LOGI(this->LOG_TAG, "Running flashscenario.");

	int8_t hasUpdate = 0;
	Statistics s = Statistics();
	Backend b = Backend();
	while(!hasUpdate){
		char *json = b.info();
		if(json == NULL){
			ESP_LOGE(this->LOG_TAG, "Backend did not respond as expected. quitting.");
			return -1;
		}
		ESP_LOGI(this->LOG_TAG, "We have response: %s", json);
		cJSON *root = cJSON_Parse(json);
		if(cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(root, "hasUpdate"))){
			hasUpdate = 1;
		}else{
			ESP_LOGI(this->LOG_TAG, "No update. Retrying.");
			vTaskDelay(50/portTICK_PERIOD_MS);
		}
		cJSON_Delete(root);
		free(json);
	}

	ESP_LOGI(this->LOG_TAG, "We should flash.");

	this->oxocard->matrix->clear();
	this->oxocard->matrix->drawImage(imageDownloadIcon, 0, 0, oxocardBrandColors[4]);

	char *url = b.craftURL(
		"oxocard/flash?id=%s",
		URLUtil::encode(System::getId())
	);

	this->ota = new HTTPSOTA(url);
	this->ota->registerProgressCallback(&static_progress_callback, this);
	int8_t success = this->ota->flash();
	free(url);
	delete this->ota;
	if(success < 0){
		ESP_LOGE(this->LOG_TAG, "Flashing failed");
		s.increaseCounter("flash_failed");
		this->oxocard->matrix->clear();
		this->oxocard->matrix->drawImage(imageXIcon, 0, 0, oxocardBrandColors[2]);
		vTaskDelay(20*1000/portTICK_PERIOD_MS);
	}else{
		ESP_LOGI(this->LOG_TAG, "Done flashing. rebooting in 1500ms.");
		s.increaseCounter("flash_success");
		this->oxocard->matrix->clear();
		this->oxocard->matrix->drawImage(imageCheckmarkIcon, 0, 0, oxocardBrandColors[1]);
		this->oxocard->matrix->clear();
		vTaskDelay(1500/portTICK_PERIOD_MS);

		oxocard->system->reboot();
	}
	return 0;
}

int8_t FlashScenario::configure(void){
	ESP_LOGI(this->LOG_TAG, "Configure flashing.");

	this->oxocard->matrix->clear();
	this->oxocard->matrix->drawImage(imageRefreshIcon, 0,0, oxocardBrandColors[1]);
	this->oxocard->matrix->clear();

	this->requireWifi();

	return 0;
}
